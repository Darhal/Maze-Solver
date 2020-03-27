#include <vector>
#include <cstdlib>
#include <utility>
#include <algorithm>
#include <iostream>


#include "Source/maze.hpp"
#include "Source/MazeSolver.hpp"

using namespace std;

int SDL_main(int argc, char* argv[])
{
	MazeSolver mazeSolv(1280, 768, 32, 32);

	mazeSolv.Init();

	mazeSolv.Loop();

	mazeSolv.Clean();

	return 0;
}
