#include "adc.h"
#include "spi.h"

#include <string.h>

HAL_StatusTypeDef read_registers(uint8_t addr, uint8_t num, uint8_t size,
                                 uint8_t *results) {
    uint8_t frame[2] = {(uint8_t)((0x01u << 4) | (addr & 0x0Fu)), num};
    send_receive_bytes_SPI(frame, results, size);
    return HAL_OK;
}

HAL_StatusTypeDef write_registers(uint8_t addr, uint8_t num, uint8_t size,
                                  const uint8_t *data) {
    uint8_t frame_len = (uint8_t)(2u + size);
    uint8_t frame[32] = {0};
    if (frame_len > sizeof(frame)) {
        return HAL_ERROR;
    }
    frame[0] = (uint8_t)((0x05u << 4) | (addr & 0x0Fu));
    frame[1] = num;
    memcpy(&frame[2], data, size);
    send_bytes_SPI(frame, frame_len);
    return HAL_OK;
}

HAL_StatusTypeDef set_mux(uint8_t psel) {
    const uint8_t data = (uint8_t)((psel << 4) | (psel & 0x0Fu));
    return write_registers(0x01u, 0u, 1u, &data);
}

HAL_StatusTypeDef adc_init(void) {
    return set_mux(0u);
}

HAL_StatusTypeDef adc_read(uint32_t *val) {
    uint8_t result[3] = {0};
    read_registers(0x0Du, 2u, sizeof(result), result);
    *val = ((uint32_t)result[0] << 16) | ((uint32_t)result[1] << 8) |
           (uint32_t)result[2];
    return HAL_OK;
}
