#!/usr/bin/make
EMU_TARGET=emulator.exe
EMU_SOURCES=emulator/main.cpp
EMU_OBJECTS=$(patsubst %.c,%.o,$(EMU_SOURCES))

TEST_TARGET=tests.exe
TEST_SOURCES=tests/AllTests.cpp
TEST_OBJECTS=$(patsubst %.c,%.o,$(TEST_SOURCES))
MY_INCLUDES=
HEADERS=$(wildcard $(MY_INCLUDES)/*.h)
OS=$(shell uname)
LDFLAGS=
#CFLAGS=-g -Wall 
CFLAGS=-g -Wall 
#-DPOSIX_BARRIER

all: $(EMU_TARGET) $(TEST_TARGET)
	
$(EMU_TARGET): $(EMU_OBJECTS)
	g++  -o  $(EMU_TARGET) $(EMU_OBJECTS) $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJECTS)
	g++ -o $(TEST_TARGET) $(TEST_OBJECTS) $(LDFLAGS)

.SUFFIXES:      .o .cpp .h

.h:

.cpp.o: $(HEADERS)
	g++  $(CFLAGS)  -I$(MY_INCLUDES) -c $< -o $@  -Wall
	
clean:
	rm -f *.o
	rm -f $(EMU_TARGET)
	rm -f $(TEST_TARGET)
	rm -f  *~
	
#CC=g++
#CFLAGS=-c -Wall
#
#EXES_DIR=bin
#OBJECTS_DIR=out
## Emulator Vars
#EMU_SOURCES=$(EMU_DIR)/main.cpp $(EMU_DIR)/other.cpp
#EMU_OBJECTS=$(OBJECTS_DIR)/$(EMU_DIR)/main.o $(OBJECTS_DIR)/$(EMU_DIR)/other.o
#EMU_DIR=emulator
##Test Vars
#TEST_DIR=tests
#TEST_SOURCES=AllTests.cpp
#TEST_OBJECTS=AllTests.o
#
#all: tests emulator
#
## Emulator build
#emulator: $(EMU_OBJECTS)
#	$(CC) $(EMU_OBJECTS) -o $(EXES_DIR)/CHIPTowers.exe
#	
#$(EMU_OBJECTS): $(EMU_SOURCES)
#	$(CC) $(CFLAGS) $(EMU_SOURCES) -o $(EMU_OBJECTS)
#
## Tests build
#tests: $(OBJECTS_DIR)/$(TEST_DIR)/$(TEST_OBJECTS)
#	$(CC) $(OBJECTS_DIR)/$(TEST_DIR)/$(TEST_OBJECTS) -o $(EXES_DIR)/tests.exe
#
#$(OBJECTS_DIR)/$(TEST_DIR)/$(TEST_OBJECTS): $(TEST_DIR)/$(TEST_SOURCES)
#	$(CC) $(CFLAGS) $(TEST_DIR)/$(TEST_SOURCES) -o $(OBJECTS_DIR)/$(TEST_DIR)/$(TEST_OBJECTS)
	