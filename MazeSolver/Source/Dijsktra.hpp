#pragma once
#include "maze.hpp"

class Maze;

class Dijsktra
{
public:
	Dijsktra(Maze* maze);

	void Start(const Pair& start, const Pair& end);

	uint32_t minDistance(uint32_t dist[], bool sptSet[], uint32_t V);
private:
	Maze* maze;
};