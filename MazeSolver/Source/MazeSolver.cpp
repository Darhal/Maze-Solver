#include "MazeSolver.hpp"
#include <thread>
#include <chrono>

#define TICK_INTERVAL    30

MazeSolver::MazeSolver(int Window_W, int Window_H, int Maze_W, int Maze_H) :
	window(NULL), renderer(NULL), event(), maze(), next_time(0), currentSelection(0),
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
	font = TTF_OpenFont("Assets/arial.ttf", 25);
	maze.Init(renderer, MAZE_H, MAZE_W);
}

uint32_t MazeSolver::time_left(void)
{
	uint32_t now;

	now = SDL_GetTicks();
	if (next_time <= now)
		return 0;
	else
		return next_time - now;
}

void MazeSolver::Loop()
{
	for (uint32_t i = 0, j = 0; i < sizeof(Text) * MAX_MENU; i += sizeof(Text), j++) {
		Text* text = new (&menu[i]) Text(renderer, font, texts[j], 0, 50);
		SDL_Rect& txt_rct = text->getRect();
		txt_rct.x = (maze.getRect().w + Window_W - text->getRect().w) / 2;

		if (i > 0) {
			SDL_Rect& r = ((Text*)&menu[i - sizeof(Text)])->getRect();
			txt_rct.y = r.h + r.y;
		}
	}
	
	((Text*)&menu[currentSelection * sizeof(Text)])->setColor({ 0, 255, 0 });
	next_time = SDL_GetTicks() + TICK_INTERVAL;

	while (1) {
		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT)
			break;

		int r = HandleEvents();

		if (r == DISJKSTRA) {
			printf("Doing Dijsktra\n");
			maze.Dijsktra(1, 1);
		} else if (r == A_STAR) {
			printf("Doing A*\n");
			maze.AStarSearch(Pair(1, 1), Pair(maze.GetH() - 1, maze.GetW() - 1));
		} else {
			maze.DisplayMaze();
		}

		for (uint32_t i = 0; i < sizeof(Text) * MAX_MENU; i += sizeof(Text)) {
			((Text*)&menu[i])->render();
		}

		SDL_RenderPresent(renderer);

		SDL_Delay(time_left());
		next_time += TICK_INTERVAL;
	}
}

int MazeSolver::HandleEvents()
{
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:

			break;
		case SDLK_d:
			return DISJKSTRA;
		case SDLK_a:
			return A_STAR;
		case SDLK_UP:
			printf("BEFORE: %d\n", currentSelection);
			((Text*)&menu[currentSelection * sizeof(Text)])->setColor({ 255, 255, 255 });
			currentSelection = (--currentSelection + MAX_MENU) % MAX_MENU;
			printf("AFTER: %d\n", currentSelection);
			((Text*)&menu[currentSelection * sizeof(Text)])->setColor({ 0, 255, 0 });
			break;
		case SDLK_DOWN:
			printf("BEFORE: %d\n", currentSelection);
			((Text*)&menu[currentSelection * sizeof(Text)])->setColor({ 255, 255, 255 });
			currentSelection = (++currentSelection) % MAX_MENU;
			printf("AFTER: %d\n", currentSelection);
			((Text*)&menu[currentSelection * sizeof(Text)])->setColor({ 0, 255, 0 });
			break;
		default:
			break;
		}
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		isMouseDown = true;
		int divX = (maze.getRect().w / maze.GetW());
		int divY = (maze.getRect().h / maze.GetH());
		printf("Position : (%d, %d)\n", event.motion.x / divX, event.motion.y / divY);
	} else if (event.type == SDL_MOUSEBUTTONUP) {
		isMouseDown = false;
	}

	if (isMouseDown) {
		int divX = (maze.getRect().w / maze.GetW());
		int divY = (maze.getRect().h / maze.GetH());
		maze.SetCell(Maze::SPACE, event.motion.x / divX, event.motion.y / divY);
	}

	return 2;
}

void MazeSolver::Clean()
{
	SDL_DestroyRenderer(renderer);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
}