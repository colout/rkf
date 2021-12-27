# Add library cpp files
add_library(rgb INTERFACE)
target_sources(rgb INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/rgb_matrix.cpp
    ${CMAKE_CURRENT_LIST_DIR}/rgb_matrix_types.h
)

# Add include directory
target_include_directories(rgb INTERFACE ${CMAKE_CURRENT_LIST_DIR})

include("${CMAKE_CURRENT_LIST_DIR}/../../external/PicoLed/PicoLed.cmake")

# Add the standard library to the build
target_link_libraries(rgb INTERFACE pico_stdlib PicoLed)