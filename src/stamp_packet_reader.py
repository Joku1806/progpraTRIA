import struct, serial, json, time

class StampPacketReader:
    # prüft erst, ob der Port übereinstimmt
    SERIAL_PORT = '/dev/ttyACM0'
    SEND_SIGNAL = 0xff.to_bytes(1,"little")
    PACKET_SIZE = 19

    def __init__(self):
        self.informant = serial.Serial(self.SERIAL_PORT, 9600)
        
    def reveive(self):
        self.informant.write(self.SEND_SIGNAL)
        count = int.from_bytes(self.informant.read(1), byteorder = 'little')
        measures = []
        
        for i in range(count):
            measure_packed = self.informant.read(self.PACKET_SIZE, byteorder = 'little')
            (_, sendid, recvid, rxtime, txtime) = struct.unpack(">BBBQQ", measure_packed)
            
            # Gesammelte Zeitstempel sind in nativer Clock-Zeit, deswegen
            # erst in Picosekunden und dann in Sekunden umrechnen.
            # Es kann sein, dass wegen "Rauschen" die ToF negativ ist, das ist
            # natürlich nicht möglich, deswegen gehen wir in dem Fall von 0 aus.
            ToF_s = max(rxtime - txtime, 0) * 15.65 / 1000000000000
            measures.append({"id": sendid, 'tof': ToF_s})

        wrapped = {"mcus": measures}
        return json.dumps(wrapped, indent=4)

if __name__ == "__main__":
    reader = StampPacketReader()
    
    while True:        
        measurement = reader.receive()
        print(measurement)
        time.sleep(0.5)
