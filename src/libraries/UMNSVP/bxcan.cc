/**
 * @file bxcan.cc
 * @author Alex Bohm (bohm0080@umn.edu) & Jacob Bunzel (bunze002@umn.edu)
 * @brief bxCAN driver for the STM32L4.
 * @date 2021-11-18
 *
 * @copyright Copyright (c) 2021
 *
 * Refer to the reference manuals linked below. Driver has been tested and is
 * being used on the L4's so far. This driver was written based on the
 * information in the reference manuals.
 *
 * For the STM32L47* MCUs:
 * Section 46.7.4 of RM0351:
 * https://www.st.com/resource/en/reference_manual/rm0351-stm32l47xxx-stm32l48xxx-stm32l49xxx-and-stm32l4axxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
 *
 * For the STM32f405 MCUs:
 * Section 32 of RM0090:
 * https://www.st.com/resource/en/reference_manual/dm00031020-stm32f405-415-stm32f407-417-stm32f427-437-and-stm32f429-439-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf
 */

#if defined(STM32L476xx) || defined(STM32F405xx) || defined(STM32F469xx)

#include "bxcan.h"

namespace umnsvp {
namespace can {

/**
 * @brief Construct a new bxcan driver for the specified peripheral.
 *
 * Bitrate timings:
 * http://www.bittiming.can-wiki.info/
 * http://www.oertel-halle.de/files/cia99paper.pdf
 *
 * @param can_peripheral The CAN peripheral to use.
 */
bxcan_driver::bxcan_driver(CAN_TypeDef* can_peripheral, bool config_isr)
    : config_isr(config_isr) {
    handle.Instance = can_peripheral;

    handle.Init.Mode = CAN_MODE_NORMAL;
    handle.Init.SyncJumpWidth = CAN_SJW_1TQ;

    handle.Init.TimeSeg1 = time_segment_1;
    handle.Init.TimeSeg2 = time_segment_2;

    handle.Init.TimeTriggeredMode = DISABLE;
    handle.Init.AutoBusOff = ENABLE;
    handle.Init.AutoWakeUp = DISABLE;
    handle.Init.AutoRetransmission = ENABLE;
    handle.Init.ReceiveFifoLocked = DISABLE;
    handle.Init.TransmitFifoPriority = ENABLE;
}

/**
 * @brief Initialize the CAN hardware.
 */
HAL_StatusTypeDef bxcan_driver::init(baud_rate rate, bool extended) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Hardcoded prescaler from: http://www.bittiming.can-wiki.info/
    // See @ref baud_ref documentation for more information.
    handle.Init.Prescaler = static_cast<uint32_t>(rate);

    if (handle.Instance == CAN1) {
        // Enable CAN1 clock.
        __HAL_RCC_CAN1_CLK_ENABLE();

        // Enable the GPIO Clock for the
        CAN1_GPIO_CLOCK_ENABLE();

        GPIO_InitStruct.Pin = can1_rx_pin | can1_tx_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = can1_af;

        // Initialize the GPIO pins for CAN1.
        HAL_GPIO_Init(can1_port, &GPIO_InitStruct);
        if (config_isr) {
            HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 1, 0);
            HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
            HAL_NVIC_SetPriority(CAN1_TX_IRQn, 1, 0);
            HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
        }
    }
#ifdef STM32F405xx
    else if (handle.Instance == CAN2) {
        // Enable CAN1 clock.
        __HAL_RCC_CAN2_CLK_ENABLE();
        // Enable GPIOB clock since the CAN pins are on GPIOB.
        CAN2_GPIO_CLOCK_ENABLE();

        GPIO_InitStruct.Pin = can2_rx_pin | can2_tx_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = can2_af;

        // Initialize the GPIO pins for CAN1.
        HAL_GPIO_Init(can2_port, &GPIO_InitStruct);
        if (config_isr) {
            HAL_NVIC_SetPriority(CAN2_RX1_IRQn, 2, 1);
            HAL_NVIC_EnableIRQ(CAN2_RX1_IRQn);
            HAL_NVIC_SetPriority(CAN2_TX_IRQn, 2, 1);
            HAL_NVIC_EnableIRQ(CAN2_TX_IRQn);
        }
    }
#elif STM32F469xx
    else if (handle.Instance == CAN2) {
        // Enable CAN1 and CAN2 clocks.
        __HAL_RCC_CAN1_CLK_ENABLE();
        __HAL_RCC_CAN2_CLK_ENABLE();
        // Enable GPIOB clock since the CAN pins are on GPIOB.
        CAN2_GPIO_CLOCK_ENABLE();

        GPIO_InitStruct.Pin = can2_rx_pin | can2_tx_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = can2_af;

        // Initialize the GPIO pins for CAN1.
        HAL_GPIO_Init(can2_port, &GPIO_InitStruct);
        if (config_isr) {
            HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
        }
    }
#endif
    return HAL_CAN_Init(&handle);
}

/**
 * @brief This method starts the CAN hardware.
 *
 * This should be called after packet filters are applied to the peripheral.
 *
 * @return HAL_StatusTypeDef HAL return status for start.
 */
