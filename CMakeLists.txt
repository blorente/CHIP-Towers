cmake_minimum_required(VERSION 2.8.9)
project(CHIPTowers)

# Use our modified FindSDL2* modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CHIPTowers_SOURCE_DIR}/cmake")
# Set an output directory for our binaries
set(BIN_DIR ${CHIPTowers_SOURCE_DIR}/bin)

# Look up SDL2 and add the include directory to our include path
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

add_subdirectory(emulator)
add_subdirectory(tests)
