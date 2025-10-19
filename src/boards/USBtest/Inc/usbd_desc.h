/* Minimal USB descriptor header */
#ifndef __USBD_DESC_H__
#define __USBD_DESC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "usbd_def.h"

#define DEVICE_ID1 (UID_BASE)
#define DEVICE_ID2 (UID_BASE + 0x4)
#define DEVICE_ID3 (UID_BASE + 0x8)

#define USB_SIZ_STRING_SERIAL 0x1A

extern USBD_DescriptorsTypeDef FS_Desc;

uint8_t *USBD_FS_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed,
                                           uint16_t *length);
uint8_t *USBD_FS_ProductStrDescriptor(USBD_SpeedTypeDef speed,
                                      uint16_t *length);
uint8_t *USBD_FS_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_InterfaceStrDescriptor(USBD_SpeedTypeDef speed,
                                        uint16_t *length);
#if (USBD_LPM_ENABLED == 1)
uint8_t *USBD_FS_USR_BOSDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __USBD_DESC_H__ */
