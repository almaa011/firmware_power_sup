from xbee import XBee
from digi.xbee.serial import FlowControl
import time

start = time.time()
modem = XBee("/dev/ttyUSB0", 115200)

print(modem.device.get_hardware_version())
print(modem.device.get_64bit_addr())

while True:
    modem.send_packet(bytes([15, 16, 1, 2, 3, 4, 5, 6, 7, 8]))
    # time.sleep(0.250)
    input("Ready to send packet...")

input("Press Return to Quit...")

end = time.time()
modem.device.close()
print(
    f"{modem.packets_received} packets in {end - start} seconds: {modem.packets_received / (end - start)} packets/second"
)
