/* USB CDC interface implementation (cleaned). */

#include "usbd_cdc_if.h"

#include <string.h>

#include "main.h"

/* Buffers for CDC RX/TX */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t *pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);
static void ToggleStatusLed(void);

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = {CDC_Init_FS, CDC_DeInit_FS,
                                              CDC_Control_FS, CDC_Receive_FS,
                                              CDC_TransmitCplt_FS};

static int8_t CDC_Init_FS(void) {
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
    return (USBD_OK);
}

static int8_t CDC_DeInit_FS(void) {
    return (USBD_OK);
}

static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length) {
    /* Control requests are not used by this application; return OK. */
    (void)cmd;
    (void)pbuf;
    (void)length;
    return (USBD_OK);
}

static int8_t CDC_Receive_FS(uint8_t *Buf, uint32_t *Len) {
    uint16_t rx_len = (uint16_t)(*Len);
    ToggleStatusLed();

    if (rx_len > 0U) {
        uint16_t copy_len = rx_len;
        if (copy_len > APP_TX_DATA_SIZE) {
            copy_len = APP_TX_DATA_SIZE;
        }
        memcpy(UserTxBufferFS, Buf, copy_len);
        CDC_Transmit_FS(UserTxBufferFS, copy_len);
    }

    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    return (USBD_OK);
}

uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len) {
    uint8_t result = USBD_OK;
    USBD_CDC_HandleTypeDef *hcdc =
        (USBD_CDC_HandleTypeDef *)hUsbDeviceFS.pClassData;
    if (hcdc->TxState != 0) {
        return USBD_BUSY;
    }
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
    result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    return result;
}

static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum) {
    (void)Buf;
    (void)Len;
    (void)epnum;
    return USBD_OK;
}

static void ToggleStatusLed(void) {
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}
