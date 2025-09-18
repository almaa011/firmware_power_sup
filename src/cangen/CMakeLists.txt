
file(GLOB tmpl_files LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/templates/*.j2)
file(GLOB pkt_files LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/packets/*.yaml)
add_custom_command(
	OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/src/skylab2_boards.cc
    ${CMAKE_CURRENT_BINARY_DIR}/inc/skylab2_boards.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/skylab2_busses.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/skylab2_packets.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/skylab2_types.h
	COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/skylab2.py
        -p ${CMAKE_CURRENT_SOURCE_DIR}/packets
        -t ${CMAKE_CURRENT_SOURCE_DIR}/templates 
        ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS
    ./can_parser.py
    ./skylab2.py
    ${tmpl_files}
    ${pkt_files}
    VERBATIM
    COMMENT generating skylab2 code
	)

add_library(skylab2 INTERFACE)

# this part creates a strong linkage to the header files.
add_custom_target(_skyfiles DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/src/skylab2_boards.cc
    ${CMAKE_CURRENT_BINARY_DIR}/inc/skylab2_boards.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/skylab2_busses.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/skylab2_packets.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/skylab2_types.h
)
add_dependencies(skylab2 _skyfiles) # this part attaches those files as a dependency
# the above technically isn't needed I think, but it could get funky otherwise.

target_sources(skylab2 INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/src/skylab2_boards.cc
    ${CMAKE_SOURCE_DIR}/libraries/skylab2/src/skylab2_can_base.cc
)
target_include_directories(skylab2 INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/inc
    ${CMAKE_SOURCE_DIR}/libraries/skylab2/inc
)
target_link_libraries(skylab2 INTERFACE STM32)
target_link_libraries(skylab2 INTERFACE _umnsvp_)
