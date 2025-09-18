# the one file to rule all HALs...
# Sets up a HAL for a given project when you do add_hal(board)
# Attempts to do it all for you :)

# Adds a hal project to the given target.
# Generates and adds a CMSIS project if it hasn't been added yet.
function(add_hal targ)

	extract_board_info(${targ})
	# the HAL is defined per-chip so it can interface with a specific CMSIS.
	set(libname _hal_${MCU_FAMILY_LOWER}) # name of the library we are looking for.

	# if we don't yet have a library with this name, make one.
	if (NOT TARGET ${libname})
		message(VERBOSE "Creating HAL for ${MCU_FAMILY_LOWER}")
		set(STM32_FAMILY_PATH ${CMAKE_SOURCE_DIR}/libraries/hal/${MCU_FAMILY_LOWER})
		set(STM32_HAL_PATH ${STM32_FAMILY_PATH}/STM32${MCU_FAMILY_UPPER}xx_HAL_Driver)
		add_library(${libname} INTERFACE)

		target_include_directories(${libname} INTERFACE
  			${STM32_HAL_PATH}/Inc
			${STM32_HAL_PATH}/Inc/Legacy)

		# include(STM32${MCU_FAMILY_UPPER}_HAL_files)
		# normally, globs are bad. But here, they're good.
		file(GLOB_RECURSE hal_files LIST_DIRECTORIES false ${STM32_HAL_PATH}/Src/*.c)
		# however, we don't want to include the _template.c files - those are bad.
		list(FILTER hal_files EXCLUDE REGEX "^.+_template.c$")

		target_sources(${libname} INTERFACE ${hal_files})

		# SEE CMSIS.cmake for why we do this!
		set_source_files_properties(
			$<TARGET_PROPERTY:${libname},INTERFACE_SOURCES> PROPERTIES
			COMPILE_OPTIONS "-w")

		# presumably we want to *use* the HAL we just made, so add a compile definition to the HAL library.
		# IF THIS CHANGES - you'll want to add the compile definition manually.
		target_compile_definitions(${libname} INTERFACE USE_HAL_DRIVER)

		# use stm32 compiler options.
		target_link_libraries(${libname} INTERFACE STM32::${MCU_FAMILY_UPPER})
		
	endif()
	# link against CMSIS for the provided target, since the HAL needs it.
	# We CANNOT add cmsis as a dependency for the HAL (no add_cmsis(HAL) allowed)!
	# This is because the CMSIS library uses MCU_specific code, but the HAL
	# is generic for each family.
	# BUT! since both of these libraries are INTERFACE libraries, the HAL
	# will have access to the CMSIS library attached to ${targ}.
	# add the HAL for this MCU family to targ.
	target_link_libraries(${targ} PUBLIC ${libname})
endfunction()

