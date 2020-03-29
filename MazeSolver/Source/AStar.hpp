#pragma once

#include "maze.hpp"
#include <stack>

class Maze;

class AStar
{
public:
	// Cell detials for A*
	struct cell
	{
		int parent_i, parent_j;
		double f, g, h; // f = g + h 
	};

	enum distance_t
	{
		EUCLIDEAN = 0,
		MANHATTAN = 1,
	};
public:
	AStar(Maze* maze);

	bool isDestination(int row, int col, const Pair& dest);

	std::stack<Pair> AStarSearch(distance_t type, const Pair& src, const Pair& dest);

	std::stack<Pair> tracePath(const std::vector<std::vector<cell>>& cellDetails, const Pair& dest);

	double calculateHValue(distance_t type, int row, int col, const Pair& dest);
private:
	Maze* maze;
};