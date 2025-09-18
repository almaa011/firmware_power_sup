

# Function to add dsp to the board `targ`

function(add_cmsis_dsp targ)

	extract_board_info(${targ})
	set(libname _cmsis_dsp_${MCU_FAMILY_LOWER})
	# ensure that the target has cmsis
	add_cmsis(${targ})

	if (NOT TARGET ${libname})
		message(VERBOSE "Creating CMSIS-DSP for ${MCU_FAMILY_LOWER}")
		add_library(${libname} STATIC)

		target_include_directories(${libname} PRIVATE ${CMSIS_DIR}/Include ${CMSIS_DSP_DIR}/PrivateInclude)
		target_include_directories(${libname} PUBLIC ${CMSIS_DSP_DIR}/Include)

		target_sources(${libname} PRIVATE
			${CMSIS_DSP_DIR}/Source/BasicMathFunctions/BasicMathFunctions.c
			${CMSIS_DSP_DIR}/Source/CommonTables/CommonTables.c
			${CMSIS_DSP_DIR}/Source/InterpolationFunctions/InterpolationFunctions.c
			${CMSIS_DSP_DIR}/Source/BayesFunctions/BayesFunctions.c
			${CMSIS_DSP_DIR}/Source/MatrixFunctions/MatrixFunctions.c
			${CMSIS_DSP_DIR}/Source/ComplexMathFunctions/ComplexMathFunctions.c
			${CMSIS_DSP_DIR}/Source/QuaternionMathFunctions/QuaternionMathFunctions.c
			${CMSIS_DSP_DIR}/Source/ControllerFunctions/ControllerFunctions.c
			${CMSIS_DSP_DIR}/Source/SVMFunctions/SVMFunctions.c
			${CMSIS_DSP_DIR}/Source/DistanceFunctions/DistanceFunctions.c
			${CMSIS_DSP_DIR}/Source/StatisticsFunctions/StatisticsFunctions.c
			${CMSIS_DSP_DIR}/Source/FastMathFunctions/FastMathFunctions.c
			${CMSIS_DSP_DIR}/Source/SupportFunctions/SupportFunctions.c
			${CMSIS_DSP_DIR}/Source/FilteringFunctions/FilteringFunctions.c
			${CMSIS_DSP_DIR}/Source/TransformFunctions/TransformFunctions.c
			${CMSIS_DSP_DIR}/Source/WindowFunctions/WindowFunctions.c
		)

		target_compile_options(${libname} PRIVATE # make these routines zippy
			-Ofast -ffast-math -fshort-enums
		)
		target_compile_definitions(${libname} PRIVATE NDEBUG) # force no debug

		target_link_libraries(${libname} INTERFACE STM32::${MCU_FAMILY_UPPER})
		target_link_libraries(${libname} PRIVATE STM32::${MCU_FAMILY_UPPER})
	endif()

	target_link_libraries(${targ} PUBLIC ${libname})
endfunction()