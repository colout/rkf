
add_executable(rkf config.h main.c matrix_scan.cpp process_record.h process_record.cpp keycode.h tusb_config.h usb_descriptors.cpp usb_descriptors.h)

target_include_directories(rkf PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

include("${CMAKE_CURRENT_LIST_DIR}/../libraries/core/helpers/helpers.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../libraries/core/rgb/rgb.cmake")
#include("${CMAKE_CURRENT_LIST_DIR}/../libraries/core/usb_hid/usb_hid.cmake")

pico_enable_stdio_usb(rkf 1)
pico_enable_stdio_uart(rkf 0)

target_link_libraries(
    rkf 
    #usb_hid 
    helpers 
    rgb 
    pico_stdlib 
    tinyusb_device
    tinyusb_board
    pico_bootsel_via_double_reset
    )

pico_add_extra_outputs(rkf)
