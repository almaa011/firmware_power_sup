# CAN Packet Parser #
This python parser can take in CAN packets and output the data names and values.

## Scrappy Usage ##
This can be used with a CAN interface using `socketcan`. It is additionally logged to the file specified with `-o`.

Example run:
`python3 scrappy.py -y ../../src/cangen/packets -o ./test.txt -i can0`
Use `python3 scrappy.py --help` for help of course.


## Xbee Scripts Usage ##

`python xbeefind.py`

Finds available USB devices. Usually this is just the xbee unless you have other stuff plugged in. Use one of these as `com_port` below.

`python xbeedump.py [com_port]`

Dump raw xbee packets to the console. Behaves similarly to candump.

`python xbeesend.py [com_port] [can_packet]`

Warning: This script may drop packets due to the firmware implementation, so this interaction could be improved. Packets may also fail to send if the xbee is busy receiving. Use with caution.

Send raw can packets through the xbee. Behaves similarly to candump. `can_packet` should be in the format of `can_id#packet_data` where `can_id` is the hexadecimal ID of the packet you are sending and `packet_data` is the hexadecimal representation of the data you are sending. 

`python xbeecan.py [com_port] [can_bus]`

This program will transmit packets it receives from the xbee to the CAN bus and will transmit packets it receives from the CAN bus to the xbee. `can_bus` is the name of the SocketCAN CAN bus being used. Linux only.

`python server.py [com_port] [yaml_directory_path] [log_file]`

Run this program to log and view telemetry data. Access the web page at http://localhost:8000/ after starting the program. `yaml_directory_path` is the path to the directory containing the yaml packet definitions. `log_file` is the file to log to.

### Parsing Telemetry Logs ###

Parsing telemetry logs from `server.py` is easy! There are now two ways to do this.

1) Running python on the command line


Run: `python -i parse_can_log.py -y path/to/yaml/files -l path/to/log_file.txt`

Note: Look in `parse_can_log.py` for other required arguments such as specifing how `log_file.txt` was generated. For example `-d` or `--dump` would sigify that `log_file.txt` was generated from a candump.

Once that completes you will be in the interactive python shell.

You will now have a variable `log` that is a dictionary containing nested dictionaries and lists.

2) Import the module.

From whatever code you want to access logging data from include `from parse_can_log import parse_can_log`.

You now have access to the `parse_can_log()` function that takes in two arguments: the parsed packets (from scrappy) and a path to the telemetry log from server.py.

#### Doing Useful Things with the Data ####

To see what data you have available run `log.keys()`. Currently you should see `dict_keys(['tritium', 'wavesculptor', 'charger', 'bms'])` but this may change in the future.

To see what packets are available for `bms` run `log['bms'].keys()`. Your output should look something like `dict_keys(['bms_set_min_fan_speed', 'bms_module', 'bms_soc', 'bms_module_min_max', 'bms_capacity', 'bms_measurment', 'battery_status'])`

Accessing `log['bms']['bms_module_min_max'][0]['module_max_temp']` will give you a list of tuples containing a pair of the timestamp and module maximum temperature (`List[Tuple[float, float]]`).

Here's an example of plotting some data (assumes you've followed the above steps and have `log`):

```
import datetime
import matplotlib.pyplot as plt

# access odometer data from the motor controller
odometer = log['wavesculptor']['wsl_odometer_bus_amphours_measurement'][0]['odometer']

# plot the odometer reading against time (convert the timestamps from unix to a date)
plt.plot([datetime.datetime.fromtimestamp(x) for x,y in odometer], [y for x,y in odometer])

# show the plot
plt.show()
```
