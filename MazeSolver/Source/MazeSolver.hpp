#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include "Text.hpp"
#include "maze.hpp"

class MazeSolver
{
public:
	enum
	{
		DISJKSTRA = 0,
		A_STAR = 1,
	};
public:
	MazeSolver(int Window_W = 1280, int Window_H = 768, int Maze_W = 32, int Maze_H = 32);

	void Init();

	void Loop();

	int HandleEvents();

	void Clean();

	uint32_t time_left(void);
private:
	constexpr static int MAX_MENU = 5;

	char menu[sizeof(Text) * MAX_MENU];
	std::string texts[5] = { "Add", "Remove", "1", "2", "3" };

	SDL_Window* window;
	SDL_Renderer* renderer;
	TTF_Font* font;
	SDL_Event event;
	Maze maze;
	uint32_t next_time;
	int currentSelection = 0;
	int Window_H = 768;
	int Window_W = 768;
	int MAZE_W = 32;
	int MAZE_H = 32;
	bool isMouseDown = false;
};

