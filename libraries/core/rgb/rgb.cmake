# Add library cpp files
add_library(rgb INTERFACE)
target_sources(rgb INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/rgb_matrix.cpp
    ${CMAKE_CURRENT_LIST_DIR}/rgb_matrix_types.h
    ${CMAKE_CURRENT_LIST_DIR}/color.h
    ${CMAKE_CURRENT_LIST_DIR}/color.cpp
)
pico_generate_pio_header(rgb ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

# Add include directory
target_include_directories(rgb INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Add the standard library to the build
target_link_libraries(rgb INTERFACE hardware_pio pico_stdlib)
