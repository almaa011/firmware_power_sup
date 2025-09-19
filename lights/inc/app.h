#pragma once

#include "application_base.h"
#include "dip_switch.h"
#include "fdcan.h"
#include "hal.h"
#include "light.h"
#include "pwm_driver.h"
#include "skylab2_boards.h"
#include "timer.h"

namespace umnsvp {
namespace lights {

class Application : public ApplicationBase {
   public:
    Application();
    void main(void);
    void send_status(void);
    void send_ID(void);
    void blinky_handler(void);

    Light &front_left;
    Light &front_right;
    Light &back_left;
    Light &back_right;
    Light &blink_light;
    Light &brake;
    Light &camera;
    Light &drl_left;
    Light &drl_right;
    Light &accent_left;
    Light &accent_right;
    
    umnsvp::timer<2, 500> timFDCan;
    umnsvp::timer<7, 500> timBlinky;

    void can_rx_callback(void);
    void can_tx_callback(void);

    FDCAN_HandleTypeDef *get_handle();

   private:
    void init();
    void system_clock_config(void);
    bool front;
    bool bms_fault;

    Light light_0 = Light(PIN_LIGHT_0, PORT_LIGHT_0, false);
    Light light_1 = Light(PIN_LIGHT_1, PORT_LIGHT_1, false);
    Light light_2 = Light(PIN_LIGHT_2, PORT_LIGHT_2, false);
    Light light_3 = Light(PIN_LIGHT_3, PORT_LIGHT_3, false);
    Light light_4 = Light(PIN_LIGHT_4, PORT_LIGHT_4, false);
    Light light_5 = Light(PIN_LIGHT_5, PORT_LIGHT_5, false);

    Light enable = Light(PIN_12V_EN, PORT_12V_EN);

    can::fdcan_driver can_device;
    static constexpr uint16_t can_rx_pin = GPIO_PIN_11;
    static constexpr uint16_t can_tx_pin = GPIO_PIN_12;

    umnsvp::skylab2::lights_can skylab;
    umnsvp::dip_switch dip = dip_switch(PORT_DIP, PIN_DIP);
};

}  // namespace lights
}  // namespace umnsvp