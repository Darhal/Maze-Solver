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
	MazeSolver mazeSolv(1280, 768, 64, 64);

	mazeSolv.Init();

	mazeSolv.Loop();

	mazeSolv.Clean();

	return 0;
}
