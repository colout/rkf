# Add library cpp files
add_library(helpers INTERFACE)
target_sources(helpers INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/helpers.cpp
    ${CMAKE_CURRENT_LIST_DIR}/helpers.h
)

# Add include directory
target_include_directories(helpers INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Add the standard library to the build
target_link_libraries(helpers INTERFACE pico_stdlib)