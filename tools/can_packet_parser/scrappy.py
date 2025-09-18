import yaml
import argparse
import os
import sys
import struct
from datetime import datetime

from pprint import pprint

from typing import Dict, Iterator, List, Union, Sequence

if sys.platform == "linux":
    from socketcan.socketcan import CanFrame
    from socketcan import CanRawSocket
else:
    print("Warning: You're not using linux. Functionality will be limited.")


class Converter:
    """Convert C bytes into python types."""

    enum_types = {}

    # Mapping C-types to python struct formatting.
    STDINT_TO_STRUCT: Dict[str, str] = {
        "uint8_t": "B",
        "int8_t": "b",
        "uint16_t": "H",
        "int16_t": "h",
        "uint32_t": "I",
        "int32_t": "i",
        "uint64_t": "Q",
        "int64_t": "q",
        "float": "f",
        "bitfield": "B",
    }

    # Size of each C-type in bytes.
    STDINT_TO_STRUCT_SIZE: Dict[str, int] = {
        "uint8_t": 1,
        "int8_t": 1,
        "uint16_t": 2,
        "int16_t": 2,
        "uint32_t": 4,
        "int32_t": 4,
        "uint64_t": 8,
        "int64_t": 8,
        "float": 4,
        "bitfield": 1,
    }

    @staticmethod
    def _int_to_bitfield(value: int) -> Sequence[bool]:
        # Convert the int into binary
        binary_string: str = f"{value:08b}"
        # Create a list of bools from the string
        binary_list: list[bool] = [c == "1" for c in binary_string]
        # Reverse the list so that bit 0 is at index 0
        binary_list.reverse()
        return binary_list

    @staticmethod
    def bytes_to_python(
        types: Sequence[str],
        conversions: Sequence[Union[float, None]],
        data: bytes,
        little_endian: bool = True,
    ) -> List[Union[float, int, List[bool]]]:
        """Using a sequence of types, conversion factors, and the byte ordering,]
        convert data into python types.

        :param types: The types of the fields in data
        :param conversions: The conversion factors, use None as no conversion.
        :param data: The bytes of data to convert
        :param little_endian: The byte ordering of the data
        """
        # The format string for struct
        struct_format: str = ""

        # Add the marker for little vs big endian for struct parsing
        if little_endian:
            struct_format = f"<{struct_format}"
        else:
            struct_format = f">{struct_format}"

        packet_types_size = 0
        # Convert the given types into struct types for later parsing
        for data_type in types:
            if data_type in Converter.STDINT_TO_STRUCT:
                struct_format = f"{struct_format}{Converter.STDINT_TO_STRUCT[data_type]}"
                packet_types_size += Converter.STDINT_TO_STRUCT_SIZE[data_type]
            else:
                base_type = Converter.enum_types[data_type]['base_type']
                struct_format = f"{struct_format}{Converter.STDINT_TO_STRUCT[base_type]}"
                packet_types_size += Converter.STDINT_TO_STRUCT_SIZE[base_type]

        for i in range(len(data) - packet_types_size):
            struct_format = f"{struct_format}{Converter.STDINT_TO_STRUCT['uint8_t']}"

        # Have struct parse the bytes into python
        values: list = list(struct.unpack(struct_format, data))

        # Convert bitfields to bool lists and apply conversion factors
        for i in range(len(types)):
            if types[i] == "bitfield":
                values[i] = Converter._int_to_bitfield(values[i])
            elif conversions[i] is not None:
                values[i] = conversions[i] * values[i]
            elif types[i] not in Converter.STDINT_TO_STRUCT:
                for k,v in Converter.enum_types[data_type]['values'].items():
                    if values[i] == v:
                        values[i] = k
                        break

        return values


