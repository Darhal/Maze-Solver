#include <vector>
#include <cstdlib>
#include <utility>
#include <algorithm>
#include <iostream>
#include <SDL.h>

#include "Source/maze.hpp"

using namespace std;

enum
{
	DISJKSTRA = 0,
	A_STAR = 1,
};

bool isMouseDown = false;

int HandleEvents(Maze& maze, SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:
			
			break;
		case SDLK_d:
			return DISJKSTRA;
		case SDLK_a:
			return A_STAR;
		default:
			break;
		}
	} else if (event.type ==  SDL_MOUSEBUTTONDOWN){
		isMouseDown = true;
	} else if (event.type == SDL_MOUSEBUTTONUP) {
		isMouseDown = false;
	}

	if (isMouseDown) {
		int divX = (768 / maze.GetW());
		int divY = (768 / maze.GetH());
		maze.SetCell(Maze::SPACE, 1 + ((event.motion.x - 1) / divX), 1 + ((event.motion.y - 1) / divY));
	}

	return 2;
}

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

	int MAZE_W = 32, MAZE_H = 32;
	Maze maze(renderer, MAZE_W, MAZE_H);

	while (1) {
		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT)
			break;

		int r = HandleEvents(maze, event);

		if (r == DISJKSTRA) {
			printf("Doing Dijsktra\n");
			maze.Dijsktra(1, 1);
		} else if (r == A_STAR) {
			printf("Doing A*\n");
			maze.AStarSearch(Pair(1, 1), Pair(maze.GetH() - 3, maze.GetW() - 3));
		} else {
			maze.DisplayMaze();
		}

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
