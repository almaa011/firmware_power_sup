/**
 * @file fdcan.cc
 * @author Maxim Erickson
 * @brief fdcan driver for the fdcan peripheral on g0, g4, and l5's
 * Heavy influence from the BXcan driver used in F1 as the driver codes are
 * written to be compatible
 * @version 1
 * @date 2022-10-12
 *
 * @copyright Copyright (c) 2022
 *
 */

// Include Header to prevent compilation with non FDCAN micro's
#include "fdcan.h"

#if defined(STM32G4) || defined(STM32G0)

namespace umnsvp {
namespace can {

/**
 * @brief Construct a new fdcan driver::fdcan driver object. This sets basic bit
 * timing and other CAN basics Bit timings must be specified for both data and
 * normal operational timings although they are currently set to the same
 *
 * @param can_peripheral
 * @param rate
 * @param config_isr
 */
fdcan_driver::fdcan_driver(FDCAN_GlobalTypeDef* can_peripheral, fdcan_gpio gpio,
                           bool config_isr)
    : gpio(gpio), config_isr(config_isr) {
    handle.Instance = can_peripheral;

    handle.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    handle.Init.Mode = FDCAN_MODE_NORMAL;
    handle.Init.NominalSyncJumpWidth = 1;
    handle.Init.NominalTimeSeg1 = time_segment_1;
    handle.Init.NominalTimeSeg2 = time_segment_2;

    handle.Init.DataSyncJumpWidth = 1;
    handle.Init.DataTimeSeg1 = time_segment_1;
    handle.Init.DataTimeSeg2 = time_segment_2;

    /* This is stored in RAM and can be any number between 0 and 28 */
    handle.Init.StdFiltersNbr = num_filter_banks;
    handle.Init.ExtFiltersNbr = extended_num_filter_banks;

    handle.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    // handle.Init.ProtocolException = ENABLE;  // We should probably have this
    handle.Init.AutoRetransmission = ENABLE;
}

/**
 * @brief Enable the GPIO clock for the CAN peripherial's GPIO port.
 *
 * @return HAL_StatusTypeDef HAL_OK if the clock has been enabled, HAL_ERROR if
 * the port is invalid.
 */
HAL_StatusTypeDef fdcan_driver::clock_enable() const {
    if (gpio.port == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        return HAL_OK;
    } else if (gpio.port == GPIOD) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
        return HAL_OK;
    } else if (gpio.port == GPIOB) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
        return HAL_OK;
    } else {
        return HAL_ERROR;
    }
}

/**
 * @brief Initialize the CAN hardware.
 * Sets up the clk for the peripheral and also enables the interupt vectors but
 * NOT the interupt sources.
 */
HAL_StatusTypeDef fdcan_driver::init(baud_rate rate, bool extended) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PCLK1;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
    __HAL_RCC_FDCAN_CLK_ENABLE();

    // Hardcoded prescaler from: http://www.bittiming.can-wiki.info/
    // See @ref baud_ref documentation for more information.
    handle.Init.NominalPrescaler = static_cast<uint32_t>(rate);
    handle.Init.DataPrescaler = static_cast<uint32_t>(rate);

    // Currently useless check but will be used when FDCAN2 & 3 are added
    if (handle.Instance == FDCAN1) {
        // Enable CAN1 clock.

        // Enable the GPIO Clock for the gpio pins
        HAL_StatusTypeDef clock_status = clock_enable();
        if (clock_status != HAL_OK) {
            return clock_status;
        }

// alternate function configuration
#if defined(STM32G4)
        const uint8_t af = GPIO_AF9_FDCAN1;
#elif defined(STM32G0)
        const uint8_t af = GPIO_AF3_FDCAN1;
#endif

        GPIO_InitStruct.Pin = gpio.rx_pin | gpio.tx_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = af;

        // Initialize the GPIO pins for CAN1.
        HAL_GPIO_Init(gpio.port, &GPIO_InitStruct);
        if (config_isr) {
// Set and enable the interupts
#if defined(STM32G4)
            HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 1, 0);
            HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
            HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 1, 0);
            HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
