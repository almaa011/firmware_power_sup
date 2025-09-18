"""This module is responsible for encoding and decoding CAN packets to and from bytes.
"""

from abc import ABC, abstractmethod
from packet import Packet


class CANPacketCodecError(BaseException):
    """A base exception for when a CAN packet codec encounters an error."""

    pass


class CANPacketEncodeError(CANPacketCodecError):
    """An exception for when a CAN packet codec encounters an error while encoding a
    packet to bytes"""

    pass


class CANPacketDecodeError(CANPacketCodecError):
    """An exception for when a CAN packet codec encounters an error while decoding a
    packet from bytes."""

    pass


class CANPacketCodec(ABC):
    """An abstract base class for standardizing CAN packet encoding and decoding methods."""

    @abstractmethod
    def encode(self, packet: Packet) -> bytes:
        """Encode a Packet into bytes."""
        pass

    @abstractmethod
    def decode(self, data: bytes) -> Packet:
        """Decode a Packet from bytes."""
        pass


class XBeeCANPacketCodec(CANPacketCodec):
    """Codec for encoding and decoding packets for an XBee radio link."""

    def __init__(self):
        super().__init__()

    def encode(self, packet: Packet) -> bytes:
        """Encode a CAN Packet into bytes to send over the XBee."""

        raw_data = bytearray()

        # First two bytes are the ID.
        raw_data.append((packet.id >> 8) & 0xFF)
        raw_data.append(packet.id & 0xFF)

        # Remaining bytes are the packet data.
        for i in packet.data:
            raw_data.append(i)

        # Convert the bytearray into bytes.
        return bytes(raw_data)

    def decode(self, data: bytes) -> Packet:
        """Decode bytes from the XBee into a CAN packet."""

        # Ensure that we got at least an ID in the payload.
        if len(data) < 2:
            raise CANPacketDecodeError(
                "XBee Packet did not contain enough bytes for ID"
            )

        packet = Packet()
        packet.id = data[0] << 8 | data[1]
        packet.data = [x for x in data[2:]]

        return packet
