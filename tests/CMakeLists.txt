cmake_minimum_required(VERSION 2.8.9)

include_directories(include)
include_directories(../emulator/src)
include_directories(../emulator/res)
add_executable(tests src/AllTests.cpp include/catch.hpp)