#elif defined(STM32G0)  // NOTE: FDCAN1 and TWO Share interupts on G0!!!!
            HAL_NVIC_SetPriority(TIM16_FDCAN_IT0_IRQn, 1, 0);
            HAL_NVIC_EnableIRQ(TIM16_FDCAN_IT0_IRQn);
            HAL_NVIC_SetPriority(TIM17_FDCAN_IT1_IRQn, 1, 0);
            HAL_NVIC_EnableIRQ(TIM17_FDCAN_IT1_IRQn);
#endif
        }
    } else if (handle.Instance == FDCAN2) {
        // Enable CAN2 clock.

        // Enable the GPIO Clock for the gpio pins
        HAL_StatusTypeDef clock_status = clock_enable();
        if (clock_status != HAL_OK) {
            return clock_status;
        }

// alternate function configuration
#if defined(STM32G4)
        const uint8_t af = GPIO_AF9_FDCAN2;
#elif defined(STM32G0)
        const uint8_t af = GPIO_AF3_FDCAN2;
#endif

        GPIO_InitStruct.Pin = gpio.rx_pin | gpio.tx_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = af;

        // Initialize the GPIO pins for CAN2.
        HAL_GPIO_Init(gpio.port, &GPIO_InitStruct);
        if (config_isr) {
// Set and enable the interupts
#if defined(STM32G4)
            HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 1, 1);
            HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
            HAL_NVIC_SetPriority(FDCAN2_IT1_IRQn, 1, 1);
            HAL_NVIC_EnableIRQ(FDCAN2_IT1_IRQn);
#elif defined(STM32G0)  // NOTE: FDCAN1 and TWO Share interupts on G0!!!!
            HAL_NVIC_SetPriority(TIM16_FDCAN_IT0_IRQn, 1, 0);
            HAL_NVIC_EnableIRQ(TIM16_FDCAN_IT0_IRQn);
            HAL_NVIC_SetPriority(TIM17_FDCAN_IT1_IRQn, 1, 0);
            HAL_NVIC_EnableIRQ(TIM17_FDCAN_IT1_IRQn);
#endif
        }

    } else {
        // Return ERROR for invalid can peripherals. (FDCAN3 is not supported.)
        return HAL_ERROR;
    }
    return HAL_FDCAN_Init(&handle);
}

/**
 * @brief This method starts the CAN hardware.
 *
 * This should be called after packet filters are applied to the peripheral.
 * The function sets up the interupt sources for the peripheral. TX FIFO empty
 * and RX element received interupts are currently set
 *
 * @return HAL_StatusTypeDef HAL return status for start.
 */
HAL_StatusTypeDef fdcan_driver::start() {
    // Tell the peripheral to discard any non mathing ID's
    HAL_FDCAN_ConfigGlobalFilter(&handle, FDCAN_REJECT, FDCAN_REJECT,
                                 FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);

    if (handle.Instance == FDCAN1) {
        // Configure the interupts. This is done even if config ISR is false bc
        // it doesn't matter Configure interupt vector 0 for the Received new
        // message intupt. The last input on the first function
        // (FDCAN_TX_BUFFER0) is ignored bc not aplicible
        HAL_StatusTypeDef FIFO0_inter_status = HAL_FDCAN_ConfigInterruptLines(
            &handle, FDCAN_IT_GROUP_RX_FIFO0, FDCAN_INTERRUPT_LINE0);
        HAL_StatusTypeDef FIFO0_notif_status = HAL_FDCAN_ActivateNotification(
            &handle, FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE, 0);

        // Configure interupt vector 1 for the transmit empty intupt. The
        // last input is ignored bc not aplicible (FDCAN_TX_BUFFER0)
        HAL_StatusTypeDef tx_inter_status = HAL_FDCAN_ConfigInterruptLines(
            &handle, FDCAN_IT_GROUP_TX_FIFO_ERROR, FDCAN_INTERRUPT_LINE1);
        HAL_StatusTypeDef tx_notif_status = HAL_FDCAN_ActivateNotification(
            &handle, FDCAN_FLAG_TX_FIFO_EMPTY, FDCAN_TX_BUFFER0);

        HAL_StatusTypeDef start_status = HAL_FDCAN_Start(&handle);

        if (start_status == HAL_OK && FIFO0_notif_status == HAL_OK &&
            FIFO0_inter_status == HAL_OK && tx_notif_status == HAL_OK &&
            tx_inter_status == HAL_OK) {
            return HAL_OK;
        }
    } else if (handle.Instance == FDCAN2) {
        // Configure the interupts. This is done even if config ISR is false bc
        // it doesn't matter Configure interupt vector 0 for the Received new
        // message intupt. The last input on the first function
        // (FDCAN_TX_BUFFER0) is ignored bc not aplicible
        HAL_StatusTypeDef FIFO0_inter_status = HAL_FDCAN_ConfigInterruptLines(
            &handle, FDCAN_IT_GROUP_RX_FIFO0, FDCAN_INTERRUPT_LINE0);
        HAL_StatusTypeDef FIFO0_notif_status = HAL_FDCAN_ActivateNotification(
            &handle, FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE, 0);

        // Configure interupt vector 1 for the transmit empty intupt. The
        // last input is ignored bc not aplicible (FDCAN_TX_BUFFER0)
        HAL_StatusTypeDef tx_inter_status = HAL_FDCAN_ConfigInterruptLines(
            &handle, FDCAN_IT_GROUP_TX_FIFO_ERROR, FDCAN_INTERRUPT_LINE1);
        HAL_StatusTypeDef tx_notif_status = HAL_FDCAN_ActivateNotification(
            &handle, FDCAN_FLAG_TX_FIFO_EMPTY, FDCAN_TX_BUFFER0);

        HAL_StatusTypeDef start_status = HAL_FDCAN_Start(&handle);

        if (start_status == HAL_OK && FIFO0_notif_status == HAL_OK &&
            FIFO0_inter_status == HAL_OK && tx_notif_status == HAL_OK &&
            tx_inter_status == HAL_OK) {
            return HAL_OK;
        }
    }
    return HAL_ERROR;
}

