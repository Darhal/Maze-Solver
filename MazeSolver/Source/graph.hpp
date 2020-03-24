#pragma once

#include <vector>

class Graph
{
public:
	Graph(uint32_t vertecies = 0) : graph(vertecies)
	{
	}

	void SetVertex(uint32_t vertex, std::vector<uint32_t> next)
	{
		graph[vertex] = std::move(next);
	}

	void AddEdgeToVertex(uint32_t vertex, uint32_t next)
	{
		graph[vertex].emplace_back(next);
	}

	std::vector<uint32_t>& GetVertex(uint32_t vertex)
	{
		return graph[vertex];
	}

	bool IsNext(uint32_t vertex, uint32_t next)
	{
		for (uint32_t v : this->GetVertex(vertex)) {
			if (v == next) {
				return true;
			}
		}

		return false;
	}
private:
	std::vector<std::vector<uint32_t>> graph;
};