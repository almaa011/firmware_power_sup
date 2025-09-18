/**
 * @file can_packet.cc
 * @author Jacob Bunzel (bunze002@umn.edu), Maxim Erickson(eric4190@umn.edu)
 * @brief Define CAN packet class methods.
 * @date 2022-10-22
 */

#include "can_packet.h"

#include "cstring"
#include "stdio.h"

namespace umnsvp {
namespace can {

/**
 * @brief Construct a new CAN packet object.
 *
 * This definition is the default constructor as well. See the header for the
 * default values.
 *
 * @param id CAN packet ID.
 * @param length CAN packet length in bytes.
 * @param data CAN packet data array.
 * @param is_extended_id True if the CAN packet ID is extended, false if it is
 * standard.
 */
packet::packet(const uint32_t id, const uint8_t length,
               const packet_data_t data, const bool is_extended_id)
    : id(is_extended_id ? ((uint32_t)id & 0x1FFFFFFF)
                        : ((uint32_t)id & 0b0000011111111111)),
      length(std::min(length, MAX_SIZE)),
      data(data),
      is_extended_id(is_extended_id) {
}

/**
 * @brief Construct a new CAN packet object.
 *
 * @param id CAN packet ID.
 * @param length CAN packet length in bytes.
 * @param data Pointer to CAN packet data.
 * @param is_extended_id True if the CAN packet ID is extended, false if it is
 standard.
 */
packet::packet(uint32_t id, uint8_t length, const uint8_t* data,
               const bool is_extended_id)
    : id(is_extended_id ? ((uint32_t)id & 0x1FFFFFFF)
                        : ((uint32_t)id & 0b0000011111111111)),
      length(std::min(length, MAX_SIZE)),
      is_extended_id(is_extended_id) {
    memcpy(this->data.data(), data, this->length);
}

/**
 * @brief Construct a CAN packet object with header.
 *
 * @param header CAN packet header.
 * @param data Pointer to CAN packet data.
 */
#if defined(FDCAN1)
packet::packet(const FDCAN_RxHeaderTypeDef& header, const uint8_t* data)
    : id((header.IdType == FDCAN_STANDARD_ID)
             ? header.Identifier & 0b0000011111111111
             : header.Identifier & 0x1FFFFFFF),
      length(header.DataLength / DATA_LENGTH_SCALAR) {
    memcpy(this->data.data(), data, this->length);
}
#elif defined(CAN1)
packet::packet(const CAN_RxHeaderTypeDef& header, const uint8_t* data)
    : id((header.IDE == CAN_ID_STD) ? header.StdId : header.ExtId),
      length(header.DLC) {
    memcpy(this->data.data(), data, this->length);
}  // n
#else
#error THIS FILE ONLY WORKS WITH bXcan and FDCAN peripherals
#endif

/**
 * @brief Get CAN packet ID.
 *
 * @return uint16_t ID
 */
uint32_t packet::get_id() const {
    return id;
}

/**
 * @brief Get CAN packet length in bytes.
 *
 * @return uint8_t Length in bytes.
 */
uint8_t packet::get_length() const {
    return length;
}

/**
 * @brief Get the timestamp the packet was received at.
 *
 * @return const time::time_point& The timestamp the packet was received at.
 */
const time::time_point& packet::get_timestamp() const {
    return timestamp;
}

/**
 * @brief Set the timestamp the packet was received at.
 *
 * @param timestamp The timestamp to set as the received timestamp.
 */
void packet::set_timestamp(const time::time_point& timestamp) {
    this->timestamp = timestamp;
}

/**
 * @brief Get the CAN packet header.
 *
 * @return CAN_TxHeaderTypeDef CAN packet header.
 */
#if defined(FDCAN1)
FDCAN_TxHeaderTypeDef packet::get_header() const {
    FDCAN_TxHeaderTypeDef header = {
        .Identifier = id,
        .IdType = is_extended_id ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID,
        .TxFrameType = FDCAN_DATA_FRAME,
        // this is a scalar value of type: FDCAN_data_length_code. If data
        // length is over 8 this simple scalar WILL NOT WORK!!!! TODO:
        .DataLength = length * DATA_LENGTH_SCALAR,
        .FDFormat = FDCAN_CLASSIC_CAN,
    };
    return header;
}
#elif defined(CAN1)
CAN_TxHeaderTypeDef packet::get_header() const {
    CAN_TxHeaderTypeDef header = {
        .StdId = std::min(id, (const uint32_t)0x7FF),
        .ExtId = std::min(id, (const uint32_t)0x1FFFFFFF),
        .IDE = is_extended_id ? CAN_ID_EXT : CAN_ID_STD,
        .RTR = CAN_RTR_DATA,
        .DLC = length,
    };
    return header;
}
#else
#error THIS FILE ONLY WORKS WITH bXcan and FDCAN peripherals
#endif

/**
 * @brief Get pointer to the CAN packet data.
 *
 * @return const uint8_t* Pointer to CAN packet data.
 */
const uint8_t* packet::get_data() const {
    return static_cast<const uint8_t*>(data.data());
}

/**
 * @brief Insert the hexidecimal representation of the payload into a string.
 *
 * This will start at index zero and write two hexidecimal characters per
 * payload byte. A final extra byte is written to null terminate the string.
 *
 * The total number of bytes written will be 2 * get_length() + 1.
 *
 * @param str The string to insert into.
 */
void packet::get_hex(char* str) const {
    for (uint8_t i = 0; i < length; ++i) {
        snprintf(&str[i * 2], 3, "%02X", data[i]);
    }
}

}  // namespace can
}  // namespace umnsvp