/**
 * @brief Stop the CAN hardware.
 *
 * @return HAL_StatusTypeDef HAL return status for stop.
 */
HAL_StatusTypeDef fdcan_driver::stop() {
    HAL_StatusTypeDef status = HAL_FDCAN_Stop(&handle);
    return status;
}

/**
 * @brief Build FDCAN filter with the filter index and the IDs to filter.
 *
 * @param filter_index
 * @param id1
 * @param id2
 * @return constexpr FDCAN_FilterTypeDef
 */
FDCAN_FilterTypeDef make_filter_from_ids(std::size_t filter_index, uint32_t id1,
                                         uint32_t id2,
                                         bool is_extended = false) {
    if (is_extended) {
        return {
            .IdType = FDCAN_EXTENDED_ID,  // Filter Standard ID's
            .FilterIndex =
                filter_index,  // Only filter so put this in the first bank
            .FilterType = FDCAN_FILTER_DUAL,  // Make it a dual discrete ID type
            // TODO: in the future we will want to explicitly provide the
            // FIFO
            .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,  // Put this in FIFO0
            .FilterID1 = id1,
            .FilterID2 = id2};
    } else {
        return {
            .IdType = FDCAN_STANDARD_ID,  // Filter Standard ID's
            .FilterIndex =
                filter_index,  // Only filter so put this in the first bank
            .FilterType = FDCAN_FILTER_DUAL,  // Make it a dual discrete ID type
            // TODO: in the future we will want to explicitly provide the
            // FIFO
            .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,  // Put this in FIFO0
            .FilterID1 = id1,
            .FilterID2 = id2};
    }
}

/**
 * @brief Setup filters for the provided list of IDs.
 *
 * @param rx_ids
 * @param length
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef fdcan_driver::filter_list(const uint32_t* rx_ids,
                                            std::size_t length,
                                            bool is_extended) {
    if (length == 0) {
        // return because you didin't input a value
        return HAL_StatusTypeDef::HAL_OK;
    } else if (is_extended) {
        if (length > handle.Init.ExtFiltersNbr * id_filter_bank_cnt) {
            // If too many inputs just filter all the ID's
            return filter_all();
        }
    } else {
        if (length > handle.Init.StdFiltersNbr * id_filter_bank_cnt) {
            // If too many inputs just filter all the ID's
            return filter_all();
        }
    }

    FDCAN_FilterTypeDef filter;

    std::size_t filter_index = 0;
    std::size_t can_id_filter_index = 0;
    while (can_id_filter_index < length) {
        if (can_id_filter_index <= length - 2) {
            filter = make_filter_from_ids(
                filter_index, rx_ids[can_id_filter_index],
                rx_ids[can_id_filter_index + 1], is_extended);
            can_id_filter_index += 2;
        } else {
            // After this you are done since it's the last ID by
            // definition of entering this branch
            filter =
                make_filter_from_ids(filter_index, rx_ids[can_id_filter_index],
                                     rx_ids[can_id_filter_index], is_extended);
            can_id_filter_index += 1;
        }

        // Apply the filter
        HAL_StatusTypeDef filter_status =
            HAL_FDCAN_ConfigFilter(&handle, &filter);
        if (filter_status == HAL_StatusTypeDef::HAL_OK) {
            // If succesful, increment to the next filter
            filter_index += 1;
        } else {
            // Return an error if it occurs
            return filter_status;
        }
    }
    // Reached the end of the ID's without an error so return true
    return HAL_OK;
}

/**
 * @brief Apply a filter to the CAN peripheral that accepts all STANDARD
 * ID packets. NOT currently set up for extended ID's
 *  TODO: Set up for extended ID's
 *
 * @return HAL_StatusTypeDef HAL return status for configuration.
 */
