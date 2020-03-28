#include "Dijsktra.hpp"
#include <stack>
#include <chrono>
#include <thread>

Dijsktra::Dijsktra(Maze* maze) : maze(maze)
{

}

// A utility function to find the vertex with minimum distance value, from 
// the set of vertices not yet included in shortest path tree 
uint32_t Dijsktra::minDistance(uint32_t dist[], bool sptSet[], uint32_t V)
{
	// Initialize min value 
	uint32_t min = UINT_MAX, min_index;

	for (uint32_t v = 0; v < V; v++) {
		if (sptSet[v] == false && dist[v] <= min) {
			min = dist[v];
			min_index = v;
		}
	}

	return min_index;
}

void Dijsktra::Start(const Pair& start, const Pair& end)
{
	uint32_t src = start.first * (uint32_t)maze->maze[0].size() + start.second;
	uint32_t V = maze->H * maze->W;
	Graph graph = maze->ConstructGraph();
	uint32_t* dist = new uint32_t[V]; // The output array.  dist[i] will hold the shortest 
	// distance from src to i 

	bool* sptSet = new bool[V]; // sptSet[i] will be true if vertex i is included in shortest 
	// path tree or shortest distance from src to i is finalized 

	// Parent array to store 
	// shortest path tree 
	int* parent = new int[V];

	// Initialize all distances as INFINITE and stpSet[] as false 
	for (uint32_t i = 0; i < V; i++) {
		parent[i] = -1;
		dist[i] = INT_MAX;
		sptSet[i] = false;
	}

	// Distance of source vertex from itself is always 0 
	dist[src] = 0;

	SDL_Rect r;
	r.w = maze->TILE_W;
	r.h = maze->TILE_H;

	// Find shortest path for all vertices 
	for (uint32_t count = 0; count < V - 1; count++) {
		// Pick the minimum distance vertex from the set of vertices not 
		// yet processed. u is always equal to src in the first iteration. 
		uint32_t u = minDistance(dist, sptSet, V);

		// Mark the picked vertex as processed 
		sptSet[u] = true;

		// Update dist value of the adjacent vertices of the picked vertex. 
		for (uint32_t v = 0; v < V; v++) {

			// Update dist[v] only if is not in sptSet, there is an edge from 
			// u to v, and total weight of path from src to  v through u is 
			// smaller than current value of dist[v] 

			if (!sptSet[v] && graph.IsNext(u, v) && dist[u] != INT_MAX && dist[u] + graph.GetCost(u, v) < dist[v]) {
				dist[v] = dist[u] + graph.GetCost(u, v);
				parent[v] = u;

				SDL_SetRenderTarget(maze->renderer, maze->texture);
				auto pair = maze->Get2DCoord(v);
				maze->ColorCase(&r, pair.first, pair.second, 0x00, 0xFF, 0x00);
				SDL_SetRenderTarget(maze->renderer, NULL);
				SDL_RenderCopy(maze->renderer, maze->texture, NULL, &maze->texture_sz);
				SDL_RenderPresent(maze->renderer);
			}
		}
	}

	std::stack<int> path;
	uint32_t j = end.first * (uint32_t)maze->maze[0].size() + end.second;
	printf("Parent : (%d, %d)\n", j, parent[j]);
	while (parent[j] != -1) {
		path.emplace(j);
		j = parent[j];
	}

	SDL_SetRenderTarget(maze->renderer, maze->texture);
	maze->ColorCase(&r, start.first, start.second, 0xFF, 0x0, 0x0);
	maze->ColorCase(&r, end.first, end.second, 255, 135, 0);
	SDL_SetRenderTarget(maze->renderer, NULL);
	SDL_RenderCopy(maze->renderer, maze->texture, NULL, &maze->texture_sz);
	SDL_RenderPresent(maze->renderer);

	while (!path.empty()) {
		int p = path.top();
		SDL_SetRenderTarget(maze->renderer, maze->texture);
		auto pair = maze->Get2DCoord(p);
		printf("Path : (%d, %d)\n", pair.first, pair.second);
		maze->ColorCase(&r, pair.first, pair.second, 0x00, 0x00, 0xFF);
		maze->ColorCase(&r, start.first, start.second, 0xFF, 0x0, 0x0);
		maze->ColorCase(&r, end.first, end.second, 255, 135, 0);
		SDL_SetRenderTarget(maze->renderer, NULL);
		SDL_RenderCopy(maze->renderer, maze->texture, NULL, &maze->texture_sz);
		SDL_RenderPresent(maze->renderer);
		path.pop();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	delete[] dist;
	delete[] sptSet;
	delete[] parent;
	std::this_thread::sleep_for(std::chrono::seconds(5));
}