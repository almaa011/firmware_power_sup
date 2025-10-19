add_executable(USBTEST)

# Setup for STM32L476 RGTX
setup_board(USBTEST STM32L476 RGTX)

# Add CMSIS, HAL, CPU options, and linker script
add_cmsis(USBTEST)
add_hal(USBTEST)
add_CPU_options(USBTEST)
use_stm32_linker_scripts(USBTEST)

# Board-local headers live in inc/
target_include_directories(USBTEST PRIVATE inc)

# Add STM32 USB Device middleware (Core + CDC class)
target_include_directories(USBTEST PRIVATE
    ${CMAKE_SOURCE_DIR}/boards/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ${CMAKE_SOURCE_DIR}/boards/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    ${CMAKE_SOURCE_DIR}/boards/USBtest/Src/Target
)

# Board sources live under Src/ (case-sensitive on some systems)
target_sources(USBTEST PRIVATE
    Src/main.c
    Src/stm32l4xx_hal_msp.c
    Src/stm32l4xx_it.c
    Src/syscalls.c
    Src/sysmem.c
    Src/usb_device.c
    Src/usbd_cdc_if.c
    Src/usbd_desc.c
    Src/Target/usbd_conf.c
    # USB middleware core and CDC class implementation
    ${CMAKE_SOURCE_DIR}/boards/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
    ${CMAKE_SOURCE_DIR}/boards/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
    ${CMAKE_SOURCE_DIR}/boards/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
    ${CMAKE_SOURCE_DIR}/boards/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c
)