add_executable(BOOST)

setup_board(BOOST STM32L476 RGTX)

add_cmsis(BOOST)
add_hal(BOOST)
add_CPU_options(BOOST)
use_stm32_linker_scripts(BOOST)

target_include_directories(BOOST PRIVATE inc)

target_sources(BOOST
  PRIVATE
    src/main.cc
    src/stm32l4xx_it.cc
)
