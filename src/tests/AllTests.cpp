#include <iostream>
#include "../emulator/Emulator.hpp"

#define CATCH_CONFIG_MAIN
#include "../include/catch.hpp"

TEST_CASE( "Test Framework Working", "[Dumb]" ) {
    REQUIRE( 1 == 1 );
    REQUIRE( 1 + 1 == 2 );
    REQUIRE( 1 - 7 != 6 );
}

TEST_CASE ("Emulator empty constructor working", "[Emulator]") {
	Emulator* e = new Emulator();
	// Test memory initialization
	for (int i = 0; i < 4096; i++) {
		REQUIRE(e->_memory[i] == 0);
	}
	// Test pc and I
	REQUIRE(e->_pc == 0);
	REQUIRE(e->_I == 0);
	// Test register bank
	for (int i = 0; i < 16; i++) {
		REQUIRE(e->_V[i] == 0);
	}
	// Test stack
	REQUIRE(e->_stack.sp == 0);
	for (int i = 0; i < 16; i++) {
		REQUIRE(e->_stack.stack[i] == 0);
	}
	// Test timers
	REQUIRE(e->_timers.d == 0);
	REQUIRE(e->_timers.s == 0);
	// Test screen buffer
	for (int i = 0; i < (64 * 32); i++) {
		REQUIRE(e->_scrbuf[i] == 0);
	}
}


