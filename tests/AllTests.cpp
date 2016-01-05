#include <iostream>
#include "../emulator/other.cpp"

#define CATCH_CONFIG_MAIN
#include "../includes/catch.hpp"

TEST_CASE( "Test Framework Working", "[Dumb]" ) {
    REQUIRE( 1 == 1 );
    REQUIRE( 1 + 1 == 2 );
    REQUIRE( 1 - 7 != 6 );
}

TEST_CASE ("Emulator import working", "[Emulator]") {
	REQUIRE( returnOne() == 1 );
}