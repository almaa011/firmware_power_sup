
import sys
import time

import can

from boards import vision

bustype = 'socketcan'
channel = 'vcan0' if len(sys.argv) < 2 else sys.argv[1]

bus = can.Bus(channel=channel, interface=bustype)


vision_board = vision.Vision(bus)
...  # add more boards

vision_board.start()
...  # start more boards


# loop forever
# to control what boards are doing, run python -i simulate_bus.py and press ctrl+c to exit the loop, then change values within boards
while True:
    time.sleep(1)
