import struct
import serial
from typing import Union
from dataclasses import dataclass

"""
wants to be send things in this order
while (Serial.read() != 0xff);
  Serial.write(1);
  
  send((uint8_t *)&(dummy.rx),8);
  send((uint8_t *)&(dummy.tx),8);
  send((uint8_t *)&(dummy.aID),1);
  send((uint8_t *)&(dummy.bID),1);
  delay(100);
"""

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
    SEND_SIGNAL = 0xff.to_bytes(1,"little")
    PACKET_SIZE = 18

    def __init__(self):
        self.informant = serial.Serial('/dev/ttyACM0', 9600)

    def receive(self):# -> Union[StampPacket, None]: #this function alrays returns a list, maybe an empty one
        self.informant.write(self.SEND_SIGNAL)
        count = int.from_bytes(self.informant.read(1),byteorder='little')
        packets = []
        print(count)
        for i in range(count):
          bytys = self.informant.read(self.PACKET_SIZE)
          #if not len(bytys) == self.PACKET_SIZE:continue should ALLWAYS be True
          (timestamp_start,
            timestamp_stop, sender_devid, receiver_devid) = struct.unpack("QQBB", bytys)
          packets.append(StampPacket(sender_devid, receiver_devid, timestamp_start, timestamp_stop))

        return packets

if __name__ == "__main__":
  reader = StampPacketReader()
  while True:
    packet = reader.receive()
    if packet == None:
      print("Nichts bekommen!")
      continue
    packet = packet[0]
    print(f"ID1 = {packet.sender_devid}, ID2 = {packet.receiver_devid}, start = {hex(packet.timestamp_start)}, stop = {hex(packet.timestamp_stop)}")
