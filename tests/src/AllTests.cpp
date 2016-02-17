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

TEST_CASE("Simple JMP working", "[Emulator]") {
    
    Emulator e = Emulator();
    e.reboot();
    
    e.memory[e.pc] = 0x12;
    e.memory[e.pc+1] = 0x20;
    e.emulateCycle();
    
    REQUIRE(e.pc == 0x220);
    
}

TEST_CASE("Simple CALL working", "[Emulator]") {
    
    Emulator e = Emulator();
    e.reboot();
    
    e.memory[e.pc] = 0x22;
    e.memory[e.pc + 1] = 0x40;
    e.emulateCycle();
    
    REQUIRE(e.stack.sp == 1);
    REQUIRE(e.stack.stack[0] == 0x0202);
    REQUIRE(e.pc == 0x0240);
}

TEST_CASE("Simple SE working", "[Emulator]") {
    
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0x40;
    e.memory[e.pc] = 0x32;
    e.memory[e.pc + 1] = 0x40;
    e.emulateCycle();
   
    REQUIRE(e.pc == 0x0204);
    
}

TEST_CASE("Simple SNE working", "[Emulator]") {
    
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0x30;
    e.memory[e.pc] = 0x42;
    e.memory[e.pc + 1] = 0x40;
    e.emulateCycle();
   
    REQUIRE(e.pc == 0x0204);
    
}

TEST_CASE("Simple SE VV working", "[Emulator]") {
    
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0x30;
    e.V[4] = 0x30;
    e.memory[e.pc] = 0x52;
    e.memory[e.pc + 1] = 0x40;
    e.emulateCycle();
    REQUIRE(e.V[2] == e.V[4]);
    REQUIRE(e.pc == 0x0204);
    
}

TEST_CASE("Simple LD working", "[Emulator]") {
    
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0x30;
    e.memory[e.pc] = 0x62;
    e.memory[e.pc + 1] = 0x05;
    e.emulateCycle();
    REQUIRE(e.V[2] == 0x05);
    REQUIRE(e.pc == 0x0202);
    
}

TEST_CASE("Simple ADD working", "[Emulator]") {
    
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0x30;
    e.memory[e.pc] = 0x72;
    e.memory[e.pc + 1] = 0x05;
    e.emulateCycle();
    REQUIRE(e.V[2] == 0x35);
    REQUIRE(e.pc == 0x0202);
    
}

TEST_CASE("Simple 8LD working", "[Emulator]") {
    
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0x30;
    e.V[0] = 0X35;
    e.memory[e.pc] = 0x82;
    e.memory[e.pc + 1] = 0x00;
    e.emulateCycle();
    REQUIRE(e.V[2] == 0x35);
    REQUIRE(e.pc == 0x0202);
    
}

TEST_CASE("Simple AND working", "[Emulator]") {
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0xFF;
    e.V[0] = 0X0;
    e.memory[e.pc] = 0x82;
    e.memory[e.pc + 1] = 0x02;
    e.emulateCycle();
    REQUIRE(e.V[2] == 0x0);
    REQUIRE(e.pc == 0x0202);
}

TEST_CASE("Simple OR working", "[Emulator]") {
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0xFF;
    e.V[0] = 0X0;
    e.memory[e.pc] = 0x82;
    e.memory[e.pc + 1] = 0x01;
    e.emulateCycle();
    REQUIRE(e.V[2] == 0xFF);
    REQUIRE(e.pc == 0x0202);
}

TEST_CASE("Simple XOR working", "[Emulator]") {
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0xFF;
    e.V[0] = 0X0F;
    e.memory[e.pc] = 0x82;
    e.memory[e.pc + 1] = 0x03;
    e.emulateCycle();
    REQUIRE(e.V[2] == 0xF0);
    REQUIRE(e.pc == 0x0202);
}

TEST_CASE("Simple ADD VV working", "[Emulator]") {
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0xF1;
    e.V[0] = 0X0F;
    e.memory[e.pc] = 0x82;
    e.memory[e.pc + 1] = 0x04;
    e.emulateCycle();
    REQUIRE(e.V[2] == 0x00);
    REQUIRE(e.V[0XF] == 1);
    REQUIRE(e.pc == 0x0202);
}

TEST_CASE("Simple SUB VV working", "[Emulator]") {
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0x0F;
    e.V[0] = 0X10;
    e.memory[e.pc] = 0x82;
    e.memory[e.pc + 1] = 0x05;
    e.emulateCycle();
    REQUIRE(e.V[2] == 0x00);
    REQUIRE(e.V[0XF] == 1);
    REQUIRE(e.pc == 0x0202);
}

TEST_CASE("Simple SHR VV working", "[Emulator]") {
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0x00;
    e.V[0] = 0X11;
    e.memory[e.pc] = 0x82;
    e.memory[e.pc + 1] = 0x06;
    e.emulateCycle();
    REQUIRE(e.V[2] == 0x08);
    REQUIRE(e.V[0XF] == 1);
    REQUIRE(e.pc == 0x0202);
}

TEST_CASE("Simple SUB VVy working", "[Emulator]") {
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0x0F;
    e.V[0] = 0X10;
    e.memory[e.pc] = 0x82;
    e.memory[e.pc + 1] = 0x07;
    e.emulateCycle();
    REQUIRE(e.V[2] == 0x01);
    REQUIRE(e.V[0XF] == 0);
    REQUIRE(e.pc == 0x0202);
}

TEST_CASE("Simple SHL VV working", "[Emulator]") {
    Emulator e = Emulator();
    e.reboot();
    
    e.V[2] = 0x00;
    e.V[0] = 0X81;
    e.memory[e.pc] = 0x82;
    e.memory[e.pc + 1] = 0x0E;
    e.emulateCycle();
    REQUIRE(e.V[2] == 0x02);
    REQUIRE(e.V[0XF] == 1);
    REQUIRE(e.pc == 0x0202);
}

#endif