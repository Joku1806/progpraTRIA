import struct
# pyUSB muss installiert sein!
import usb.core
import usb.util
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
    # prüft mit lsusb erst, ob diese IDs übereinstimmen!
    VENDOR_ID = 0x239a
    PRODUCT_ID = 0x800b

    SEND_SIGNAL = 0xff
    PACKET_SIZE = 18

    informant: usb.core.Device
    rendpoint: int
    wendpoint: int

    def __init__(self):
        self.informant = self.find_informant()
        self.detach_informant_kernel_drivers()
        self.informant.set_configuration()

        self.rendpoint = self.find_informant_endpoint(usb.util.ENDPOINT_IN)
        self.wendpoint = self.find_informant_endpoint(usb.util.ENDPOINT_OUT)

    def find_informant(self):
        # id{Vendor, Product} sind KEINE! positionellen Parameter. Es ist
        # wichtig, dass das so stehen bleibt, sonst werden die Argumente
        # falsch interpretiert.
        informant = usb.core.find(
            idVendor=self.VENDOR_ID, idProduct=self.PRODUCT_ID)
        if informant is None:
            raise ValueError(
                'Konnte Informant nicht finden! Ist er per USB angeschlossen und sind VENDOR_ID und PRODUCT_ID richtig gesetzt?')
        return informant

    def detach_informant_kernel_drivers(self):
        for config in self.informant:
            for interface in config:
                if self.informant.is_kernel_driver_active(interface.bInterfaceNumber):
                    try:
                        self.informant.detach_kernel_driver(
                            interface.bInterfaceNumber)
                    except usb.core.USBError as e:
                        raise RuntimeError(
                            f"Konnte Kernel-Driver nicht von Interface({interface.bInterfaceNumber}) lösen: {str(e)}")

    def find_informant_endpoint(self, type: usb.util.ENDPOINT_IN | usb.util.ENDPOINT_OUT) -> int:
        # FIXME: findet immer den letzten Endpoint seiner Art,
        # read/write muss also nicht unbedingt vom gleichen Typ sein.
        # ka ob wir das noch mehr spezifizieren wollen
        endpoint = None
        for interface in self.informant.get_active_configuration():
            endpoint = usb.util.find_descriptor(
                interface,
                custom_match=lambda e:
                usb.util.endpoint_direction(e.bEndpointAddress) == type
            )

        if (endpoint is None):
            raise RuntimeError(
                f"Konnte keinen Endpoint vom Typ {type} finden!")
        return endpoint

    def receive(self) -> Union[StampPacket, None]:
        # TODO: C Implementierung von Empfangen/Senden auf anderer Seite
        try:
            self.informant.write(self.wendpoint, [self.SEND_SIGNAL])
            bytes = self.informant.read(self.rendpoint, self.PACKET_SIZE)
        except usb.core.USBTimeoutError:
            # Für den Fall, dass der Informant noch kein neues
            # Paket bekommen hat.
            return None

        if not len(bytes) == self.PACKET_SIZE:
            raise BufferError(
                f"Habe {self.PACKET_SIZE} Bytes erwartet, aber {len(bytes)} Bytes bekommen:\n{bytes}")

        (sender_devid, receiver_devid, timestamp_start,
         timestamp_stop) = struct.unpack("BBQQ", bytes)
        return StampPacket(sender_devid, receiver_devid, timestamp_start, timestamp_stop)
