import sys
import os
from typing import List, Dict, Optional
import can_parser
from subprocess import call
from can_parser import (
    CANMessageFieldDef,
    CANMessageBitfieldDef,
    CANBusDef,
    CANBoardDef,
    CANMessageDef,
)
from jinja2 import Environment, FileSystemLoader, select_autoescape
from pathlib import Path

Packets = Dict[str, Dict[int, CANMessageDef]]


def generate_packets(env: Environment, packets: Packets):
    # Multiple packets with the same name will cause a problem
    ps = []
    for bus, vals in packets.items():
        ps += list(vals.values())
    packet = ps

    # TODO: check for duplicate names

    print("\nFound " + str(len(packets)) + " packets.\n")

    packet = sorted(packet, key=lambda p: p.id)

    tmpl = env.get_template("skylab2_packets.h.j2")
    return tmpl.render(packets=packet)


def generate_busses(env: Environment, busses):
    tmpl = env.get_template("skylab2_busses.h.j2")
    return tmpl.render(busses=busses)


def packet_def_from_name(name: str, packets: Packets) -> Optional[CANMessageDef]:
    """
    packet_def_from_name iterates through the packets and returns the packet definition if it matches the packet name
    :name: a string of the packet name
    :packets: the packet
    :return: the bus packet or None
    """
    for bus, bus_packets in packets.items():
        for bus_packet in bus_packets.values():
            if name == bus_packet.name:
                return bus_packet
    return None


def bus_from_packet_name(name: str, packets: Packets) -> Optional[str]:
    """
    bus_from_packet_names takes in a string and a packets object and finds the bus name
    :name: name of packet
    :packets: the packers
    :return: the bus name
    """
    for bus, bus_packets in packets.items():
        for bus_packet in bus_packets.values():
            if name == bus_packet.name:
                return bus
    return None


def packet_name_is_repeated(name: str, packets: Packets) -> Optional[bool]:
    """
    packet_name_is_repeated takes in a packet name and packet and returns a bool
    :name: the packet name
    :packets: the packets
    :return: true if the bus packet is repeated and false if bus packet is not repeated
    """
    for bus_packets in packets.values():
        for bus_packet in bus_packets.values():
            if name == bus_packet.name:
                return bus_packet.repeat > 1
    return None


def generate_types(env: Environment, types):
    for type_name, type_def in types.items():
        base_type = type_def.base_type
        if base_type not in {
            "int8_t",
            "uint8_t",
            "int16_t",
            "uint16_t",
            "int32_t",
            "uint32_t",
        }:
            raise RuntimeError(f"{base_type} is not a valid base type.")
    tmpl = env.get_template("skylab2_types.h.j2")
    return tmpl.render(types=types)

def generate_boards(env: Environment, boards, packets, busses, types):
    h_tmpl = env.get_template("skylab2_boards.h.j2")
    cc_tmpl = env.get_template("skylab2_boards.cc.j2")
    env_args = {"boards": boards,"packets": packets,"busses": busses,"types": types}
    return h_tmpl.render(**env_args), cc_tmpl.render(**env_args)


# ---------------Custom Filters ------------------
def bus_sort(busses: list[str], all_busses: List[CANBusDef]) -> List[str]:
    """This filter sorts a list of bus names by their ordering in the busses yaml file. 
    This ensures stable ordering when finding busses that belong to a board (main first!)"""
    all_bus_names = [x.name for x in all_busses]
    # this next one might be a little funky
    # basically we can replace the key that is used to sort with our own function
    # we return the position (index) of the bus in the main busses definition.
    return sorted(busses, key=lambda bus: all_bus_names.index(bus))

def gen_packing_code(packet: CANMessageDef, types) -> str:
    """This filter takes a packet and all the data types and 
    generates a block of code to pack the packet into a uint8 array"""
    packing_code = ""
    dlc = 0
    for field in packet.data:
        if type(field) == CANMessageBitfieldDef: # if it's a bitfield
            bits = []
            bitcount = 0
            for subfield in field.bits:
                mask = hex(2**subfield.bits-1)
                bits.append(f'((msg.{field.name}.{subfield.name} & {mask})<<{str(bitcount)})')
                bitcount += subfield.bits
                if bitcount > 8:
                    raise RuntimeError(f"Too many fields in {field.name}.{subfield.name}")
            packing_code += '\tdata['+str(dlc)+'] =   ' + ' \n\t\t\t\t| '.join(bits) + ';\n'
            dlc += 1
        else:
            if field.data_type == "float":
                for i in range(4):
                    packing_code += '\tdata['+str(dlc)+'] = ((can_float_union_t){.f=msg.'+field.name+'}).b['+str(i)+'];\n'
                    dlc += 1
            elif field.data_type in can_parser.c_lengths: # standard values
                for i in range(can_parser.c_lengths[field.data_type]):
                    packing_code += '\tdata['+str(dlc)+'] = msg.' + field.name+'>>'+str(i*8)+';\n'       
                    dlc += 1
            else: # custom enum case
                base_type = types[field.data_type].base_type
                type_length = can_parser.c_lengths[base_type]
                for i in range(type_length):
                    packing_code += '\tdata['+str(dlc)+'] = static_cast<' + base_type + '>(msg.' + field.name + ')>>'+str(i*8)+';\n'       
                    dlc += 1
    return packing_code

