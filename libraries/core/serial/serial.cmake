# Add library cpp files
add_library(serial INTERFACE)
target_sources(serial INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/serial.h
    ${CMAKE_CURRENT_LIST_DIR}/serial.cpp
)
pico_generate_pio_header(serial ${CMAKE_CURRENT_LIST_DIR}/serial.pio)

# Add include directory
target_include_directories(serial INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Add the standard library to the build
target_link_libraries(serial INTERFACE hardware_pio pico_stdlib)
