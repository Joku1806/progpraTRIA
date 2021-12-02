import struct
import serial
from typing import Union
from dataclasses import dataclass


@dataclass
class StampPacket:
    # 1 Byte, oberstes bit bestimmt ob Tracker (1), oder Trackee (0)
    sender_devid: int
    # 1 Byte, oberstes bit bestimmt ob Tracker (1), oder Trackee (0)
    receiver_devid: int
    # 8 Byte
    timestamp_start: int
    # 8 Byte
    timestamp_stop: int


class StampPacketReader:
    # prüft erst, ob der Port übereinstimmt
    SERIAL_PORT = '/dev/ttyACM0'
    SEND_SIGNAL = 0xff
    PACKET_SIZE = 18

    def __init__(self):
        self.informant = serial.Serial('/dev/ttyACM0', 9600)

    def receive(self) -> Union[StampPacket, None]:
        self.informant.write(self.SEND_SIGNAL)
        count = self.informant.read(1)
        packets = []

        for i in range(count):
          bytes = self.informant.read(self.PACKET_SIZE)
          if not len(bytes) == self.PACKET_SIZE:
            continue
          (timestamp_start,
            timestamp_stop, sender_devid, receiver_devid) = struct.unpack("QQBB", bytes)
          packets.push(StampPacket(sender_devid, receiver_devid, timestamp_start, timestamp_stop))

        return packets

if __name__ == "__main__":
  reader = StampPacketReader()
  while True:
    packet = reader.receive()
    if packet == None:
      print("Nichts bekommen!")
      continue

    print(f"ID1 = {packet.sender_devid}, ID2 = {packet.receiver_devid}, start = {hex(packet.timestamp_start)}, stop = {hex(packet.timestamp_stop)}")