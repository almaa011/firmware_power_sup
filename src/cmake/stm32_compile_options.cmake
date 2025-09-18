# Interface library that sets common compiler options.
# DON'T link against this directly, instead use the sub-libraries
# listed down below.
add_library(STM32 INTERFACE IMPORTED)

# Set the options we want for all of our micros.
# 
# See https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
target_compile_options(STM32
INTERFACE
  --specs=nosys.specs
  --specs=nano.specs
  -mthumb
  -mabi=aapcs
  -Wall
  -ffunction-sections
  -fdata-sections
  -fno-strict-aliasing
  -fno-builtin
  -ffast-math
)
target_link_options(STM32
INTERFACE
  # Remove unused code
  -Wl,--gc-sections
)


# creates a library for a specific micro with given flags!
# to find what flags to use, read the datasheets - it can be tricky to figure
# out FPU
function(make_cpu_opts name)
	message(VERBOSE "Definining CPU compile options for ${name}: ${ARGN}")
	string(REGEX MATCH "^STM32::([FGL][0-9])$" m ${name})
	if (NOT m)
		message(FATAL_ERROR "invalid option lib name given, follow other formats! ${name}")
	endif()
	add_library(${name} INTERFACE IMPORTED)
	target_compile_options(${name} INTERFACE ${ARGN})
	target_link_options(${name} INTERFACE ${ARGN})
	target_link_libraries(${name} INTERFACE STM32)
endfunction()

make_cpu_opts(STM32::F4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
make_cpu_opts(STM32::L4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
make_cpu_opts(STM32::G4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
make_cpu_opts(STM32::G0 -mcpu=cortex-m0plus)
make_cpu_opts(STM32::L5 -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard)

# Function to add the correct architecture-specific commands
# to the board. For example, the G4 uses a Cortex M4, but the L5
# uses a Cortex M33. The G0 uses a Cortex M0+. if you add
# a new micro, you'll need to handle this here. and add a new 
function(add_CPU_options targ)
extract_board_info(${targ})
# using MCU family upper to link
target_link_libraries(${targ} INTERFACE STM32::${MCU_FAMILY_UPPER})
endfunction()
