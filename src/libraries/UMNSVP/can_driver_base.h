/**
 * @file can_driver_base.h
 * @author Brett Duncan (dunca384@umn.edu)
 * @brief Abstract CAN Driver base class and other related classes.
 * @date 2022-11-12
 */

#pragma once

#if STM32L476xx
#include "stm32l4xx.h"
#elif STM32F405xx || STM32F469xx
#include "stm32f4xx.h"
#elif STM32G474xx || STM32G473xx
#include "stm32g4xx.h"
#elif STM32G0B1xx
#include "stm32g0xx.h"
#elif STM32L562xx
#include "stm32l5xx.h"
#else
#error "can_driver_base.h requires that the microcontroller be defined."
#endif

#include "baud_rate.h"
#include "can_packet.h"

namespace umnsvp {
namespace can {

/**
 * @brief A status class which is returned upon various completed actions
 of
 * the driver such as sent or receiving of packets
 */
enum class status
{
    /**
     * @brief Operation completed successfully.
     */
    OK,
    /**
     * @brief Packet source did not contain packets.
     */
    EMPTY,
    /**
     * @brief Packet destination did not have space for packet.
     */
    FULL,
    /**
     * @brief Operation failed.
     */
    ERROR,
};

/**
 * @brief
 *
 */
enum class fifo : uint32_t
{
    /**
     * @brief Drivers will map FIFO0 to the correct HAL value.
     *
     */
    FIFO0,
    /**
     * @brief Drivers will map FIFO1 to the correct HAL value.
     *
     */
    FIFO1,
};

class can_driver_base {
   public:
    /**
     * @brief Initialize the driver.
     *
     * @return HAL_StatusTypeDef
     */
    virtual HAL_StatusTypeDef init(can::baud_rate baud_rate, bool extended) = 0;

    /**
     * @brief Configure the hardware and enable interrupts.
     *
     * @return HAL_StatusTypeDef
     */
    virtual HAL_StatusTypeDef start() = 0;

    /**
     * @brief Shutdown hardware and disable interrupts.
     *
     * @return HAL_StatusTypeDef
     */
    virtual HAL_StatusTypeDef stop() = 0;

    /**
     * @brief Allow all incoming packets.
     *
     * @return HAL_StatusTypeDef
     */
    virtual HAL_StatusTypeDef filter_all() = 0;

    /**
     * @brief Allow the provided list of packets to be received.
     *
     * @param length
     * @return HAL_StatusTypeDef
     */
    virtual HAL_StatusTypeDef filter_list(const uint32_t*, std::size_t length,
                                          bool is_extended = false) = 0;

    /**
     * @brief Transmit the packet on the CAN bus.
     *
     * @param send_packet
     * @return status
     */
    virtual status send(const packet& send_packet) = 0;

    /**
     * @brief Receive a packet from the CAN bus.
     *
     * @param received_packet
     * @param fifo
     * @return status
     */
    virtual status receive(packet& received_packet,
                           const fifo fifo = fifo::FIFO0) = 0;

    /**
     * @brief Enable the CAN transmit interrupt.
     *
     */
    virtual void enable_tx_it() = 0;

    /**
     * @brief Disable the CAN transmit interrupt.
     *
     */
    virtual void disable_tx_it() = 0;
};

}  // namespace can
}  // namespace umnsvp