
add_executable(rkf 
        main.cpp
        matrix_scan.cpp
        process_record.cpp
        config.h
        keycodes.h
        matrix_scan.h
        process_record.h
    )

include("${CMAKE_CURRENT_LIST_DIR}/../libraries/core/rgb/rgb.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../libraries/core/helpers/helpers.cmake")

pico_enable_stdio_usb(rkf 1)
pico_enable_stdio_uart(rkf 0)

target_link_libraries(rkf pico_stdlib helpers rgb pico_bootsel_via_double_reset)

# create map/bin/hex file etc.
pico_add_extra_outputs(rkf)