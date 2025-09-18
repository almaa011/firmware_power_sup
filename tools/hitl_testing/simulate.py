
import sys

import can  # pip install python-can

from messages import wavesculptor, car_control, steering, lights, user, bms

bustype = 'socketcan'
channel = 'can0' if len(sys.argv) < 2 else sys.argv[1]

bus = can.Bus(channel=channel, interface=bustype)


# flight computer testing
# bp_minmax = bp_control.BpMinmax()
# wsr_velocity = wavesculptor.WsrVelocity()
# wsl_velocity = wavesculptor.WslVelocity()

# bp_minmax.max_cell = 4200  # cell voltage is too high to charge
# bp_minmax.max_temp = 4000  # cell temp is too high to charge

# wsr_velocity.motor_velocity = -900.0  # too fast to shift
# wsl_velocity.motor_velocity = -900.0 # too fast to shift

# bus.send(bp_minmax.msg)
# bus.send(wsr_velocity.msg)
# bus.send(wsl_velocity.msg)


# display dashboard testing
# pedal_percentages = dashboard.DashboardPedalPercentages()
# soc = bp_control.BpSoc()

# pedal_percentages.accel_pedal_value = 0
# pedal_percentages.brake_pedal_value = 0

# soc.soc = 1.0

# bus.send(pedal_percentages.msg)
# bus.send(soc.msg)


# lights testing
# headlights_cmd = car_control.VisionHeadlightsCommand()
# headlights_cmd.headlights = 1
# headlights_cmd.high_beams = 1

# turn_signal_cmd = car_control.VisionTurnSignalsCommand()
# turn_signal_cmd.left_turn_signal = 1
# turn_signal_cmd.right_turn_signal = 1

# brake_light_cmd = car_control.VisionBrakeLightsCommand()
# brake_light_cmd.brake_lights = 1

# bus.send(headlights_cmd.msg)
# bus.send(turn_signal_cmd.msg)
# bus.send(brake_light_cmd.msg)

bms_ah_set = user.BmsAhSet()
bms_wh_set = user.BmsWhSet()
#bms_min_fan_speed = bms.BmsSetMinFanSpeed()
#bms_min_fan_speed = 0.2

#bus.send(bms_min_fan_speed)


bus.send(bms_ah_set.msg)
bus.send(bms_wh_set.msg)
