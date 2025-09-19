#include "app.h"

#include "main.h"
namespace umnsvp {
namespace lights {
Application::Application()

    // Front Lights Board
    : drl_left(light_0),
      drl_right(light_1),
      front_left(light_2),
      front_right(light_3),
      accent_left(light_4),
      accent_right(light_5),

      // Back Lights Board
      brake(light_0),
      blink_light(light_1),
      back_left(light_3),
      back_right(light_4),
      camera(light_5),

      can_device(FDCAN1, {can_rx_pin, can_tx_pin, GPIOA}),
      skylab(can_device, can::fifo::FIFO0) {
    front = false;
    bms_fault = false;
}
/**
 * @brief Main method of Lights application
 */
void Application::main() {
    init();

    // Check front or Back
    if (front) {
        // front: DRL Left:0, DRL Right:1, Left Turn:2,
        //  Right Turn:3, Accent Left:4, Accent Right:5

        // forever loop
        while (1) {
            // check headlights
            if (skylab.vision_headlights_command_buffer.pop()) {
                skylab2::can_packet_vision_headlights_command cmd =
                    skylab.vision_headlights_command_buffer.output();
                float brightness = cmd.brightness;

                if (cmd.lights.high_beams) {
                    drl_left.set_brightness(brightness);
                    drl_right.set_brightness(brightness);
                    drl_left.on();
                    drl_right.on();
                } else {
                    drl_left.off();
                    drl_right.off();
                }

                if (cmd.lights.headlights) {
                    accent_left.set_brightness(brightness);
                    accent_right.set_brightness(brightness);
                    accent_left.on();
                    accent_right.on();
                } else {
                    accent_left.off();
                    accent_right.off();
                }
            }

            // check turn signals
            if (skylab.vision_turn_signals_command_buffer.pop()) {
                skylab2::can_packet_vision_turn_signals_command cmd =
                    skylab.vision_turn_signals_command_buffer.output();
                if (cmd.lights.left_turn_signal) {
                    front_left.on();
                } else {
                    front_left.off();
                }
                if (cmd.lights.right_turn_signal) {
                    front_right.on();
                } else {
                    front_right.off();
                }
            }
        }
    } else {
        // Back: Brake Lights:0, Blinky Light:1, Left Turn:3
        // Right Turn:4, Camera:5

        // init can bus

        // forever loop
        while (1) {
            // camera.toggle();
            // brake.toggle();
            // back_left.toggle();
            // back_right.toggle();
            // blink_light.toggle();
            // HAL_Delay(2000);

            if (skylab.vision_brake_lights_command_buffer.pop()) {
                skylab2::can_packet_vision_brake_lights_command cmd =
                    skylab.vision_brake_lights_command_buffer.output();
                if (cmd.lights.brake_lights) {
                    brake.on();

                } else {
                    brake.off();
                }
            }

            // check turn signals
            if (skylab.vision_turn_signals_command_buffer.pop()) {
                skylab2::can_packet_vision_turn_signals_command cmd =
                    skylab.vision_turn_signals_command_buffer.output();
                if (cmd.lights.left_turn_signal) {
                    back_left.on();
                } else {
                    back_left.off();
                }
                if (cmd.lights.right_turn_signal) {
                    back_right.on();
                } else {
                    back_right.off();
                }
            }

            // battery fault indicator
            if (skylab.bms_kill_reason_buffer.pop()) {
                bms_fault = true;
            }
        }
    }
}

/**
 * @brief Application initialization for Hardware related interfaces
 *
 */
void Application::init() {
    /* STM32G4xx HAL library initialization:
     * - Configure the Flash prefetch
     * - Systick timer is configured by default as source of time
     * base, but user can eventually implement his proper time base
     * source (a general purpose timer for example or other time
     * source), keeping in mind that Time base duration should be
     * kept 1ms since PPP_TIMEOUT_VALUEs are defined and handled in
     * milliseconds basis.
     * - Set NVIC Group Priority to 4
     * - Low Level Initialization
     */

    // Initialize HAL, clock, etc...
    sys_init();

    skylab.init();

    // initialize telemetry manager

    // read DIP switch and set FRONT
    dip.init();

    front = dip.get_state();
    // initialize GPIO ports

    if (front) {
        // front: DRL Left:0, Left Turn:1, Right Turn:2, DRL Right:3
        // High Beams:4, Camera:5
        drl_left.init();
        drl_right.init();
        front_left.init();
        front_right.init();
        accent_left.init();
        accent_right.init();

    } else {
        // Back: Brake Lights:0 and 4, Left Turn:1, Right Turn:2,
        // Blinky Light:3, Camera:5
        brake.init();
        back_left.init();
        back_right.init();
        blink_light.init();
    }
    // write on state to cameras

    camera.init();

    camera.on();

    // 12V en

    enable.init();
    enable.on();

    // start timers
    timFDCan.start_timer(&timer_handler_callback);
    timBlinky.start_timer(&timer_handler_callback);
}

void Application::blinky_handler(void) {
    if (bms_fault) {
        blink_light.toggle();
    }
}

/**
 * @brief rx callback function for skylab2
 *
 */
void Application::can_rx_callback(void) {
    skylab.main_bus_rx_handler();
}

void Application::can_tx_callback(void) {
    skylab.main_bus_tx_handler();
}

FDCAN_HandleTypeDef* Application::get_handle() {
    return can_device.get_handle();
}

void Application::send_status() {
    if (front) {
        skylab2::can_packet_vision_status_front status_msg =
            skylab2::can_packet_vision_status_front();

        // accent_right or accent_left can be read for headlight packets
        status_msg.lights.headlights = accent_right.read();

        // drl_right or drl_left can be read for high-beam packets
        status_msg.lights.high_beams = drl_right.read();

        status_msg.lights.left_turn_signal = front_left.read();
        status_msg.lights.right_turn_signal = front_right.read();

        skylab.send_vision_status_front(status_msg);
    } else {
        skylab2::can_packet_vision_status_rear status_msg =
            skylab2::can_packet_vision_status_rear();
        status_msg.lights.brake_lights = brake.read();
        status_msg.lights.left_turn_signal = back_left.read();
        status_msg.lights.right_turn_signal = back_right.read();

        skylab.send_vision_status_rear(status_msg);
    }
}

void Application::send_ID() {
    static skylab2::can_packet_lights_front_id front_id =
        skylab2::can_packet_lights_front_id();
    static skylab2::can_packet_lights_back_id back_id =
        skylab2::can_packet_lights_back_id();

    if (front) {
        skylab.send_lights_front_id(front_id);
    } else {
        skylab.send_lights_back_id(back_id);
    }
}

}  // namespace lights
}  // namespace umnsvp