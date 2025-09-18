
from boards.abstract_board import AbstractBoard


# TODO: send status messages
class Photon3(AbstractBoard):

    def __init__(self, bus):
        super().__init__(bus=bus)
        
        # Internal state
        ...

        # Messages to send
        ...

        # Messages to receive
        ...

    ...
