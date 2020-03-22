#include <vector>
#include <cstdlib>
#include <utility>
#include <algorithm>
#include <iostream>
#include <SDL.h>

#include "Source/maze.hpp"

using namespace std;

int SDL_main(int argc, char* argv[])
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event event;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}

	window = SDL_CreateWindow("AI Project",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		768, 768,
		SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1, 0);

	int MAZE_W = 80, MAZE_H = 80;
	Maze maze(renderer, MAZE_W, MAZE_H);

	while (1) {
		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT)
			break;
		
		maze.DisplayMaze(renderer);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
