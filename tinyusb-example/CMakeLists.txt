cmake_minimum_required(VERSION 3.13)

add_executable(dev_hid_composite)

target_sources(dev_hid_composite PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        )



pico_enable_stdio_usb(dev_hid_composite 1)
pico_enable_stdio_uart(dev_hid_composite 0)
                

# Make sure TinyUSB can find tusb_config.h
target_include_directories(dev_hid_composite PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(dev_hid_composite PUBLIC pico_stdlib tinyusb_device tinyusb_board pico_bootsel_via_double_reseta)

pico_add_extra_outputs(dev_hid_composite)

