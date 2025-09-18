/**
 * @file timer.h
 * @author Joe Rendleman (rendl008@umn.edu)
 * @brief
 * @version 0.1
 * @date 2022-10-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include "hal.h"

#if !(STM32L476xx || STM32G474xx || STM32G473xx || STM32G0B1xx)
#error "Timer class currently only supports L4, G0, and G4."
#endif

namespace umnsvp {

/**
 * Timer Class- provides some abstraction for the creation of timers on a G4
 * microcontroller.

 * Available timers on G4: 1-8, 15, 16, 17, 20
 * Available timers of L4: 1-8, 15, 16, 17
 *
 * DO NOT USE TIM1 AND TIM16 AT THE SAME TIME, they use the same interrupt
 *
 * The user MUST include some things in ``application.cc`` in order for this
 * class to work correctly.
 *
 * initialize the timer in the header file as a public variable
 * start the timer by calling:
 * ``timer.start_timer(&timer_handler_callback);``
 * in your initialization function
 *
 * include the following function in ``main.cc``
 *
 * ```void timer_handler_callback(TIM_HandleTypeDef* htim) {
 *  if (htim->Instance == <TIMER_NUMBER_HTIM>) {
 *       <DESIRED_FUNCTION_CALL>;
 *   }
 * }```
 *
 * also include this function
 *
 * ```
 * extern "C" void <TIMER_INTERRUPT_NAME>_IRQHandler(void) {
 *    HAL_TIM_IRQHandler(app.tim.get_htim());
 * }```
 *
 * In the ``stm32g4xx_hal_conf.h``/``stm32L4xx_hal_conf.h`` file of your
 * project, uncomment the line ``#define HAL_TIM_MODULE_ENABLED`` and change
 *  ``#define USE_HAL_TIM_REGISTER_CALLBACKS`` from ``0U`` to ``1U``
 */
template <uint16_t TIM,
          uint16_t period_ms>  // Available timers on G4: 1-8, 15, 16, 17, 20
                               // Available timers on L4: 1-8, 15, 16, 17
                               // period_ms must be between 0 and 10000ms
class timer {
   private:
    uint16_t priority;
    uint16_t sub_priority;
    TIM_HandleTypeDef htim;

    /**
     * @brief initialize htim.Instance in accordance to timer_number
     * set priority of the corresponding interrupt
     * enable the IRQ for the corresponding interrupt
     * enable the clk for the timer
     *
     */
    void interrupt_setup();

    /**
     * @brief initialize the period and prescaler of the timer
     * currently only works for period_ms values between 0 and 6800 ms
     *
     */
    void period_conversion();

   public:
    /**
     * @brief Construct a new timer::timer object
     * Initializes priority and sub-priority to 7
     *
     */
    timer() {
        timer(7, 7);
    }

    /**
     * @brief Construct a new timer::timer object
     * including sub-priority and priority specifications
     *
     * @param priority
     * @param sub_priority
     */
    timer(uint16_t priority, uint16_t sub_priority)
        : priority(priority), sub_priority(sub_priority) {
#if STM32G474xx || STM32G473xx
        static_assert((1 <= TIM && TIM <= 8) || (15 <= TIM && TIM <= 17) ||
                      TIM == 20);
#elif STM32G0B1xx
        static_assert((1 <= TIM && TIM <= 4) || (6 <= TIM && TIM <= 7) ||
                      TIM == 15);
#elif STM32L476xx
        static_assert((1 <= TIM && TIM <= 8) || (15 <= TIM && TIM <= 17));

#endif
        // restrict period to 10 seconds which is the max we can do with the
        // prescaler
        static_assert((0 <= period_ms && period_ms <= 10000));
    }

    /**
     * @brief start timer counting and return the htim object
     *
     * @param USER_TIM_PeriodElapsedCallback pointer to the user callback
     * function
     */
    void start_timer(pTIM_CallbackTypeDef USER_TIM_PeriodElapsedCallback);

    /**
     * @brief Get the htim object
     *
     * @return TIM_HandleTypeDef*
     */
    TIM_HandleTypeDef* get_htim();
};

template <uint16_t TIM, uint16_t period_ms>
void timer<TIM, period_ms>::start_timer(
    pTIM_CallbackTypeDef USER_TIM_PeriodElapsedCallback) {
    interrupt_setup();

    TIM_MasterConfigTypeDef sMasterConfig = {0};
    period_conversion();
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    // Init timer
    if (HAL_TIM_Base_Init(&htim) != HAL_OK) {
        while (1)
            ;
    }
    // Register the timer callback function
    if (HAL_TIM_RegisterCallback(&htim, HAL_TIM_PERIOD_ELAPSED_CB_ID,
                                 USER_TIM_PeriodElapsedCallback) != HAL_OK) {
        while (1)
            ;
    }
    // Clear the IT flag before starting the timer interrupt
    __HAL_TIM_CLEAR_IT(&htim, TIM_IT_UPDATE);
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig) !=
        HAL_OK) {
        while (1)
            ;
    }
    // Start the timer interrupt
    HAL_TIM_Base_Start_IT(&htim);
}

