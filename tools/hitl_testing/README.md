# HITL Testing #

## Important ##
* Using a virtual environment for scrappy and HITL stuff is a good idea. It can save you headaches down the line.
* `python-can` is used to communicate to the CAN bus. This uses linux's SocketCAN, so running simulations and tests will only be supported on linux.


## Generating Python Code ##
Run `hitl_msg_gen.py path_to_firmware_repo` in `src/cangen`


## Getting Started with Virtual CAN ##
* Use linux
* This won't be used for real testing but is helpful for development

See https://elinux.org/Bringing_CAN_interface_up for more details

`modprobe vcan`

`sudo ip link add dev vcan0 type vcan`

`sudo ip link set up vcan0`


## Getting Started with CAN ##

`sudo ip link set can0 type can bitrate 125000` (In the future our bitrate may change.)

`sudo ip link set up can0`


## Directory Structure ##

```
hitl_testing/
  |-- boards/
  |-- messages/
  |-- tests/
  |-- requirements.txt
```

`boards/` contains classes that implement the simulation of individual boards in the car.

`messages/` contains classes that provide the CAN message data structures and pack/unpack the data.

`tests/` contains pytest tests that we want to add into the repo for future use.

`requirements.txt` contains the python modules that must be installed in order to use this.


## Sending Messages ##

Here's an example of sending vision commands to lights.

`import can` (Install with `pip install python-can`)

`from messages import vision` (Here `vision` will have the messages defined in `car_control.yaml`, import the modules that you will need)

`bustype = 'socketcan'`

`channel = 'vcan0'` (change `vcan0` to `can0` if you are trying to send messages on the actual bus)

`bus = can.Bus(channel=channel, interface=bustype)`

`brake_light_cmd = car_control.VisionBrakeLightsCommand()`

`brake_light_cmd.brake_lights = 1`

`bus.send(brake_light_cmd.msg)`
