
import sys

from interface_digi_xbee import interface_digi_xbee
from can_packet_codec import XBeeCANPacketCodec
from packet import Packet


com_port = sys.argv[1]


def handler(_):
    pass


temp = sys.argv[2].split('#')
id = int(temp[0], 16)
data = [int(temp[1][i:i+2], 16) for i in range(0, len(temp[1]), 2)]

p = Packet()
p.id = id
p.data = data


interface = interface_digi_xbee(com_port, handler, codec=XBeeCANPacketCodec())

interface.start()

# wait for the interface to open
while not interface.xbee.is_open():
    pass

interface.send(p)

interface.stop()
