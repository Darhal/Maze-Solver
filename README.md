# Maze Solver - IA Project

## GitHub Link:
https://github.com/Darhal/Maze-Solver

## General Overview:
* The Project is written using C++11 and SDL2 library for display, its demonstrates the usage of Dijkstra and A* alogrithm to 
find a shortest path in a randomly generated maze.

## Features:
* Randomly generated maze
* Dijkstra
* A* Algorithm
  * Using Euclidean distance
  * Using Manhattan distance
* Different costs depends on the cell
* Ability to regenrate the maze
* Ability to clear the maze
* Ability to modify the start and the end point
* Ability to modify the maze with adding or removing cells
* Mouse & Cat Simulation where the start and the end point move
* Ability to visulaizee the algorithm execution (visisted cells, taken path) in real-time

## Cells Explained:
The maze have different cell types that have different costs:
| Color:        | Cell:         | Cost: | Frequency: |
| ------------- |:-------------:| -----:| ----------:|
| White         | Empty         | 1     | 50%        |
| Gray          | Stone         | 2     | 30%        |
| Light Yellow  | Sand          | 3     | 15%        |
| Light Blue    | Water         | 4     | 5%         |

Start and end point:
| Color:        | Function:     |
| ------------- |:-------------:|
| Red           | Start Point   |
| Orange        | End Point     |

## Usage:
### Execution:
To execute the program use `./MazeSolver size` where size is a number of cells for the maze.</br>
**PLEASE NOTE:** that since the maze is generated randomly using recursive function, the bigger your maze size is the higher the risk of getting stack overflow error

*Example of execution*:
`./MazeSolver 16` (or `./MazeSolv 16` if you're using linux) will generate a 16x16 maze

### During execution:
* During execution the maze will be displayed on the left and the menu will be on the right. Use ↑ and ↓ to navigate the menu, 
press enter confirm the current option and run the algorithm (The option will go in red, till the simulation ends).</br> 
* Or you can select one of the following options:
  * Empty Cell => Will turn the current cell to an empty one
  * Add Wall => Will block the current cell
  * Add Stone => Will turn the current cell to stone cell
  * Add Sand => Will turn the current cell to sand cell
  * Add Water => Will turn the current cell to water cell
* Click on a cell to modify it. You can, as well, change multiple cells by holding the left mouse button and moving the mouse arround these cells.
* Borders can't be deleted or changed by default
* You can select 'Edit Start' or 'Edit End' and then clicking on the maze to modify the start or the end point of the shortest path search
* 'The Mouse & The cat' option will start simulating a mouse chasing a cat, both end and start point are moving till the mouse get stuck (the mouse can't walk into cells he already walked into)

## Compilation:
### Windows:
1. Open the .sln file with visual studio
2. Build and run the project
3. The excutable will be in `Executable/x64/Release/MazeSolver`

### Linux:
1. Open terminal, type `make`
2. The executeable will be in `Executable/Linux/MazeSolv`
3. Run `./MazeSolv size`

## Some Pictures:
 ![alt text](https://i.imgur.com/VGKvVRS.png)
 ![alt text](https://i.imgur.com/q2KBGSj.png)
 ![alt text](https://i.imgur.com/VgDobGT.png)
 ![alt text](https://i.imgur.com/YGlQylp.png)
 ![alt text](https://i.imgur.com/eS7tr1g.png)