class packet_t:
    """Class for representing a CAN packet structure."""

    def __init__(self, yaml_dump: Dict, yaml_name) -> None:
        """Intialize the CAN packet structure via the YAML.

        Args:
            yaml_dump: Output from YAML file for this packet structure.
            yaml_name: Name of the YAML, giving the board name.
        """
        # Name of the board this packet belongs to should be the name of the yaml.
        self.board = yaml_name

        # Get the ids for this packet type based on the repeats and offset.
        # Note: repeats isn't acutally number of repeats, but total number
        # CAN packets that are of this packet structure.
        start_id = yaml_dump.get("id", None)
        assert start_id is not None
        self.ids: List[int] = []
        self.repeat: int = yaml_dump.get("repeat", 1)
        self.offset: int = yaml_dump.get("offset", 1)
        if not isinstance(start_id, list):
            self.ids = [start_id + i * self.offset for i in range(0, self.repeat)]
        else:  # Apparently the ID can be a list of ids...
            self.ids = start_id

        # As there can be a list of ids, there can be a list of names... :[
        self.name = yaml_dump["name"]
        if not isinstance(self.name, list):
            self.name = [self.name] * self.repeat

        self.description: str = yaml_dump.get(
            "descripton",
            "Sadness, no docs for this one. Get off your butt and add some.",
        )
        self.little_endian: bool = yaml_dump.get("endian", True)

        # Store the data's types for parsing an actual packet when it arrives.
        self.data: List[Dict[str, Union[str, Dict[str, str]]]] = yaml_dump["data"]

    def parse_packet(
        self, can_id: int, data: bytes
    ) -> Dict[str, Union[float, int, List[bool]]]:
        """Parse a CAN packet using this CAN packet structure.

        Args:
            can_id: CAN packet's id.
            data: CAN packet's data.

        Returns:
            List of CAN packet data as python types.
        """
        # Get the index of this CAN packet in the series of repeats of this CAN structure.
        idx: str = self.ids.index(can_id)

        # Get the types and conversions from the packet structure.
        types = [element["type"] for element in self.data]
        conversions = [element.get("conversion", None) for element in self.data]

        # Get the data converted to python values.
        values: List[Union[float, int, List[bool]]] = Converter.bytes_to_python(
            types, conversions, data, self.little_endian
        )

        # Get the output data.
        # Additionally construct the name of the data using all of the
        # parent directories.
        output_data = {}
        prefix: str = ".".join([self.board, self.name[idx], str(idx)])
        for element_idx, element in enumerate(self.data):
            element_name = ".".join([prefix, element["name"]])

            # If there is a bitfield, make each bitfield a different output
            # value labeled with its name.
            if element["type"] == "bitfield":
                for bit_idx, bit in enumerate(element["bits"]):
                    bit_name = bit["name"]
                    element_bit_name = ".".join([element_name, bit_name])
                    output_data[element_bit_name] = values[element_idx][bit_idx]
            else:
                output_data[element_name] = values[element_idx]
        return output_data

    def parse_packet_to_json(
        self, can_id: int, data: bytes
    ) -> Dict[str, Union[float, int, List[bool]]]:
        """Parse a CAN packet using this CAN packet structure.

        Args:
            can_id: CAN packet's id.
            data: CAN packet's data.

        Returns:
            Dictionary of parsed CAN packet data.
        """
        # Get the index of this CAN packet in the series of repeats of this CAN structure.
        idx: str = self.ids.index(can_id)

        # Get the types and conversions from the packet structure.
        types = [element["type"] for element in self.data]
        conversions = [element.get("conversion", None) for element in self.data]

        # Get the data converted to python values.
        values: List[Union[float, int, List[bool]]] = Converter.bytes_to_python(
            types, conversions, data, self.little_endian
        )

        # Get the output data.
        # Additionally construct the name of the data using all of the
        # parent directories.
        output_data = {self.board: {self.name[idx]: {idx: {}}}}
        for element_idx, element in enumerate(self.data):
            element_name = element["name"]

            # If there is a bitfield, make each bitfield a different output
            # value labeled with its name.
            if element["type"] == "bitfield":
                d = dict()
                for bit_idx, bit in enumerate(element["bits"]):
                    bit_name = bit["name"]
                    d[bit_name] = values[element_idx][bit_idx]
                output_data[self.board][self.name[idx]][idx][element_name] = d
            else:
                output_data[self.board][self.name[idx]][idx][element_name] = values[element_idx]
        return output_data


def yamls_to_packets(yaml_dir_path: str) -> Dict[int, packet_t]:
    """Convert CAN packet YAMLs to list of packets.

    Args:
        yaml_dir_path: Path to the CAN packet YAML directory.

    Raises:
        ValueError: If there were no YAMLs at the directory.

    Returns:
        Dict[int, packet_t]: Dict of CAN packet structures.
    """
    packets: Dict[int, packet_t] = {}

    for yaml_file in os.listdir(yaml_dir_path):
        if yaml_file.endswith(".yaml"):
            with open(os.path.join(yaml_dir_path, yaml_file), "r", encoding="utf-8") as stream:
                yaml_dict = yaml.safe_load(stream)
                if "packets" in yaml_dict:
                    for packet_yaml in yaml_dict["packets"]:
                        # Get the board name without extension.
                        yaml_name = yaml_file.split(".")[0]
                        # Get the packet structure and assign all the appropriate ids
                        # to that structure.
                        packet_structure = packet_t(packet_yaml, yaml_name)
                        for can_id in packet_structure.ids:
                            packets[can_id] = packet_structure
                else:
                    print(f"WARNING: {yaml_file} did not have packets.")
                if "types" in yaml_dict:
                    for enum in yaml_dict['types']:
                        Converter.enum_types[enum['name']] = enum

    # If no YAMLs or YAML content, report error.
    if packets == []:
        raise ValueError("YAML directory did not contain any YAMLs")

    return packets


