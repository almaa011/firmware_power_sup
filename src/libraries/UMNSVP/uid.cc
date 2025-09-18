/**
 * @file uid.cc
 * @author Alex Bohm (bohm0080@umn.edu)
 * @brief This file defines some helpers to get a Unique Identifier for a
 * microcontroller.
 * @version 1.0
 * @date 2021-11-30
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "uid.h"

namespace umnsvp {
namespace uid {
/**
 * @brief Get the unique identifier (UID) for the current microcontroller.
 *
 * For STM32 targets this is a 96 bit number.
 *
 * STM32L4:
 *  See section 49.1 of reference manual RM0351 (Rev 9).
 * STM32L5:
 *  See section 53.1 of reference manual RM0438 (Rev 7).
 *
 * @return std::array<uint32_t, 3>
 */
std::array<uint32_t, 3> get_mcu_uid() {
    // The STM32 has a 96 bit unique id register located at UID_BASE.
    std::array<uint32_t, 3> output = {
        *reinterpret_cast<uint32_t*>(UID_BASE + sizeof(uint32_t) * 0),
        *reinterpret_cast<uint32_t*>(UID_BASE + sizeof(uint32_t) * 1),
        *reinterpret_cast<uint32_t*>(UID_BASE + sizeof(uint32_t) * 2),
    };
    return output;
}  // namespace uid

}  // namespace uid
}  // namespace umnsvp
