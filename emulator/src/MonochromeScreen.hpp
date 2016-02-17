#ifndef MONOCHROME_SCREEN_H
#define MONOCHROME_SCREEN_H

#include <SDL.h>
#include <SDL_render.h>
#include <iostream>
#include <vector>

class MonochromeScreen {
    
public:
    
    MonochromeScreen(int pixelsWide, int pixelsHigh, const char* windowName) {
        SDL_Init(SDL_INIT_EVERYTHING);

        window = window = SDL_CreateWindow
            (
                windowName,
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                600, 300,
                SDL_WINDOW_SHOWN
                );
                
        renderer = SDL_CreateRenderer
		(
			window,
			-1,
			SDL_RENDERER_ACCELERATED
			);

        SDL_RendererInfo info;
        SDL_GetRendererInfo(renderer, &info);
        
        std::cout << "Renderer name: " << info.name << std::endl;
        std::cout << "Texture formats: " << std::endl;
        for (Uint32 i = 0; i < info.num_texture_formats; i++)
        {
            std::cout << SDL_GetPixelFormatName(info.texture_formats[i]) << std::endl;
        }
        
        texture = SDL_CreateTexture
		(
			renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			pixelsWide, pixelsHigh
			);
            
        std::vector< unsigned char > px(pixelsHigh * pixelsWide * 4, 0);
	    pixels = px;
        
        screenWidth = pixelsWide;
        screenHeight = pixelsHigh;
    }
    
    ~MonochromeScreen() {
       SDL_DestroyRenderer(renderer);
	   SDL_DestroyWindow(window);
    }
    
    void display(const std::vector<bool>& data ) {
        int i = 0;
        for ( auto itr = data.begin(), end = data.end(); itr != end; itr++ ) {            
            if (*itr)
            {
                pixels[i + 0] = 255;        // b
                pixels[i + 1] = 255;        // g
                pixels[i + 2] = 255;        // r
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

        SDL_UpdateTexture
            (
                texture,
                NULL,
                &pixels[0],
                screenWidth *4
                );

        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    
private:
    
    SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
    
    std::vector< unsigned char > pixels;
    
    int screenHeight;
    int screenWidth;
    
};

#endif