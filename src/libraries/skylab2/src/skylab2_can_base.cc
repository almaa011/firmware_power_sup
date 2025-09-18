
#include "can_driver_base.h"
#include "skylab2_busses.h"

namespace umnsvp {
namespace skylab2 {

/**
 * @brief
 *
 * @param baud_rate
 * @param extended
 */
void can_base::init(can::baud_rate baud_rate, bool extended,
                    const uint32_t* rx_ids, size_t length) {
    HAL_StatusTypeDef stat1 = can_device.init(baud_rate, extended);
    setup_filter(rx_ids, length);
    HAL_StatusTypeDef stat2 = can_device.start();
    int c = 1;
}

/**
 * @brief send the given packet to the can device and push
 * the packet if tx_buffer is full
 *
 * @param packet
 * @return can::status
 */
can::status can_base::send_packet(can::packet packet) {
    can::status result = can_device.send(packet);
    if (result != can::status::OK) {
        tx_buffer.push(packet);
        can_device.enable_tx_it();
    }
    return result;
}

/**
 * @brief receive a packet from the given FIFO ;
 * for now, only using FIFO0 to all packets
 *
 * @param received_packet
 * @return can::status
 */
can::status can_base::receive(can::packet& received_packet) {
    // default the fifo to 0
    can::status result = can_device.receive(received_packet, fifo);
    // TODO: FIFO needs checking
    return result;
}

/**
 * @brief tx_handler for the can_base class
 *
 */
void can_base::tx_handler() {
    // this function may be blocking until the queue is cleared
    if (tx_buffer.peek()) {  // peek don't pop in case sending fails again
        can::status result = can_device.send(tx_buffer.output());
        if (result == can::status::OK) {
            tx_buffer.pop();  // if we were successful it's okay to pop now
        }
    } else {
        can_device.disable_tx_it();
    }
}

/**
 * @brief Call the CAN driver specific filter_list() function
 * and set up the filters of the specific CAN board class
 *
 * @param rx_ids
 * @param length
 */
void can_base::setup_filter(const uint32_t* rx_ids, size_t length) {
    can_device.filter_list(rx_ids, length);
}

}  // namespace skylab2
}  // namespace umnsvp