def gen_unpacking_code(packet: CANMessageDef, types) -> str:
    """This filter takes a packet and generates C code to unpack an array into the struct."""
    unpacking_code = ""
    dlc = 0 
    for field in packet.data:
        if type(field) == CANMessageBitfieldDef:
            bitcount = 0
            for idx, subfield in enumerate(field.bits):
                # TODO: doesn't use the mask parameter.
                unpacking_code += f"\t\tmsg.{field.name}.{subfield.name} = (data[{dlc}]>>{bitcount}) & 0x1;\n"
                bitcount += 1
            dlc += 1
        elif field.data_type == "float":
            unpacking_code += f"\t\tcan_float_union_t {field.name} = {{0}};\n"
            for idx in range(can_parser.c_lengths[field.data_type]):
                unpacking_code += f"{field.name}.b[{idx}] = data[{dlc + idx}];\n"
            unpacking_code += f"msg.{field.name} = {field.name}.f;\n"
            dlc += can_parser.c_lengths[field.data_type]
        elif field.data_type in can_parser.c_lengths:
            blist = []
            for idx in range(can_parser.c_lengths[field.data_type]):
                blist.append(f"(data[{dlc + idx}]<< {idx * 8})")
            unpacking_code += f"msg.{field.name} = ({field.data_type})({'|'.join(blist)});\n"
            dlc += can_parser.c_lengths[field.data_type]
        else:
            # attempt enum
            blist = []
            type_len = can_parser.c_lengths[types[field.data_type].base_type]
            for idx in range(type_len):
                blist.append(f"(data[{dlc + idx}]<< {idx * 8})")
            unpacking_code += f"msg.{field.name} = ({field.data_type})({'|'.join(blist)});\n"
            dlc += type_len

    return unpacking_code

def load_environment(path: str):
    e = Environment(loader=FileSystemLoader(path), autoescape=select_autoescape())
    e.filters['name2packet'] = packet_def_from_name
    e.filters['gen_packing_code'] = gen_packing_code
    e.filters['gen_unpacking_code'] = gen_unpacking_code
    e.filters['bus_sort'] = bus_sort
    return e
    
def main():
    import argparse

    parser = argparse.ArgumentParser(
        prog="skylab2",
        description="Generates CAN bus library code",
    )

    parser.add_argument(
        "source_path",
        type=Path,
        metavar="<path>",
        help="The firmware repository source path to insert generated code.",
    )

    parser.add_argument(
        "--templates_path",
        "-t",
        type=Path,
        help="The path that the code templates are stored in",
        default="./templates",
    )

    parser.add_argument(
        "--packets_path",
        "-p",
        type=Path,
        help="The path containing YAML packet definitions.",
        default="./packets"
    )
    parser.add_argument("--verbose", "-v", action="count", default=0)

    args = parser.parse_args()
    print(args)
    # The jinja environment contains a loader for files and autoescape sequences
    env = load_environment(args.templates_path)

    # get all the packet info
    # TODO: support loading other paths.
    all_packets, all_types, all_busses, all_boards = can_parser.load_all(args.packets_path)

    # construct our output files.
    os.makedirs(args.source_path / "src", exist_ok=True)
    os.makedirs(args.source_path / "inc", exist_ok=True)
    skylab2_packets_h_path = args.source_path / "inc" / "skylab2_packets.h"
    skylab2_busses_h_path = args.source_path / "inc" / "skylab2_busses.h"
    skylab2_types_h_path = args.source_path / "inc" / "skylab2_types.h"
    skylab2_boards_h_path = args.source_path / "inc" / "skylab2_boards.h"
    skylab2_boards_cc_path = args.source_path / "src" / "skylab2_boards.cc"

    h = generate_packets(env, all_packets)
    with open(skylab2_packets_h_path, "w") as f:
        f.write(h)

    h = generate_busses(env, all_busses)
    with open(skylab2_busses_h_path, "w") as f:
        f.write(h)

    h = generate_types(env, all_types)
    with open(skylab2_types_h_path, "w") as f:
        f.write(h)

    h, cc = generate_boards(env, all_boards, all_packets, all_busses, all_types)
    with open(skylab2_boards_h_path, "w") as f:
        f.write(h)
    with open(skylab2_boards_cc_path, "w") as f:
        f.write(cc)

    print("\nSuccess!")
    # sys.exit(retcode)


if __name__ == "__main__":
    main()
