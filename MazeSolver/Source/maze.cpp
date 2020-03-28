#include "maze.hpp"
#include <stack>
#include <SDL.h>
#include <thread>
#include <chrono>
#include <set>

// A utility function to find the vertex with minimum distance value, from 
// the set of vertices not yet included in shortest path tree 
static int minDistance(int dist[], bool sptSet[], int V)
{
	// Initialize min value 
	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++) {
		if (sptSet[v] == false && dist[v] <= min) {
			min = dist[v];
			min_index = v;
		}
	}

	return min_index;
}

Maze::Maze() : texture_sz{ 0, 0, 768, 768 }, start(1, 1), end(1, 1)
{
}

Maze::wall_t Maze::getRandomCell()
{
	int pick = random_n(101);

	if (pick <= 50) {
		return wall_t::SPACE;
	} else if (pick > 50 && pick <= 80) {
		return wall_t::STONE;
	} else if (pick > 80 && pick <= 95) {
		return wall_t::SAND;
	} else if (pick > 95 && pick <= 100) {
		return wall_t::WATER;
	}

	return wall_t::SPACE;
}

int Maze::getCellCost(int row, int col) const
{
	switch (maze[row][col]) {
	case wall_t::SPACE:
		return 1;
	case wall_t::STONE:
		return 2;
	case wall_t::SAND:
		return 3;
	case wall_t::WATER:
		return 4;
	}
}

void Maze::Clear()
{
	maze = MazeArray(H, std::vector<uint32_t>(W, wall_t::WALL));
}


void Maze::Reset()
{
	maze = MazeArray(H, std::vector<uint32_t>(W, wall_t::SPACE));

	maze[0] = std::vector<uint32_t>(W, wall_t::WALL);
	maze[H - 2] = std::vector<uint32_t>(W, wall_t::WALL);

	for (int i = 0; i < H; i++) {
		maze[i][0] = wall_t::WALL;
		maze[i][W - 2] = wall_t::WALL;
	}
}

void Maze::Generate()
{
	this->BuildMaze();
}

void Maze::Init(SDL_Renderer* renderer, int h, int w)
{
	W = w;
	H = h;

	start = std::pair<uint32_t, uint32_t>(1, 1);
	end = std::pair<uint32_t, uint32_t>(W - 3, H - 3);

	maze = MazeArray(H, std::vector<uint32_t>(W, wall_t::WALL));
	this->renderer = renderer;
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, W - 1, H - 1);
	this->Generate();
}

void Maze::DigMaze(int r, int c, uint32_t* wall)
{
	// Are we out of bounds?
	if (r < 0 || c < 0 || r >= maze.size() || c >= maze[0].size())
		return;

	// Are we on a wall, or on a cell that we have visited already?
	if (maze[r][c] == wall_t::WALL || maze[r][c] == wall_t::SPACE)
		return;

	// The wall pointer points to the wall that we jumped over 
	// between recursive calls.  Knock it down.
	// (wall == NULL for the 1st invocation of this function.)
	if (wall) {
		*wall = getRandomCell();
	}

	// Dig this cell.
	maze[r][c] = getRandomCell();
	// this->DisplayMaze();

	// Randomly decide the order in which we explore the directions
	// N, S, E, W.  We use STL's random_shuffle() to shuffle an array.
	char D[4] = { 'N', 'S', 'E', 'W' };
	std::random_shuffle(D, D + 4, random_n);

	for (int i = 0; i < 4; i++) {
		// m[rr][cc] will be the cell that we'll try to dig.
		int rr = r;
		int cc = c;
		uint32_t* wall;

		switch (D[i]) {
		case 'N':
			rr -= 2;
			wall = &maze[r - 1][c];
			break;
		case 'S':
			rr += 2;
			wall = &maze[r + 1][c];
			break;
		case 'W':
			cc -= 2;
			wall = &maze[r][c - 1];
			break;
		case 'E':
			cc += 2;
			wall = &maze[r][c + 1];
			break;
		default:
			// std::cerr << "unknown direction" << std::endl;
			exit(1);
		}

		// recursively dig from (rr,cc).
		this->DigMaze(rr, cc, wall);
	}
}

void Maze::BuildMaze()
{
	// min/maxRow/Col indicate the "interior" of the maze.
	int minRow = 1;
	int minCol = 1;
	size_t maxRow = maze.size() - 2;
	size_t maxCol = maze[0].size() - 2;

	// Seed the pseudo-random number generator with the screen size
	// so that we get the same maze on a given terminal size.
	srand(maxRow * maxCol);

	// Initialize the maze matrix m in alternating walls and cells
	// like this:
	//
	//      #########
	//      #.#.#.#.# 
	//      #########
	//      #.#.#.#.# 
	//      #########
	//
	for (int r = minRow; r <= maxRow; r += 2) {
		for (int c = minCol; c <= maxCol; c += 2) {
			maze[r][c] = wall_t::TRACE;
		}
	}

	// Start digging recursively.
	this->DigMaze(1, 1, NULL);
}

