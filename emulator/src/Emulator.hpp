#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <iostream>
#include <vector>

class Emulator {
public:

	Emulator() {	
		_opcode = 0;
		_memory = std::vector<unsigned char>(4096);
		_pc = 0;
		_I = 0;
		_V = std::vector<unsigned char>(16);	
		_scrbuf = std::vector<unsigned char>(64 * 32);	
		_stack.stack = std::vector<unsigned char>(16);
		_stack.sp = 0;
		_timers.d = 0;
		_timers.s = 0;
	}
		
	~Emulator(){}
	
public: //protected:
	
	typedef struct {
		unsigned char sp;
		std::vector<unsigned char>stack;
	} stack_t;

	typedef struct {
		unsigned char d;
		unsigned char s;
	} timers_t;

public: //private:
	
	unsigned char _opcode;
	
	std::vector<unsigned char> _memory; //Memory allocation
	unsigned short _pc; //Program counter (16 bit)
	unsigned short _I; // Index counter (16 bit)
	std::vector<unsigned char> _V;
	stack_t _stack;
	timers_t _timers;
	std::vector<unsigned char> _scrbuf; // Monochrome display buffer
	
};


#endif