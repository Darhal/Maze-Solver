#include "maze.hpp"
#include <stack>
#include <SDL.h>
#include <thread>
#include <chrono>
#include <set>
#include "AStar.hpp"

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

uint32_t Maze::getCellCost(int row, int col) const
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

	return UINT32_MAX;
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
	uint32_t D[4] = { 'N', 'S', 'E', 'W' };
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
	p.first = coord / (uint32_t)maze[0].size();
	p.second = coord % (uint32_t)maze[0].size();
	return p;
}

Graph Maze::ConstructGraph()
{
	Graph graph(H * W);
	constexpr int coords[] = { -1, 1 };
	
	for (uint32_t row = 0; row < (uint32_t)maze.size(); row++) {
		for (uint32_t col = 0; col < (uint32_t)maze[0].size(); col++) {
			uint32_t vert_id = row * (uint32_t)maze[0].size() + col;

			for (int offset : coords) {
				uint32_t ncol = col;
				uint32_t nrow = row + offset;
			
				if (nrow >= 0 && nrow < (uint32_t)maze.size() && maze[nrow][ncol] != wall_t::WALL) {
					uint32_t nvert_id = nrow * (uint32_t)maze[0].size() + ncol;
					graph.AddEdgeToVertex(vert_id, nvert_id, getCellCost(nrow, col));
				}
			}

			for (int offset : coords) {
				uint32_t ncol = col + offset;
				uint32_t nrow = row;

				if (ncol >= 0 && ncol < (uint32_t)maze[0].size() && maze[nrow][ncol] != wall_t::WALL) {
					uint32_t nvert_id = nrow * (uint32_t)maze[0].size() + ncol;
					graph.AddEdgeToVertex(vert_id, nvert_id, getCellCost(nrow, col));
				}
			}
		}
	}
	
	return graph;
}


bool Maze::isValid(int row, int col)
{

	return (row >= 0) && (row < this->H) && (col >= 0) && (col < this->W);
}


bool Maze::isUnBlocked(int row, int col)
{
	if (this->maze[row][col] != Maze::wall_t::WALL)
		return true;
	else
		return false;
}

// TODO: Change this so the cat just move to the first cell in the path and then the mouse move
// And not jump right away
void Maze::CatAndMouse()
{
	// Save intial position
	Pair start = this->getStart();
	Pair end = this->getEnd();

	bool blocked = false;
	Pair cat = this->getStart();
	Pair mouse = this->getEnd();
	std::set<Pair> mouse_steps;
	mouse_steps.insert(mouse);

	SDL_Event event;

	Pair coords[] = {
		             Pair(-1, 0),
		Pair(0, -1)             , Pair(0, 1),
		             Pair(1, 0) ,
	};
	
	while(true) {
		SDL_PollEvent(&event);
		// Check where the mouse can run:
		blocked = true;
		std::random_shuffle(coords, coords + 4, random_n); //  Randomise psotion

		for (const Pair& off : coords) {
			Pair future_step = mouse + off;
			const bool is_in = mouse_steps.find(future_step) != mouse_steps.end();

			if (isValid(future_step) && isUnBlocked(future_step) && !is_in) {
				mouse = future_step;
				mouse_steps.insert(mouse);
				blocked = false;
				setEnd(mouse);

				this->DisplayMaze();
				SDL_RenderPresent(renderer);
				break;
			}
		}

		AStar astar(this);
		std::stack<Pair> path = astar.AStarSearch(AStar::MANHATTAN, cat, mouse);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (!path.empty()) {
			path.pop(); // Pop our current position 
			cat = path.top();
			setStart(cat);

			if (cat == mouse) {
				break;
			}
		} else {
			break; // No Path
		}

		this->DisplayMaze();
		SDL_RenderPresent(renderer);
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));
	setStart(start);
	setEnd(end);
}