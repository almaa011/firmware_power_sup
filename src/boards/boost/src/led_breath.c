#include ""led_breath.h""

#include ""led_pwm.h""

static const float kLed2MinBrightness = 0.0f;
static const float kLed2MaxBrightness = 1.0f;
static const float kLed2StepSize = 0.01f;  // Smaller values slow the breathing rhythm.
static float s_led2_brightness = 0.0f;
static float s_led2_delta = 0.0f;

void LED2_Breath_Init(void) {
    s_led2_brightness = kLed2MinBrightness;
    s_led2_delta = kLed2StepSize;
    LED2_SetBrightness(s_led2_brightness);
}

void LED2_Breath_Update(void) {
    s_led2_brightness += s_led2_delta;
    if (s_led2_brightness >= kLed2MaxBrightness) {
        s_led2_brightness = kLed2MaxBrightness;
        s_led2_delta = -kLed2StepSize;
    } else if (s_led2_brightness <= kLed2MinBrightness) {
        s_led2_brightness = kLed2MinBrightness;
        s_led2_delta = kLed2StepSize;
    }

    LED2_SetBrightness(s_led2_brightness);
}
