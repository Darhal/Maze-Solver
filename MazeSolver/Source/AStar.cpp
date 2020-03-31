#include "AStar.hpp"
#include <stack>
#include <set>
#include <chrono>
#include <thread>

AStar::AStar(Maze* maze) : maze(maze)
{

}

// Function to check whether destination cell has been reached or not 
bool AStar::isDestination(int row, int col, const Pair& dest)
{
	if (row == dest.first && col == dest.second)
		return (true);
	else
		return (false);
}

// Function to calculate the 'h' heuristic. 
double AStar::calculateHValue(distance_t type, int row, int col, const Pair& dest)
{
	// Return using the distance formula  based on the type:
	if (type == EUCLIDEAN) { // Euclidean
		return ((double)sqrt((row - dest.first)*(row - dest.first)
			+ (col - dest.second)*(col - dest.second)));
	} else if (type == MANHATTAN) { // Manhattan
		return (double)(std::abs(row - dest.first) + std::abs(col - dest.second));
	}

	return 0;
}

// Function to trace the path from the source to destination 
std::stack<Pair> AStar::tracePath(const std::vector<std::vector<cell>>& cells, const Pair& dest)
{
	SDL_Rect r;
	r.w = maze->TILE_W;
	r.h = maze->TILE_H;

	// printf("\nPath: ");
	int row = dest.first;
	int col = dest.second;
	std::stack<Pair> Path;

	while (!(cells[row][col].parent_i == row && cells[row][col].parent_j == col)) {
		Path.push(std::make_pair(row, col));
		int temp_row = cells[row][col].parent_i;
		int temp_col = cells[row][col].parent_j;
		row = temp_row;
		col = temp_col;
	}

	Path.push(std::make_pair(row, col));

	std::stack<Pair> res(Path);

	SDL_SetRenderTarget(maze->renderer, maze->texture);

	while (!Path.empty()) {
		std::pair<int, int> p = Path.top();
		Path.pop();
		maze->ColorCase(&r, p.first, p.second, 0x00, 0x00, 0xFF);
		// printf("-> (%d,%d) ", p.first, p.second);
	}

	maze->ColorCase(&r, maze->start.first, maze->start.second, 0xFF, 0x0, 0x0);
	maze->ColorCase(&r, maze->end.first, maze->end.second, 255, 135, 0);
	SDL_SetRenderTarget(maze->renderer, NULL);
	SDL_RenderCopy(maze->renderer, maze->texture, NULL, &maze->texture_sz);
	SDL_RenderPresent(maze->renderer);

	return res;
}

