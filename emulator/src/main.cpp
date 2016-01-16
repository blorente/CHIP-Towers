#include <iostream>
#include "other.cpp"
#include "SDL.h"

extern void doSomeStuff();

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << std::endl;
	}
	SDL_Quit();
    return 0;
}