#include <array>

#include "can_driver_base.h"
#include "can_packet.h"
#include "circular_buffer.h"
#include "skylab2_packets.h"
#include "triple_buffer.h"

#pragma once

namespace umnsvp {
namespace skylab2 {

class can_base {
    // The can_base class will be the base class for the can board classes and
    // will be used as a reference skylab2_boards.cc
   private:
    can::fifo fifo;
    can::can_driver_base& can_device;

    umnsvp::circular_buffer::CircularBuffer<can::packet, 75> tx_buffer;

   protected:
    can_base(can::can_driver_base& can_driver_ref, can::fifo fifo)
        : fifo(fifo), can_device(can_driver_ref) {
    }

   public:
    void init(can::baud_rate baud_rate, bool extended, const uint32_t* rx_ids,
              size_t length);
    can::status send_packet(can::packet packet);
    can::status receive(can::packet& received_packet);
    void tx_handler();
    void setup_filter(const uint32_t* rx_ids, size_t length);
};

}  // namespace skylab2
}  // namespace umnsvp