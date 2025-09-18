#pragma once

#include "hal.h"

#if STM32L476xx || STM32F405xx || STM32F469xx

#include "baud_rate.h"
#include "can_driver_base.h"
#include "can_packet.h"

namespace umnsvp {
namespace can {

/**
 * @brief Pin configuration for STM32L476 CAN1 peripheral.
 *
 * These were added to help document and to start to prepare for needing
 * configurations for multiple micros.
 *
 * NOTE: The GPIO port cannot be constexpr due to the reinterpret cast in the
 * HAL. The GPIO port is a magic number in the driver, please update throughout
 * when needed.
 *
 * @{
 */

#ifdef STM32L476xx
constexpr auto can1_rx_pin = GPIO_PIN_8;
constexpr auto can1_tx_pin = GPIO_PIN_9;
constexpr auto can1_af = GPIO_AF9_CAN1;
GPIO_TypeDef* const can1_port = GPIOB;

#define CAN1_GPIO_CLOCK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE
#elif STM32F405xx
constexpr auto can1_rx_pin = GPIO_PIN_11;
constexpr auto can1_tx_pin = GPIO_PIN_12;
constexpr auto can1_af = GPIO_AF9_CAN1;
GPIO_TypeDef* const can1_port = GPIOA;

constexpr auto can2_rx_pin = GPIO_PIN_12;
constexpr auto can2_tx_pin = GPIO_PIN_13;
constexpr auto can2_af = GPIO_AF9_CAN2;
GPIO_TypeDef* const can2_port = GPIOB;

#define CAN1_GPIO_CLOCK_ENABLE __HAL_RCC_GPIOA_CLK_ENABLE
#define CAN2_GPIO_CLOCK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE

#elif STM32F469xx
// Note that CAN1 cannot be used on the STM32F469i-disco development
// board that we use as the display dash, since those pins are taken up
// by display related peripherals.
constexpr auto can1_rx_pin = GPIO_PIN_11;
constexpr auto can1_tx_pin = GPIO_PIN_12;
constexpr auto can1_af = GPIO_AF9_CAN1;
GPIO_TypeDef* const can1_port = GPIOA;

constexpr auto can2_rx_pin = GPIO_PIN_5;
constexpr auto can2_tx_pin = GPIO_PIN_13;
constexpr auto can2_af = GPIO_AF9_CAN2;
GPIO_TypeDef* const can2_port = GPIOB;

#define CAN1_GPIO_CLOCK_ENABLE __HAL_RCC_GPIOA_CLK_ENABLE
#define CAN2_GPIO_CLOCK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE

#else
#error "bxcan.h requires that the microcontroller be defined."
#endif

/**
 * @}
 */

/**
 * @brief A driver for the STM32 bxCAN peripheral.
 *
 * This driver currently is a loose wrapper around the HAL driver.
 */
class bxcan_driver : public can_driver_base {
   private:
    /**
     * @brief The HAL CAN handle for interacting with the hardware.
     */
    CAN_HandleTypeDef handle;

    const bool config_isr;

   public:
    /**
     * @brief Hardcoded Time Segment 1 and 2.
     *
     * Calculated from: http://www.bittiming.can-wiki.info/
     * The inputs for the calulator were:
     * Clock Rate = 80MHz
     * Sample Point = 87.5%
     * SJW = 1
     *
     * NOTE: If the clock frequency to the CAN peripheral changes, the
     * prescalers and time segment values will need to be updated.
     *
     * @{
     */

    #ifdef STM32F469xx
    static constexpr auto time_segment_1 = CAN_BS1_15TQ;
    static constexpr auto time_segment_2 = CAN_BS2_2TQ;
    #else
    static constexpr auto time_segment_1 = CAN_BS1_13TQ;
    static constexpr auto time_segment_2 = CAN_BS2_2TQ;
    #endif

    /**
     * @}
     */

    /**
     * Number of filter banks in STM32L4 CAN Peripheral.
     */
    static constexpr auto num_filter_banks = 14;

    /**
     * Number of IDs that can be listed in a filter bank when using 16-bit
     * filter scale and a ID-list filter.
     */
    static constexpr auto id_filter_bank_cnt = 4;

    /**
     * Max number of IDs that could fit in the 16-bit scale ID-list filters.
     */
    static constexpr auto max_num_filter_ids =
        id_filter_bank_cnt * num_filter_banks;

    bxcan_driver(CAN_TypeDef* can_peripheral, bool config_isr = true);

    virtual HAL_StatusTypeDef init(baud_rate rate, bool extended) override;

    virtual HAL_StatusTypeDef start() override;
    virtual HAL_StatusTypeDef stop() override;

    virtual HAL_StatusTypeDef filter_all() override;
    virtual HAL_StatusTypeDef filter_list(const uint32_t*, std::size_t length,
                                          bool is_extended) override;

    virtual status send(const packet& send_packet) override;
    virtual status receive(packet& received_packet,
                           const fifo fifo = fifo::FIFO0) override;

    virtual void enable_tx_it() override;
    virtual void disable_tx_it() override;

    CAN_HandleTypeDef* get_handle();
    HAL_StatusTypeDef set_filter(const CAN_FilterTypeDef& filter);
};

}  // namespace can
}  // namespace umnsvp

#endif
