#ifndef __DIJKSTRA_H_
#define __DIJKSTRA_H_

#include <vector>
#include <unordered_map>
#include <list>
#include <tuple>
#include <queue>
#include <optional>
#include "fheap.hpp"

// A vertex is typed as `vertex_t`. An edge is typed as `edge_t`.
using vertex_t = std::size_t;
using edge_weight_t = float;
using edge_t = std::tuple<vertex_t, vertex_t, edge_weight_t>;
using edges_t = std::vector<edge_t>;

enum class GraphType {
UNDIRECTED,
DIRECTED
};


class Graph {
    public:
        Graph() = delete;
        Graph(const Graph&) = delete;
        Graph(Graph&&) = delete;

        // We assume that if num_vertices is V, a graph contains n vertices from 0 to V-1.
        Graph(size_t num_vertices, const edges_t& edges, GraphType type)
			: num_vertices(num_vertices), type(type) {
			this->num_vertices = num_vertices;
			graph.resize(num_vertices);

			if(type == GraphType::UNDIRECTED) {
				for(auto &edge : edges) {
					const auto& [ from, to, weight ] = edge;
					graph[from].emplace_back(from, to, weight);
					graph[to].emplace_back(to, from, weight);
				}
			} else {
				for(auto &edge : edges) {
					const auto& [ from, to, weight ] = edge;
					graph[from].emplace_back(from, to, weight);
				}
			}
        }

		size_t get_num_vertices() { return num_vertices; }
		std::vector<edge_t> adj_list(vertex_t v) { return graph[v]; }
		
    private:
		size_t num_vertices;
		std::vector<std::vector<edge_t>> graph;
		GraphType type;
};




std::unordered_map<vertex_t, std::optional<std::tuple<vertex_t, edge_weight_t>>>
dijkstra_shortest_path(Graph& g, vertex_t src) {
	
	std::unordered_map<vertex_t, std::optional<std::tuple<vertex_t, edge_weight_t>>> M;

    // std::nullopt if vertex v is not reacheble from the source.
    for(vertex_t v = 0; v < g.get_num_vertices(); v++) M[v] = std::nullopt;
	std::vector<vertex_t> previous(g.get_num_vertices(), -1);
	std::vector<edge_weight_t> dist(g.get_num_vertices(), 1e10);

	std::vector<std::shared_ptr<FibonacciNode<float>>> nodes(g.get_num_vertices(), nullptr);

    // TODO
	dist[src] = 0;
	nodes[src] = std::make_shared<FibonacciNode<float>>(dist[src]);
    FibonacciHeap<float> heap = {};

	for (vertex_t v = 0; v < g.get_num_vertices(); v++)
	{
		if (v != src) {
			dist[v] = INFINITY;
			nodes[v] = std::make_shared<FibonacciNode<float>>(dist[v]);
		}
		heap.insert(nodes[v]);
	}

	while (!heap.is_empty())
	{
		float min_key = heap.extract_min().value();
		// 이 min_key를 가지고 있는 u를 dist에서 찾아야 한다.
		size_t u;
		std::vector<edge_t> adjacency_list = g.adj_list(u);
		for (vertex_t v = 0; v < g.get_num_vertices(); v++)
		{
			if (dist[u] + (adjacency_list[v])[2] < dist[v])
			{
				dist[v] = dist[u] + (adjacency_list[v])[2];
				previous[v] = u;
				heap.decrease_key(nodes[v], dist[v]);
			}
			
		}
		
		
	}

	return M;
}


#endif // __DIJKSTRA_H_
