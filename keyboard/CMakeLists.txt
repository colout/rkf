
add_executable(keyboard config.h keyboard.cpp matrix_scan.cpp)

include("${CMAKE_CURRENT_LIST_DIR}/../libraries/core/rgb/rgb.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../libraries/core/helpers/helpers.cmake")

pico_enable_stdio_usb(keyboard 1)
pico_enable_stdio_uart(keyboard 0)

target_link_libraries(keyboard pico_stdlib helpers rgb pico_bootsel_via_double_reset)

pico_add_extra_outputs(keyboard)
