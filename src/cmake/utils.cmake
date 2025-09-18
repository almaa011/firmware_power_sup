# Cmake utility functions for use when defining new boards/libs


#[[
A short haiku about scope

cmake is quite strange
vars are directory scoped
except for functions

so basically, there are three scopes: directory, function, and cache. if you're
in a function (between function() and endfunction(), yes, cmake is bonkers) the
variable is *not* set outside of that function _unless_ you pass PARENT_SCOPE
to set. see: https://cmake.org/cmake/help/latest/command/set.html. cache
variables are basically just globals

this is really scuffed since knowing what directory you're in is quite hard to
think about. so we don't think. instead, we use "target properties", which are
like variables, but attached to a target (library or executable). this is way
easier to understand. to keep the variable state clean, and also allow for more
dynamic behavior, we use functions.

]]

# this function attaches properties to the target about
# the mcu/variant that it uses.
# example usage: setup_board(lights STM32L476 RGTX)
# target properties are like member variables! no more strange scoping problems.
function(setup_board targ mcu variant)
	get_target_property(target_type ${targ} TYPE)
	if (NOT target_type STREQUAL "EXECUTABLE")
		message(FATAL_ERROR "${targ} not an executable - you screwed up.")
	endif()
	string(TOUPPER ${mcu} MCU_UPPER)
	string(TOUPPER ${variant} MCU_VARIANT)

	# check if our given mcu is a valid one:
	string(REGEX MATCH "^STM32([FGL][0-9])([0-9B][0-9])$" mcu_match ${MCU_UPPER})

	if(NOT mcu_match) # we failed to validate the mcu.
		message(FATAL_ERROR "Could not determine MCU: ${MCU_UPPER}")
	endif()

	#TODO: maybe check that the variant is valid?
	

	# set the properties on the target board.
	set_property(TARGET ${targ} PROPERTY MCU ${MCU_UPPER})
	set_property(TARGET ${targ} PROPERTY MCU_VARIANT ${MCU_VARIANT})


endfunction()

# extract_board_info: internal macro to set local variables to board MCU info.
# you should only need to use this when defining a new MCU-specific library
# like CMSIS or HAL - i.e very rarely!
# remember, when you run a macro *it shares the scope of the caller*.
# so all the variables that we set here are set in the caller scope
# (usually an add_<library> function like add_cmsis so it can 
# get all the mcu info from a board)
macro(extract_board_info board)
	get_property(MCU_UPPER TARGET ${board} PROPERTY MCU)
	string(TOLOWER ${MCU_UPPER} MCU_LOWER)

	# extract the family: g4, l4, etc. both upper and lowercase.
	string(SUBSTRING ${MCU_UPPER} 5 2 MCU_FAMILY_UPPER)
	string(TOLOWER ${MCU_FAMILY_UPPER} MCU_FAMILY_LOWER)

	# extract the VARIANT property from the board.
	get_property(MCU_VARIANT TARGET ${board} PROPERTY MCU_VARIANT)
endmacro()

# Adds the correct linker script for the MCU to the 
# given board. The linker script contains the memory layout
# of the MCU, i.e where the code and data and stack go.
# This is variant specific, so we link it in directly to the board.
function(use_stm32_linker_scripts board)

extract_board_info(${board})
	
set(STM32_FAMILY_PATH ${CMAKE_SOURCE_DIR}/libraries/hal/${MCU_FAMILY_LOWER})

set(ldscript ${STM32_FAMILY_PATH}/${MCU_UPPER}${MCU_VARIANT}_FLASH.ld)
target_link_options(${board} PUBLIC -T${ldscript})
set_property(TARGET ${board} PROPERTY LINK_DEPENDS ${ldscript})

endfunction()