template <uint16_t TIM, uint16_t period_ms>
void timer<TIM, period_ms>::interrupt_setup() {
    switch (TIM) {
        case 1:
#if STM32G0B1xx
            HAL_NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, priority,
                                 sub_priority);
            HAL_NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
#else
            HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
#endif
            __HAL_RCC_TIM1_CLK_ENABLE();

            htim.Instance = TIM1;
            break;

        case 2:
            HAL_NVIC_SetPriority(TIM2_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM2_IRQn);
            __HAL_RCC_TIM2_CLK_ENABLE();

            htim.Instance = TIM2;
            break;

        case 3:
#if STM32G0B1xx
            HAL_NVIC_SetPriority(TIM3_TIM4_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM3_TIM4_IRQn);
#else
            HAL_NVIC_SetPriority(TIM3_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM3_IRQn);
#endif
            __HAL_RCC_TIM3_CLK_ENABLE();

            htim.Instance = TIM3;
            break;

        case 4:
#if STM32G0B1xx
            HAL_NVIC_SetPriority(TIM3_TIM4_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM3_TIM4_IRQn);
#else
            HAL_NVIC_SetPriority(TIM4_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM4_IRQn);
#endif
            __HAL_RCC_TIM4_CLK_ENABLE();

            htim.Instance = TIM4;
            break;

#if STM32G0B1xx
#else
        case 5:

            HAL_NVIC_SetPriority(TIM5_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM5_IRQn);
            __HAL_RCC_TIM5_CLK_ENABLE();

            htim.Instance = TIM5;
            break;
#endif
        case 6:
#if STM32G0B1xx
            HAL_NVIC_SetPriority(TIM6_DAC_LPTIM1_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM6_DAC_LPTIM1_IRQn);
#else
            HAL_NVIC_SetPriority(TIM6_DAC_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
#endif
            __HAL_RCC_TIM6_CLK_ENABLE();

            htim.Instance = TIM6;
            break;

        case 7:
#if STM32G474xx || STM32G473xx
            HAL_NVIC_SetPriority(TIM7_DAC_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM7_DAC_IRQn);
#elif STM32G0B1xx
            HAL_NVIC_SetPriority(TIM7_LPTIM2_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM7_LPTIM2_IRQn);
#elif STM32L476xx
            HAL_NVIC_SetPriority(TIM7_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM7_IRQn);
#endif
            __HAL_RCC_TIM7_CLK_ENABLE();

            htim.Instance = TIM7;
            break;
#if STM32G0B1xx
#else
        case 8:
            HAL_NVIC_SetPriority(TIM8_UP_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM8_UP_IRQn);
            __HAL_RCC_TIM8_CLK_ENABLE();

            htim.Instance = TIM8;
            break;
#endif
        case 15:
#if STM32G0B1xx
            HAL_NVIC_SetPriority(TIM15_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM15_IRQn);
#else
            HAL_NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);
#endif
            __HAL_RCC_TIM15_CLK_ENABLE();

            htim.Instance = TIM15;
            break;
#if STM32G0B1xx
#else
        case 16:
            HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
            __HAL_RCC_TIM16_CLK_ENABLE();

            htim.Instance = TIM16;
            break;

        case 17:
            HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn, priority,
                                 sub_priority);
            HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
            __HAL_RCC_TIM17_CLK_ENABLE();

            htim.Instance = TIM17;
            break;
#if STM32G474xx || STM32G473xx
        case 20:
            HAL_NVIC_SetPriority(TIM20_UP_IRQn, priority, sub_priority);
            HAL_NVIC_EnableIRQ(TIM20_UP_IRQn);
            __HAL_RCC_TIM20_CLK_ENABLE();

            htim.Instance = TIM20;
            break;
#endif
#endif
    }
}

template <uint16_t TIM, uint16_t period_ms>
void timer<TIM, period_ms>::period_conversion() {
#if STM32G474xx || STM32G473xx
    // on G4: 160,000,000/40,000 = 4000 thus giving us anywhere between 1 ms and
    // 10 s timer
    htim.Init.Prescaler = 40000;
#elif STM32G0B1xx
    // on G0: 64,000,000/16,000 = 4000 thus giving us anywhere between 1 ms and
    // 10 s
    htim.Init.Prescaler = 16000;
#elif STM32L476xx
    // on L4: 80,000,000/20,000 = 4000 thus giving us anywhere between 1 ms and
    // 10 s
    htim.Init.Prescaler = 20000;
#endif
    htim.Init.Period = 4 * period_ms;
}

template <uint16_t TIM, uint16_t period_ms>
TIM_HandleTypeDef* timer<TIM, period_ms>::get_htim() {
    return &htim;
}

}  // namespace umnsvp