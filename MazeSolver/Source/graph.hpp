#pragma once

#include <vector>
#include <utility>
#include <stdint.h>
#include <limits.h>

class Graph
{
public:
	Graph(uint32_t vertecies = 0) : graph(vertecies)
	{
	}

	void SetVertex(uint32_t vertex, std::vector<std::pair<uint32_t, uint32_t>> next)
	{
		graph[vertex] = std::move(next);
	}

	void AddEdgeToVertex(uint32_t vertex, uint32_t next, uint32_t cost = 1)
	{
		graph[vertex].emplace_back(next, cost);
	}

	std::vector<std::pair<uint32_t, uint32_t>>& GetVertex(uint32_t vertex)
	{
		return graph[vertex];
	}

	bool IsNext(uint32_t vertex, uint32_t next)
	{
		for (std::pair<uint32_t, uint32_t> v : this->GetVertex(vertex)) {
			if (v.first == next) {
				return true;
			}
		}

		return false;
	}

	uint32_t GetCost(uint32_t vertex, uint32_t next)
	{
		for (std::pair<uint32_t, uint32_t> v : this->GetVertex(vertex)) {
			if (v.first == next) {
				return v.second;
			}
		}

		return UINT_MAX;
	}
private:
	std::vector<std::vector<std::pair<uint32_t, uint32_t>>> graph;
};