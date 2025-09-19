add_executable(BOOST)

# Setup for STM32L476 RGTX
setup_board(BOOST STM32L476 RGTX)

# Add CMSIS, HAL, CPU options, and linker script
add_cmsis(BOOST)
add_hal(BOOST)
add_CPU_options(BOOST)
use_stm32_linker_scripts(BOOST)

# Board-local headers live in inc/
target_include_directories(BOOST PRIVATE inc)

# Board sources live under src/
target_sources(BOOST PRIVATE
    src/main.c
    src/adc.c
    src/i2c.c
    src/led_pwm.c
    src/spi.c
    src/stm32l4xx_hal_msp.c
    src/stm32l4xx_it.c
    src/syscalls.c
    src/sysmem.c
)
