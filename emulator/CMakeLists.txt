cmake_minimum_required(VERSION 2.8.9)

add_executable(emulator src/main.cpp)

target_link_libraries(emulator ${SDL2_LIBRARY} -L${SDL2_PATH} -lmingw32 -lSDL2main -lSDL2 -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid)