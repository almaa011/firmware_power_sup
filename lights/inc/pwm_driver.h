/**
 * @file pwm_driver.h
 * @author Maxim Erickson (eric4190@umn.edu)
 * @brief Files for controlling  light brightness. Contains driver code
 * @version 1
 * @date 2022-4-1
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once
#include "hal.h"

namespace umnsvp {
namespace lights {

/* Definition for TIMx Channel Pins */

/* Exported macro ------------------------------------------------------------*/

class pwmDriver {
   public:
    pwmDriver(const uint16_t pin, GPIO_TypeDef* const port)
        : pin(pin), port(port) {
    }
    void set_light_brightness(float brightness_percentage);
    void init();
    void turn_on();
    void turn_off();

   private:
    void init_light_brightness();
    void init_timer_gpio();
    const uint16_t pin;
    GPIO_TypeDef* const port;
};

}  // namespace lights
}  // namespace umnsvp
