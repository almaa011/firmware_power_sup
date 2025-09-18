#include "main.h"  // Board-specific HAL includes and Error_Handler prototype

namespace {
void SystemClock_Config();  // Forward declaration for system clock setup
                            // routine
void MX_GPIO_Init();  // Forward declaration for GPIO initialization helper
}  // namespace

int main() {
    HAL_Init();  // Initialize the HAL state, Flash interface, and SysTick
    SystemClock_Config();  // Configure system clock to the desired HSI-based
                           // setup
    MX_GPIO_Init();        // Prepare GPIO pins used by this application

    while (true) {  // Main loop keeps running indefinitely
        HAL_GPIO_TogglePin(
            GPIOA, GPIO_PIN_5);  // Flip the PA5 LED state each iteration
        HAL_Delay(2000);         // Wait 3 seconds for visible blink rate
    }
}

namespace {
void SystemClock_Config() {  // Sets up oscillators and bus clocks for the MCU
    RCC_OscInitTypeDef osc_config =
        {};  // Zero-initialized oscillator configuration struct
    RCC_ClkInitTypeDef clk_config =
        {};  // Zero-initialized clock tree configuration struct

    __HAL_RCC_PWR_CLK_ENABLE();  // Enable PWR peripheral clock so we can adjust
                                 // voltage scaling
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) !=
        HAL_OK) {         // Request Scale 1 for highest frequency
        Error_Handler();  // Abort if voltage scaling change fails
    }

    osc_config.OscillatorType =
        RCC_OSCILLATORTYPE_HSI;        // Use the internal high-speed oscillator
    osc_config.HSIState = RCC_HSI_ON;  // Ensure HSI oscillator is running
    osc_config.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;  // Stick with factory calibration trim
    osc_config.PLL.PLLState =
        RCC_PLL_OFF;  // Keep PLL disabled for this simple clock tree

    if (HAL_RCC_OscConfig(&osc_config) !=
        HAL_OK) {         // Apply oscillator configuration to hardware
        Error_Handler();  // Trap if oscillator configuration fails
    }

    clk_config.ClockType =
        RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;  // Explicitly update all main clock domains
    clk_config.SYSCLKSource =
        RCC_SYSCLKSOURCE_HSI;  // Drive the system clock directly from HSI
    clk_config.AHBCLKDivider =
        RCC_SYSCLK_DIV1;  // Leave AHB at full system clock speed
    clk_config.APB1CLKDivider = RCC_HCLK_DIV1;  // Leave APB1 bus undivided
    clk_config.APB2CLKDivider = RCC_HCLK_DIV1;  // Leave APB2 bus undivided

    if (HAL_RCC_ClockConfig(&clk_config, FLASH_LATENCY_1) !=
        HAL_OK) {         // Commit clock tree setup and set Flash wait states
        Error_Handler();  // Trap if clock distribution fails
    }

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() /
                       1000U);  // Configure SysTick to tick every millisecond
    HAL_SYSTICK_CLKSourceConfig(
        SYSTICK_CLKSOURCE_HCLK);  // Run SysTick from the core clock for
                                  // accuracy
}

void MX_GPIO_Init() {  // Configure GPIO pins referenced by the firmware
    __HAL_RCC_GPIOA_CLK_ENABLE();  // Enable clock for GPIO port A before
                                   // touching registers

    GPIO_InitTypeDef gpio = {};  // Zero-initialize GPIO configuration structure
    // LD2 on the NUCLEO-L476RG maps to PA5 (Arduino D13)
    gpio.Pin = GPIO_PIN_5;            // Select pin 5 on port A
    gpio.Mode = GPIO_MODE_OUTPUT_PP;  // Configure as push-pull output
    gpio.Pull = GPIO_NOPULL;  // Do not enable internal pull-up or pull-down
    gpio.Speed =
        GPIO_SPEED_FREQ_LOW;  // Use lowest slew rate for simple LED drive
    HAL_GPIO_Init(GPIOA,
                  &gpio);  // Apply configuration to the hardware registers

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,
                      GPIO_PIN_RESET);  // Start with LED turned off
}
}  // namespace

void Error_Handler() {  // Fallback routine used when initializing subsystems
                        // fails
    __disable_irq();    // Prevent further interrupts from firing while halted
    while (true) {      // Stay here forever so a debugger can inspect the fault
    }
}

#ifdef USE_FULL_ASSERT
extern "C" void assert_failed(
    uint8_t* file, uint32_t line) {  // Weak hook for HAL assertion failures
    (void)file;                      // Ignore filename in release builds
    (void)line;                      // Ignore line number in release builds
}
#endif
