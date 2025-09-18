# Jenkins builds with this version and Ubuntu 20.04 has a newer version.
# If a user is on a older version (i.e. using Ubuntu 16.04), a manual upgrade is required. 
cmake_minimum_required(VERSION 3.20)

# point cmake to our custom modules
# this way we can do `include(CMSIS)` to load the add_cmsis function
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Compiler toolchain has to be specified in its own file
# See https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html
# https://cmake.org/cmake/help/latest/variable/CMAKE_TOOLCHAIN_FILE.html#variable:CMAKE_TOOLCHAIN_FILE
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/stm32_gcc.cmake)
project(UMNSVP_Firmware C CXX ASM)

# Enforce showing all warnings for all projects.
add_compile_options(-Wall)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# We don't care about using volatiles in cursed ways, this is a microcontroller!
# this is mostly because we use C++20, which complains a lot more about `volatile` in code,
# since sometimes people use it for "atomic" operations. We usually use it to access registers,
# so our use case is valid.
set(CMAKE_CXX_FLAGS -Wno-volatile)

# Include global targets that set up interface libraries for
# setting compiler options
include(stm32_compile_options)
# utility functions.
include(utils)

# Include the libraries in the build
# TODO: remove this - we don't use these variables smh
add_subdirectory(libraries)
add_subdirectory(cangen)

# our library functions
include(CMSIS)
include(CMSIS_DSP)
include(STM32_HAL)
include(FreeRTOS)
include(UMNSVP)
include(fatfs)
include(SEGGER)


# Include the boards in the build
add_subdirectory(boards)
