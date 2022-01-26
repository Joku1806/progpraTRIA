import struct, serial, json, time

class StampPacketReader:
    # prüft erst, ob der Port übereinstimmt
    SERIAL_PORT = '/dev/ttyACM0'
    SEND_SIGNAL = 0xff.to_bytes(1,"little")
    PACKET_SIZE = 19
    MAX_MEASURE_TIMEDIFF = 0x6819f

    def __init__(self):
        self.informant = serial.Serial(self.SERIAL_PORT, 9600)
        
    def reveive(self):
        self.informant.write(self.SEND_SIGNAL)
        count = int.from_bytes(self.informant.read(1), byteorder = 'little')
        measures = []
        
        for i in range(count):
            measure_packed = self.informant.read(self.PACKET_SIZE, byteorder = 'little')
            (_, sendid, recvid, rxtime, txtime) = struct.unpack(">BBBQQ", measure_packed)
            
            timediff = 0
            if rxttime >= txtime:
                timediff = rxtime - txtime
            else if txtime - rxtime > MAX_MEASURE_TIMEDIFF:
                # Timer Overflow während der Messung, Prüfwert wurde durch
                # maximale Distanz von 2km mit (2000m / c_air) / 15.65 * 1000000000000
                # berechnet.
                timediff = 0xffffffffff - txtime + rxtime
            else:
                # Sehr kleine Distanzen können durch Ungenauigkeiten in der Messung
                # negativ werden, in dem Fall einfach von 0 ausgehen.
                timediff = 0

            # Berechnete Zeitdifferenz ist in nativer Clock-Zeit, deswegen
            # erst in Picosekunden und dann in Sekunden umrechnen.
            ToF_s = timediff * 15.65 / 1000000000000
            measures.append({"id": sendid, 'tof': ToF_s})

        wrapped = {"mcus": measures}
        return json.dumps(wrapped, indent = 4)

if __name__ == "__main__":
    reader = StampPacketReader()
    
    while True:        
        measurement = reader.receive()
        print(measurement)
        time.sleep(0.5)