HAL_StatusTypeDef fdcan_driver::filter_all() {
    // A filter that accepts all packets.
    // TODO: Update to work for extended ID's
    FDCAN_FilterTypeDef filterStd = {
        .IdType = FDCAN_STANDARD_ID,  // Filter Standard ID's
        .FilterIndex = 0,  // Only filter so put this in the first bank
        .FilterType = FDCAN_FILTER_MASK,          // Make it a mask type
        .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,  // Put this in FIFO0
        .FilterID1 = 0x0000,                      // Mask all
        .FilterID2 = 0x0000,                      // Mask all
    };
    FDCAN_FilterTypeDef filterExt = {
        .IdType = FDCAN_EXTENDED_ID,  // Filter Extended ID's
        .FilterIndex = 0,  // Only filter so put this in the first bank
        .FilterType = FDCAN_FILTER_MASK,          // Make it a mask type
        .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,  // Put this in FIFO0
        .FilterID1 = 0x0000,                      // Mask all
        .FilterID2 = 0x0000,                      // Mask all
    };

    HAL_StatusTypeDef filterStdHal =
        HAL_FDCAN_ConfigFilter(&handle, &filterStd);
    HAL_StatusTypeDef filterExtHal =
        HAL_FDCAN_ConfigFilter(&handle, &filterExt);

    if (filterStdHal == HAL_OK && filterExtHal == HAL_OK) {
        return HAL_OK;
    } else {
        return HAL_ERROR;
    }
    // Apply the filter on the hardware.
}

/**
 * @brief Add a mask or ID list filter for the CAN peripheral.
 *
 * @param filter The id list or mask filter to apply.
 * @return HAL_StatusTypeDef HAL return status for the configuration.
 */
HAL_StatusTypeDef fdcan_driver::set_filter(const FDCAN_FilterTypeDef& filter) {
    return HAL_FDCAN_ConfigFilter(&handle,
                                  &const_cast<FDCAN_FilterTypeDef&>(filter));
}

/**
 * @brief Add a CAN packet to the transmit FIFO.
 *
 * NOTE: This does not guarantee transmission nor reception of the
 * packet. The can driver has autotransmission set so every effort will
 * happen to send the packets The TX buffer is a FIFO and **NOT** a
 * Queue. This means no matter the ID, the first packet in will be the
 * first one sent out
 *
 * @param send_packet The packet to send.
 * @return bxcan_driver::status The status of adding the CAN packet.
 */
status fdcan_driver::send(const packet& send_packet) {
    // Generate the HAL header for the packet.
    FDCAN_TxHeaderTypeDef header = send_packet.get_header();

    // Attempt to add the message into a transmit mailbox.
    // TODO: Double check inputs to this to make sure structs line up
    // TODO: We should not be using const_cast like this.
    handle.Instance->CCCR &= ~(0x01);
    HAL_StatusTypeDef send_status = HAL_FDCAN_AddMessageToTxFifoQ(
        &handle, &header, const_cast<uint8_t*>(send_packet.get_data()));

    // Check the response.
    if (send_status == HAL_OK) {
        return status::OK;
    }

    return status::ERROR;
}

