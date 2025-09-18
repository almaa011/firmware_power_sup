
from abc import ABC, abstractmethod
from typing import Union
import struct

import can  # pip install python-can

class AbstractMessage(ABC):

    def __init__(self, msg: can.Message = None):
        if msg is None:
            msg = can.Message(arbitration_id=self.can_id, data=[0]*self.length, is_extended_id=False)
        self.msg = msg
        self._task = None

    def _update_task(self):
        if self._task:
            self._task.modify_data([self.msg])

    def _get_bitfield(self, byte: int, pos: int) -> int:
        return (self.msg.data[byte] >> pos) & 1

    def _set_bitfield(self, byte: int, pos: int, value: Union[bool, int]) -> None:
        n = self.msg.data[byte]
        mask = 1 << pos
        b = value & 1

        if b != value:
            print('Warning: Illegal value was set!')

        self.msg.data[byte] = (n & ~mask) | (b << pos)
        self._update_task()

    def _get_uint8(self, byte: int) -> int:
        return int.from_bytes(self.msg.data[byte:byte+1], byteorder='little', signed=False)

    def _set_uint8(self, byte: int, value: int) -> None:
        self.msg.data[byte:byte+1] = value.to_bytes(length=1, byteorder='little', signed=False)[:1]
        self._update_task()

    def _get_int8(self, byte: int) -> int:
        return int.from_bytes(self.msg.data[byte:byte+1], byteorder='little', signed=True)

    def _set_int8(self, byte: int, value: int) -> None:
        self.msg.data[byte:byte+1] = value.to_bytes(length=1, byteorder='little', signed=True)[:1]
        self._update_task()

    def _get_uint16(self, byte: int) -> int:
        return int.from_bytes(self.msg.data[byte:byte+2], byteorder='little', signed=False)

    def _set_uint16(self, byte: int, value: int) -> None:
        self.msg.data[byte:byte+2] = value.to_bytes(length=2, byteorder='little', signed=True)
        self._update_task()

    def _get_int16(self, byte: int) -> int:
        return int.from_bytes(self.msg.data[byte:byte+2], byteorder='little', signed=True)

    def _set_int16(self, byte: int, value: int) -> None:
        self.msg.data[byte:byte+2] = value.to_bytes(length=2, byteorder='little', signed=False)
        self._update_task()
    
    def _get_uint32(self, byte: int) -> int:
        return int.from_bytes(self.msg.data[byte:byte+4], byteorder='little', signed=False)

    def _set_uint32(self, byte: int, value: int) -> None:
        self.msg.data[byte:byte+4] = value.to_bytes(length=4, byteorder='little', signed=False)
        self._update_task()

    def _get_int32(self, byte: int) -> int:
        return int.from_bytes(self.msg.data[byte:byte+4], byteorder='little', signed=True)

    def _set_int32(self, byte: int, value: int) -> None:
        self.msg.data[byte:byte+4] = value.to_bytes(length=4, byteorder='little', signed=True)
        self._update_task()
    
    def _get_float(self, byte: int) -> float:
        # struct returns a 1-tuple
        return struct.unpack('f', self.msg.data[byte:byte+4])[0]

    def _set_float(self, byte: int, value: float) -> None:
        # make sure value is really a float
        value = float(value)
        self.msg.data[byte:byte+4] = struct.pack('f', value)
        self._update_task()

    def __repr__(self):
        return repr(self.msg)

    @property
    @abstractmethod
    def can_id(self) -> int:
        pass
    
    @property
    @abstractmethod
    def length(self) -> int:
        pass
    
    @property
    @abstractmethod
    def period(self) -> float:
        pass
    