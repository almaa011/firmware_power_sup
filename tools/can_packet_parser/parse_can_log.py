
import argparse
import codecs
import csv
import datetime
import time
from typing import Dict, List, Tuple

from scrappy import yamls_to_packets


PacketList = List[Tuple[float, int, bytearray]]


def load_data_from_ptelem_log(path: str) -> PacketList:
    """
    Load data from ptelem log.

    :param path: Path to the ptelem log file.

    :return: List of tuples containing packet information.
    """
    packets: PacketList = []

    with open(path) as f:
        lines = f.readlines()

    for line in lines:
        try:
            line = line.split(' ')
            timestamp = float(line[0])
            # CAN ID is the first 3 digits, convert from string
            can_id = int(line[1][:3], 16)
            data = bytearray.fromhex(line[1][3:])
            packets.append((timestamp, can_id, data))
        except KeyboardInterrupt:
            raise
        except Exception as e:
            print(f'{hex(can_id)}: {e}')

    return packets


def load_data_from_telem_sd_log(path: str, ids: List[int]=None) -> PacketList:
    """
    Load data from SD card log.

    :param path: Path to the SD card log file.
    :param ids: List of IDs to include in the outputs. None will include all packets in the output.

    :return: List of tuples containing packet information.
    """
    packets: PacketList = []

    with codecs.open(path) as file:
        # This .replace statement is to remove null charectors that are present in the SD card logs
        lines = csv.reader((x.replace('\0', '') for x in file), delimiter=',')
        for line in lines:
            try:
                can_id = int(line[7], 16)

                # Don't append to the list if this id isn't in the list of IDs we want
                # This helps to reduce the memory used later on.
                if (ids is not None) and (can_id not in ids):
                    continue

                data = bytearray.fromhex(line[8])

                # Convert to a datetime object in order to evetually get unix.
                year = int(line[0])
                month = int(line[1])
                day = int(line[2])
                hour = int(line[3])
                minute = int(line[4])
                second = int(line[5])
                micro_second = int(line[6])*1000
                if micro_second == 1000000:
                    micro_second -= 1

                date_time = datetime.datetime(year, month, day, hour, minute, second, micro_second)
                # the timetuple doesn't deal with microseconds so we have to add it back
                timestamp = time.mktime(date_time.timetuple()) + (date_time.microsecond / 1.0e6)

                packets.append((timestamp, can_id, data))
            except KeyboardInterrupt:
                raise
            except Exception as e:
                print(f'{hex(can_id)}: {e}')
                print(line)

        return packets


def load_data_from_candump(path: str) -> PacketList:
    """
    Load data from a candump.

    :param path: Path to the candump file.

    :return: List of tuples containing packet information.
    """
    packets: PacketList = []

    with open(path) as file:
        for line in file.readlines():
            try:
                line = line.split(' ')
                # the timestamp is the first element, but is covered in paranthesis so discard them
                timestamp = float(line[0][1:-1])
                # the second element is the can interace, this isn't needed so don't use it
                # CAN ID is the first 3 digits (before the '#'), convert from string
                line = line[2].split('#')
                can_id = int(line[0][:3], 16)
                data = bytearray.fromhex(line[1])
                packets.append((timestamp, can_id, data))
            except KeyboardInterrupt:
                # Allow the user to exit while the file is being parsed
                raise
            except Exception as e:
                print(f'{hex(can_id)}: {e}')

    return packets


def parse_ptelem_log(packet_definitions: Dict, packets: PacketList, translate: bool = True) -> Dict:
    """
    Parse packets using the provided packet definitions.

    :param packet_definitions: Dictionary containing packet definitions used for parsing data.
    :param packets: List of CAN packets to parse.
    :param translate: Decode the CAN packets if True.

    :return: Dictionary containing parsed data.
    """
    # read the log file into memory
    log = {}
    unknown_ids = set()
    for packet in packets:
        try:
            if translate:
                timestamp = packet[0]
                can_id = packet[1]
                data = packet[2]
                can_packet_t = packet_definitions.get(can_id, None)

                if can_packet_t is not None:
                    parsed_packet = can_packet_t.parse_packet_to_json(can_id, data)
                else:
                    unknown_ids.add(can_id)

            for board in parsed_packet:
                if board not in log:
                    log[board] = {}

                for message in parsed_packet[board]:
                    if message not in log[board]:
                        log[board][message] = {}

                    for idx in parsed_packet[board][message]:
                        if idx not in log[board][message]:
                            log[board][message][idx] = {}

                        for field,val in parsed_packet[board][message][idx].items():
                            if type(val) == dict:
                                if field not in log[board][message][idx]:
                                    log[board][message][idx][field] = {}
                                for subfield, subval in val.items():
                                    if subfield not in log[board][message][idx][field]:
                                        log[board][message][idx][field][subfield] = []
                                    log[board][message][idx][field][subfield] = subval
                            else:
                                if field not in log[board][message][idx]:
                                    log[board][message][idx][field] = []
                                log[board][message][idx][field].append((timestamp, val))
        except KeyboardInterrupt:
            raise
        except Exception as e:
            print(f'{hex(can_id)}: {e}')

    print('unknown packets:', [hex(x) for x in unknown_ids])
    return log

if __name__ == "__main__":
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description="Parses different CAN logs files"
    )
    parser.add_argument(
        "--yamls",
        "-y",
        help="path to CAN yamls for all the boards",
        required=True,
        type=str,
    )
    parser.add_argument(
        "--log_path",
        "-l",
        help="CAN log file to parse",
        required=True,
        type=str,
    )
    parser.add_argument(
        "--sd",
        "-s",
        action='store_true',
        help="Specifies the log file to parse is SD card formated from telem",
        required=False,
    )
    parser.add_argument(
        "--ptelem",
        "-p",
        action='store_true',
        help="Specifies the log file to parse is a ptelem log",
        required=False,
    )
    parser.add_argument(
        "--dump",
        "-d",
        action='store_true',
        help="Specifies the log file to parse is candump file",
        required=False,
    )

    # get the arguments
    args = parser.parse_args()

    # Make sure that some kind of format is specified
    assert getattr(args, "sd", False) \
        or getattr(args, "ptelem", False) \
        or getattr(args, "dump", False)

    #TODO: Prevent against multiple formats selected

    # Get the path to log and yamls
    packet_definitions = yamls_to_packets(args.yamls)
    path = args.log_path

    if args.sd:
        packets = load_data_from_telem_sd_log(path)
        log = parse_ptelem_log(packet_definitions, packets)
    elif args.ptelem:
        packets = load_data_from_ptelem_log(path)
        log = parse_ptelem_log(packet_definitions, packets)
    elif args.dump:
        packets = load_data_from_candump(path)
        log = parse_ptelem_log(packet_definitions, packets)
    else:
        print("ERROR, invalid format flag")
