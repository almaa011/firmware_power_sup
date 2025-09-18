# We are cross compiling for an embedded system
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Use arm-none-eabi-* for our compiling/assembling/linking/etc programs
set(STM32_TARGET_TRIPLET "arm-none-eabi")
# Search in /usr/bin by default
set(TOOLCHAIN_BIN_PATH "/usr/bin")


# makes CMAKE not try and build a test binary that would normally use
# illegal system calls and fail to compile.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# All the systemcalls (i.e. printf) get replaced by a stub that returns an error
# (nosys option). Some small systemcalls are replaced with their embedded system
# implementation (nano option). The `nosys` option must be used first here
# because not all systemcalls are implemented for embedded systems. Thus,
# doing this first prevents any systemcalls from getting missed and all that
# can be implemented for embedded systems are.
set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs --specs=nano.specs" CACHE INTERNAL "")

# Find the programs for cross compiling
find_program(CMAKE_C_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_CXX_COMPILER NAMES ${STM32_TARGET_TRIPLET}-g++ PATHS ${TOOLCHAIN_BIN_PATH})
# TODO: set assembler options for gcc
find_program(CMAKE_ASM_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH})

# Set executable suffixes to .elf
set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)
