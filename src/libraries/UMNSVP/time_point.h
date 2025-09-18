
#pragma once
#include <stdint.h>

namespace umnsvp {
namespace time {
class time_point {
   public:
    uint16_t year = 0;
    uint8_t month = 0;
    uint8_t day = 0;
    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;
    uint16_t millisecond = 0;

    time_point() = default;
};

}  // namespace time
}  // namespace umnsvp