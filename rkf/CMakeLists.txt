
add_executable(rkf 
        main.cpp
        matrix_scan.cpp
        process_record.cpp
        config.h
        config_rgb.h
        keycodes.h
        matrix_scan.h
        process_record.h
        usb_descriptors.c
        usb_descriptors.h
        tusb_config.h
        hid.c
        hid.h
        animation.h
        animation.cpp
        serial.h
        serial.cpp
        transactions.h
        transactions.cpp
        transaction_id_define.h
    ) 

include("${CMAKE_CURRENT_LIST_DIR}/../libraries/core/rgb/rgb.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../libraries/core/helpers/helpers.cmake")

pico_enable_stdio_usb(rkf 1)
pico_enable_stdio_uart(rkf 0)

target_link_libraries(rkf 
        pico_stdlib 
        tinyusb_device
        tinyusb_board
        helpers 
        rgb 
        pico_bootsel_via_double_reset
    )

# create map/bin/hex file etc.
pico_add_extra_outputs(rkf)