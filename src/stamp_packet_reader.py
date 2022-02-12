import struct, serial, json, time
from rich.progress import track
from statistics import mean, variance

class StampPacketReader:
    # prüft erst, ob der Port übereinstimmt
    SERIAL_PORT = '/dev/ttyACM0'
    SEND_SIGNAL = 0x6D # m
    MEASURE_SIZE = 9
    MAX_MEASURE_DURATION = 0.25 # 200ms

    def __init__(self):
        self.informant = serial.Serial(self.SERIAL_PORT, 9600, timeout = self.MAX_MEASURE_DURATION)
        self.request_count = 0
        self.successful_request_count = 0
        self.bench_stats = []
        self.dist_stats = {}
        
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
            measure_packed = self.informant.read(self.MEASURE_SIZE)
            (receiver_id, tof_native) = struct.unpack(">BQ", measure_packed)

            # Berechnete Zeitdifferenz ist in nativer Clock-Zeit, deswegen
            # erst in Picosekunden und dann in Sekunden umrechnen.
            tof_s = tof_native * 15.65 / 1000000000000
            self.dist_stats.setdefault(receiver_id & 0b00011111, []).append(tof_s * 299709000.0)
            measures.append({"id": receiver_id & 0b00011111, 'tof': tof_s})

        self.informant.reset_input_buffer()
        wrapped = {"mcus": measures}
        
        stop_bench = time.time()
        self.bench_stats.append(stop_bench - start_bench)
        self.successful_request_count += 1

        return json.dumps(wrapped, indent = 4)

    def print_stats(self):
      print(f"Received measurements in {self.successful_request_count / self.request_count * 100}% of cases.")
      print(f"Average execution time = {mean(self.bench_stats)}s, Variance = {variance(self.bench_stats)}s")
      
      for key in self.dist_stats.keys():
        print(f"Average distance to UNIT {key} = {mean(self.dist_stats[key])}m, Variance = {variance(self.dist_stats[key])}m")

if __name__ == "__main__":
    reader = StampPacketReader()
    
    for i in track(range(1000)):
      reader.receive()
    
    reader.print_stats()