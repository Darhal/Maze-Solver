#include "AStar.hpp"
#include <stack>
#include <set>
#include <chrono>
#include <thread>

AStar::AStar(Maze* maze) : maze(maze)
{

}

// A Utility Function to check whether given cell (row, col) 
// is a valid cell or not. 
bool AStar::isValid(int row, int col)
{
	// Returns true if row number and column number 
	// is in range 
	return (row >= 0) && (row < maze->H) && (col >= 0) && (col < maze->W);
}

// A Utility Function to check whether the given cell is 
// blocked or not 
bool AStar::isUnBlocked(int row, int col)
{
	// Returns true if the cell is not blocked else false 
	if (maze->maze[row][col] != Maze::wall_t::WALL)
		return true;
	else
		return false;
}

// A Utility Function to check whether destination cell has 
// been reached or not 
bool AStar::isDestination(int row, int col, const Pair& dest)
{
	if (row == dest.first && col == dest.second)
		return (true);
	else
		return (false);
}

// A Utility Function to calculate the 'h' heuristics. 
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


// A Utility Function to trace the path from the source 
// to destination 
void AStar::tracePath(const std::vector<std::vector<cell>>& cellDetails, const Pair& dest)
{
	SDL_Rect r;
	r.w = maze->TILE_W;
	r.h = maze->TILE_H;

	printf("\nThe Path is ");
	int row = dest.first;
	int col = dest.second;
	std::stack<Pair> Path;

	while (!(cellDetails[row][col].parent_i == row
		&& cellDetails[row][col].parent_j == col)) {
		Path.push(std::make_pair(row, col));
		int temp_row = cellDetails[row][col].parent_i;
		int temp_col = cellDetails[row][col].parent_j;
		row = temp_row;
		col = temp_col;
	}

	SDL_SetRenderTarget(maze->renderer, maze->texture);
	Path.push(std::make_pair(row, col));

	while (!Path.empty()) {
		std::pair<int, int> p = Path.top();
		Path.pop();
		maze->ColorCase(&r, p.first, p.second, 0x00, 0x00, 0xFF);
		printf("-> (%d,%d) ", p.first, p.second);
	}

	maze->ColorCase(&r, maze->start.first, maze->start.second, 0xFF, 0x0, 0x0);
	maze->ColorCase(&r, maze->end.first, maze->end.second, 255, 135, 0);
	SDL_SetRenderTarget(maze->renderer, NULL);
	SDL_RenderCopy(maze->renderer, maze->texture, NULL, &maze->texture_sz);
	SDL_RenderPresent(maze->renderer);

	std::this_thread::sleep_for(std::chrono::seconds(5));
	return;
}

