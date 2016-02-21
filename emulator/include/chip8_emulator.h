#ifndef CHIP8_EMULATOR_H
#define CHIP8_EMULATOR_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <functional>
#include <bitset>
#include <random>
#include <SDL.h>

#include "monochrome_screen.hpp"
#include "chip8_keypad.hpp"

class chip8_emulator {
public:

	chip8_emulator () {
        if (SDL_WasInit(0) != SDL_INIT_EVERYTHING) {
            SDL_Init(SDL_INIT_EVERYTHING);
        }
            
        screen = new monochrome_screen(screen_width, screen_height, "CHIP Towers");  
        keypad = new chip8_keypad();      
        
        memory = std::vector<unsigned char>(4096);
        V = std::vector<unsigned char>(16);	
        scrbuf = std::vector<bool>(screen_width * screen_height, {false});	
        stack.stack = std::vector<unsigned short>(16);
        stack.sp = 0;
        timers.d = 0;
        timers.s = 0;
        I = 0;
        
        draw_flag = false;
        
        setup_instruction_set();
    }
		
	~chip8_emulator(){
        
        delete screen;
        delete keypad;
        SDL_Quit();
    }
    
private:
    
    const unsigned char screen_width = 64;
    const unsigned char screen_height = 32;
    
    const bool draw_on_console = false;
    const bool debug_instructions_enabled = false;
    const bool debug_input_enabled = false;
    const int fps = 60;
    
