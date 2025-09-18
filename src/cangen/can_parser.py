from dataclasses import dataclass
import os
import yaml
from pathlib import Path
from typing import List, Dict, Union


@dataclass
class CANBusDef:
    """Class to store CAN bus definitions."""

    name: str
    baud_rate: int
    use_extended_ids: bool


@dataclass
class CANTypeDef:
    name: str
    description: str
    base_type: str
    values: List[Dict[str, int]]


@dataclass
class CANMessageFieldDef:
    name: str
    data_type: str
    unit: str
    conversion: float


@dataclass
class CANMessageSubfieldDef:
    name: str
    bits: int


@dataclass
class CANMessageBitfieldDef:
    name: str
    bits: List[CANMessageSubfieldDef]


@dataclass
class CANMessageDef:
    name: str
    description: str
    id: int
    endian: str
    repeat: int
    offset: int
    bus: str
    frequency: float
    is_extended: bool
    data: List[Union[CANMessageFieldDef, CANMessageBitfieldDef]]


@dataclass
class CANBoardDef:
    name: str
    transmit: List[str]
    receive: List[str]


c_lengths = {
    "uint8_t": 1,
    "int8_t": 1,
    "uint16_t": 2,
    "int16_t": 2,
    "uint32_t": 4,
    "int32_t": 4,
    "uint64_t": 8,
    "int64_t": 8,
    "bitfield": 1,
    "float": 4,
}


# These are the baud rates currently supported by the bxcan driver
valid_baud_rates = [
    100,  # 100000
    125,  # 125000
    250,  # 250000
    500,  # 500000
    1000,  # 10000000
]


def load_all(path: Path):
    all_packets: Dict[int, Dict[int, CANMessageDef]] = dict()
    all_types: Dict[str, CANTypeDef] = dict()
    all_busses: List[CANBusDef] = []
    all_boards = dict()

    print("Found the following files:")

    for file in os.listdir(path):
        if file.endswith(".ignore.yaml") or not file.endswith(".yaml"):
            continue

        print(file)

        with open(path / file, "r", encoding="utf-8") as f:
            yaml_contents = yaml.safe_load(f)

        for raw_message_def in yaml_contents.get("packets", []):
            # names and IDs can be in a list, but must be the same length
            name = raw_message_def["name"]
            if type(name) != list:
                name = [name]
            _id = raw_message_def["id"]
            if type(_id) != list:
                _id = [_id]

            if len(name) != len(_id):
                raise RuntimeError(f"Number of names and ids must be equal!")

            description = raw_message_def.get("description", "")
            if type(description) != str:
                ...

            endian = raw_message_def.get("endian", "little")
            if type(endian) != str:
                ...
                
            is_extended = raw_message_def["is_extended"] if "is_extended" in raw_message_def else False

            repeat = int(raw_message_def["repeat"]) if "repeat" in raw_message_def else None

            offset = int(raw_message_def["offset"]) if "offset" in raw_message_def else None

            bus = raw_message_def["bus"] if "bus" in raw_message_def else "main"
            if type(bus) != str:
                raise TypeError(
                    f"Type for bus should be `str` got `{type(bus)}` instead!"
                )

            frequency = (
                raw_message_def["frequency"] if "frequency" in raw_message_def else 0.0
            )

            fields = list()
            for raw_field_def in raw_message_def["data"]:
                field_name = raw_field_def["name"]
                data_type = raw_field_def["type"]
                if data_type == "bitfield":
                    bits = [
                        CANMessageSubfieldDef(name=raw_subfield_def["name"], bits=1)
                        for raw_subfield_def in raw_field_def["bits"]
                    ]
                    field_def = CANMessageBitfieldDef(name=field_name, bits=bits)
                else:
                    unit = (
                        raw_field_def["unit"] if "unit" in raw_field_def else ""
                    )  # unitless by default
                    conversion = (
                        raw_field_def["conversion"]
                        if "conversion" in raw_field_def
                        else 1.0
                    )  # no conversion by default
                    field_def = CANMessageFieldDef(
                        name=field_name,
                        data_type=data_type,
                        unit=unit,
                        conversion=conversion,
                    )
                fields.append(field_def)

            message_defs = [
                CANMessageDef(
                    name=n,
                    description=description,
                    id=x,
                    endian=endian,
                    repeat=repeat,
                    offset=offset,
                    bus=bus,
                    frequency=frequency,
                    is_extended=is_extended,
                    data=fields
                )
                for n, x in zip(name, _id)
            ]

            # if the bus doesn't have any packets already make a dictionary
            if bus not in all_packets:
                all_packets[bus] = dict()

            # Check if the id is already in use on this bus
            for x in _id:
                if x in all_packets[bus]:
                    raise RuntimeError(
                        f"Error: CAN ID {x} is already in use on bus {bus}!"
                    )

            # TODO: check if ID is in range
            # TODO: add further validation to other attributes

            for message_def in message_defs:
                all_packets[message_def.bus][message_def.id] = message_def

        for raw_type_def in yaml_contents.get("types", []):
            name = raw_type_def["name"]
            description = raw_type_def.get("description", "")
            base_type = raw_type_def["base_type"]
            values = raw_type_def["values"]

            # TODO: add more error checking

            type_def = CANTypeDef(
                name=name, description=description, base_type=base_type, values=values
            )
            if type_def.name in all_types:
                raise RuntimeError("ERROR: Redefinition of type.")

            all_types[type_def.name] = type_def

        for raw_bus_def in yaml_contents.get("busses", []):
            name = raw_bus_def["name"]
            if type(name) != str:
                raise TypeError()

            baud_rate = raw_bus_def["baud_rate"]
            if baud_rate not in valid_baud_rates:
                raise ValueError()

            extended_id = raw_bus_def["extended_id"]
            if type(extended_id) != bool:
                raise TypeError()

            bus = CANBusDef(
                name=name, baud_rate=baud_rate, use_extended_ids=extended_id
            )
            if bus in all_busses:
                raise RuntimeError("ERROR: Redefinition of bus.")

            all_busses.append(bus)

        for raw_board_def in yaml_contents.get("boards", []):
            name = raw_board_def["name"]
            transmit = raw_board_def["transmit"]
            receive = raw_board_def["receive"]
            board = CANBoardDef(name=name, transmit=transmit, receive=receive)
            all_boards[board.name] = board

    return all_packets, all_types, all_busses, all_boards
