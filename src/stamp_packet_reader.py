import struct,serial,json,time
from dataclasses import dataclass


@dataclass
class StampPacket:
    # 4 Byte, eigentlich 1 Byte, oberstes bit bestimmt ob Tracker (1), oder Trackee (0)
    sendid: int
    # 4 Byte, eigentlich 1 Byte, oberstes bit bestimmt ob Tracker (1), oder Trackee (0)
    recvid: int
    # 8 Byte
    rxtime: int
    # 8 Byte
    txtime: int


class StampPacketReader:
    # prüft erst, ob der Port übereinstimmt
    SERIAL_PORT = '/dev/ttyACM0'
    SEND_SIGNAL = 0xff.to_bytes(1,"little")
    PACKET_SIZE = 19

    def __init__(self,Fake = False):
        if not Fake:
            self.informant = serial.Serial(self.SERIAL_PORT, 9600)

    def receive(self):# -> Union[StampPacket, None]: #this function alrays returns a list, maybe an empty one
        self.informant.write(self.SEND_SIGNAL)
        count = int.from_bytes(self.informant.read(1),byteorder='little')
        packets = []
        print(count)
        for i in range(count):
            
            bytys = self.informant.read(self.PACKET_SIZE)
            print(f"bytys len={len(bytys)}")
            #if not len(bytys) == self.PACKET_SIZE:continue should ALLWAYS be True
            (_, sendid,recvid,rxtime,txtime) = struct.unpack(">BBBQQ", bytys)#L:uint32 - Q:unint64 

            packets.append(StampPacket(sendid, recvid, rxtime, txtime))

        return packets
    def build_fake_pakets(self):
        packets = []

        packets.append(StampPacket(1, 2, 0x420, 0x96))
        packets.append(StampPacket(3, 2, 0x22, 0x11))
        packets.append(StampPacket(4, 2, 0xbabe, 0xaf))
        packets.append(StampPacket(5, 2, 0xFA, 0xAB))

        return packets

        
    def reveive_json(self):
        packets = self.build_fake_pakets()

        #i doubt precision bejond 1 sec will be needed here
        #i would love to use datetime(that even gives me usecs) but it gives me a sane YY-MM-DD format not what is specified
        #mm-dd-YY HH:MM:SS.ff
        t = time.localtime()
        timestamp = f"{t.tm_mon}-{t.tm_mday}-{t.tm_year} {t.tm_hour}:{t.tm_min}:{t.tm_sec}.0"
        num = len(packets)

        times = []

        # TODO: stattdessen erst in Sekunden umrechnen (clock frequenz nachsehen),
        # dann durch 2 teilen (Simon)(sollte das nicht datateam aufgabe sein?

        # speed of light in air = 299702458.0 m/s
        # or, if you ask wolfram  299709000 m/2
        # mein tafelwerk sagt     299711000 m/2
        #magicalconversionnumber = (128*499.2*10**6)
        #ToF_s = (RX - TX) * 15.65 picoseconds  / 1000000000000
        magicalconversionnumber = 15.65 / 1000000000000
        ID=0
        TOF=0
        
        for packet in packets:
            ID = packet.sendid
            TOF = packet.rxtime - packet.txtime
            TOF = TOF / magicalconversionnumber
            times.append({"id":ID,'tof':TOF})

        # TODO: Zeitstempel fehlt noch, sollte am Anfang der Funktion genommen werden (Simon)

        #funfact; python already produces nearly valid json if you repr a dict, just needs to change ' to "
            #i should very much still convert to json
        ret = {"timestamp":timestamp,"mcus":times}
        retstr = json.dumps(ret,indent=4)

        return retstr
            



if __name__ == "__main__":
    
    reader = StampPacketReader(Fake=True)
    print(reader.reveive_json())
    while False:
        
        packets = reader.build_fake_pakets()
        num=0
        for packet in packets:
            num+=1
            print(f"packet {num} :: ID1 = {packet.sendid}, ID2 = {packet.recvid}, rx = {hex(packet.rxtime)}, tx = {hex(packet.txtime)}")