HAL_StatusTypeDef bxcan_driver::start() {
    HAL_StatusTypeDef start_status = HAL_CAN_Start(&handle);

    HAL_StatusTypeDef notif_status = HAL_OK;

    if (config_isr) {
        if (handle.Instance == CAN1) {
            notif_status = HAL_CAN_ActivateNotification(
                &handle, CAN_IT_RX_FIFO0_MSG_PENDING);
        }
#ifdef STM32F405xx
        else if (handle.Instance == CAN2) {
            notif_status = HAL_CAN_ActivateNotification(
                &handle, CAN_IT_RX_FIFO1_MSG_PENDING);
        }
#elif STM32F469xx
        else if (handle.Instance == CAN2) {
            notif_status = HAL_CAN_ActivateNotification(
                &handle, CAN_IT_RX_FIFO0_MSG_PENDING);
        }
#endif
    }

    if (start_status != HAL_OK)
        return start_status;
    else if (notif_status != HAL_OK) {
        return notif_status;
    } else {
        return HAL_OK;
    }
}

/**
 * @brief Stop the CAN hardware.
 *
 * @return HAL_StatusTypeDef HAL return status for stop.
 */
HAL_StatusTypeDef bxcan_driver::stop() {
    HAL_StatusTypeDef status = HAL_CAN_Stop(&handle);

    return status;
}

/**
 * @brief
 *
 * @param filter_index
 * @param id0
 * @param id1
 * @param id2
 * @param id3
 * @return constexpr CAN_FilterTypeDef
 */
constexpr CAN_FilterTypeDef make_filter_from_ids(std::size_t filter_index,
                                                 uint16_t id0, uint16_t id1,
                                                 uint16_t id2, uint16_t id3) {
    return {
        .FilterIdHigh = static_cast<uint32_t>(id0) << 5,
        .FilterIdLow = static_cast<uint32_t>(id1) << 5,
        .FilterMaskIdHigh = static_cast<uint32_t>(id2) << 5,
        .FilterMaskIdLow = static_cast<uint32_t>(id3) << 5,
        .FilterFIFOAssignment = CAN_RX_FIFO0,
        .FilterBank = filter_index,
        .FilterMode = CAN_FILTERMODE_IDLIST,
        .FilterScale = CAN_FILTERSCALE_16BIT,
        .FilterActivation = ENABLE,
    };
}

/**
 * @brief
 *
 * @param length
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef bxcan_driver::filter_list(const uint32_t* rx_ids,
                                            std::size_t length,
                                            bool is_extended = false) {
    // Check if the quantity of RX ids can fit into the filters
    if (is_extended) {
        return HAL_ERROR;
    }

    constexpr std::size_t filter_count = 13;
    constexpr std::size_t ids_per_filter = 4;

    if (handle.Instance == CAN1) {
        if (length < filter_count * ids_per_filter) {
            std::size_t rx_id_index = 0;
            std::size_t filter_index = 0;

            // Loop through RX ids while they are available and filters are
            // still available
            while (rx_id_index < length) {
                std::array<uint32_t, 4> ids = {
                    rx_ids[rx_id_index], rx_ids[rx_id_index],
                    rx_ids[rx_id_index], rx_ids[rx_id_index]};

                rx_id_index++;

                // Add ids to the array of ids
                for (int j = 1; j < 4 && rx_id_index < length;
                     j++, rx_id_index++) {
                    if (rx_id_index < length) {
                        ids[j] = rx_ids[rx_id_index];
                    }
                }

                // use list of ids to create another filter
                set_filter(make_filter_from_ids(filter_index, ids[0], ids[1],
                                                ids[2], ids[3]));
                filter_index++;
            }

        } else {
            // For now if you need more IDs than this, everything will be
            // filtered
            filter_all();
        }
        return HAL_OK;
    } else {
        // TODO: support other CAN devices
        return HAL_ERROR;
    }
}

/**
 * @brief Apply a filter to the CAN peripheral that accepts all packets.
 *
 * @return HAL_StatusTypeDef HAL return status for configuration.
 */
