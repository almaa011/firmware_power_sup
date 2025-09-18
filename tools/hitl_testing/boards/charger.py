
from boards.abstract_board import AbstractBoard


# TODO: implement this class, at the very least implement sending prox sense for flight computer testing
class Charger(AbstractBoard):

    def __init__(self, bus):
        super().__init__(bus=bus)
        
        # Internal state
        ...

        # Messages to send
        ...

        # Messages to receive
        ...

    ...
