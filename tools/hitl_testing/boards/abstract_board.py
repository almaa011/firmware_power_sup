
import can


class AbstractBoard:

    def __init__(self, bus):
        self.bus = bus
        self._tasks = dict()

        self._periodic_messages = dict() # id: msg
        self._callbacks = dict()  # id: func(msg)
        self._notifier = None
    
    def start(self):
        self._notifier = can.Notifier(self.bus, [self._packet_handler])

        for can_id, msg in self._periodic_messages.items():
            task = self.bus.send_periodic(msgs=[msg.msg], period=0.5)  # TODO: use actual period but make sure it's not 0.0
            self._tasks[can_id] = task
            msg._task = task

    def stop(self):
        if self._notifier:
            self._notifier.stop()

        for task in self._tasks:
            task.stop()
        self._tasks = dict()
        for message in self._periodic_messages.values():
            message._task = None

    # handle packets as they come in
    def _packet_handler(self, msg):
        if msg.arbitration_id in self._callbacks:
            self._callbacks[msg.arbitration_id](msg)

    # add the callback function to the dictionary
    def register_callback(self, msg_id, func):
        self._callbacks[msg_id] = func

    # currently has to be called before start()
    def register_periodic_message(self, msg):
        self._periodic_messages[msg.can_id] = msg
