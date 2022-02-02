import struct, serial, json
from rich.progress import track

class StampPacketReader:
    # prüft erst, ob der Port übereinstimmt
    SERIAL_PORT = '/dev/ttyACM0'
    SEND_SIGNAL = 0x6D # m
    PACKET_SIZE = 19
    MAX_MEASURE_TIMEDIFF = 0x6819f

    def __init__(self):
        self.informant = serial.Serial(self.SERIAL_PORT, 9600)
        self.count_stats = [0] * 4
        
    def receive(self):
        self.informant.write(struct.pack("<B", self.SEND_SIGNAL))
        count = int.from_bytes(self.informant.read(4), byteorder = 'little')
        self.count_stats[count] += 1
        measures = []
        
        for _ in range(count):
            measure_packed = self.informant.read(self.PACKET_SIZE)
            (_, sender_id, _, rx_time, tx_time) = struct.unpack(">BBBQQ", measure_packed)
            
            timediff = 0
            if rx_time >= tx_time:
                timediff = rx_time - tx_time
            elif tx_time - rx_time > self.MAX_MEASURE_TIMEDIFF:
                # Timer Overflow während der Messung, Prüfwert wurde durch
                # maximale Distanz von 2km mit (2000m / c_air) / 15.65 * 1000000000000
                # berechnet.
                timediff = 0xffffffffff - tx_time + rx_time
            else:
                # Sehr kleine Distanzen können durch Ungenauigkeiten in der Messung
                # negativ werden, in dem Fall einfach von 0 ausgehen.
                timediff = 0

            # Berechnete Zeitdifferenz ist in nativer Clock-Zeit, deswegen
            # erst in Picosekunden und dann in Sekunden umrechnen.
            # Durch 2 teilen, weil nur der Hinweg gebraucht wird
            ToF_s = timediff * 15.65 / 2000000000000
            measures.append({"id": sender_id, 'tof': ToF_s})

        self.informant.reset_input_buffer()
        wrapped = {"mcus": measures}
        return json.dumps(wrapped, indent = 4)

    def print_count_stats(self):
      print(f"Number of measurement requests: {sum(self.count_stats)}")
      for i in range(4):
        print(f"Received measurements containing {i} datapoints {self.count_stats[i]} times.")

if __name__ == "__main__":
    reader = StampPacketReader()
    
    for i in track(range(10000)):
      measurement = reader.receive()
    
    reader.print_count_stats()
