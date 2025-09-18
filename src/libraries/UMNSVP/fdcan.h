/**
 * @file fdcan.h
 * @author Maxim Erickson (eric4190@umn.edu)
 * @brief fdcan driver for peripheral found on g0's, g4's, and l5's
 * @version 1
 * @date 2022-10-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

// Include Header to prevent compilation with non FDCAN micro's
#include "hal.h"
#if defined(STM32G4) || defined(STM32L5) || defined(STM32G0)

#include "baud_rate.h"
#include "can_driver_base.h"
#include "can_packet.h"
namespace umnsvp {
namespace can {

/**
 * @brief Struct for storing the pins and port used by the FDCAN peripheral.
 *
 */
struct fdcan_gpio {
    const uint16_t rx_pin;
    const uint16_t tx_pin;
    GPIO_TypeDef* const port;
};

class fdcan_driver : public can_driver_base {
   private:
    FDCAN_HandleTypeDef handle;

    const fdcan_gpio gpio;

    bool config_isr;

    HAL_StatusTypeDef clock_enable() const;

    /**
     * Number of filter banks a single FIFO can use.
     */
    static constexpr auto num_filter_banks = 14;

    /**
     * Number of IDs that can be listed in a filter bank when using 16-bit
     * filter scale and a ID-list filter.
     */
    static constexpr auto extended_num_filter_banks = 4;

    /**
     * Number of IDs that can be listed in a filter bank when using
     * extended IDs.
     */
    static constexpr auto id_filter_bank_cnt = 2;

    /**
     * Max number of IDs that could fit in the 16-bit scale ID-list filters.
     */
    static constexpr auto max_num_filter_ids =
        id_filter_bank_cnt * num_filter_banks;

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
    static constexpr auto time_segment_1 = 13;
    static constexpr auto time_segment_2 = 2;

   public:
    fdcan_driver(FDCAN_GlobalTypeDef* can_peripheral, fdcan_gpio gpio,
                 bool config_isr = true);

    virtual HAL_StatusTypeDef init(baud_rate baud_rate, bool extended) override;

    virtual HAL_StatusTypeDef start() override;
    virtual HAL_StatusTypeDef stop() override;

    virtual HAL_StatusTypeDef filter_all() override;
    virtual HAL_StatusTypeDef filter_list(const uint32_t*, std::size_t length,
                                          bool is_extended = false) override;

    virtual status send(const packet& send_packet) override;
    virtual status receive(packet& received_packet,
                           const fifo fifo = fifo::FIFO0) override;

    virtual void enable_tx_it() override;
    virtual void disable_tx_it() override;

    FDCAN_HandleTypeDef* get_handle();
    HAL_StatusTypeDef set_filter(const FDCAN_FilterTypeDef& filter);
};

}  // namespace can
}  // namespace umnsvp

#endif