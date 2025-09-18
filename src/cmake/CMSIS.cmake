# Define CMSIS for this board if it hasn't been already.
# and then link it to the project if it exists.

# Creates a CMSIS library for the target, and links the target against it.
# general usage: `add_cmsis(lights)`. Assumets that `setup_mcu` has been run.
function(add_cmsis targ)

	extract_board_info(${targ})

set(libname _cmsis_${MCU_LOWER})
# If we haven't made a CMSIS project for this MCU, make one.
# remember - MCU_LOWER is stm32g474. MCU_FAMILY_LOWER is g4.
# the CMSIS library contains the startup routine for a specific MCU - so 
# we want to create one for each MCU we use (not variant though - that's too specific).
# linker script handled in another function
if (NOT TARGET ${libname})
	message(VERBOSE "Creating CMSIS for ${MCU_LOWER}")
	set(CMSIS_MCU_PATH ${CMSIS_DIR}/Device/ST/STM32${MCU_FAMILY_UPPER}xx/)
	add_library(${libname} INTERFACE)

	# add the MCU-specific sources.
	target_sources(${libname} INTERFACE
		${CMSIS_MCU_PATH}/Source/Templates/gcc/startup_${MCU_LOWER}xx.s
		${CMSIS_MCU_PATH}/Source/Templates/system_stm32${MCU_FAMILY_LOWER}xx.c	
	)
	# odd part here - since our libraries are INTERFACEs, we can't
	# set library-specific compiler flags.
	# instead we hack it with generator expressions:
	# $<TARGET_PROPERTY:${libname},SOURCES> basically reads the properties (member variables)
	# of ${libname} and returns the INTERFACE_SOURCES property, which is a list of all sources
	# that were listed with either INTERFACE or PUBLIC. We can't use PUBLIC because our library
	# is an INTERFACE library.
	# then we set the COMPILE_OPTIONS property for those sources to supress warnings.
	set_source_files_properties(
		${CMSIS_MCU_PATH}/Source/Templates/gcc/startup_${MCU_LOWER}xx.s
		${CMSIS_MCU_PATH}/Source/Templates/system_stm32${MCU_FAMILY_LOWER}xx.c	
		PROPERTIES
		COMPILE_OPTIONS "-w")
	target_compile_definitions(${libname} INTERFACE ${MCU_UPPER}xx)
	target_include_directories(
		${libname}
		INTERFACE
		${CMSIS_MCU_PATH}/Include
  		${CMSIS_DIR}/Include
		${CMSIS_DIR}/RTOS2/Include
	)
	# use our virtual STM32 compiler options library.
	target_link_libraries(${libname} INTERFACE STM32)
endif()
# now we have created the library for this target, so let's link it.
# since it's an INTERFACE library, it is only built alongside all
# the files for a target executable.
target_link_libraries(${targ} PUBLIC ${libname})

endfunction()
