#pragma once

#include <vector>
#include <array>
#include <utility>
#include <algorithm>
#include <random>
#include "graph.hpp"
#include <SDL.h>

static uint32_t random_n(uint32_t n)
{ 
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, n - 1);
	return dist(rng);
}

// Creating a shortcut for int, int pair type 
typedef std::pair<int, int> Pair;

// Creating a shortcut for pair<int, pair<int, int>> type 
typedef std::pair<double, std::pair<int, int>> pPair;

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Rect;

class Maze
{
public:
	typedef std::vector<std::vector<uint32_t>> MazeArray;

	enum wall_t {
		WALL = 0x000000,
		SPACE = 0xffffff,

		STONE = 0xabaeaf,
		SAND = 0xDBD1B4,
		WATER = 0xadd8e6,

		TRACE = 0xffff00,
	};
public:
    Maze();

	// Maze generation and clearing functions:

	void Init(SDL_Renderer* renderer, int h = 32, int w = 32);

    void DigMaze(int r, int c, uint32_t* wall);

    void BuildMaze();

	void Clear();

	void Reset();

	void Generate();

    void DisplayMaze();

	// Cat & Mouse functions:
	void CatAndMouse(const Pair& start, const Pair& end);

	// Dijsktra functions:
	Graph ConstructGraph();

	// Utility functions:
	int GetH() const { return H - 2; }

	int GetW() const { return W - 2; }

	void SetCell(wall_t type, int row, int col);

	SDL_Rect& getRect() { return texture_sz; }

	uint32_t getCellCost(int row, int col) const;

	wall_t getRandomCell();

	void setStart(const std::pair<uint32_t, uint32_t>& s) { start = s; };

	void setEnd(const std::pair<uint32_t, uint32_t>& e) { end = e; };

	const std::pair<uint32_t, uint32_t>& getStart() { return start; };

	const std::pair<uint32_t, uint32_t>& getEnd() { return end; };

	std::pair<uint32_t, uint32_t> Get2DCoord(uint32_t coord);

	MazeArray& GetMazeArray() { return maze; }

	void ColorCase(SDL_Rect* rect, int row, int col, int r, int g, int b);

	void ColorCase(SDL_Rect* rect, int row, int col, uint32_t color);
private:
    MazeArray maze;
	SDL_Texture* texture;
	SDL_Renderer* renderer;
	SDL_Rect texture_sz;
	std::pair<uint32_t, uint32_t> start, end;
    int H, W;

	static constexpr int TILE_W = 1;
	static constexpr int TILE_H = 1;

	friend class AStar;
	friend class Dijsktra;
};