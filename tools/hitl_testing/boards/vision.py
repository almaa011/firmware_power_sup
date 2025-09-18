
from boards.abstract_board import AbstractBoard
from messages.vision import *


class Vision(AbstractBoard):

    def __init__(self, bus):
        super().__init__(bus=bus)

        # Internal state
        ...

        # Messages to send
        self.vision_status_front = VisionStatusFront()
        self.register_periodic_message(self.vision_status_front)

        # Messages to receive
        self.register_callback(VisionTurnSignalsCommand.can_id, self.vision_turn_signals_command_handler)
        self.register_callback(VisionBrakeLightsCommand.can_id, self.vision_brake_lights_command_handler)
        self.register_callback(VisionHeadlightsCommand.can_id, self.vision_headlights_command_handler)
        self.register_callback(VisionHornCommand.can_id, self.vision_horn_command_handler)

    def vision_turn_signals_command_handler(self, msg):
        cmd = VisionTurnSignalsCommand(msg=msg)

        self.vision_status_front.left_turn_signal = cmd.left_turn_signal
        self.vision_status_front.right_turn_signal = cmd.right_turn_signal

    def vision_brake_lights_command_handler(self, msg):
        cmd = VisionBrakeLightsCommand(msg=msg)

        self.vision_status_front.brake_lights = cmd.brake_lights

    def vision_headlights_command_handler(self, msg):
        cmd = VisionHeadlightsCommand(msg=msg)

        self.vision_status_front.headlights = cmd.headlights

    def vision_horn_command_handler(self, msg):
        cmd = VisionHornCommand(msg=msg)

        self.vision_status_front.horn = cmd.horn
