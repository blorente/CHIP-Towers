#include <iostream>
#include "Emulator.hpp"
#include "SDL.h"


int main(int argc, char *argv[]) {
	
    Emulator emulator = Emulator();
    emulator.reboot();
    if (argc > 1) {
        emulator.loadROM(argv[1]);
    } else {
        emulator.loadROM("INVADERS");            
    }
    emulator.run();
    
    return 0;
}