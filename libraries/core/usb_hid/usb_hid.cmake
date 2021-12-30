# Add library cpp files
add_library(usb_hid INTERFACE)
target_sources(usb_hid INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/usb_hid.cpp
    ${CMAKE_CURRENT_LIST_DIR}/usb_hid.h
    ${CMAKE_CURRENT_LIST_DIR}/tusb_config.h
    ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.h
    ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.cpp

)

# Add include directory
target_include_directories(usb_hid INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Add the standard library to the build
target_link_libraries(usb_hid INTERFACE pico_stdlib tinyusb_device tinyusb_board)
