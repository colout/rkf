# Add library cpp files
add_library(serial_1wire INTERFACE)
target_sources(serial_1wire INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/serial_1wire.cpp
    ${CMAKE_CURRENT_LIST_DIR}/serial_1wire.h
)
pico_generate_pio_header(serial_1wire ${CMAKE_CURRENT_LIST_DIR}/serial_1wire.pio)

# Add include directory
target_include_directories(serial_1wire INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Add the standard library to the build
target_link_libraries(serial_1wire INTERFACE hardware_pio pico_stdlib)
