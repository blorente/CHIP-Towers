#ifndef EMULATORH
#define EMULATORH

#include <iostream>
#include <vector>
#include <fstream>

class Emulator {
public:

	Emulator() {	
		memory = std::vector<unsigned char>(4096);
		V = std::vector<unsigned char>(16);	
		scrbuf = std::vector<unsigned char>(64 * 32);	
		stack.stack = std::vector<unsigned char>(16);
		stack.sp = 0;
		timers.d = 0;
		timers.s = 0;
	}
		
	~Emulator(){}
	
/* Methods */
public:

	void reboot() {
        initComponents();
        loadFontset();
    }
    
	void loadROM(std::string filename) {
        std::cout << filename << std::endl;       
        
    }
    
	void Run() {
        int cycle = 0;
        char q;
        do {
            std::cin >> q;
            std::cin.get();     
            std::cout << "Cycle:" << cycle++;
            
            emulateCycle();
            
        } while (q != 'q');
    }
	
private:    

    void initComponents() {       
        memory.assign(memory.size(), 0x0);     
        V.assign(V.size(), 0x0);		
        scrbuf.assign((64 * 32), 0x0);
        stack.stack.assign(16, 0x0); 
        pc = 0x200;
        I = 0x0;        
		stack.sp = 0;
		timers.d = 0;
		timers.s = 0;                  
    }
	
	
	void loadFontset() {		
		for (int i = 0; i < 80; i++) {
			memory[i] = chip8fontset[i];			
		}
	}
    
    void emulateCycle() {
        //Fetch
        instruction = memory[pc] << 8 | memory[pc + 1]; // 1 instr = 2 bytes = 2 memory locations
        pc += 2;
        
        //Decode
        switch (instruction & 0xF000) {
            //Execute
            
            
            default:
                std::cout << " Unknown Instruction: " << instruction << std::endl;
                break;            
        } 
    }
		
/* Fields */
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
	
	unsigned char instruction;
	
	std::vector<unsigned char> memory; //Memory allocation
	unsigned short pc; //Program counter (16 bit)
	unsigned short I; // Index counter (16 bit)
	std::vector<unsigned char> V;
	stack_t stack;
	timers_t timers;
	std::vector<unsigned char> scrbuf; // Monochrome display buffer
	
	const unsigned char chip8fontset[80] =
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

/* opcodes */
private:
	
};


#endif