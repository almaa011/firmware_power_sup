#pragma once

#include <stdint.h>

#include <array>
#include <utility>

namespace umnsvp {
constexpr float MIN_CELL_VOLTAGE = 2.7;  // volts
constexpr uint16_t NUM_SERIES_CELLS = 36;
constexpr float MAX_CELL_VOLTAGE = 4.15;

namespace charger {
constexpr float CELL_CHARGE_TARGET_VOLTAGE = 4.14;  // volts

}  // namespace charger

// constexpr float BATTERY_VOLTAGE_MAX =
//     charger::CELL_CHARGE_TARGET_VOLTAGE * NUM_SERIES_CELLS;  // volts

constexpr float BATTERY_VOLTAGE_MAX = 150.0;              // volts
constexpr float BATTERY_VOLTAGE_CHARGING_TARGET = 149.0;  // volts

constexpr float CHARGING_TEMP_LIMIT_FOR_BATTERY = 60.0;  // Celsius
constexpr float BATTERY_VOLTAGE_MIN =
    (MIN_CELL_VOLTAGE * NUM_SERIES_CELLS);  // volts

constexpr float BATTERY_CURRENT_CHARGING_TARGET =
    1.0;  // used to compare if the charging current is less than 1 to enter
          // charging_done state
}  // namespace umnsvp