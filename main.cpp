#include <iostream>
#include <queue>
#include <vector>
#include <random>
#include <algorithm>
#include <optional>

#include "fheap.hpp"
#include "dijkstra.hpp"

int main(){

    // Fibonacci Heap

    FibonacciHeap<int> heap(3);
    std::cout<<(heap.get_min()==std::nullopt)<<std::endl;

    std::vector<int> inserted;

    for(int i = 0 ; i < 10 ; ++i) {
        int temp = rand() % 100;
        heap.insert(temp);
        inserted.push_back(temp);
    }

    std::cout<<heap.get_min().value()<<std::endl;

    int min_value = heap.extract_min().value();

    std::cout<<min_value<<std::endl;

    // std::shared_ptr<FibonacciNode<int>> test_node1 = std::make_shared<FibonacciNode<int>>(15);
    // std::shared_ptr<FibonacciNode<int>> test_node2 = std::make_shared<FibonacciNode<int>>(8);
    // std::shared_ptr<FibonacciNode<int>> test_node3 = std::make_shared<FibonacciNode<int>>(5);
    // std::shared_ptr<FibonacciNode<int>> test_node4 = std::make_shared<FibonacciNode<int>>(9);
    // std::shared_ptr<FibonacciNode<int>> test_node5 = std::make_shared<FibonacciNode<int>>(10);
    // std::shared_ptr<FibonacciNode<int>> test_node6 = std::make_shared<FibonacciNode<int>>(1);
    // std::shared_ptr<FibonacciNode<int>> test_node7 = std::make_shared<FibonacciNode<int>>(28);
    // std::shared_ptr<FibonacciNode<int>> test_node8 = std::make_shared<FibonacciNode<int>>(29);

    

    // heap.insert(test_node7);
    // heap.insert(test_node2);
    // heap.insert(test_node3);
    // heap.insert(test_node4);
    // heap.extract_min();

    // heap.remove(test_node7);
    // heap.insert(test_node8);
    // heap.merge(test_node4, test_node8);
    // heap.remove(test_node8);

    // heap.insert(test_node1);
    // heap.merge(test_node1, test_node2);

    // heap.insert(test_node6);
    // heap.insert(test_node5);
    // heap.merge(test_node5, test_node6);
    // heap.merge(test_node6, test_node3);
    
    // heap.decrease_key(test_node4, 4);
    // heap.decrease_key(test_node1, 7);
    // std::cout << test_node6->degree << std::endl;
    // std::cout << ((test_node1->parent).lock() == nullptr ) << std::endl;
    // std::cout << ((test_node2->parent).lock() == nullptr ) << std::endl;
    // std::cout << ((test_node3->parent).lock() == nullptr ) << std::endl;
    // std::cout << ((test_node4->parent).lock() == nullptr ) << std::endl;

    // std::cout << (test_node2->child == nullptr ) << std::endl;
    // std::cout << (test_node3->child == nullptr ) << std::endl;
    // std::cout << (test_node4->child == nullptr ) << std::endl;



    // // Dijkstra's Algorithm

    edges_t edges1 = {{0, 1, 3.0f},
                    {0, 2, 1.0f},
                    {1, 2, 7.0f},
                    {1, 3, 5.0f},
                    {1, 4, 1.0f},
                    {2, 3, 2.0f},
                    {3, 4, 7.0f}};


    Graph g1(5, edges1, GraphType::UNDIRECTED);

    std::unordered_map<vertex_t, std::optional<std::tuple<vertex_t, edge_weight_t>>> result
            = dijkstra_shortest_path(g1, 2);

    // Previous vertex of src are not checked.
    std::vector<vertex_t> previous = {2, 0, (vertex_t)-1, 2, 1}; 
    std::vector<edge_weight_t> dist = {1.0f, 4.0f, 0.0f, 2.0f, 5.0f};


    // The printed result should be same as above.

    for(size_t i = 0 ; i < 5; ++i) {
        std::cout<< "[vertex " << i << "] ";
        std::cout<<"previous: "<<std::get<0>(result[i].value())<<", ";
        std::cout<<"distance: "<<std::get<1>(result[i].value())<<std::endl;
    }

    return 0;
    
}