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
		A_STAR_EUC = 1,
		A_STAR_MAN = 2,
		CAT_MOUSE = 3,
	};
public:
	MazeSolver(int Window_W = 1280, int Window_H = 768, int Maze_W = 32, int Maze_H = 32);

	void Init();

	void Loop();

	void HandleEvents(int& res);

	void Clean();
private:
	constexpr static int MAX_MENU = 13;

	char menu[sizeof(Text) * MAX_MENU];
	std::string texts[MAX_MENU] = { "Dijkstra", "A* Euclidean", "A* Manhattan", "Empty Cell", "Add Wall", 
		"Add Stone", "Add Sand", "Add Water", "Re-Generate Maze", "Clear Maze", "Edit Start", "Edit End", "The Mouse & The Cat"};

	SDL_Window* window;
	SDL_Renderer* renderer;
	TTF_Font* font;
	Text* current_cell;
	SDL_Event event;
	Maze maze;
	int currentSelection = 0;
	int Window_H = 768;
	int Window_W = 768;
	int MAZE_W = 32;
	int MAZE_H = 32;
	bool isMouseDown = false;
};

