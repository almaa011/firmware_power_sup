
#Make a SEGGER for the MCU if it doesn't exist already, and link it against targ.
function(add_segger targ)

    extract_board_info(${targ})

    set(libname _segger_)

    if (NOT TARGET ${libname})
        message(VERBOSE "Creating SEGGER library")

        add_library(${libname} INTERFACE)

        target_sources(${libname} INTERFACE
                ${SEGGER_DIR}/SEGGER_RTT.c
                ${SEGGER_DIR}/SEGGER_RTT_printf.c
                ${SEGGER_DIR}/SEGGER_RTT_Syscalls_GCC.c
                ${SEGGER_DIR}/SEGGER_SYSVIEW.c
                ${SEGGER_DIR}/SEGGER_SYSVIEW_Config_NoOS.c
                ${SEGGER_DIR}/SEGGER_RTT_ASM_ARMv7M.S
                )

        # add includes
        target_include_directories(${libname} INTERFACE ${SEGGER_DIR})

        target_link_libraries(${libname} INTERFACE STM32)
    endif ()

    target_link_libraries(${targ} PUBLIC ${libname})
endfunction()

