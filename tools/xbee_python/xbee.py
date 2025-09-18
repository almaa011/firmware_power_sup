import digi.xbee

import digi.xbee.devices as devices
from digi.xbee.models.options import TransmitOptions
from digi.xbee.serial import FlowControl
import time
import threading


class XBee:
    def __init__(
        self,
        port: str,
        baudrate: int = 115200,
        flow: FlowControl = FlowControl.HARDWARE_RTS_CTS,
    ) -> None:

        self.device = devices.XBeeDevice(port, baud_rate=baudrate, flow_control=flow)

        self.device.open()

        self.device.add_data_received_callback(lambda m: self.data_received_callback(m))

        self.lock = threading.Lock()
        self.packets_received: int = 0

    def get_signal_strength(self) -> int:
        """Get the signal strength of the last received packet from the module.

        :return: The signal strength in dBm of the last received packet.
        """
        signal_strength: bytearray = self.device.get_parameter("DB")
        # Value is reported in negative dBm so add a negative sign to the value
        return -signal_strength[0]

    def data_received_callback(
        self, message: digi.xbee.models.message.XBeeMessage
    ) -> None:
        # print(f"Message from: {message.remote_device.get_64bit_addr()}")
        # print(f"{message.data.hex()}")
        # print(f"{message.data.hex()},{self.get_signal_strength()}")
        self.lock.acquire()
        self.packets_received += 1
        self.lock.release()

    def send_packet(self, data: bytes) -> None:
        self.device.send_data_broadcast(
            data,
            transmit_options=TransmitOptions.POINT_MULTIPOINT_MODE.value,
        )
