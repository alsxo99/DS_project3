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
	// previous[src] = src;
	dist[src] = 0;
	nodes[src] = std::make_shared<FibonacciNode<float>>(dist[src]);
    FibonacciHeap<float> heap = {};

	for (vertex_t v = 0; v < g.get_num_vertices(); v++)
	{
		if (v != src) {
			nodes[v] = std::make_shared<FibonacciNode<float>>(dist[v]);
		}
		heap.insert(nodes[v]);
	}

	vertex_t dest = 0;
	edge_weight_t edge_weight = 0;

	while (!heap.is_empty())
	{
		// get_min_node로 받은 min_node의 pointer와 같은 node를 가리키는 node u 를 nodes에서 찾는다.
		size_t u;
		FibonacciNode<float>* min_node = heap.get_min_node();
		for (vertex_t i = 0; i < nodes.size(); i++)
		{
			if (nodes[i].get() == min_node) {
				u = i;
				break;
			}
		}
		float min_key = heap.extract_min().value();
		// size_t u = std::find(dist.begin(), dist.end(), min_key) - dist.begin();
		std::vector<edge_t> adjacency_list = g.adj_list(u);
		for (vertex_t v = 0; v < adjacency_list.size(); v++)
		{
			dest = std::get<1>(adjacency_list[v]);
			edge_weight = std::get<2>(adjacency_list[v]);

			if (dist[u] + edge_weight < dist[dest])
			{
				dist[dest] = dist[u] + edge_weight;
				previous[dest] = u;
				heap.decrease_key(nodes[dest], dist[dest]);
			}
		}
	}

	for (vertex_t v = 0; v < g.get_num_vertices(); v++)
	{
		if (dist[v] != 1e10)
			M[v] = std::make_tuple(previous[v], dist[v]);
	}
	

	return M;
}


#endif // __DIJKSTRA_H_
