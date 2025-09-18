/**
 * @file can_packet.h
 * @author Jacob Bunzel (bunze002@umn.edu)
 * @brief CAN packet class.
 * @date 2022-02-04
 */

#pragma once

#include <array>

#include "hal.h"
#include "time_point.h"

namespace umnsvp {
namespace can {

/**
 * @brief Build an array of blank data for a packet.
 */
/// Max size of the data in CAN packet in bytes.
static constexpr uint8_t MAX_SIZE = 8;
static constexpr uint32_t DATA_LENGTH_SCALAR = 0x10000;
constexpr std::array<uint8_t, MAX_SIZE> default_data = {};
using packet_data_t = std::array<uint8_t, MAX_SIZE>;

/**
 * @brief Class to represent a CAN packet.
 */
class packet {
   private:
    /// CAN packet ID.
    uint32_t id;

    /// CAN packet data length in bytes.
    uint8_t length;

    /// CAN packet data stored in array.
    packet_data_t data;

    // Stores whether the ID is standard or extended.
    bool is_extended_id;

    /**
     * @brief The timestamp of the packet
     */
    time::time_point timestamp;

   public:
    packet(const uint32_t id, const uint8_t length, const uint8_t* data,
           const bool is_extended_id = false);
    packet(const uint32_t id = 0, const uint8_t length = MAX_SIZE,
           const packet_data_t data = default_data,
           const bool is_extended_id = false);

#if defined(FDCAN1) || defined(FDCAN2)
    packet(const FDCAN_RxHeaderTypeDef& header, const uint8_t* data);
#else
    packet(const CAN_RxHeaderTypeDef& header, const uint8_t* data);
#endif

    uint32_t get_id() const;
    uint8_t get_length() const;

    const time::time_point& get_timestamp() const;
    void set_timestamp(const time::time_point& timestamp);

#if defined(FDCAN1) || defined(FDCAN2)
    FDCAN_TxHeaderTypeDef get_header() const;
#else
    CAN_TxHeaderTypeDef get_header() const;
#endif

    const uint8_t* get_data() const;
    void get_hex(char* str) const;
};
}  // namespace can
}  // namespace umnsvp
