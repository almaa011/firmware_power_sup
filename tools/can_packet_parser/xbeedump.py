
import sys
import time

from interface_digi_xbee import interface_digi_xbee
from can_packet_codec import XBeeCANPacketCodec


def handler(packet):
    print(f'xbee  {packet.id:02x}   [{len(packet.data)}]  {" ".join([f"{x:02x}" for x in packet.data])}')


com_port = sys.argv[1]

interface = interface_digi_xbee(com_port, handler, codec=XBeeCANPacketCodec())

# start the thread
interface.start()

try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    interface.stop()
    sys.exit()