/**
 * @brief Read a packet from the receive FIFO.
 *
 * NOTE: It is not guarenteed that a packed was placed in the passed in
 * output variable. Be sure to check the returned status to know whether
 * a packet was received.
 *
 * @param received_packet The variable to store the received packet in.
 * @param fifo Which FIFO to recieve from. Defaults to 0.
 * @return bxcan_driver::status Whether a packet was read.
 */
status fdcan_driver::receive(packet& received_packet, const fifo fifo) {
    uint32_t RxFifo;

    switch (fifo) {
        case fifo::FIFO0:
            RxFifo = FDCAN_RX_FIFO0;
            break;
        case fifo::FIFO1:
            RxFifo = FDCAN_RX_FIFO1;
            break;
        default:
            // If an invalid fifo value is supplied, return an error
            return status::ERROR;
    }

    // Check if packets are available to read.
    const uint32_t fill_level = HAL_FDCAN_GetRxFifoFillLevel(&handle, RxFifo);

    // Return empty if no packets are available to read.
    if (fill_level == 0) {
        return status::EMPTY;
    }

    // Prepare a header and data structure to load a packet into.
    FDCAN_RxHeaderTypeDef header = {0};
    uint8_t data[8] = {0};

    // Read a packet into the prepared data structures.
    HAL_StatusTypeDef receive_status =
        HAL_FDCAN_GetRxMessage(&handle, RxFifo, &header, data);

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
 * NOTE: Be very careful with this handle. Only the telemetry manager
 * and telemetry firmware should be calling this accessor.
 *
 * @return CAN_HandleTypeDef CAN handle.
 */
FDCAN_HandleTypeDef* fdcan_driver::get_handle() {
    return &handle;
}

void fdcan_driver::enable_tx_it() {
    // TODO: needing to OR all three buffers may mean something is not
    // configured correctly
    HAL_FDCAN_ActivateNotification(
        &handle, FDCAN_IT_TX_COMPLETE,
        FDCAN_TX_BUFFER0 | FDCAN_TX_BUFFER1 | FDCAN_TX_BUFFER2);
}

void fdcan_driver::disable_tx_it() {
    HAL_FDCAN_DeactivateNotification(&handle, FDCAN_IT_TX_COMPLETE);
}

}  // namespace can
}  // namespace umnsvp

#elif defined(STM32L5)

namespace umnsvp {
namespace can {

/**
 * @brief Construct a new fdcan driver::fdcan driver object. This
 * sets basic bit timing and other CAN basics Bit timings must be
 * specified for both data and normal operational timings although
 * they are currently set to the same
 *
 * @param can_peripheral
 * @param rate
 * @param config_isr
 */
fdcan_driver::fdcan_driver(FDCAN_GlobalTypeDef* can_peripheral, fdcan_gpio gpio,
                           bool config_isr)
    : gpio(gpio), config_isr(config_isr) {
    handle.Instance = can_peripheral;

    handle.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    handle.Init.Mode = FDCAN_MODE_NORMAL;
    handle.Init.NominalSyncJumpWidth = 1;
    handle.Init.NominalTimeSeg1 = time_segment_1;
    handle.Init.NominalTimeSeg2 = time_segment_2;

    handle.Init.DataSyncJumpWidth = 1;
    handle.Init.DataTimeSeg1 = time_segment_1;
    handle.Init.DataTimeSeg2 = time_segment_2;

    /* This is stored in RAM and can be any number between 0 and 28
     */
    handle.Init.StdFiltersNbr = num_filter_banks;
    handle.Init.ExtFiltersNbr = extended_num_filter_banks;

    handle.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    // handle.Init.ProtocolException = ENABLE;  // We should
    // probably have this
    handle.Init.AutoRetransmission = DISABLE;
}

/**
 * @brief Enable the GPIO clock for the CAN peripherial's GPIO port.
 *
 * @return HAL_StatusTypeDef HAL_OK if the clock has been enabled,
 * HAL_ERROR if the port is invalid.
 */
HAL_StatusTypeDef fdcan_driver::clock_enable() const {
    if (gpio.port == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        return HAL_OK;
    } else if (gpio.port == GPIOD) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
        return HAL_OK;
    } else if (gpio.port == GPIOB) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
        return HAL_OK;
    } else {
        return HAL_ERROR;
    }
}

/**
 * @brief Initialize the CAN hardware.
 * Sets up the clk for the peripheral and also enables the interupt
 * vectors but NOT the interupt sources.
 */
