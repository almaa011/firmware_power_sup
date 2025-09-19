/**
 * @file main.cc
 * @author Joe Rendleman (rendl008@umn.edu)
 * @brief Lights main task.
 * @version 0.1
 * @date 2022-01-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "main.h"

#include "app.h"

umnsvp::lights::Application app;
/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void) {
    /*
     * Create and run the main application.
     */
    app.main();
}

void timer_handler_callback(TIM_HandleTypeDef* htim) {
    if (htim->Instance == TIM2) {
        app.send_status();
        app.send_ID();
    }
    if (htim->Instance == TIM7) {
        app.blinky_handler();
    }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */

    /* Infinite loop */
    while (1) {
    }
}

#endif

extern "C" void TIM2_IRQHandler(void) {
    HAL_TIM_IRQHandler(app.timFDCan.get_htim());
}

extern "C" void TIM7_DAC_IRQHandler(void) {
    HAL_TIM_IRQHandler(app.timBlinky.get_htim());
}

/**
 * @brief Receive interupt for the peripheral.
 *
 */
extern "C" void FDCAN1_IT0_IRQHandler(void) {
    HAL_FDCAN_IRQHandler(app.get_handle());
}

/**
 * @brief TX interupt for the FDCan peripheral
 */
extern "C" void FDCAN1_IT1_IRQHandler(void) {
    HAL_FDCAN_IRQHandler(app.get_handle());
}

/**
 * @brief Callback function called when a message is received into FIFO 0.
 *
 * @param hfdcan
 * @param RxFifo0ITs
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hfdcan,
                               uint32_t RxFifo0ITs) {
    // Note: In the future we may need to use RxFifo0ITs depending on filter
    // configuration.
    app.can_rx_callback();
}

/**
 * @brief Callback function called after a message is transmitted on the CAN
 * bus.
 *
 * @param hfdcan
 * @param BufferIndexes
 */
void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef* hfdcan,
                                        uint32_t BufferIndexes) {
    app.can_tx_callback();
}
