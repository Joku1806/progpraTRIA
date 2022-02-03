import struct, serial, json, time
from rich.progress import track
from statistics import mean, variance

class StampPacketReader:
    # prüft erst, ob der Port übereinstimmt
    SERIAL_PORT = '/dev/ttyACM0'
    SEND_SIGNAL = 0x6D # m
    PACKET_SIZE = 19
    MAX_MEASURE_TIMEDIFF = 0x6819f
    MAX_MEASURE_DURATION = 0.25 # 200ms

    def __init__(self):
        self.informant = serial.Serial(self.SERIAL_PORT, 9600, timeout = self.MAX_MEASURE_DURATION)
        self.request_count = 0
        self.successful_request_count = 0
        self.bench_stats = []
        
    def receive(self):
        self.request_count += 1
        start_bench = time.time()

        self.informant.write(struct.pack("<B", self.SEND_SIGNAL))
        count_lb = self.informant.read(4)
        if len(count_lb) != 4:
          self.informant.reset_input_buffer()
          return None
        
        count = int.from_bytes(count_lb, byteorder = 'little')
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
        
        stop_bench = time.time()
        self.bench_stats.append(stop_bench - start_bench)
        self.successful_request_count += 1

        return json.dumps(wrapped, indent = 4)

    def print_stats(self):
      print(f"Received measurements in {self.successful_request_count / self.request_count * 100}% of cases.")
      print(f"Average execution time = {mean(self.bench_stats)}s, Variance = {variance(self.bench_stats)}s")

if __name__ == "__main__":
    reader = StampPacketReader()
    
    for i in track(range(1000)):
      reader.receive()
    
    reader.print_stats()