HAL_StatusTypeDef bxcan_driver::filter_all() {
    // A filter that accepts all packets.
    if (handle.Instance == CAN1) {
        CAN_FilterTypeDef filter = {
            // The lower 3 bits are matched against other CAN
            // packet fields that change packet to packet and should not be
            // matched
            // against. So these are set to 0
            .FilterIdHigh = 0xffff << 5,
            .FilterIdLow = 0xfff8,

            // Require that none of the bits match.
            .FilterMaskIdHigh = 0x0000,
            .FilterMaskIdLow = 0x0000,
            // Put the packets that match this filter into FIFO 0.
            .FilterFIFOAssignment = 0,
            // This is the only filter so use the first filter bank.
            .FilterBank = 0,
            // Use an ID and a Mask to match packet IDs.
            .FilterMode = CAN_FILTERMODE_IDMASK,
            // Use a 32 bit scale to include all the ID's extended or not
            .FilterScale = CAN_FILTERSCALE_32BIT,
            // Enable this filter.
            .FilterActivation = ENABLE,
            // Start the slave filter banks at #14
            .SlaveStartFilterBank = 14,
        };
        // Apply the filter on the hardware.
        return HAL_CAN_ConfigFilter(&handle, &filter);
    } else {
        CAN_FilterTypeDef filter = {
            // The lower 3 bits are matched against other CAN
            // packet fields that change packet to packet and should not be
            // matched
            // against. So these are set to 0
            .FilterIdHigh = 0xffff << 5,
            .FilterIdLow = 0xfff8,

            // Require that none of the bits match.
            .FilterMaskIdHigh = 0x0000,
            .FilterMaskIdLow = 0x0000,
            // Put the packets that match this filter into FIFO 1.
            #ifdef STM32F469xx
            .FilterFIFOAssignment = 0,
            #else
            .FilterFIFOAssignment = 1,
            #endif
            // I like the number 20 and it's above 14
            .FilterBank = 20,
            // Use an ID and a Mask to match packet IDs.
            .FilterMode = CAN_FILTERMODE_IDMASK,
            // Use a 32 bit scale to include all the ID's extended or not
            .FilterScale = CAN_FILTERSCALE_32BIT,
            // Enable this filter.
            .FilterActivation = ENABLE,
            // Start the slave filter banks at #14
            .SlaveStartFilterBank = 14,
        };
        // Apply the filter on the hardware.
        // handle.Instance = CAN1;
        HAL_StatusTypeDef status = HAL_CAN_ConfigFilter(&handle, &filter);
        // handle.Instance = CAN2;

        return status;
    }
}

/**
 * @brief Add a mask or ID list filter for the CAN peripheral.
 *
 * @param filter The id list or mask filter to apply.
 * @return HAL_StatusTypeDef HAL return status for the configuration.
 */
HAL_StatusTypeDef bxcan_driver::set_filter(const CAN_FilterTypeDef& filter) {
    return HAL_CAN_ConfigFilter(&handle,
                                &const_cast<CAN_FilterTypeDef&>(filter));
}

/**
 * @brief Add a CAN packet to the transmit FIFO.
 *
 * NOTE: This does not guarantee transmission nor reception of the packet.
 *
 * @param send_packet The packet to send.
 * @return bxcan_driver::status The status of adding the CAN packet.
 */
status bxcan_driver::send(const packet& send_packet) {
    // Generate the HAL header for the packet.
    CAN_TxHeaderTypeDef header = send_packet.get_header();

    // Mailbox that the message was placed into. Updated by
    // HAL_CAN_AddTxMessage.
    uint32_t mailbox = -1;

    // Attempt to add the message into a transmit mailbox.
    HAL_StatusTypeDef send_status = HAL_CAN_AddTxMessage(
        &handle, &header, const_cast<uint8_t*>(send_packet.get_data()),
        &mailbox);

    // Check the response.
    if (send_status == HAL_OK) {
        return status::OK;
    }

    return status::ERROR;
}

/**
 * @brief Read a packet from the receive FIFO.
 *
 * NOTE: It is not guarenteed that a packed was placed in the passed in output
 * variable. Be sure to check the returned status to know whether a packet was
 * received.
 *
 * @param received_packet The variable to store the received packet in.
 * @param fifo Which FIFO to recieve from. Defaults to 0.
 * @return bxcan_driver::status Whether a packet was read.
 */
status bxcan_driver::receive(packet& received_packet, const fifo fifo) {
    uint32_t RxFifo;

    switch (fifo) {
        case fifo::FIFO0:
            RxFifo = CAN_RX_FIFO0;
            break;
        case fifo::FIFO1:
            RxFifo = CAN_RX_FIFO1;
            break;
        default:
            // If an invalid fifo value is supplied, return an error
            return status::ERROR;
    }

    // Check if packets are available to read.
    uint32_t fill_level = HAL_CAN_GetRxFifoFillLevel(&handle, RxFifo);

    // Return empty if no packets are available to read.
    if (fill_level == 0) {
        return status::EMPTY;
    }

    // Prepare a header and data structure to load a packet into.
    CAN_RxHeaderTypeDef header = {0};
    uint8_t data[8] = {0};

    // Read a packet into the prepared data structures.
    HAL_StatusTypeDef receive_status =
        HAL_CAN_GetRxMessage(&handle, RxFifo, &header, data);

    // Return an error on failure.
    if (receive_status != HAL_OK) {
        return status::ERROR;
    }
    // Update the received packet to the freshly read packet.
    received_packet = packet(header, data);

    return status::OK;
}

/**
 * @brief Get a reference to the CAN HAL handle.
 *
 * NOTE: Be very careful with this handle. Only the telemetry manager and
 * telemetry firmware should be calling this accessor.
 *
 * @return CAN_HandleTypeDef CAN handle.
 */
CAN_HandleTypeDef* bxcan_driver::get_handle() {
    return &handle;
}

void bxcan_driver::enable_tx_it() {
    __HAL_CAN_ENABLE_IT(get_handle(), 0x01);
}

void bxcan_driver::disable_tx_it() {
    __HAL_CAN_DISABLE_IT(get_handle(), 0x01);
}

}  // namespace can
}  // namespace umnsvp

#endif