/**
 * @file uid.h
 * @author Alex Bohm (bohm0080@umn.edu)
 * @brief This file declares some helpers to get a Unique Identifier for a
 * microcontroller.
 * @version 1.0
 * @date 2021-11-30
 *
 * @copyright Copyright (c) 2021
 *
 */

// Include the register mapping for the appropriate microcontroller.
#pragma once

#include <array>

#include "hal.h"

namespace umnsvp {
namespace uid {
std::array<uint32_t, 3> get_mcu_uid();
}
}  // namespace umnsvp