// A Function to find the shortest path between 
// a given source cell to a destination cell according 
// to A* Search Algorithm 
void AStar::AStarSearch(distance_t type, const Pair& src, const Pair& dest)
{
	int ROW = maze->H;
	int COL = maze->W;
	constexpr Pair coords[] = {
		Pair(-1, -1), Pair(-1, 0), Pair(-1, 1),
		Pair(0, -1)              , Pair(0, 1),
		Pair(1, -1), Pair(1, 0)  , Pair(1, 1),
	};

	// If the source is out of range 
	if (isValid(src.first, src.second) == false) {
		printf("Source is invalid\n");
		return;
	}

	// If the destination is out of range 
	if (isValid(dest.first, dest.second) == false) {
		printf("Destination is invalid\n");
		return;
	}

	// Either the source or the destination is blocked 
	if (isUnBlocked(src.first, src.second) == false ||
		isUnBlocked(dest.first, dest.second) == false) {
		printf("Source or the destination is blocked\n");
		return;
	}

	// If the destination cell is the same as source cell 
	if (isDestination(src.first, src.second, dest) == true) {
		printf("We are already at the destination\n");
		return;
	}

	// Create a closed list and initialise it to false which means 
	// that no cell has been included yet 
	// This closed list is implemented as a boolean 2D array 
	std::vector<std::vector<bool>> closedList(ROW, std::vector<bool>(COL, false));

	// Declare a 2D array of structure to hold the details 
	//of that cell 
	std::vector<std::vector<cell>> cellDetails(ROW, std::vector<cell>(COL));

	int i, j;

	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COL; j++) {
			cellDetails[i][j].f = FLT_MAX;
			cellDetails[i][j].g = FLT_MAX;
			cellDetails[i][j].h = FLT_MAX;
			cellDetails[i][j].parent_i = -1;
			cellDetails[i][j].parent_j = -1;
		}
	}

	// Initialising the parameters of the starting node 
	i = src.first, j = src.second;
	cellDetails[i][j].f = 0.0;
	cellDetails[i][j].g = 0.0;
	cellDetails[i][j].h = 0.0;
	cellDetails[i][j].parent_i = i;
	cellDetails[i][j].parent_j = j;

	/*
	 Create an open list having information as-
	 <f, <i, j>>
	 where f = g + h,
	 and i, j are the row and column index of that cell
	 Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
	 This open list is implenented as a set of pair of pair.*/
	std::set<pPair> openList;

	// Put the starting cell on the open list and set its 
	// 'f' as 0 
	openList.insert(std::make_pair(0.0, std::make_pair(i, j)));

	// We set this boolean value as false as initially 
	// the destination is not reached. 
	bool foundDest = false;

	SDL_Rect r;
	r.w = maze->TILE_W;
	r.h = maze->TILE_H;

	while (!openList.empty()) {
		pPair p = *openList.begin();

		// Remove this vertex from the open list 
		openList.erase(openList.begin());

		// Add this vertex to the closed list 
		i = p.second.first;
		j = p.second.second;
		closedList[i][j] = true;

		SDL_SetRenderTarget(maze->renderer, maze->texture);
		maze->ColorCase(&r, i, j, 0x00, 0xFF, 0x00);
		maze->ColorCase(&r, maze->start.first, maze->start.second, 0xFF, 0x0, 0x0);
		maze->ColorCase(&r, maze->end.first, maze->end.second, 255, 135, 0);
		SDL_SetRenderTarget(maze->renderer, NULL);
		SDL_RenderCopy(maze->renderer, maze->texture, NULL, &maze->texture_sz);
		SDL_RenderPresent(maze->renderer);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

		// To store the 'g', 'h' and 'f' of the 8 successors 
		double gNew, hNew, fNew;

		// Only process this cell if this is a valid one 
		//----------- (North - South) ------------ 
		for (const Pair& dir : coords) {
			uint32_t dirX = dir.first;
			uint32_t dirY = dir.second;

			if (isValid(i + dirX, j + dirY) == true) {
				// If the destination cell is the same as the 
				// current successor 
				if (isDestination(i + dirX, j + dirY, dest) == true) {
					// Set the Parent of the destination cell 
					cellDetails[i + dirX][j + dirY].parent_i = i;
					cellDetails[i + dirX][j + dirY].parent_j = j;
					printf("The destination cell is found\n");
					tracePath(cellDetails, dest);
					foundDest = true;
					return;
				}
				// If the successor is already on the closed 
				// list or if it is blocked, then ignore it. 
				// Else do the following 
				else if (closedList[i + dirX][j + dirY] == false && isUnBlocked(i + dirX, j + dirY) == true) {
					gNew = cellDetails[i][j].g + maze->getCellCost(i + dirX, j + dirY);
					hNew = calculateHValue(type, i + dirX, j + dirY, dest);
					fNew = gNew + hNew;

					// If it isn’t on the open list, add it to 
					// the open list. Make the current square 
					// the parent of this square. Record the 
					// f, g, and h costs of the square cell 
					//                OR 
					// If it is on the open list already, check 
					// to see if this path to that square is better, 
					// using 'f' cost as the measure. 
					if (cellDetails[i + dirX][j + dirY].f == FLT_MAX || cellDetails[i + dirX][j + dirY].f > fNew) {
						openList.insert(std::make_pair(fNew, std::make_pair(i + dirX, j + dirY)));

						// Update the details of this cell 
						cellDetails[i + dirX][j + dirY].f = fNew;
						cellDetails[i + dirX][j + dirY].g = gNew;
						cellDetails[i + dirX][j + dirY].h = hNew;
						cellDetails[i + dirX][j + dirY].parent_i = i;
						cellDetails[i + dirX][j + dirY].parent_j = j;
					}
				}
			}
		}
	}

	// When the destination cell is not found and the open 
	// list is empty, then we conclude that we failed to 
	// reach the destiantion cell. This may happen when the 
	// there is no way to destination cell (due to blockages) 
	if (foundDest == false)
		printf("Failed to find the Destination Cell\n");

	return;
}