def main():
    # Set up argument parser.
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description="Let's freakin parse some packets."
    )
    parser.add_argument(
        "--yamls",
        "-y",
        help="path to CAN yamls for all the boards",
        required=True,
        type=str,
    )
    parser.add_argument(
        "--interface",
        "-i",
        help="interface to listen for packets on",
        required=False,
        type=str,
    )
    parser.add_argument(
        "--packet_log", "-l", help="CAN packet logs to parse", required=False, type=str
    )
    parser.add_argument(
        "--output",
        "-o",
        help="Path to output parsed data to.",
        required=True,
        type=str,
    )
    args = parser.parse_args()

    # Make sure there is a CAN interface or CAN logs provided to parse.
    assert getattr(args, "interface", False) or getattr(args, "packet_log", False)

    parse_can_packets(
        packets=yamls_to_packets(args.yamls),
        output=args.output,
        interface=getattr(args, "interface", None),
        log=getattr(args, "packet_log", False),
    )


class SVPCanFrame:
    def __init__(self, _can_id: int, _data: bytes, _timestamp: str) -> None:
        self.can_id = _can_id
        self.data = _data
        self.timestamp = _timestamp

    def get_readable_timestamp(self) -> str:
        return str(
            datetime.utcfromtimestamp(float(self.timestamp)).strftime(
                "%Y-%m-%d %H:%M:%S"
            )
        )


class CanDumpParse:
    def __init__(self, dump_path: str):
        self.dump_packets: List[SVPCanFrame] = []
        with open(dump_path, "r") as dump_file:
            for packet_line in dump_file:
                if "#" in packet_line:
                    open_paren: int = packet_line.find("(")
                    closed_paren: int = packet_line.find(")")  # , open_paren)
                    timestamp: str = packet_line[open_paren + 1 : closed_paren]
                    id_separator: int = packet_line.index("#")
                    can_id_str: str = packet_line[id_separator - 3 : id_separator]
                    data_str: str = packet_line[id_separator + 1 :]

                    data = int.from_bytes(bytes.fromhex(data_str), "little").to_bytes(
                        8, "little"
                    )

                    self.dump_packets.append(
                        SVPCanFrame(int(can_id_str, 16), data, timestamp)
                    )

    def iterate_packets(self) -> Iterator[SVPCanFrame]:
        for packet in self.dump_packets:
            yield packet


def parse_can_packets(
    packets: Dict[int, packet_t], output: str, interface: str = None, log: str = None
):
    with open(output, "w+") as output_file:
        if interface is not None:
            # TODO: test whether this socket/python script can keep up with a lot of data.
            socket = CanRawSocket(interface=interface)
            while 1:
                packet: CanFrame = socket.recv()
                can_packet_t: packet_t = packets.get(packet.can_id, None)

                if can_packet_t is not None:
                    parsed_packet = can_packet_t.parse_packet(
                        packet.can_id, packet.data
                    )
                    for name, value in parsed_packet.items():
                        string = f"{name} = {value}"
                        print(string)
                        output_file.write(f"{string}\n")
                else:
                    string = "WARNING UNKNOWN PACKET: {0}  {1:8X}   [{2}]  {3}".format(
                        interface,
                        packet.can_id,
                        len(packet.data),
                        " ".join(["{0:02X}".format(b) for b in packet.data]),
                    )
                    print(string)
                    output_file.write(f"{string}\n")

        if log is not None:
            for can_frame in CanDumpParse(log).iterate_packets():
                can_packet_t: packet_t = packets.get(can_frame.can_id, None)
                if can_packet_t is not None:
                    # output_file.write(f"{can_frame.timestamp}\n")
                    output_file.write(f"{can_frame.get_readable_timestamp()}\n")
                    parsed_packet = can_packet_t.parse_packet(
                        can_frame.can_id, can_frame.data
                    )
                    for name, value in parsed_packet.items():
                        string = f"{name} = {value}"
                        # print(string)
                        output_file.write(f"{string}\n")
                else:
                    string = "WARNING UNKNOWN PACKET: {0}  {1:8X}   [{2}]  {3}".format(
                        interface,
                        can_frame.can_id,
                        len(can_frame.data),
                        " ".join(["{0:02X}".format(b) for b in can_frame.data]),
                    )
                    print(string)
                    output_file.write(f"{string}\n")


if __name__ == "__main__":
    main()
