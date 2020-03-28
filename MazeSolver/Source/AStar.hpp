#pragma once

#include "maze.hpp"

class Maze;

class AStar
{
public:
	// A structure to hold the neccesary parameters 
	struct cell
	{
		// Row and Column index of its parent 
		// Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1 
		int parent_i, parent_j;
		// f = g + h 
		double f, g, h;
	};

	enum distance_t
	{
		EUCLIDEAN = 0,
		MANHATTAN = 1,
	};
public:
	AStar(Maze* maze);

	bool isValid(int row, int col);

	bool isUnBlocked(int row, int col);

	bool isDestination(int row, int col, const Pair& dest);

	void AStarSearch(distance_t type, const Pair& src, const Pair& dest);

	void tracePath(const std::vector<std::vector<cell>>& cellDetails, const Pair& dest);

	double calculateHValue(distance_t type, int row, int col, const Pair& dest);
private:
	Maze* maze;
};