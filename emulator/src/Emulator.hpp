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
		stack.stack = std::vector<unsigned short>(16);
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
        
        instruction_set_base = std::vector<instruction_t>(36, {"unimplemented", [&] (){printInstruction("unimplemented", instruction);}});
        instruction_set_8x = std::vector<instruction_t>(36, {"unimplemented", [&] (){printInstruction("unimplemented", instruction);}});
        
        
        instruction_set_base[0] = {"zero", [&] () {
                                        if (instruction & 0x000F > 0) {
                                            printInstruction("RET", instruction);
                                        } else {
                                            printInstruction("CLS", instruction);
                                        }
                                  }
                             };
        instruction_set_base[0x1] = {"0x1nnn: JP addr", [&] (){printInstruction("0x1nnn: JP addr", instruction); pc = instruction & 0x0FFF;}};
        instruction_set_base[0x2] = {"0x2nnn: CALL addr", [&] (){printInstruction("0x2nnn: CALL addr", instruction); 
                                                            stack.stack[stack.sp] = pc; 
                                                            stack.sp++;
                                                            pc = instruction & 0x0FFF;}};
        instruction_set_base[0x3] = {"0x3xkk: SE Vx, kk", [&] () {printInstruction("0x3xkk: SE Vx, kk", instruction); 
                                                             if (V[(instruction & 0x0F00) >> 8] == (unsigned char) instruction & 0x00FF) {pc += 2;}}};
        instruction_set_base[0x4] = {"0x4xkk: SNE Vx, kk", [&] () {printInstruction("0x4xkk: SNE Vx, kk", instruction); 
                                                             if (V[(instruction & 0x0F00) >> 8] != (unsigned char) instruction & 0x00FF) {pc += 2;}}};
        instruction_set_base[0x5] = {"0x5xy0: SE Vx, Vy", [&] () {printInstruction("0x5xy0: SE Vx, Vy", instruction); 
                                                             if (V[(instruction & 0x0F00) >> 8] == V[(instruction & 0x00F0) >> 4]) {pc += 2;}}};
        instruction_set_base[0x6] = {"0x6xkk: LD Vx, kk", [&] () {printInstruction("0x6xkk: LD Vx, kk", instruction); 
                                                             V[(instruction & 0x0F00) >> 8] = (unsigned char)(instruction & 0x00FF);}};
        instruction_set_base[0x7] = {"0x7xkk: LD Vx, kk", [&] () {printInstruction("0x7xkk: LD Vx, kk", instruction); 
                                                            V[(instruction & 0x0F00) >> 8] += (unsigned char)(instruction & 0x00FF);}};
                                                            
        instruction_set_base[0x8] = {"", [&] () {printInstruction(instruction_set_8x[instruction & 0x000F].dissasembly, instruction);
                                                 instruction_set_8x[instruction & 0x000F].function();}};
                                                 
        instruction_set_8x[0x0] = {"8xy0: LD Vx, Vy", [&] () {V[(instruction & 0x0F00) >> 8] = V[(instruction & 0x00F0) >> 4];}};
              
            
    }
    
    void emulateCycle() {
        //Fetch
        instruction = ((memory[pc] << 8) | memory[pc + 1]); // 1 instr = 2 bytes = 2 memory locations 
        pc += 2;
        //Decode & Execute
        instruction_set_base[(instruction & 0xF000) >> 12].function();
               
        
        
    }
    
    void printInstruction(std::string descr, unsigned short hexvalue) {
        std::cout << descr << " -> 0x" << std::hex << hexvalue << " memory[" << (short)memory[pc-2] << "|" << (short)memory[pc-1] << "]" << std::endl;
    }
		
/* Fields */
public: //protected:
	
	typedef struct {
		unsigned char sp;
		std::vector<unsigned short>stack;
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
    
    std::vector<instruction_t> instruction_set_base;
    std::vector<instruction_t> instruction_set_8x;
    
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