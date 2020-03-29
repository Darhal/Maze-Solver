#include "MazeSolver.hpp"
#include "AStar.hpp"
#include "Dijsktra.hpp"
#include <thread>
#include <chrono>

MazeSolver::MazeSolver(int Window_W, int Window_H, int Maze_W, int Maze_H) :
	window(NULL), renderer(NULL), event(), maze(), currentSelection(0),
	Window_H(Window_H), Window_W(Window_W), MAZE_W(Maze_W), MAZE_H(Maze_H),
	isMouseDown(false)
{
}

void MazeSolver::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		exit(3);
		return;
	}

	TTF_Init();

	window = SDL_CreateWindow("AI Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Window_W, Window_H, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, 0);
	font = TTF_OpenFont("Assets/arial.ttf", 48);
	maze.Init(renderer, MAZE_H, MAZE_W);
}

void MazeSolver::Loop()
{
	for (uint32_t i = 0, j = 0; i < sizeof(Text) * MAX_MENU; i += sizeof(Text), j++) {
		Text* text = new (&menu[i]) Text(renderer, font, texts[j], 0, 25);
		SDL_Rect& txt_rct = text->getRect();
		txt_rct.x = (maze.getRect().w + Window_W - text->getRect().w) / 2;

		if (i > 0) {
			SDL_Rect& r = ((Text*)&menu[i - sizeof(Text)])->getRect();
			txt_rct.y = r.h + r.y;
		}
	}
	
	((Text*)&menu[currentSelection * sizeof(Text)])->setColor({ 0, 255, 0 });
	bool run = true;

	while (run) {
		int r = -1;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				run = false;
				break;
			}

			((Text*)&menu[currentSelection * sizeof(Text)])->setColor({ 0, 255, 0 });
			HandleEvents(r);
		}

		if (r == DISJKSTRA) {
			printf("Starting Dijsktra Algorithm:\n");
			Dijsktra dijsktra(&maze);
			dijsktra.Start(maze.getStart(), maze.getEnd());
			std::this_thread::sleep_for(std::chrono::seconds(3));
		} else if (r == A_STAR_EUC || r == A_STAR_MAN) {
			printf("Starting A* Algorithm:\n");
			AStar astar(&maze);
			astar.AStarSearch((AStar::distance_t)(r - 1), maze.getStart(), maze.getEnd());
			std::this_thread::sleep_for(std::chrono::seconds(3));
		} else if (r == CAT_MOUSE) {
			maze.CatAndMouse();
		} else {
			maze.DisplayMaze();
		}

		for (uint32_t i = 0; i < sizeof(Text) * MAX_MENU; i += sizeof(Text)) {
			((Text*)&menu[i])->render();
		}

		SDL_RenderPresent(renderer);
	}
}

void MazeSolver::HandleEvents(int& r)
{
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:
			break;
		case SDLK_RETURN:
			{
				Text* t = NULL;
				switch (currentSelection) {
				case 8:
					maze.Clear();
					maze.Generate();
					break;
				case 9:
					maze.Reset();
					break;
				case 12:
					r = CAT_MOUSE;
					t = ((Text*)&menu[currentSelection * sizeof(Text)]);
					t->setColor({ 255, 0, 0 });
					t->render();
					break;
				case 0:
					r = DISJKSTRA;
					t = ((Text*)&menu[currentSelection * sizeof(Text)]);
					t->setColor({ 255, 0, 0 });
					t->render();
					break;
				case 1:
					r = A_STAR_EUC;
					t = ((Text*)&menu[currentSelection * sizeof(Text)]);
					t->setColor({ 255, 0, 0 });
					t->render();
					break;
				case 2:
					r = A_STAR_MAN;
					t = ((Text*)&menu[currentSelection * sizeof(Text)]);
					t->setColor({ 255, 0, 0 });
					t->render();
					break;
				default:
					break;
				}
			}
			break;
		case SDLK_UP:
			((Text*)&menu[currentSelection * sizeof(Text)])->setColor({ 255, 255, 255 });
			currentSelection = (--currentSelection + MAX_MENU) % MAX_MENU;
			((Text*)&menu[currentSelection * sizeof(Text)])->setColor({ 0, 255, 0 });
			break;
		case SDLK_DOWN:
			((Text*)&menu[currentSelection * sizeof(Text)])->setColor({ 255, 255, 255 });
			currentSelection = (++currentSelection) % MAX_MENU;
			((Text*)&menu[currentSelection * sizeof(Text)])->setColor({ 0, 255, 0 });
			break;
		default:
			break;
		}
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		int divX = (maze.getRect().w / maze.GetW());
		int divY = (maze.getRect().h / maze.GetH());

		if (currentSelection == 10) { // Edit start
			maze.setStart(Pair(event.motion.x / divX, event.motion.y / divY));
		} else if (currentSelection == 11) {
			maze.setEnd(Pair(event.motion.x / divX, event.motion.y / divY));
		}

		isMouseDown = true;
	} else if (event.type == SDL_MOUSEBUTTONUP) {
		isMouseDown = false;
	}

	if (isMouseDown) {
		int divX = (maze.getRect().w / maze.GetW());
		int divY = (maze.getRect().h / maze.GetH());

		if (currentSelection == 3) {
			maze.SetCell(Maze::SPACE, event.motion.x / divX, event.motion.y / divY);
		} else if (currentSelection == 4) {
			maze.SetCell(Maze::WALL, event.motion.x / divX, event.motion.y / divY);
		}else if (currentSelection == 5) {
			maze.SetCell(Maze::STONE, event.motion.x / divX, event.motion.y / divY);
		} else if (currentSelection == 6) {
			maze.SetCell(Maze::SAND, event.motion.x / divX, event.motion.y / divY);
		} else if (currentSelection == 7) {
			maze.SetCell(Maze::WATER, event.motion.x / divX, event.motion.y / divY);
		}
	}
}

void MazeSolver::Clean()
{
	SDL_DestroyRenderer(renderer);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
}