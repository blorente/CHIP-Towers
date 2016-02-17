#include <iostream>
#include "other.cpp"
#include "Emulator.hpp"
#include "SDL.h"

extern void doSomeStuff();

int main(int argc, char *argv[]) {
	
    Emulator emulator = Emulator();
    emulator.reboot();
    if (argc > 1) {
        emulator.loadROM(argv[1]);
    } else {
        emulator.loadROM("INVADERS");            
    }
    emulator.Run();
    
    return 0;
}