// A Function to find the shortest path between src and dest using the A* Search Algorithm (type is the distance type)
std::stack<Pair> AStar::AStarSearch(distance_t type, const Pair& src, const Pair& dest)
{
	int ROW = maze->H;
	int COL = maze->W;
	std::stack<Pair> res;

	// All directions to check:
	constexpr Pair coords[] = {
		Pair(-1, -1), Pair(-1, 0), Pair(-1, 1),
		Pair(0, -1)              , Pair(0, 1),
		Pair(1, -1), Pair(1, 0)  , Pair(1, 1),
	};

	// If the source is out of range 
	if (maze->isValid(src.first, src.second) == false) {
		printf("Source is invalid\n");
		return res;
	}

	// If the destination is out of range 
	if (maze->isValid(dest.first, dest.second) == false) {
		printf("Destination is invalid\n");
		return res;
	}

	// Either the source or the destination is blocked 
	if (maze->isUnBlocked(src.first, src.second) == false || maze->isUnBlocked(dest.first, dest.second) == false) {
		printf("Source or the destination is blocked\n");
		return res;
	}

	// If the destination cell is the same as source cell 
	if (isDestination(src.first, src.second, dest) == true) {
		printf("We are already at the destination\n");
		return res;
	}

	// Create a closed list and initialise it to false which means  
	std::vector<std::vector<bool>> closedList(ROW, std::vector<bool>(COL, false));

	// Declare a 2D array of structure to hold the details of that cell 
	std::vector<std::vector<cell>> cells(ROW, std::vector<cell>(COL));

	int i, j;
	// Init verything:
	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COL; j++) {
			cells[i][j].f = FLT_MAX;
			cells[i][j].g = FLT_MAX;
			cells[i][j].h = FLT_MAX;
			cells[i][j].parent_i = -1;
			cells[i][j].parent_j = -1;
		}
	}

	// Initialising the parameters of the starting node 
	i = src.first, j = src.second;
	cells[i][j].f = 0.0;
	cells[i][j].g = 0.0;
	cells[i][j].h = 0.0;
	cells[i][j].parent_i = i;
	cells[i][j].parent_j = j;

	// Create an open list having information as-<f, <i, j>>
	// where f = g + h, and i, j are the row and column index of that cell
	std::set<pPair> openList;

	// Put the starting cell on the open list and set its 'f' as 0 
	openList.insert(std::make_pair(0.0, std::make_pair(i, j)));

	// set to false, intially . 
	bool foundDest = false;

	// Rct for rednering
	SDL_Rect r;
	r.w = maze->TILE_W;
	r.h = maze->TILE_H;
	SDL_Event event;

	while (!openList.empty()) { // While the list is not empty
		SDL_PollEvent(&event);

		pPair p = *openList.begin(); // Get the top of the list
		openList.erase(openList.begin()); // Remove this vertex from the open list 

		// Add this vertex to the closed list 
		i = p.second.first;
		j = p.second.second;
		closedList[i][j] = true;

		// Render the cell we are visiting
		SDL_SetRenderTarget(maze->renderer, maze->texture);
		maze->ColorCase(&r, i, j, 0x00, 0xFF, 0x00);
		maze->ColorCase(&r, maze->start.first, maze->start.second, 0xFF, 0x0, 0x0);
		maze->ColorCase(&r, maze->end.first, maze->end.second, 255, 135, 0);
		SDL_SetRenderTarget(maze->renderer, NULL);
		SDL_RenderCopy(maze->renderer, maze->texture, NULL, &maze->texture_sz);
		SDL_RenderPresent(maze->renderer);
		std::this_thread::sleep_for(std::chrono::milliseconds(2));

		// To store the 'g', 'h' and 'f' of the 8 successors 
		double newG, newH, newF;

		// Process the neighbour cells
		for (const Pair& dir : coords) {
			uint32_t dirX = dir.first;
			uint32_t dirY = dir.second;

			if (maze->isValid(i + dirX, j + dirY) == true) {
				// If the destination cell is the same as the current successor 
				if (isDestination(i + dirX, j + dirY, dest) == true) {
					// Set the Parent of the destination cell 
					cells[i + dirX][j + dirY].parent_i = i;
					cells[i + dirX][j + dirY].parent_j = j;
					// printf("The destination cell is found\n");
					foundDest = true;
					res = tracePath(cells, dest);
					return res; // return the path
				} else if (closedList[i + dirX][j + dirY] == false && maze->isUnBlocked(i + dirX, j + dirY) == true) {
					newG = cells[i][j].g + maze->getCellCost(i + dirX, j + dirY);
					newH = calculateHValue(type, i + dirX, j + dirY, dest);
					newF = newG + newH;

					if (cells[i + dirX][j + dirY].f == FLT_MAX || cells[i + dirX][j + dirY].f > newF) {
						openList.insert(std::make_pair(newF, std::make_pair(i + dirX, j + dirY)));

						// Update the details of this cell 
						cells[i + dirX][j + dirY].f = newF;
						cells[i + dirX][j + dirY].g = newG;
						cells[i + dirX][j + dirY].h = newH;
						cells[i + dirX][j + dirY].parent_i = i;
						cells[i + dirX][j + dirY].parent_j = j;
					}
				}
			}
		}
	}

	if (foundDest == false) // No destination found
		printf("Failed to find the Destination Cell\n");

	return res;
}