add_executable(BOOST)

# Setup for STM32L476 RGTX
setup_board(BOOST STM32L476 RGTX)

# Add CMSIS, HAL, CPU options, and linker script
add_cmsis(BOOST)
add_hal(BOOST)
add_CPU_options(BOOST)
use_stm32_linker_scripts(BOOST)

# Board-local headers live in inc/, plus bundled USB middleware.
target_include_directories(BOOST PRIVATE
    inc
    usb_lib/Core/Inc
    usb_lib/Class/CDC/Inc
)

# Board sources live under src/
target_sources(BOOST PRIVATE
    src/main.c
    src/stm32l4xx_hal_msp.c
    src/stm32l4xx_it.c
    src/usb_device.c
    src/usbd_conf.c
    src/usbd_desc.c
    src/usbd_cdc_if.c
    src/syscalls.c
    src/sysmem.c
    usb_lib/Core/Src/usbd_core.c
    usb_lib/Core/Src/usbd_ctlreq.c
    usb_lib/Core/Src/usbd_ioreq.c
    usb_lib/Class/CDC/Src/usbd_cdc.c
)
