
#pragma once

#include <stdint.h>

namespace umnsvp {
namespace can {

/**
 * @brief CAN baud rates (Kb/s) available and their corresponding prescaler.
 *
 * These prescalers and the corresponding CAN config settings were
 * calculated at: http://www.bittiming.can-wiki.info/
 *
 * The inputs for the calulator for the L4 were:
 * Clock Rate = 80MHz
 * Sample Point = 87.5%
 * SJW = 1
 *
 * The inputs for the calulator for the G4 were:
 * Clock Rate = 160MHz
 * Sample Point = 87.5%
 * SJW = 1
 *
 * The inputs for the calulator for the G4 were:
 * Clock Rate = 64MHz
 * Sample Point = 87.5%
 * SJW = 1
 *
 * NOTE: If the clock frequency to the CAN peripheral changes, the
 * prescalers and time segment values will need to be updated.
 *
 * NOTE: for the stm32f405 CAN1 and CAN2 use the APB1 bus
 */
#if defined(STM32G474xx) || defined(STM32G473xx)
// Based off of a 160Mhz clock freq for the CAN peripheral
// ON the G4's this is not necessary the AHB Frequency
enum class baud_rate : uint32_t
{
    BAUD_RATE_1000 = 10UL,
    BAUD_RATE_500 = 20UL,
    BAUD_RATE_250 = 40UL,
    BAUD_RATE_125 = 80UL
};
#elif defined(STM32L476xx)
enum class baud_rate : uint32_t
{
    BAUD_RATE_1000 = 5UL,
    BAUD_RATE_500 = 10UL,
    BAUD_RATE_250 = 20UL,
    BAUD_RATE_125 = 40UL,
    BAUD_RATE_100 = 50UL,
};
#elif defined(STM32F405xx)
enum class baud_rate : uint32_t
{
    // BAUD_RATE_1000 =
    //     4UL,  // THIS RATE WILL NOT WORK WITHOUT A TIME QUANTA CHANGE
    BAUD_RATE_500 = 5UL,
    BAUD_RATE_250 = 10UL,
    BAUD_RATE_125 = 20UL,
    BAUD_RATE_100 = 25UL,
};
#elif defined(STM32F469xx)
enum class baud_rate : uint32_t
{
    // BAUD_RATE_1000 =
    //     3UL,  // THIS RATE WILL NOT WORK WITHOUT A TIME QUANTA CHANGE
    BAUD_RATE_500 = 5UL,
    BAUD_RATE_250 = 10UL,
    BAUD_RATE_125 = 20UL,
    BAUD_RATE_100 = 25UL,
};
#elif defined(STM32L562xx)
// Based off of a 96Mhz clock freq for the CAN peripheral
// The L5s use the primary PLL Q output which drives the main clock (96MHz)
enum class baud_rate : uint32_t
{
    BAUD_RATE_1000 = 6UL,
    BAUD_RATE_500 = 12UL,
    BAUD_RATE_250 = 24UL,
    BAUD_RATE_125 = 48UL
};
#elif defined(STM32G0B1xx)
// Based off of a 64Mhz clock freq for the CAN peripheral
enum class baud_rate : uint32_t
{
    BAUD_RATE_1000 = 4UL,
    BAUD_RATE_500 = 8UL,
    BAUD_RATE_250 = 16UL,
    BAUD_RATE_125 = 32UL
};
#else
#error "baud_rate.h requires that the microcontroller be defined."
#endif

}  // namespace can
}  // namespace umnsvp
