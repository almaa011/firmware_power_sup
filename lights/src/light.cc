/**
 * @file boardInteract.cc
 * @author Joe Rendleman (rendl008@umn.edu)
 * @brief Initializes GPIO pins and other necessary information based upon DIP
 * switch state.
 * Contains control functions for the Lights
 * @version 0.1
 * @date 2022-01-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "light.h"

namespace umnsvp {
namespace lights {

/**
 * @brief Construct a new Light:: Light object
 *
 * @param gpio_port GPIO PORT number
 * @param gpio_pin GPIO Pin number
 * @param pwm_mode whether light is using pwm or not, default false
 */

Light::Light(const uint16_t gpio_pin, GPIO_TypeDef* const gpio_port,
             bool pwm_mode)
    : pin(gpio_pin), port(gpio_port), pwm(pwm_mode) {
}

/**
 * @brief Initialize underlying GPIO configurations
 */
void Light::init() {
    if (pwm) {
        // When initializing pwm, set brightness to 1 right
        // away to allow for easier usage as a normal light
        pwm_pin.init();
        pwm_pin.set_light_brightness(1);
    } else {
        if (port == GPIOA) {
            __HAL_RCC_GPIOA_CLK_ENABLE();
        } else if (port == GPIOB) {
            __HAL_RCC_GPIOB_CLK_ENABLE();
        } else if (port == GPIOC) {
            __HAL_RCC_GPIOC_CLK_ENABLE();
        } else if (port == GPIOD) {
            __HAL_RCC_GPIOD_CLK_ENABLE();
        } else if (port == GPIOE) {
            __HAL_RCC_GPIOE_CLK_ENABLE();
        } else if (port == GPIOF) {
            __HAL_RCC_GPIOF_CLK_ENABLE();
        } else if (port == GPIOG) {
            __HAL_RCC_GPIOG_CLK_ENABLE();
        }

        // Pack the pin configuration into a struct.
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.Pin = pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

        // Configure the GPIO port with the packed pin configuration.
        HAL_GPIO_Init(port, &GPIO_InitStruct);
    }
}

/**
 * @brief Write ON state to light Pin
 */
void Light::on() {
    on_status = true;
    if (!pwm) {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    } else {
        pwm_pin.turn_on();
    }
}

/**
 * @brief Write OFF state to light Pin
 */
void Light::off() {
    on_status = false;
    if (!pwm) {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
    } else {
        pwm_pin.turn_off();
    }
}

/**
 * @brief Toggle pin output
 */
void Light::toggle() {
    if (!pwm) {
        HAL_GPIO_TogglePin(port, pin);
    } else {
        if (on_status) {
            pwm_pin.turn_off();
        } else {
            pwm_pin.turn_on();
        }
    }
    on_status = !on_status;
}

/**
 * @brief Read the current ouput condition of the light pin
 *
 * @return uint8_t true if ON
 */
bool Light::read() {
    return HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET;
}

/**
 * @brief set brightness/duty percent for pwm light
 *        if called on non-pwm light, does nothing
 *
 * @param brightness_percentag: duty percent, default 1
 */
void Light::set_brightness(float brightness_percentage) {
    if (pwm) {
        pwm_pin.set_light_brightness(brightness_percentage);
    }
}

}  // namespace lights
}  // namespace umnsvp