HAL_StatusTypeDef fdcan_driver::init(baud_rate rate, bool extended) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
    __HAL_RCC_FDCAN1_CLK_ENABLE();

    // Hardcoded prescaler from: http://www.bittiming.can-wiki.info/
    // See @ref baud_ref documentation for more information.
    handle.Init.NominalPrescaler = static_cast<uint32_t>(rate);
    handle.Init.DataPrescaler = static_cast<uint32_t>(rate);

    // Currently useless check but will be used when FDCAN2 & 3 are
    // added
    if (handle.Instance == FDCAN1) {
        // Enable CAN1 clock.

        // Enable the GPIO Clock for the gpio pins
        HAL_StatusTypeDef clock_status = clock_enable();
        if (clock_status != HAL_OK) {
            return clock_status;
        }

        // alternate function configuration
        const uint8_t af = GPIO_AF9_FDCAN1;

        GPIO_InitStruct.Pin = gpio.rx_pin | gpio.tx_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = af;

        // Initialize the GPIO pins for CAN1.
        HAL_GPIO_Init(gpio.port, &GPIO_InitStruct);
        if (config_isr) {
            // Set and enable the interupts
            HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 1, 0);
            HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
            HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 1, 0);
            HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
        }
    } else {
        // Return ERROR for invalid can peripherals. (FDCAN3 is not
        // supported.)
        return HAL_ERROR;
    }
    return HAL_FDCAN_Init(&handle);
}

/**
 * @brief This method starts the CAN hardware.
 *
 * This should be called after packet filters are applied to the
 * peripheral. The function sets up the interupt sources for the
 * peripheral. TX FIFO empty and RX element received interupts are
 * currently set
 *
 * @return HAL_StatusTypeDef HAL return status for start.
 */
HAL_StatusTypeDef fdcan_driver::start() {
    // Tell the peripheral to discard any non mathing ID's
    HAL_FDCAN_ConfigGlobalFilter(&handle, FDCAN_REJECT, FDCAN_REJECT,
                                 FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);

    if (handle.Instance == FDCAN1) {
        // Configure the interupts. This is done even if config ISR
        // is false bc it doesn't matter Configure interupt vector 0
        // for the Received new message intupt. The last input on
        // the first function (FDCAN_TX_BUFFER0) is ignored bc not
        // aplicible
        HAL_StatusTypeDef FIFO0_inter_status = HAL_FDCAN_ConfigInterruptLines(
            &handle, FDCAN_IT_GROUP_RX_FIFO0, FDCAN_INTERRUPT_LINE0);
        HAL_StatusTypeDef FIFO0_notif_status = HAL_FDCAN_ActivateNotification(
            &handle, FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE, 0);

        // Configure interupt vector 1 for the transmit empty
        // intupt. The last input is ignored bc not aplicible
        // (FDCAN_TX_BUFFER0)
        HAL_StatusTypeDef tx_inter_status = HAL_FDCAN_ConfigInterruptLines(
            &handle, FDCAN_IT_GROUP_TX_FIFO_ERROR, FDCAN_INTERRUPT_LINE1);
        HAL_StatusTypeDef tx_notif_status = HAL_FDCAN_ActivateNotification(
            &handle, FDCAN_FLAG_TX_FIFO_EMPTY, FDCAN_TX_BUFFER0);

        HAL_StatusTypeDef start_status = HAL_FDCAN_Start(&handle);

        if (start_status == HAL_OK && FIFO0_notif_status == HAL_OK &&
            FIFO0_inter_status == HAL_OK && tx_notif_status == HAL_OK &&
            tx_inter_status == HAL_OK) {
            return HAL_OK;
        }
    }
    return HAL_ERROR;
}

/**
 * @brief Stop the CAN hardware.
 *
 * @return HAL_StatusTypeDef HAL return status for stop.
 */
HAL_StatusTypeDef fdcan_driver::stop() {
    HAL_StatusTypeDef status = HAL_FDCAN_Stop(&handle);
    return status;
}

/**
 * @brief Build FDCAN filter with the filter index and the IDs to
 * filter.
 *
 * @param filter_index
 * @param id1
 * @param id2
 * @return constexpr FDCAN_FilterTypeDef
 */
