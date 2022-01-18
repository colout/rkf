cmake_minimum_required(VERSION 3.12)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-10000")


set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/pico-sdk)
set(PICO_BOARD sparkfun_promicro)
include(pico_sdk_import.cmake)

# This is bad...should I use a specific config directory???
include_directories ("${CMAKE_CURRENT_LIST_DIR}/rkf")

set(ProjectName "rkf")

project(rkf C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)
set(PICO_CXX_ENABLE_RTTI 1)

pico_sdk_init()

add_subdirectory(rkf)