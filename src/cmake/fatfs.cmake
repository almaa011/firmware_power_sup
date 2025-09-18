# Make a new library called fatfs_${PROJECT_NAME}.
add_library(fatfs INTERFACE)

target_link_libraries(fatfs INTERFACE STM32)
# target_compile_options(fatfs PRIVATE -w)

# Set the sources.
target_sources(fatfs
	INTERFACE
    ${FATFS_DIR}/source/ff.c
    ${FATFS_DIR}/source/ffsystem.c
    ${FATFS_DIR}/source/ffunicode.c
)

set_source_files_properties(
	$<TARGET_PROPERTY:fatfs,INTERFACE_SOURCES> PROPERTIES
	COMPILE_OPTIONS "-w")
# Set the include directories.
target_include_directories(fatfs
	INTERFACE
    	${FATFS_DIR}/source
)