FDCAN_FilterTypeDef make_filter_from_ids(std::size_t filter_index, uint32_t id1,
                                         uint32_t id2,
                                         bool is_extended = false) {
    if (is_extended) {
        return {.IdType = FDCAN_EXTENDED_ID,      // Filter Standard ID's
                .FilterIndex = filter_index,      // Only filter so put this
                                                  // in the first bank
                .FilterType = FDCAN_FILTER_DUAL,  // Make it a dual
                                                  // discrete ID type
                // TODO: in the future we will want to explicitly
                // provide the FIFO
                .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,  // Put this in FIFO0
                .FilterID1 = id1,
                .FilterID2 = id2};
    } else {
        return {.IdType = FDCAN_STANDARD_ID,      // Filter Standard ID's
                .FilterIndex = filter_index,      // Only filter so put this
                                                  // in the first bank
                .FilterType = FDCAN_FILTER_DUAL,  // Make it a dual
                                                  // discrete ID type
                // TODO: in the future we will want to explicitly
                // provide the FIFO
                .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,  // Put this in FIFO0
                .FilterID1 = id1,
                .FilterID2 = id2};
    }
}

/**
 * @brief Setup filters for the provided list of IDs.
 *
 * @param rx_ids
 * @param length
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef fdcan_driver::filter_list(const uint32_t* rx_ids,
                                            std::size_t length,
                                            bool is_extended) {
    if (length == 0) {
        // return because you didin't input a value
        return HAL_StatusTypeDef::HAL_OK;
    } else if (is_extended) {
        if (length > handle.Init.ExtFiltersNbr * id_filter_bank_cnt) {
            // If too many inputs just filter all the ID's
            return filter_all();
        }
    } else {
        if (length > handle.Init.StdFiltersNbr * id_filter_bank_cnt) {
            // If too many inputs just filter all the ID's
            return filter_all();
        }
    }

    FDCAN_FilterTypeDef filter;

    std::size_t filter_index = 0;
    std::size_t can_id_filter_index = 0;
    while (can_id_filter_index < length) {
        if (can_id_filter_index <= length - 2) {
            filter = make_filter_from_ids(
                filter_index, rx_ids[can_id_filter_index],
                rx_ids[can_id_filter_index + 1], is_extended);
            can_id_filter_index += 2;
        } else {
            // After this you are done since it's the last ID by
            // definition of entering this branch
            filter =
                make_filter_from_ids(filter_index, rx_ids[can_id_filter_index],
                                     rx_ids[can_id_filter_index], is_extended);
            can_id_filter_index += 1;
        }

        // Apply the filter
        HAL_StatusTypeDef filter_status =
            HAL_FDCAN_ConfigFilter(&handle, &filter);
        if (filter_status == HAL_StatusTypeDef::HAL_OK) {
            // If succesful, increment to the next filter
            filter_index += 1;
        } else {
            // Return an error if it occurs
            return filter_status;
        }
    }
    // Reached the end of the ID's without an error so return true
    return HAL_OK;
}

/**
 * @brief Apply a filter to the CAN peripheral that accepts all
 * STANDARD ID packets. NOT currently set up for extended ID's
 *  TODO: Set up for extended ID's
 *
 * @return HAL_StatusTypeDef HAL return status for configuration.
 */
HAL_StatusTypeDef fdcan_driver::filter_all() {
    // A filter that accepts all packets.
    // TODO: Update to work for extended ID's
    FDCAN_FilterTypeDef filterStd = {
        .IdType = FDCAN_STANDARD_ID,  // Filter Standard ID's
        .FilterIndex = 0,  // Only filter so put this in the first bank
        .FilterType = FDCAN_FILTER_MASK,          // Make it a mask type
        .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,  // Put this in FIFO0
        .FilterID1 = 0x0000,                      // Mask all
        .FilterID2 = 0x0000,                      // Mask all
    };
    FDCAN_FilterTypeDef filterExt = {
        .IdType = FDCAN_EXTENDED_ID,  // Filter Extended ID's
        .FilterIndex = 0,  // Only filter so put this in the first bank
        .FilterType = FDCAN_FILTER_MASK,          // Make it a mask type
        .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,  // Put this in FIFO0
        .FilterID1 = 0x0000,                      // Mask all
        .FilterID2 = 0x0000,                      // Mask all
    };

    HAL_StatusTypeDef filterStdHal =
        HAL_FDCAN_ConfigFilter(&handle, &filterStd);
    HAL_StatusTypeDef filterExtHal =
        HAL_FDCAN_ConfigFilter(&handle, &filterExt);

    if (filterStdHal == HAL_OK && filterExtHal == HAL_OK) {
        return HAL_OK;
    } else {
        return HAL_ERROR;
    }
    // Apply the filter on the hardware.
}

