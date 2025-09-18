/**
 * @file dip_switch.h
 * @author Alex Bohm (bohm0080@umn.edu)
 * @brief This file declares a driver for DIP switches.
 * @version 1.0
 * @date 2021-12-01
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once

#include "hal.h"

namespace umnsvp {

/**
 * @brief A DIP switch driver class.
 *
 * This driver is used with a dip switch that pulls a GPIO pin to a high voltage
 * when the switch is enabled. When not enabled a pull down resistor will pull
 * the pin to a low voltage.
 *
 *
 * Circuit Diagram:
 *
 *             +3.3V
 *               |
 *               |
 *             \     Switch
 *              \
 *               |
 * GPIO Pin -----------
 *                    |
 *                    |
 *                    \
 *                    /  Pull down resistor
 *                    \
 *                    /
 *                    |
 *                    |
 *                   GND
 */
class dip_switch {
   private:
    /**
     * @brief The GPIO port for the DIP switch..
     */
    GPIO_TypeDef* const port;

    /**
     * @brief The GPIO pin for the DIP switch.
     */
    const uint16_t pin;

   public:
    dip_switch(GPIO_TypeDef* const gpio_port, const uint16_t gpio_pin);

    void init();

    void deinit();

    bool get_state() const;
};

}  // namespace umnsvp