void Maze::DisplayMaze()
{
	SDL_Rect r;
	r.w = TILE_W;
	r.h = TILE_H;
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);

	for (int row = 0; row < maze.size(); row++) {
		for (int col = 0; col < maze[0].size(); col++) {
			this->ColorCase(&r, row, col, maze[row][col]);
		}
	}

	this->ColorCase(&r, start.first, start.second, 0xFF, 0x0, 0x0);
	this->ColorCase(&r, end.first, end.second, 255, 135, 0);
	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, texture, NULL, &texture_sz);
}

void Maze::SetCell(wall_t type, int row, int col)
{
	if (row < maze.size() && col < maze[0].size())
		maze[row][col] = type;
}

void Maze::ColorCase(SDL_Rect* rect, int row, int col, int r, int g, int b)
{
	rect->x = row * TILE_W;
	rect->y = col * TILE_H;
	SDL_SetRenderDrawColor(renderer, r, g, b, 0x00);
	SDL_RenderFillRect(renderer, rect);
}

void Maze::ColorCase(SDL_Rect* rect, int row, int col, uint32_t color)
{
	this->ColorCase(rect, row, col, 
		(color & 0xff0000) >> 16,
		(color & 0x00ff00) >> 8,
		(color & 0x0000ff)
	);
}

std::pair<uint32_t, uint32_t> Maze::Get2DCoord(uint32_t coord)
{
	std::pair<uint32_t, uint32_t> p;
	p.first = coord / maze[0].size();
	p.second = coord % maze[0].size();
	return p;
}

Graph Maze::ConstructGraph()
{
	Graph graph(H * W);
	constexpr int coords[] = { -1, 1 };
	
	for (int row = 0; row < maze.size(); row++) {
		for (int col = 0; col < maze[0].size(); col++) {
			int vert_id = row * maze[0].size() + col;

			for (int offset : coords) {
				int ncol = col;
				int nrow = row + offset;
			
				if (nrow >= 0 && nrow < maze.size() && maze[nrow][ncol] != wall_t::WALL) {
					int nvert_id = nrow * maze[0].size() + ncol;
					graph.AddEdgeToVertex(vert_id, nvert_id, getCellCost(nrow, col));
				}
			}

			for (int offset : coords) {
				int ncol = col + offset;
				int nrow = row;

				if (ncol >= 0 && ncol < maze[0].size() && maze[nrow][ncol] != wall_t::WALL) {
					int nvert_id = nrow * maze[0].size() + ncol;
					graph.AddEdgeToVertex(vert_id, nvert_id, getCellCost(nrow, col));
				}
			}
		}
	}
	
	return graph;
}

void Maze::Solve()
{

}

void Maze::Dijsktra(Pair start, Pair end)
{
	int src = start.first * maze[0].size() + start.second;
	int V = H * W;
	Graph graph = ConstructGraph();
	int* dist = new int[V]; // The output array.  dist[i] will hold the shortest 
	// distance from src to i 

	bool* sptSet = new bool[V]; // sptSet[i] will be true if vertex i is included in shortest 
	// path tree or shortest distance from src to i is finalized 

	// Parent array to store 
	// shortest path tree 
	int* parent = new int[V];

	// Initialize all distances as INFINITE and stpSet[] as false 
	for (int i = 0; i < V; i++) {
		parent[i] = -1;
		dist[i] = INT_MAX;
		sptSet[i] = false;
	}

	// Distance of source vertex from itself is always 0 
	dist[src] = 0;

	SDL_Rect r;
	r.w = TILE_W;
	r.h = TILE_H;

	// Find shortest path for all vertices 
	for (int count = 0; count < V - 1; count++) {
		// Pick the minimum distance vertex from the set of vertices not 
		// yet processed. u is always equal to src in the first iteration. 
		int u = minDistance(dist, sptSet, V);

		// Mark the picked vertex as processed 
		sptSet[u] = true;

		// Update dist value of the adjacent vertices of the picked vertex. 
		for (int v = 0; v < V; v++) {

			// Update dist[v] only if is not in sptSet, there is an edge from 
			// u to v, and total weight of path from src to  v through u is 
			// smaller than current value of dist[v] 

			if (!sptSet[v] && graph.IsNext(u, v) && dist[u] != INT_MAX && dist[u] + graph.GetCost(u, v) < dist[v]) {
				dist[v] = dist[u] + graph.GetCost(u, v);
				parent[v] = u;

				SDL_SetRenderTarget(renderer, texture);
				auto pair = this->Get2DCoord(v);
				this->ColorCase(&r, pair.first, pair.second, 0x00, 0xFF, 0x00);
				SDL_SetRenderTarget(renderer, NULL);
				SDL_RenderCopy(renderer, texture, NULL, &texture_sz);
				SDL_RenderPresent(renderer);
			}
		}
	}

	std::stack<int> path;
	int j = end.first * maze[0].size() + end.second;
	printf("Parent : (%d, %d)\n", j, parent[j]);
	while (parent[j] != -1) {
		path.emplace(j);
		j = parent[j];
	}

	SDL_SetRenderTarget(renderer, texture);
	this->ColorCase(&r, start.first, start.second, 0xFF, 0x0, 0x0);
	this->ColorCase(&r, end.first, end.second, 255, 135, 0);
	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, texture, NULL, &texture_sz);
	SDL_RenderPresent(renderer);

	while (!path.empty()) {
		int p = path.top();
		SDL_SetRenderTarget(renderer, texture);
		auto pair = this->Get2DCoord(p);
		printf("Path : (%d, %d)\n", pair.first, pair.second);
		this->ColorCase(&r, pair.first, pair.second, 0x00, 0x00, 0xFF);
		this->ColorCase(&r, start.first, start.second, 0xFF, 0x0, 0x0);
		this->ColorCase(&r, end.first, end.second, 255, 135, 0);
		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopy(renderer, texture, NULL, &texture_sz);
		SDL_RenderPresent(renderer);
		path.pop();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	delete[] dist;
	delete[] sptSet;
	delete[] parent;
	std::this_thread::sleep_for(std::chrono::seconds(5));
}


