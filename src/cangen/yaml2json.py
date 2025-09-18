#!/usr/bin/python

import yaml
import json
import canParser

packet_enum_prefix = "CAN_PACKET_"
packet_enum_name = "CanPacketId"

yaml_file = canParser.loadAsList(expandRepeatPackets=True)

file_name = "packets.json"
with open(file_name, "w") as f:
    f.write(json.dumps(yaml_file, indent=4))
