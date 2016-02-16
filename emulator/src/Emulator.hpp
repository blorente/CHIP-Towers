#ifndef EMULATORH
#define EMULATORH

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <functional>

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
        
        setupInstructionSet();
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
        char buf[memory.size() - 512];     
        
        std::ifstream rom_file(filename, std::ios::binary | std::ios::in);
        if (rom_file.is_open()) {
            int i = 0;
            while(!rom_file.eof()) {
                memory[i + 512] = rom_file.get();
                i++;
            }
            rom_file.close();
        } else {
            std::cout << "File not loaded properly" << std::endl;
        }
    }
    
	void Run() {
        int cycle = 0;
        char q;
        do {
            std::cin >> q;
            std::cin.get();     
            std::cout << "Cycle: " << std::dec << cycle++ << " | ";
            
            emulateCycle();
            
        } while (q != 'q');
    }
	
public: //private:    

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
    
    void setupInstructionSet() {
        
        instruction_set = std::vector<instruction_t>(36, {"unimplemented", [&] (){printInstruction("unimplemented", instruction);}});
        
        instruction_set[0] = {"zero", [&] () {
                                        if (instruction & 0x000F > 0) {
                                            printInstruction("RET", instruction);
                                        } else {
                                            printInstruction("CLS", instruction);
                                        }
                                  }
                             };
        instruction_set[1] = {"0x1nnn: JP addr", [&] (){printInstruction("0x1nnn: JP addr", instruction); pc = instruction & 0x0FFF;}};
        
    }
    
    void emulateCycle() {
        //Fetch
        instruction = ((memory[pc] << 8) | memory[pc + 1]); // 1 instr = 2 bytes = 2 memory locations 
        //Decode & Execute
        instruction_set[(instruction & 0xF000) >> 12].function();
        
        pc += 2;
        
    }
    
    void printInstruction(std::string descr, unsigned short hexvalue) {
        std::cout << descr << " -> 0x" << std::hex << hexvalue << " memory[" << (short)memory[pc] << "|" << (short)memory[pc+1] << "]";
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
    
    typedef struct {
        std::string dissasembly;
        std::function<void()> function;
    } instruction_t;

public: //private:
	
	unsigned short instruction;
	
	std::vector<unsigned char> memory; //Memory allocation
	unsigned short pc; //Program counter (16 bit)
	unsigned short I; // Index counter (16 bit)
	std::vector<unsigned char> V;
	stack_t stack;
	timers_t timers;
	std::vector<unsigned char> scrbuf; // Monochrome display buffer	
    
    std::vector<instruction_t> instruction_set;
    
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