import threading
from typing import Callable

import digi.xbee  # install: pip install digi-xbee
from digi.xbee.models.options import TransmitOptions
from digi.xbee.serial import FlowControl
import digi.xbee.devices as devices

from can_packet_codec import CANPacketCodec
from packet import Packet


class interface_digi_xbee(threading.Thread):
    """This class creates a source of CAN packets for pTelem from a Digi XBee module."""

    def __init__(
        self, port: str, packet_handler: Callable, codec: CANPacketCodec
    ):
        threading.Thread.__init__(self)

        self.xbee = devices.XBeeDevice(
            port, baud_rate=115200, flow_control=FlowControl.HARDWARE_RTS_CTS
        )

        self.transmit_options: list[TransmitOptions] = [
            TransmitOptions.POINT_MULTIPOINT_MODE,
        ]
        """The transmit options for the XBee.
        
        The most relevant options are the DIGIMESH_MODE and POINT_MULTIPOINT_MODE.
        The DigiMesh option will transmit to all devices and route packets between
        multiple XBee devices if the receiving devices are out of range, but at a slower speed.

        The Point to Multipoint option will transmit at a faster speed but only to the devices
        within direct radio range.
        """

        self.packet_handler = packet_handler
        self.codec = codec

    def data_received_callback(
        self, message: digi.xbee.models.message.XBeeMessage
    ) -> None:
        """Process a data received callback from the XBee.

        This method will process the data from a packet and send it to the packet handler.
        """
        # Decode the message using the supplied codec.
        packet = self.codec.decode(message.data)

        # Notify other software components of the new packet.
        self.packet_handler(packet)

    def run(self):
        """Start receiving packets from the XBee."""

        self.xbee.open()

        # Add a callback that is called when the XBee receives data.
        self.xbee.add_data_received_callback(self.data_received_callback)

    def stop(self):
        """Stop receiving packets from the XBee."""

        # Remove the callback that is called when the XBee receives data.
        self.xbee.del_data_received_callback(self.data_received_callback)

    def send(self, packet: Packet):
        """Send a packet to the car."""

        # Encode the message to bytes using the supplied codec.
        raw_data = self.codec.encode(packet)

        # Combine the options into a single number for the XBee broadcast.
        raw_transmit_options = 0
        for option in self.transmit_options:
            raw_transmit_options = raw_transmit_options | option.value

        # Send the packet.
        self.xbee.send_data_broadcast(raw_data, raw_transmit_options)
