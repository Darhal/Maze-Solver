#include <vector>
#include <array>
#include <utility>
#include <algorithm>
#include <random>

static int random_n(int n) 
{ 
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, n - 1);
	return dist(rng);
}

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Rect;

class Maze
{
public:
	typedef std::vector<std::vector<uint8_t>> MazeArray;

	enum wall_t {
		WALL = 0,
		SPACE = 1,
		TRACE = 2,
	};
public:
    Maze(SDL_Renderer* renderer, int h = 32, int w = 32);

    void DigMaze(int r, int c, uint8_t* wall);

    void BuildMaze();

    void DisplayMaze(SDL_Renderer* renderer);

    MazeArray& GetMazeArray() { return maze; }

	void ColorCase(SDL_Renderer* render, SDL_Rect* rect, int row, int col, int r, int g, int b);

	void Solve();
private:
    MazeArray maze;
	SDL_Texture* texture;
    int H, W;

	static constexpr int TILE_W = 1;
	static constexpr int TILE_H = 1;
};