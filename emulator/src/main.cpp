#include <iostream>
#include "chip8_emulator.hpp"
#include "SDL.h"


int main(int argc, char *argv[]) {
	
    cpp::emulator::chip8_emulator emulator;
    emulator.reboot();
    
    if (argc > 1) {
        emulator.load_rom(argv[1]);
        emulator.run();
    } else {
        std::cout << "Please specify a file to load\n";           
    }    
    
    return 0;
}