
import sys

import can  # pip install python-can

from interface_digi_xbee import interface_digi_xbee
from can_packet_codec import XBeeCANPacketCodec
from packet import Packet


com_port = sys.argv[1]

bustype = 'socketcan'
channel = sys.argv[2]

bus = can.Bus(channel=channel, interface=bustype)


def handler(packet):
    print(f'xbee  {packet.id:02x}   [{len(packet.data)}]  {" ".join([f"{x:02x}" for x in packet.data])}')
    msg = can.Message(arbitration_id=packet.id, data=packet.data, is_extended_id=False)
    bus.send(msg)


interface = interface_digi_xbee(com_port, handler, codec=XBeeCANPacketCodec())

# start the thread
interface.start()

# wait for the interface to open
while not interface.xbee.is_open():
    pass

try:
    while True:
        msg = bus.recv()
        p = Packet()
        p.id = msg.arbitration_id
        p.data = [b for b in msg.data]
        interface.send(p)
        print(f'{channel}  {p.id:02x}   [{len(p.data)}]  {" ".join([f"{x:02x}" for x in p.data])}')
except KeyboardInterrupt:
    interface.stop()
    sys.exit()