/**
 * @brief Add a mask or ID list filter for the CAN peripheral.
 *
 * @param filter The id list or mask filter to apply.
 * @return HAL_StatusTypeDef HAL return status for the
 * configuration.
 */
HAL_StatusTypeDef fdcan_driver::set_filter(const FDCAN_FilterTypeDef& filter) {
    return HAL_FDCAN_ConfigFilter(&handle,
                                  &const_cast<FDCAN_FilterTypeDef&>(filter));
}

/**
 * @brief Add a CAN packet to the transmit FIFO.
 *
 * NOTE: This does not guarantee transmission nor reception of the
 * packet. The can driver has autotransmission set so every effort
 * will happen to send the packets The TX buffer is a FIFO and
 * **NOT** a Queue. This means no matter the ID, the first packet in
 * will be the first one sent out
 *
 * @param send_packet The packet to send.
 * @return bxcan_driver::status The status of adding the CAN packet.
 */
status fdcan_driver::send(const packet& send_packet) {
    // Generate the HAL header for the packet.
    FDCAN_TxHeaderTypeDef header = send_packet.get_header();

    // Attempt to add the message into a transmit mailbox.
    // TODO: Double check inputs to this to make sure structs line
    // up
    // TODO: We should not be using const_cast like this.
    HAL_StatusTypeDef send_status = HAL_FDCAN_AddMessageToTxFifoQ(
        &handle, &header, const_cast<uint8_t*>(send_packet.get_data()));

    // Check the response.
    if (send_status == HAL_OK) {
        return status::OK;
    }

    return status::ERROR;
}

/**
 * @brief Read a packet from the receive FIFO.
 *
 * NOTE: It is not guarenteed that a packed was placed in the passed
 * in output variable. Be sure to check the returned status to know
 * whether a packet was received.
 *
 * @param received_packet The variable to store the received packet
 * in.
 * @param fifo Which FIFO to recieve from. Defaults to 0.
 * @return bxcan_driver::status Whether a packet was read.
 */
status fdcan_driver::receive(packet& received_packet, const fifo fifo) {
    uint32_t RxFifo;

    switch (fifo) {
        case fifo::FIFO0:
            RxFifo = FDCAN_RX_FIFO0;
            break;
        case fifo::FIFO1:
            RxFifo = FDCAN_RX_FIFO1;
            break;
        default:
            // If an invalid fifo value is supplied, return an error
            return status::ERROR;
    }

    // Check if packets are available to read.
    const uint32_t fill_level = HAL_FDCAN_GetRxFifoFillLevel(&handle, RxFifo);

    // Return empty if no packets are available to read.
    if (fill_level == 0) {
        return status::EMPTY;
    }

    // Prepare a header and data structure to load a packet into.
    FDCAN_RxHeaderTypeDef header = {0};
    uint8_t data[8] = {0};

    // Read a packet into the prepared data structures.
    HAL_StatusTypeDef receive_status =
        HAL_FDCAN_GetRxMessage(&handle, RxFifo, &header, data);

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
 * NOTE: Be very careful with this handle. Only the telemetry
 * manager and telemetry firmware should be calling this accessor.
 *
 * @return CAN_HandleTypeDef CAN handle.
 */
FDCAN_HandleTypeDef* fdcan_driver::get_handle() {
    return &handle;
}

void fdcan_driver::enable_tx_it() {
    // TODO: needing to OR all three buffers may mean something is
    // not configured correctly
    HAL_FDCAN_ActivateNotification(
        &handle, FDCAN_IT_TX_COMPLETE,
        FDCAN_TX_BUFFER0 | FDCAN_TX_BUFFER1 | FDCAN_TX_BUFFER2);
}

void fdcan_driver::disable_tx_it() {
    HAL_FDCAN_DeactivateNotification(&handle, FDCAN_IT_TX_COMPLETE);
}

}  // namespace can
}  // namespace umnsvp

#endif
