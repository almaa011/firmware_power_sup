
from boards.abstract_board import AbstractBoard


# TODO: use updated packets
class FlightComputer(AbstractBoard):

    def __init__(self, bus):
        super().__init__(bus=bus)
        
        # Internal state
        ...

        # Messages to send
        ...

        # Messages to receive
        ...

    ...