    const unsigned char chip8_fontset[80] = 
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
        init_components();
        load_fontset();   
    }
    
	void load_rom(std::string filename) {
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
        Uint32 starting_tick;
        bool running = true;        
        SDL_Event sdl_event;
        
        int cycle = 0;
        
        while (running)  { 
            starting_tick = SDL_GetTicks();
                  
            emulate_cycle();
            
            while( SDL_PollEvent(&sdl_event)) {
                if (sdl_event.type == SDL_QUIT) {
                    running = false;
                    break;
                } else if (sdl_event.type == SDL_KEYDOWN) {
                    keypad->process_key_down(sdl_event);
                } else if (sdl_event.type == SDL_KEYUP) {
                    keypad->process_key_up(sdl_event);
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
            
            if (debug_input_enabled) {                
                if (cycle == 0) {
                    keypad->print();
                }
            }
            
            SDL_Delay(1);
            
                
        };
    }
	
private:    

    void init_components() {       
        memory.assign(memory.size(), 0x0);     
        V.assign(V.size(), 0x0);		
        scrbuf.assign(scrbuf.size(), false);
        stack.stack.assign(stack.stack.size(), 0x0); 
        pc = 0x200;
        I = 0x0;        
		stack.sp = 0;
		timers.d = 0;
		timers.s = 0;    
        draw_flag = false;              
    }
	
	
	void load_fontset() {		
		for (int i = 0; i < 80; i++) {
			memory[i] = chip8_fontset[i];			
		}
	}
    
    void setup_instruction_set() {
        
        setup_instruction_set_base();
        setup_instruction_set_8x();
        setup_instruction_set_Fx();        
         
    }
    
    
    void setup_instruction_set_base() {
        instruction_set_base = std::vector<instruction_t>(36, {"unimplemented", [&] (){print_instruction("unimplemented", instruction);}});        
        
        instruction_set_base[0] = {"zero", [&] () {
                                        if ((instruction & 0x000F) > 0) {
                                            print_instruction("0x00EE: RET" + (instruction & 0x000F), instruction);
                                            pc = stack.stack[stack.sp - 1];
                                            stack.sp --; 
                                        } else {
                                            print_instruction("0x00E0: CLS" + (instruction & 0x000F), instruction);
                                            scrbuf.assign(scrbuf.size(), false);
                                            draw_flag = true;
                                        }
                                  }
                             };
        instruction_set_base[0x1] = {"0x1nnn: JP addr", [&] (){print_instruction("0x1nnn: JP addr", instruction); 
                                                               pc = instruction & 0x0FFF;}};
        instruction_set_base[0x2] = {"0x2nnn: CALL addr", [&] (){print_instruction("0x2nnn: CALL addr", instruction); 
                                                                stack.stack[stack.sp] = pc; 
                                                                stack.sp++;
                                                                pc = instruction & 0x0FFF;}};
        instruction_set_base[0x3] = {"0x3xkk: SE Vx, kk", [&] () {print_instruction("0x3xkk: SE Vx, kk", instruction); 
                                                             if (V[(instruction & 0x0F00) >> 8] == (unsigned char) instruction & 0x00FF) {pc += 2;}}};
        instruction_set_base[0x4] = {"0x4xkk: SNE Vx, kk", [&] () {print_instruction("0x4xkk: SNE Vx, kk", instruction); 
                                                             if (V[(instruction & 0x0F00) >> 8] != (unsigned char) instruction & 0x00FF) {pc += 2;}}};
        instruction_set_base[0x5] = {"0x5xy0: SE Vx, Vy", [&] () {print_instruction("0x5xy0: SE Vx, Vy", instruction); 
                                                             if (V[(instruction & 0x0F00) >> 8] == V[(instruction & 0x00F0) >> 4]) {pc += 2;}}};
        instruction_set_base[0x6] = {"0x6xkk: LD Vx, kk", [&] () {print_instruction("0x6xkk: LD Vx, kk", instruction); 
                                                             V[(instruction & 0x0F00) >> 8] = (unsigned char)(instruction & 0x00FF);}};
        instruction_set_base[0x7] = {"0x7xkk: LD Vx, kk", [&] () {print_instruction("0x7xkk: LD Vx, kk", instruction); 
                                                            V[(instruction & 0x0F00) >> 8] += (unsigned char)(instruction & 0x00FF);}};       
                                                            
        instruction_set_base[0x8] = {"", [&] () {print_instruction(instruction_set_8x[instruction & 0x000F].dissasembly, instruction);
                                                 instruction_set_8x[instruction & 0x000F].function();}};
                                                 
        instruction_set_base[0x9] = {"0x9xy0: SNE Vx, Vy", [&] () {print_instruction("0x9xy0: SKNE Vx, Vy", instruction); 
                                                                    if (V[(instruction & 0x0F00) >> 8] != V[(instruction & 0x00F0) >> 4]) {pc += 2;}}};
        instruction_set_base[0xA] = {"0xAnnn: LD I, addr", [&] () {print_instruction("0xAnnn: LD I, addr", instruction); 
                                                                 I = instruction & 0x0FFF;}};
        instruction_set_base[0xB] = {"0xBnnn: JP V0, addr", [&] () {print_instruction("0xBnnn: JP V0, addr", instruction); 
                                                                 pc = V[0] + (instruction & 0x0FFF);}};
        
        instruction_set_base[0xC] = {"Cxkk: RND Vx, byte", [&] () {print_instruction("Cxkk: RND Vx, byte", instruction);
                                                            unsigned char rnd = std::rand() % 256;
                                                            unsigned short x = (instruction & 0x0F00) >> 8;
                                                            unsigned short kk= (instruction & 0x00FF);
                                                            V[x] = rnd & kk; 
                                                            }};
        
        instruction_set_base[0xD] = {"0xDxyn: DRW Vx, Vy, nibble", [&] () {print_instruction("0xDxyn: DRW Vx, Vy, nibble", instruction);
                                                                           unsigned char xx = (instruction & 0x0F00) >> 8;
                                                                           unsigned char yy = (instruction & 0x00F0) >> 4; 
                                                                           unsigned char n =  (instruction & 0x000F);
                                                                           draw_sprite(V[xx], V[yy], n);}};
        
        instruction_set_base[0xE] = {"", [&] () {if ((instruction & 0x00FF) == 0x9E) {
                                                    print_instruction("Ex9E: SKP Vx", instruction);
                                                    if (keypad->is_key_pressed(V[(instruction & 0x0F00) >> 8])) {
                                                        pc += 2;
                                                    }
                                                 } else if ((instruction & 0x00FF) == 0xA1) {
                                                    print_instruction("ExA1: SKNP Vx", instruction);
                                                    if (!keypad->is_key_pressed(V[(instruction & 0x0F00) >> 8])) {
                                                        pc += 2;
                                                    } 
                                                 }
                                                 }};
        
        instruction_set_base[0xF] = {"", [&] () {print_instruction(instruction_set_Fx[instruction & 0x00FF].dissasembly, instruction); 
                                                                 instruction_set_Fx[instruction & 0x00FF].function();}};
    }
    
    void setup_instruction_set_8x () {
        instruction_set_8x = std::vector<instruction_t>(36, {"unimplemented", [&] (){print_instruction("unimplemented", instruction);}});        
        
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
    }
    
    void setup_instruction_set_Fx () {
        instruction_set_Fx = std::vector<instruction_t>(0x70, {"unimplemented", [&] (){print_instruction("unimplemented", instruction);}});      
                                                                                       
        
        instruction_set_Fx[0x07] = {"0xFx07: LD Vx, DT", [&] () {V[(instruction & 0x0F00) >> 8] = timers.d;}};  
        instruction_set_Fx[0x0A] = {"0xFx0A: LD Vx, K", [&] () {if (!keypad->is_any_key_pressed()) {
                                                                   pc -= 2; 
                                                                } else {
                                                                    V[(instruction & 0x0F00) >> 8] = keypad->last_key_pressed();
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
    
    void emulate_cycle() {
        //Fetch
        instruction = ((memory[pc] << 8) | memory[pc + 1]); // 1 instr = 2 bytes = 2 memory locations 
        pc += 2;
        //Decode & Execute
        instruction_set_base[(instruction & 0xF000) >> 12].function();
        
        if (draw_flag) {
            
            screen->display(scrbuf);
            
            if (draw_on_console) {
                draw_screen_to_console(); 
            }   
          
            draw_flag = false;
        }
        
    }
    
    void print_instruction(std::string descr, unsigned short hexvalue) {
        if (debug_instructions_enabled) {
        std::cout << descr << " -> 0x" << std::hex << hexvalue << " memory[" << (short)memory[pc-2] << "|" << (short)memory[pc-1] << "] [" 
                                                                             << (short)memory[pc] << "|" << (short)memory[pc+1] << "]" << std::endl;
        }
    }
    
    void draw_screen_to_console() {
          //Draw screen
            for (int col = 0; col < screen_width+2; col++)  {
                std::cout << "_";
            }          
            std::cout << std::endl;    
            for (int col = 0; col < screen_width+2; col++)  {
                std::cout << "_";
            }    
            std::cout << std::endl;
            for (int row = 0; row < screen_height; row++) {
                std::cout << "|";
                for (int col = 0; col < screen_width; col++) {
                    if (scrbuf[(row*screen_width)+col] == true) {std::cout << "X";} else {std::cout << " ";}
                }
                std::cout << "|" << std::endl;
            }
            for (int col = 0; col < screen_width + 2; col++)  {
                std::cout << "_";
            } 
            std::cout << std::endl; 
    }
    
    void draw_sprite(unsigned char posx, unsigned char posy, unsigned char n) {
        //1 memory location = 8 bits = 8 pixels
        //n=2 -> xxxxxxxx = memory[I]
        //       xxxxxxxx = memory[I + 1]
        V[0xF] = 0;              
        unsigned char sprite_row;  
        unsigned char pixel;
        bool overwrite = false;
        for (int row = 0; row < n; row++) {            
            sprite_row = memory[I + row];  
            
            if (debug_instructions_enabled) {
                std::bitset<8> x(sprite_row);
                std::cout << x << std::endl; 
            }         
            for (int col = 0; col < 8; col++) {                              
                pixel =  (sprite_row & (0x80 >> col)) ;
                if (pixel > 0) {
                    overwrite = draw_pixel((col + posx), (row + posy));                   
                    if (overwrite) {
                        V[0xF] = 1;
                    }
                }                
            }            
        }
        draw_flag = true;
    }
    
    bool draw_pixel(unsigned char x, unsigned char y) {
        
        bool res = false;
        int pos = x + (y * screen_width);
        if (scrbuf[pos] == true) {
            res = true;
        }
        scrbuf[pos] = !scrbuf[pos];
        
        return res;
    }
		
/* Fields */
protected:
	
	struct stack_t {
		unsigned char sp;
		std::vector<unsigned short>stack;
	};

	struct timers_t{
		unsigned char d;
		unsigned char s;
	};
    
    struct instruction_t {
        std::string dissasembly;
        std::function<void()> function;
    };

private:
	
	unsigned short instruction;
    bool draw_flag;    
	
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
    
    monochrome_screen* screen;
    chip8_keypad* keypad;
    
	
};


#endif