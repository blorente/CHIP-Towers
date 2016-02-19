#ifndef KEYPAD_H
#define KEYPAD_H

#include <SDL.h>
#include <SDL_events.h>
#include <vector>
#include <iostream>

class Keypad {
    
public:
    
    Keypad() {
        keys = std::vector<bool>(16, {false});
    }
    ~Keypad(){}
    
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
    
    bool isKeyPressed(unsigned char key) {        
        
        if (key >= keys.size()) {
            return false;
        } else {
            return keys[key];
        }
        
    }
    
    void processKeyDown(const SDL_Event& event) {
        if (event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.sym) {
                case SDLK_KP_1:
                    keys[0x1] = true;
                    break;
                case SDLK_KP_2:
                    keys[0x2] = true;
                    break;
                case SDLK_KP_3:
                    keys[0x3] = true;
                    break;
                case SDLK_KP_4:
                    keys[0x4] = true;
                    break;
                case SDLK_KP_5:
                    keys[0x5] = true;
                    break;
                case SDLK_KP_6:
                    keys[0x6] = true;
                    break;
                case SDLK_KP_7:
                    keys[0x7] = true;
                    break;
                case SDLK_KP_8:
                    keys[0x8] = true;
                    break;
                case SDLK_KP_9:
                    keys[0x9] = true;
                    break;
                case SDLK_DOWN:
                    keys[0x0] = true;
                    break;               
                case SDLK_LEFT:
                    keys[0xA] = true;
                    break;
                case SDLK_RIGHT:
                    keys[0xB] = true;
                    break;
                case SDLK_KP_MINUS:
                    keys[0xC] = true;
                    break;                 
                case SDLK_KP_PLUS:
                    keys[0xD] = true;
                    break;
                case SDLK_KP_ENTER:
                    keys[0xE] = true;
                    break;
                case SDLK_KP_PERIOD:
                    keys[0xF] = true;
                    break;
                default:
                    std::cout << "Unknown Key pressed!" << std::endl;                                    
            }           
        } 
    }
    
    void processKeyUp(const SDL_Event& event) {
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
};

#endif