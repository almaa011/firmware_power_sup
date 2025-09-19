#pragma once

/*includes*/
#include "hal.h"
#include "pwm_driver.h"

/*declarations*/

static GPIO_TypeDef* const PORT_LIGHT_0 = GPIOC;
constexpr uint16_t PIN_LIGHT_0 = GPIO_PIN_0;

static GPIO_TypeDef* const PORT_LIGHT_1 = GPIOC;
constexpr uint16_t PIN_LIGHT_1 = GPIO_PIN_1;

static GPIO_TypeDef* const PORT_LIGHT_2 = GPIOC;
constexpr uint16_t PIN_LIGHT_2 = GPIO_PIN_2;

static GPIO_TypeDef* const PORT_LIGHT_3 = GPIOC;
constexpr uint16_t PIN_LIGHT_3 = GPIO_PIN_3;

static GPIO_TypeDef* const PORT_LIGHT_4 = GPIOA;
constexpr uint16_t PIN_LIGHT_4 = GPIO_PIN_0;

static GPIO_TypeDef* const PORT_LIGHT_5 = GPIOA;
constexpr uint16_t PIN_LIGHT_5 = GPIO_PIN_1;
// DIP
static GPIO_TypeDef* const PORT_DIP = GPIOD;
constexpr uint16_t PIN_DIP = GPIO_PIN_11;
// 12V Enable
static GPIO_TypeDef* const PORT_12V_EN = GPIOA;
constexpr uint16_t PIN_12V_EN = GPIO_PIN_2;
// LED B
static GPIO_TypeDef* const PORT_LED_B = GPIOD;
constexpr uint16_t PIN_LED_B = GPIO_PIN_4;
// LED Y
static GPIO_TypeDef* const PORT_LED_Y = GPIOD;
constexpr uint16_t PIN_LED_Y = GPIO_PIN_3;
// LED G
static GPIO_TypeDef* const PORT_LED_G = GPIOD;
constexpr uint16_t PIN_LED_G = GPIO_PIN_2;

namespace umnsvp {
namespace lights {

class Light {
   private:
    const uint16_t pin;
    GPIO_TypeDef* const port;
    bool pwm;
    pwmDriver pwm_pin = pwmDriver(pin, port);
    bool on_status = false;

   public:
    Light(const uint16_t gpio_pin, GPIO_TypeDef* const gpio_port,
          bool pwm_mode = false);
    void init();
    void on();
    void off();
    void toggle();
    bool read();
    void set_brightness(float brightness_percentage = 1);
};

}  // namespace lights
}  // namespace umnsvp
