
class Packet:
    def __init__(self, id=0, data=None):
        self.id = id
        if data is None:
            data = []
        self.data = data
