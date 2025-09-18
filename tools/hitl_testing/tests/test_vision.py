
import logging
import time

import can  # pip install python-can
import pytest

from messages.vision import *

bustype = 'socketcan'
channel = 'can0'
# bitrate = 125000

logging.basicConfig()
logging.getLogger().setLevel(logging.INFO)

# @pytest.fixture(scope='global')
# def bus():
#     b = can.Bus(channel=channel, interface=bustype)
#     yield b
#     ...  # cleanup?
@pytest.fixture
def bus():
    return can.Bus(channel=channel, interface=bustype)


def wait_for_value(bus, msg, signal, value, timeout=1.0):
    
    val = None
    t = time.time()
    
    while time.time() - t < timeout:
        m = bus.recv(timeout=0.001)
        if m is None:
            continue

        if m.arbitration_id == msg.can_id:
            val = signal(msg(m))
            if val == value:
                return val

    if val is not None:
        raise Exception(f'Timeout! Most recent value was {val}')
    raise Exception('Timeout! Signal was not seen!')
    


def test_left_turn_signal(bus):

    turn_signal_cmd = VisionTurnSignalsCommand()
    brake_light_cmd = VisionBrakeLightsCommand()
    head_lights_cmd = VisionHeadlightsCommand()
    horn_cmd = VisionHornCommand()
    array_latch_cmd = VisionArrayLatchesCommand()
    rear_view_cmd = VisionRearviewCommand()

    # wait_for_value(bus, func=lambda , timeout=1.0)
    # wait_for_value(bus, msg=VisionStatusFront, signal=lambda msg: msg.left_turn_signal, value=1)

    bus.send(turn_signal_cmd.msg)
    bus.send(brake_light_cmd.msg)
    bus.send(head_lights_cmd.msg)
    bus.send(horn_cmd.msg)
    bus.send(array_latch_cmd.msg)
    bus.send(rear_view_cmd.msg)

    logging.info('Wait 5 seconds with everything off')
    # print('Wait 5 seconds with everything off')
    wait_for_value(bus,
                   msg=VisionStatusFront,
                   signal=lambda msg: msg.left_turn_signal,
                   value=0
                   )
    time.sleep(5)

    logging.info('Turn on left turn signal')
    # print('Turn on left turn signal')
    turn_signal_cmd.left_turn_signal = True
    bus.send(turn_signal_cmd.msg)
    wait_for_value(bus,
                   msg=VisionStatusFront,
                   signal=lambda msg: msg.left_turn_signal,
                   value=1
                   )
    time.sleep(5)

    logging.info('Turn off left turn signal')
    # print('Turn off left turn signal')
    turn_signal_cmd.left_turn_signal = False
    bus.send(turn_signal_cmd.msg)
    wait_for_value(bus,
                   msg=VisionStatusFront,
                   signal=lambda msg: msg.left_turn_signal,
                   value=0
                   )
    time.sleep(5)

    # msgs = {
    #     turn_signal_cmd.can_id: turn_signal_cmd.msg,
    #     brake_light_cmd.can_id: brake_light_cmd.msg,
    #     head_lights_cmd.can_id: head_lights_cmd.msg,
    #     horn_cmd.can_id: horn_cmd.msg,
    #     array_latch_cmd.can_id: array_latch_cmd.msg,
    #     rear_view_cmd.can_id: rear_view_cmd.msg
    # }

    # with can.interface.Bus(channel=channel, interface=bustype) as bus:
    #     tasks = {can_id: bus.send_periodic(msgs=msg, period=0.2) for can_id, msg in msgs.items()}

    #     print('Wait 5 seconds with everything off')
    #     # TODO: check response on CAN
    #     time.sleep(5)

    #     print('Turn on left turn signal')
    #     # updating information could be more elegant
    #     turn_signal_cmd.left_turn_signal = True
    #     tasks[turn_signal_cmd.can_id].modify_data(turn_signal_cmd.msg)
    #     # brake_light_cmd.brake_lights = True
    #     # tasks[brake_light_cmd.can_id].modify_data(brake_light_cmd.msg)
    #     # TODO: check response on CAN
    #     time.sleep(5)

    #     print('Turn off left turn signal')
    #     turn_signal_cmd.left_turn_signal = False
    #     tasks[turn_signal_cmd.can_id].modify_data(turn_signal_cmd.msg)
    #     # TODO: check response on CAN
    #     time.sleep(5)

def test_right_turn_signal(bus):
    ...

def test_brake_lights(bus):
    ...


# test_left_turn_signal()