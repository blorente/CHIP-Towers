#ifndef MONOCHROME_SCREEN_H
#define MONOCHROME_SCREEN_H

#include <SDL.h>
#include <SDL_render.h>
#include <iostream>
#include <vector>

class MonochromeScreen {
    
public:
    
    MonochromeScreen(int pixels_wide, int pixels_high, const char* window_name) {
        //Assignments
        screen_height = pixels_high;
        screen_width = pixels_wide;
                 
        std::vector< unsigned char > px(pixels_high * pixels_wide * 4, 0);
        pixels = px;
        
        //SDL stuff
        window = window = SDL_CreateWindow
            (
                window_name,
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                600, 300,
                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
                );
                
        renderer = SDL_CreateRenderer (
            window, //Target window
            -1, //The first one that has the appropriate flags
            0 //No flags -> Accelerated
        );
        
        texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888, //Pixel format, RGBA with 8 bits
            SDL_TEXTUREACCESS_STREAMING, //For textures that change frequently
            screen_width,
            screen_height
        );
        
        
    }
    
    ~MonochromeScreen() {
       SDL_DestroyWindow(window);
       SDL_DestroyRenderer(renderer);
    }
    
    void display(const std::vector<bool>& data) {   
        //Draw new pixels      
        int i = 0;       
        for ( auto itr = data.begin(), end = data.end(); itr != end; itr++ ) {            
            if (*itr)
            {
                pixels[i + 0] = 250;        // b
                pixels[i + 1] = 205;        // g
                pixels[i + 2] = 57;        // r
                pixels[i + 3] = SDL_ALPHA_OPAQUE;    // a                
            }
            else
            {
                pixels[i + 0] = 0;        // b
                pixels[i + 1] = 0;        // g
                pixels[i + 2] = 0;        // r
                pixels[i + 3] = SDL_ALPHA_OPAQUE;    // a
            }
            i += 4;
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	    SDL_RenderClear(renderer);
        SDL_UpdateTexture(
            texture,
            NULL, //update the whole texture
            &pixels[0], //Vector of data to update, in RGBA8888 format
            4*screen_width //Number of pixel info bytes per row
        ); 
        
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);   
    }
    
private:
    
    SDL_Window* window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    
    std::vector<unsigned char> pixels;
	
    int screen_height;
    int screen_width;
    
};

#endif