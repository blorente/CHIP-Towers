#ifndef CHIP8_KEYPAD_H
#define CHIP8_KEYPAD_H

#include <SDL.h>
#include <SDL_events.h>
#include <vector>
#include <iostream>


namespace cpp {
    namespace emulator {
        class chip8_keypad {
            
        public:
            
            chip8_keypad() {
                keys = std::vector<bool>(16, {false});
                last_pressed = 0;
            }
            ~chip8_keypad(){}
            
            void print() {        
                std::cout << "|";
                for ( auto itr = keys.begin(), end = keys.end(); itr != end; itr++ ) {
                    if (*itr) {
                        std::cout << "X";
                    } else {
                        std::cout << " ";
                    }
                    std::cout << "|";
                }   
                std::cout << std::endl; 
            }
            
            bool is_any_key_pressed() {
                for ( auto itr = keys.begin(), end = keys.end(); itr != end; itr++ ) {
                    if (*itr) {
                        return true;
                    }
                }
            }    
            
            bool is_key_pressed(unsigned char key) {        
                
                if (key >= keys.size()) {
                    return false;
                } else {
                    return keys[key];
                }
                
            }
            
            unsigned char last_key_pressed() {
                return last_pressed;
            }
            
            void process_key_down(const SDL_Event& event) {
                if (event.type == SDL_KEYDOWN) {
                    switch(event.key.keysym.sym) {
                        case SDLK_KP_1:
                            keys[0x1] = true;
                            last_pressed = 0x1;
                            break;
                        case SDLK_KP_2:
                            keys[0x2] = true;
                            last_pressed = 0x2;
                            break;
                        case SDLK_KP_3:
                            keys[0x3] = true;
                            last_pressed = 0x3;
                            break;
                        case SDLK_KP_4:
                            keys[0x4] = true;
                            last_pressed = 0x4;
                            break;
                        case SDLK_KP_5:
                            keys[0x5] = true;
                            last_pressed = 0x5;
                            break;
                        case SDLK_KP_6:
                            keys[0x6] = true;
                            last_pressed = 0x6;
                            break;
                        case SDLK_KP_7:
                            keys[0x7] = true;
                            last_pressed = 0x7;
                            break;
                        case SDLK_KP_8:
                            keys[0x8] = true;
                            last_pressed = 0x8;
                            break;
                        case SDLK_KP_9:
                            keys[0x9] = true;
                            last_pressed = 0x9;
                            break;
                        case SDLK_DOWN:
                            keys[0x0] = true;
                            last_pressed = 0x0;
                            break;               
                        case SDLK_LEFT:
                            keys[0xA] = true;
                            last_pressed = 0xA;
                            break;
                        case SDLK_RIGHT:
                            keys[0xB] = true;
                            last_pressed = 0xB;
                            break;
                        case SDLK_KP_MINUS:
                            keys[0xC] = true;
                            last_pressed = 0xC;
                            break;                 
                        case SDLK_KP_PLUS:
                            keys[0xD] = true;
                            last_pressed = 0xD;
                            break;
                        case SDLK_KP_ENTER:
                            keys[0xE] = true;
                            last_pressed = 0xE;
                            break;
                        case SDLK_KP_PERIOD:
                            keys[0xF] = true;
                            last_pressed = 0xF;
                            break;
                        default:
                            std::cout << "Unknown Key pressed!" << std::endl;                                    
                    }           
                } 
            }
            
            void process_key_up(const SDL_Event& event) {
                if (event.type == SDL_KEYUP) {
                    switch(event.key.keysym.sym) {
                        case SDLK_KP_1:
                            keys[0x1] = false;
                            break;
                        case SDLK_KP_2:
                            keys[0x2] = false;
                            break;
                        case SDLK_KP_3:
                            keys[0x3] = false;
                            break;
                        case SDLK_KP_4:
                            keys[0x4] = false;
                            break;
                        case SDLK_KP_5:
                            keys[0x5] = false;
                            break;
                        case SDLK_KP_6:
                            keys[0x6] = false;
                            break;
                        case SDLK_KP_7:
                            keys[0x7] = false;
                            break;
                        case SDLK_KP_8:
                            keys[0x8] = false;
                            break;
                        case SDLK_KP_9:
                            keys[0x9] = false;
                            break;
                        case SDLK_DOWN:
                            keys[0x0] = false;
                            break;               
                        case SDLK_LEFT:
                            keys[0xA] = false;
                            break;
                        case SDLK_RIGHT:
                            keys[0xB] = false;
                            break;
                        case SDLK_KP_MINUS:
                            keys[0xC] = false;
                            break;                 
                        case SDLK_KP_PLUS:
                            keys[0xD] = false;
                            break;
                        case SDLK_KP_ENTER:
                            keys[0xE] = false;
                            break;
                        case SDLK_KP_PERIOD:
                            keys[0xF] = false;
                            break;
                        default:
                            std::cout << "Unknown Key released!" << std::endl;                                    
                    }
                } 
            }
        
        private:
            std::vector<bool> keys;  
            unsigned char last_pressed;
        };
    }
}

#endif