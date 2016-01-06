cmake_minimum_required(VERSION 2.8.9)
project(emulator)


add_executable(emulator src/emulator/main.cpp)

include_directories(include)
add_executable(tests src/tests/AllTests.cpp src/include/catch.hpp src/emulator/Emulator.hpp)