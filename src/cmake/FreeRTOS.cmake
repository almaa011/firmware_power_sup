
function(add_freertos targ heap_version)
# this is probably one of the most advanced functions for the new cmake system. We
# have multiple things to track - CMSIS_RTOS, microcontroller architecture, and
# heap implementation.

extract_board_info(${targ})

set(libname _freertos_${MCU_FAMILY_LOWER})

if (NOT TARGET ${libname})
  message(VERBOSE "Creating FreeRTOS for ${libname}")
  add_library(${libname} INTERFACE)

  # an amalgamation to get the port path. this is because some micros have 
  if (${MCU_FAMILY_UPPER} MATCHES "^[FGL]4$")
    message(VERBOSE "Using cortex-M4")
    set(FREERTOS_PORT_PATH ${FREERTOS_DIR}/portable/GCC/ARM_CM4F)
  elseif(${MCU_FAMILY_UPPER} MATCHES "^L5")
    message(VERBOSE "Using cortex-M33")
    set(FREERTOS_PORT_PATH ${FREERTOS_DIR}/portable/GCC/ARM_CM33_NTZ/non_secure)
  elseif(${MCU_FAMILY_UPPER} MATCHES "^G0")
    message(VERBOSE "Using cortex-M0")
    set(FREERTOS_PORT_PATH ${FREERTOS_DIR}/portable/GCC/ARM_CM0)
  endif()

  # add includes
  target_include_directories(${libname} INTERFACE
    ${FREERTOS_DIR}/include
    ${FREERTOS_DIR}/CMSIS_RTOS_V2
    ${FREERTOS_PORT_PATH}
    )
  # add base sources
  target_sources(${libname} INTERFACE
    ${FREERTOS_DIR}/croutine.c
    ${FREERTOS_DIR}/event_groups.c
    ${FREERTOS_DIR}/list.c
    ${FREERTOS_DIR}/queue.c
    ${FREERTOS_DIR}/stream_buffer.c
    ${FREERTOS_DIR}/tasks.c
    ${FREERTOS_DIR}/timers.c
    ${FREERTOS_DIR}/CMSIS_RTOS_V2/cmsis_os2.c
  )
  # add port-specific sources
  # because the cm33 has multiple files (port.c and portasm.c) we need to glob.
  FILE(GLOB portSources ${FREERTOS_PORT_PATH}/*.c)
  target_sources(${libname} INTERFACE ${portSources})

  # TODO: allow for different heap versions
  target_sources(${libname} INTERFACE ${FREERTOS_DIR}/portable/MemMang/heap_4.c)
endif()

target_link_libraries(${targ} PUBLIC ${libname})

endfunction()