/**
 * @file dip_switch.cc
 * @author Alex Bohm (bohm0080@umn.edu)
 * @brief This file defines the DIP switch driver class.
 * @version 1.0
 * @date 2021-12-01
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "dip_switch.h"

namespace umnsvp {
/**
 * @brief Constructor for dip object.
 *
 * NOTE: Does not initialize hardware, use @ref init()
 *
 * @param gpio_port GPIO port connected to DIP switch.
 * @param gpio_pin  GPIO pin connected to DIP switch.
 *
 */
dip_switch::dip_switch(GPIO_TypeDef* const gpio_port, const uint16_t gpio_pin)
    : port(gpio_port), pin(gpio_pin) {
}

/**
 * @brief Read the current state of the DIP switch.
 *
 * @return true The DIP switch is on.
 * @return false The DIP switch is off.
 */
bool dip_switch::get_state() const {
    return HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET;
}

/**
 * @brief Initialize the hardware for the DIP switch.
 *
 * This function enables the appropriate GPIO clock and sets up the appropriate
 * pin as an input.
 *
 */
void dip_switch::init() {
    // Enable the appropriate clock.
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
    }
#ifdef GPIOF
    else if (port == GPIOF) {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
#endif
#ifdef GPIOG
    else if (port == GPIOG) {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }
#endif
#ifdef GPIOH
    else if (port == GPIOH) {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
#endif

    // Pack the pin configuration into a struct.
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    // DIP switches have an external resistor to pull them to ground when
    // not enabled.
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    // Configure the GPIO port with the packed pin configuration.
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}  // namespace umnsvp

/**
 * @brief Deinitializes the hardware for the DIP switch GPIO pins.
 *
 * De-initializes the GPIOx peripheral registers to their default reset
 * values. No parameters passed when calling the function, and no values
 * returned.
 */
void dip_switch::deinit() {
    HAL_GPIO_DeInit(port, pin);
}

}  // namespace umnsvp