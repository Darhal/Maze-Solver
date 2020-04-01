#include <vector>
#include <cstdlib>
#include <utility>
#include <algorithm>
#include <iostream>


#include "Source/maze.hpp"
#include "Source/MazeSolver.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	uint32_t maze_sz = 64;

	if (argc >= 2) {
		maze_sz = atoi(argv[1]);
		maze_sz = (maze_sz % 2 ? maze_sz + 1 : maze_sz);
	}

	MazeSolver mazeSolv(1280, 768, maze_sz, maze_sz);

	mazeSolv.Init();

	mazeSolv.Loop();

	mazeSolv.Clean();

	return 0;
}
