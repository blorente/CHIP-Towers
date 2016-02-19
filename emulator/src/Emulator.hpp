#ifndef EMULATOR_H
#define EMULATOR_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <functional>
#include <bitset>
#include <random>
#include <SDL.h>

#include "MonochromeScreen.hpp"
#include "Keypad.hpp"

class Emulator {
public:

	Emulator() {
        
        if (SDL_WasInit(0) != SDL_INIT_EVERYTHING) {
            SDL_Init(SDL_INIT_EVERYTHING);
        }
        	
        screen = new MonochromeScreen(SCREEN_WIDTH, SCREEN_HEIGHT, "CHIP Towers");  
        keypad = new Keypad();      
        
		memory = std::vector<unsigned char>(4096);
		V = std::vector<unsigned char>(16);	
		scrbuf = std::vector<bool>(SCREEN_WIDTH * SCREEN_HEIGHT, {false});	
        stack.stack = std::vector<unsigned short>(16);
		stack.sp = 0;
		timers.d = 0;
		timers.s = 0;
        I = 0;
        
        drawFlag = false;
        
        setupInstructionSet();
	}
		
	~Emulator(){
        
        delete screen;
        delete keypad;
        SDL_Quit();
    }
    
private:
    
    const unsigned char SCREEN_WIDTH = 64;
    const unsigned char SCREEN_HEIGHT = 32;
    
    const bool DRAW_ON_CONSOLE = false;
    const bool DEBUG_MODE = false;
    const bool DEBUG_INPUT_MODE = false;
    const int fps = 60;
    
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
    
	void run() {
        Uint32 startingTick;
        bool running = true;        
        SDL_Event sdlEvent;
        
        int cycle = 0;
        
        while (running)  { 
            startingTick = SDL_GetTicks();
                  
            emulateCycle();
            
            while( SDL_PollEvent(&sdlEvent)) {
                if (sdlEvent.type == SDL_QUIT) {
                    running = false;
                    break;
                } else if (sdlEvent.type == SDL_KEYDOWN) {
                    keypad->processKeyDown(sdlEvent);
                } else if (sdlEvent.type == SDL_KEYUP) {
                    keypad->processKeyUp(sdlEvent);
                }
            }
            
            cycle = (cycle + 1) % (fps);
            
            if (cycle % 4 == 0) {
                if (timers.d == 0) {
                    timers.d = 60;
                } else {
                    timers.d--;
                }
            }
            
            if (DEBUG_INPUT_MODE) {                
                if (cycle == 0) {
                    keypad->print();
                }
            }
            
            SDL_Delay(1);
            
            /*
            // Cap framerate
            if ((1000.0 / fps) > (SDL_GetTicks() - startingTick)) {
                SDL_Delay((1000 / fps) - (SDL_GetTicks() - startingTick)); 
            } */          
                       
        };
    }
	
public: //private:    

    void initComponents() {       
        memory.assign(memory.size(), 0x0);     
        V.assign(V.size(), 0x0);		
        scrbuf.assign((64 * 32), false);
        stack.stack.assign(16, 0x0); 
        pc = 0x200;
        I = 0x0;        
		stack.sp = 0;
		timers.d = 0;
		timers.s = 0;    
        drawFlag = false;              
    }
	
	
	void loadFontset() {		
		for (int i = 0; i < 80; i++) {
			memory[i] = chip8fontset[i];			
		}
	}
    
