#ifndef ALLTESTS
#define ALLTESTS

#include <iostream>
#include <fstream>
#include "Emulator.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE( "Test Framework Working", "[Dumb]" ) {
    REQUIRE( 1 == 1 );
    REQUIRE( 1 + 1 == 2 );
    REQUIRE( 1 - 7 != 6 );
}


TEST_CASE ("Emulator reboot working", "[Emulator]") {
    Emulator* e = new Emulator();
    
    e->V[3] = 2;
    
    e->reboot();
    
    unsigned char chip8fontset[80] =
	{ 
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};	
	
	
	// Test memory initialization
	for (int i = 0; i < 80; i++) {
		REQUIRE(e->memory[i] == chip8fontset[i]);
	}    
    
	for (int i = 80; i < e->memory.size(); i++) {
		REQUIRE(e->memory[i] == 0);
	}
	// Test pc and I
	REQUIRE(e->pc == 0x200);
	REQUIRE(e->I == 0);
	// Test register bank
	for (int i = 0; i < e->V.size(); i++) {
		REQUIRE(e->V[i] == 0);
	}
	// Test stack
	REQUIRE(e->stack.sp == 0);
	for (int i = 0; i < 16; i++) {
		REQUIRE(e->stack.stack[i] == 0);
	}
	// Test timers
	REQUIRE(e->timers.d == 0);
	REQUIRE(e->timers.s == 0);
	// Test screen buffer
	for (int i = 0; i < (64 * 32); i++) {
		REQUIRE(e->scrbuf[i] == 0);
	}
}
/*
TEST_CASE ("Fontset load working", "[Emulator]") {	
	
	unsigned char chip8fontset[80] =
	{ 
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	
	Emulator* e = new Emulator();
	
	e->reboot();
	
	for (int i = 0; i < 80; i++) {
		REQUIRE(e->memory[i] == chip8fontset[i]);
	}
} */

TEST_CASE("Load ROM working", "[Emulator]") {
        
     Emulator e = Emulator();
     e.reboot();
     e.loadROM("15PUZZLE");
     
    std::vector<unsigned char> buf;     
        
    std::ifstream rom_file("15PUZZLE", std::ios::binary | std::ios::in);
    if (rom_file.is_open()) {
        while(!rom_file.eof()) {
            buf.push_back((unsigned char)rom_file.get());
        }
        rom_file.close();
    } else {
        std::cout << "File not loaded properly" << std::endl;
    }
    
    for (int i = 0; i < buf.size(); i++){
       std::cout << std::dec << (short)buf[i] << ' ' ;
       REQUIRE(e.memory[i + 512] == buf[i]);
    }
    
    std::cout << std::endl;
  
}

TEST_CASE("JMP working", "[Emulator]") {
    
    Emulator e = Emulator();
    e.reboot();
    
    e.memory[e.pc] = 0x12;
    e.memory[e.pc+1] = 0x20;
    unsigned short ins = (unsigned short)((e.memory[e.pc] << 8) | e.memory[e.pc + 1]);
    e.instruction = ins;
    e.instruction_set[(e.instruction & 0xF000) >> 12].function();
    
    REQUIRE(e.pc == 0x220);
    
}

#endif