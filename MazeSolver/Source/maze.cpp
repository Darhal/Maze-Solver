#include "maze.hpp"
#include <SDL.h>

Maze::Maze(SDL_Renderer* renderer, int h, int w) :
	maze(h, std::vector<uint8_t>(w, wall_t::WALL)),
	H(h), W(w)
{
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
	this->BuildMaze();
}

void Maze::DigMaze(int r, int c, uint8_t* wall)
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
		*wall = wall_t::SPACE;
	}

	// Dig this cell.
	maze[r][c] = wall_t::SPACE;
	// this->DisplayMaze();

	// Randomly decide the order in which we explore the directions
	// N, S, E, W.  We use STL's random_shuffle() to shuffle an array.
	char D[4] = { 'N', 'S', 'E', 'W' };
	std::random_shuffle(D, D + 4, random_n);

	for (int i = 0; i < 4; i++) {
		// m[rr][cc] will be the cell that we'll try to dig.
		int rr = r;
		int cc = c;
		uint8_t* wall;

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

void Maze::DisplayMaze(SDL_Renderer* renderer)
{
	SDL_Rect r;
	r.w = TILE_W;
	r.h = TILE_H;
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);

	for (int row = 0; row < maze.size(); row++) {
		for (int col = 0; col < maze[0].size(); col++) {
			int color = 0x00;

			if (maze[row][col] == wall_t::SPACE) {
				color = 0xFF;
			}
			
			this->ColorCase(renderer, &r, row, col, color, color, color);
		}
	}

	this->ColorCase(renderer, &r, 1, 1, 0xFF, 0x0, 0x0);
	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void Maze::ColorCase(SDL_Renderer* render, SDL_Rect* rect, int row, int col, int r, int g, int b)
{
	rect->x = row * TILE_W;
	rect->y = col * TILE_H;
	SDL_SetRenderDrawColor(render, r, g, b, 0x00);
	SDL_RenderFillRect(render, rect);
}

void Maze::Solve()
{

}