
#Make a UMNSVP for the MCU if it doesn't exist already, and link it against targ.
function(add_umnsvp targ)
	
	extract_board_info(${targ})

	set(libname _umnsvp_)

	if (NOT TARGET ${libname})
		message(VERBOSE "Creating UMNSVP library")

		add_library(${libname} INTERFACE)

		target_sources(${libname} INTERFACE
		    ${UMNSVP_DIR}/uid.cc
		    ${UMNSVP_DIR}/dip_switch.cc
		    ${UMNSVP_DIR}/can_packet.cc
		    ${UMNSVP_DIR}/application_base.cc
		    ${UMNSVP_DIR}/fdcan.cc
		    ${UMNSVP_DIR}/bxcan.cc
		)
		
		# add includes
		target_include_directories(${libname} INTERFACE ${UMNSVP_DIR})

		target_link_libraries(${libname} INTERFACE STM32)
	endif()

	target_link_libraries(${targ} PUBLIC ${libname})
endfunction()

