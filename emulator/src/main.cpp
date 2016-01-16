#include <iostream>
#include "other.cpp"
#include "SDL.h"

extern void doSomeStuff();

int main(int argc, char *argv[]) {
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << std::endl;
	}	
	SDL_Window* w = SDL_CreateWindow(
		"SDL2",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		1200, 600,
		SDL_WINDOW_SHOWN
	);
	
	std::cin.get();
	
	SDL_Quit();
	
	
    return 0;
}