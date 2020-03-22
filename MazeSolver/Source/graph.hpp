#pragma once

#include <vector>

class Graph
{
public:
	Graph();

	void AddEdge();
private:
	std::vector<std::vector<uint8_t>> graph;
};