// A Utility Function to check whether given cell (row, col) 
// is a valid cell or not. 
bool Maze::isValid(int row, int col)
{
	// Returns true if row number and column number 
	// is in range 
	return (row >= 0) && (row < H) && (col >= 0) && (col < W);
}

// A Utility Function to check whether the given cell is 
// blocked or not 
bool Maze::isUnBlocked(int row, int col)
{
	// Returns true if the cell is not blocked else false 
	if (maze[row][col] != wall_t::WALL)
		return true;
	else
		return false;
}

// A Utility Function to check whether destination cell has 
// been reached or not 
bool Maze::isDestination(int row, int col, Pair dest)
{
	if (row == dest.first && col == dest.second)
		return (true);
	else
		return (false);
}

// A Utility Function to calculate the 'h' heuristics. 
double calculateHValue(int type, int row, int col, Pair dest)
{
	// Return using the distance formula  based on the type:
	if (type == 0) { // Euclidean
		return ((double)sqrt((row - dest.first)*(row - dest.first)
			+ (col - dest.second)*(col - dest.second)));
	} else if (type == 1) { // Manhattan
		return (double)(std::abs(row - dest.first) + std::abs(col - dest.second));
	}
}


// A Utility Function to trace the path from the source 
// to destination 
void Maze::tracePath(const std::vector<std::vector<cell>>& cellDetails, Pair dest)
{
	SDL_Rect r;
	r.w = TILE_W;
	r.h = TILE_H;

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

	SDL_SetRenderTarget(renderer, texture);
	Path.push(std::make_pair(row, col));

	while (!Path.empty()) {
		std::pair<int, int> p = Path.top();
		Path.pop();
		this->ColorCase(&r, p.first, p.second, 0x00, 0x00, 0xFF);
		printf("-> (%d,%d) ", p.first, p.second);
	}

	this->ColorCase(&r, start.first, start.second, 0xFF, 0x0, 0x0);
	this->ColorCase(&r, end.first, end.second, 255, 135, 0);
	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, texture, NULL, &texture_sz);
	SDL_RenderPresent(renderer);

	std::this_thread::sleep_for(std::chrono::seconds(5));
	return;
}

// A Function to find the shortest path between 
// a given source cell to a destination cell according 
// to A* Search Algorithm 
void Maze::AStarSearch(int type, Pair src, Pair dest)
{
	int ROW = H;
	int COL = W;
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
	r.w = TILE_W;
	r.h = TILE_H;

	while (!openList.empty()) {
		pPair p = *openList.begin();

		// Remove this vertex from the open list 
		openList.erase(openList.begin());

		// Add this vertex to the closed list 
		i = p.second.first;
		j = p.second.second;
		closedList[i][j] = true;

		SDL_SetRenderTarget(renderer, texture);
		this->ColorCase(&r, i, j, 0x00, 0xFF, 0x00);
		this->ColorCase(&r, start.first, start.second, 0xFF, 0x0, 0x0);
		this->ColorCase(&r, end.first, end.second, 255, 135, 0);
		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopy(renderer, texture, NULL, &texture_sz);
		SDL_RenderPresent(renderer);
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
					gNew = cellDetails[i][j].g + getCellCost(i + dirX, j + dirY);
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