#include <iostream>
#include "Emulator.hpp"
#include "SDL.h"


int main(int argc, char *argv[]) {
	
    Emulator emulator = Emulator();
    emulator.reboot();
    
    if (argc > 1) {
        emulator.load_rom(argv[1]);
        emulator.run();
    } else {
        std::cout << "Please specify a file to load\n";           
    }    
    
    return 0;
}