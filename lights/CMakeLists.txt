add_executable(lights)

setup_board(lights STM32G473 VETX)

add_cmsis(lights)
add_hal(lights)
add_umnsvp(lights)
target_link_libraries(lights PUBLIC skylab2)
use_stm32_linker_scripts(lights)

# Add the `inc` directory for this board
target_include_directories(lights PRIVATE inc)
add_CPU_options(lights)

# Set all the sources for this board
target_sources(lights
PRIVATE
  src/main.cc
  src/app.cc
  src/stm32g4xx_it.cc
  src/light.cc
  src/pwm_driver.cc
)