    void setupInstructionSet() {
        
        instruction_set_base = std::vector<instruction_t>(36, {"unimplemented", [&] (){printInstruction("unimplemented", instruction);}});
        instruction_set_8x = std::vector<instruction_t>(36, {"unimplemented", [&] (){printInstruction("unimplemented", instruction);}});
        instruction_set_Fx = std::vector<instruction_t>(0x70, {"unimplemented", [&] (){printInstruction("unimplemented", instruction);}});
        
        
        instruction_set_base[0] = {"zero", [&] () {
                                        if ((instruction & 0x000F) > 0) {
                                            printInstruction("0x00EE: RET" + (instruction & 0x000F), instruction);
                                            pc = stack.stack[stack.sp - 1];
                                            stack.sp --; 
                                        } else {
                                            printInstruction("0x00E0: CLS" + (instruction & 0x000F), instruction);
                                            scrbuf.assign(scrbuf.size(), false);
                                            drawFlag = true;
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
                                                 
        instruction_set_base[0x9] = {"0x9xy0: SNE Vx, Vy", [&] () {printInstruction("0x9xy0: SKNE Vx, Vy", instruction); 
                                                                    if (V[(instruction & 0x0F00) >> 8] != V[(instruction & 0x00F0) >> 4]) {pc += 2;}}};
        instruction_set_base[0xA] = {"0xAnnn: LD I, addr", [&] () {printInstruction("0xAnnn: LD I, addr", instruction); 
                                                                 I = instruction & 0x0FFF;}};
        instruction_set_base[0xB] = {"0xBnnn: JP V0, addr", [&] () {printInstruction("0xBnnn: JP V0, addr", instruction); 
                                                                 pc = V[0] + (instruction & 0x0FFF);}};
        
        instruction_set_base[0xC] = {"Cxkk: RND Vx, byte", [&] () {printInstruction("Cxkk: RND Vx, byte", instruction);
                                                            unsigned char rnd = std::rand() % 256;
                                                            unsigned short x = (instruction & 0x0F00) >> 8;
                                                            unsigned short kk= (instruction & 0x00FF);
                                                            V[x] = rnd & kk; 
                                                            }};
        
        instruction_set_base[0xD] = {"0xDxyn: DRW Vx, Vy, nibble", [&] () {printInstruction("0xDxyn: DRW Vx, Vy, nibble", instruction);
                                                                           unsigned char xx = (instruction & 0x0F00) >> 8;
                                                                           unsigned char yy = (instruction & 0x00F0) >> 4; 
                                                                           unsigned char n =  (instruction & 0x000F);
                                                                           drawSprite(V[xx], V[yy], n);}};
        
        instruction_set_base[0xE] = {"", [&] () {if ((instruction & 0x00FF) == 0x9E) {
                                                    printInstruction("Ex9E: SKP Vx", instruction);
                                                    if (keypad->isKeyPressed(V[(instruction & 0x0F00) >> 8])) {
                                                        pc += 2;
                                                    }
                                                 } else if ((instruction & 0x00FF) == 0xA1) {
                                                    printInstruction("ExA1: SKNP Vx", instruction);
                                                    if (!keypad->isKeyPressed(V[(instruction & 0x0F00) >> 8])) {
                                                        pc += 2;
                                                    } 
                                                 }
                                                 }};
        
        instruction_set_base[0xF] = {"", [&] () {printInstruction(instruction_set_Fx[instruction & 0x00FF].dissasembly, instruction); 
                                                                 instruction_set_Fx[instruction & 0x00FF].function();}};
                                                                                          
        instruction_set_8x[0x0] = {"0x8xy0: LD Vx, Vy", [&] () {V[(instruction & 0x0F00) >> 8] = V[(instruction & 0x00F0) >> 4];}};
        instruction_set_8x[0x1] = {"0x8xy1: OR Vx, Vy", [&] () {V[(instruction & 0x0F00) >> 8] |= V[(instruction & 0x00F0) >> 4];}};
        instruction_set_8x[0x2] = {"0x8xy2: AND Vx, Vy", [&] () {V[(instruction & 0x0F00) >> 8] &= V[(instruction & 0x00F0) >> 4];}};
        instruction_set_8x[0x3] = {"0x8xy3: XOR Vx, Vy", [&] () {V[(instruction & 0x0F00) >> 8] ^= V[(instruction & 0x00F0) >> 4];}};
        instruction_set_8x[0x4] = {"0x8xy4: ADD Vx, Vy", [&] () {int res = V[(instruction & 0x0F00) >> 8] + V[(instruction & 0x00F0) >> 4];
                                                                 if (res > 0xFF) {V[0xF] = 1;} else {V[0xF] = 0;}
                                                                 V[(instruction & 0x0F00) >> 8] = res & 0xFF;}};
        instruction_set_8x[0x5] = {"0x8xy5: SUB Vx, Vy", [&] () {unsigned char xx = (instruction & 0x0F00) >> 8;
                                                                 unsigned char yy = (instruction & 0x00F0) >> 4;            
                                                                 if ( V[xx] < V[yy]) {V[xx] = (unsigned char)(0xFF - (V[yy] - V[xx])); V[0xF] = 1;} 
                                                                 else {V[xx] = V[xx] - V[yy]; V[0xF] = 0;}}};
        instruction_set_8x[0x6] = {"0x8xy6: SHR Vx, Vy", [&] () {unsigned char xx = (instruction & 0x0F00) >> 8;
                                                                 unsigned char yy = (instruction & 0x00F0) >> 4;            
                                                                 V[0xF] = V[yy] && 1;
                                                                 V[xx] = V[yy] >> 1;}};
        instruction_set_8x[0x7] = {"0x8xy7: SUB Vy, Vx", [&] () {unsigned char xx = (instruction & 0x0F00) >> 8;
                                                                 unsigned char yy = (instruction & 0x00F0) >> 4;            
                                                                 if ( V[xx] > V[yy]) {V[xx] = (unsigned char)(0x0100 + V[yy] - V[xx]); V[0xF] = 1;} 
                                                                 else {V[xx] = V[yy] - V[xx]; V[0xF] = 0;}}};
        instruction_set_8x[0xE] = {"0x8xy6: SHL Vx, Vy", [&] () {unsigned char xx = (instruction & 0x0F00) >> 8;
                                                                 unsigned char yy = (instruction & 0x00F0) >> 4;            
                                                                 V[0xF] = V[yy] && 0x80;
                                                                 V[xx] = V[yy] << 1;}};
        instruction_set_Fx[0x07] = {"0xFx07: LD Vx, DT", [&] () {V[(instruction & 0x0F00) >> 8] = timers.d;}};  
        instruction_set_Fx[0x0A] = {"0xFx0A: LD Vx, K", [&] () {if (!keypad->isAnyKeyPressed()) {
                                                                   pc -= 2; 
                                                                } else {
                                                                    V[(instruction & 0x0F00) >> 8] = keypad->lastKeyPressed();
                                                                }
                                                                }};    
        instruction_set_Fx[0x15] = {"0xFx15: LD DT, Vx", [&] () {timers.d = V[(instruction & 0x0F00) >> 8];}};    
        instruction_set_Fx[0x18] = {"0xFx18: LD ST, Vx", [&] () {timers.s = V[(instruction & 0x0F00) >> 8];}};         
        instruction_set_Fx[0x1E] = {"0xFx1E: ADD I, Vx", [&] () {I += V[(instruction & 0x0F00) >> 8];}};    
        instruction_set_Fx[0x29] = {"0xFx29: LD F, Vx", [&] () {I = V[(instruction & 0x0F00) >> 8] * 5;}};
        instruction_set_Fx[0x33] = {"0xFx33: LD B, Vx", [&] () {int n = V[(instruction & 0x0F00) >> 8];
                                                                memory[I] = n / 100;
                                                                memory[I + 1] = (n / 10) % 10 ;
                                                                memory[I + 2] =n % 10;}};    
        instruction_set_Fx[0x55] = {"0xFx55: STR [I], Vx", [&] () {int last = (instruction & 0x0F00) >> 8;
                                                                    for (int i = 0; i < last + 1; i++) {memory[I + i] = V[i];}
                                                                    I = I + last + 1;}};    
        instruction_set_Fx[0x65] = {"0xFx65: LD Vx, [I]", [&] () {int last = (instruction & 0x0F00) >> 8;
                                                                    for (int i = 0; i < last + 1; i++) {V[i] = memory[I + i];}
                                                                    I = I + last + 1;}};            
    }
    
    void emulateCycle() {
        //Fetch
        instruction = ((memory[pc] << 8) | memory[pc + 1]); // 1 instr = 2 bytes = 2 memory locations 
        pc += 2;
        //Decode & Execute
        instruction_set_base[(instruction & 0xF000) >> 12].function();
        
        if (drawFlag) {
            
            screen->display(scrbuf);
            
            if (DRAW_ON_CONSOLE) {
                drawScreenToConsole(); 
            }   
          
            drawFlag = false;
        }
        
    }
    
    void printInstruction(std::string descr, unsigned short hexvalue) {
        if (DEBUG_MODE) {
        std::cout << descr << " -> 0x" << std::hex << hexvalue << " memory[" << (short)memory[pc-2] << "|" << (short)memory[pc-1] << "] [" 
                                                                             << (short)memory[pc] << "|" << (short)memory[pc+1] << "]" << std::endl;
        }
    }
    
    void drawScreenToConsole() {
          //Draw screen
            for (int col = 0; col < SCREEN_WIDTH+2; col++)  {
                std::cout << "_";
            }          
            std::cout << std::endl;    
            for (int col = 0; col < SCREEN_WIDTH+2; col++)  {
                std::cout << "_";
            }    
            std::cout << std::endl;
            for (int row = 0; row < SCREEN_HEIGHT; row++) {
                std::cout << "|";
                for (int col = 0; col < SCREEN_WIDTH; col++) {
                    if (scrbuf[(row*SCREEN_WIDTH)+col] == true) {std::cout << "X";} else {std::cout << " ";}
                }
                std::cout << "|" << std::endl;
            }
            for (int col = 0; col < SCREEN_WIDTH + 2; col++)  {
                std::cout << "_";
            } 
            std::cout << std::endl; 
    }
    
    void drawSprite(unsigned char posx, unsigned char posy, unsigned char n) {
        //1 memory location = 8 bits = 8 pixels
        //n=2 -> xxxxxxxx = memory[I]
        //       xxxxxxxx = memory[I + 1]
        V[0xF] = 0;              
        unsigned char spriteRow;  
        unsigned char pixel;
        bool overwrite = false;
        for (int row = 0; row < n; row++) {            
            spriteRow = memory[I + row];  
            
            if (DEBUG_MODE) {
                std::bitset<8> x(spriteRow);
                std::cout << x << std::endl; 
            }         
            for (int col = 0; col < 8; col++) {                              
                pixel =  (spriteRow & (0x80 >> col)) ;
                if (pixel > 0) {
                    overwrite = drawPixel((col + posx), (row + posy));                   
                    if (overwrite) {
                        V[0xF] = 1;
                    }
                }                
            }            
        }
        drawFlag = true;
    }
    
    bool drawPixel(unsigned char x, unsigned char y) {
        
        bool res = false;
        int pos = x + (y * SCREEN_WIDTH);
        if (scrbuf[pos] == true) {
            res = true;
        }
        scrbuf[pos] = !scrbuf[pos];
        
        return res;
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
    bool drawFlag;    
	
	std::vector<unsigned char> memory; //Memory allocation
	unsigned short pc; //Program counter (16 bit)
	unsigned short I; // Index counter (16 bit)
	std::vector<unsigned char> V;
	stack_t stack;
	timers_t timers;
	std::vector<bool> scrbuf; // Monochrome display buffer
    
    std::vector<instruction_t> instruction_set_base;
    std::vector<instruction_t> instruction_set_8x;
    std::vector<instruction_t> instruction_set_Fx;
    
    MonochromeScreen* screen;
    Keypad* keypad;
    
	
};


#endif