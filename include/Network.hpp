/* ====================================================================================
for network analysis and pathfinding
- Represent the city as a weighted directed graph 
    - Nodes: Intersections/Locations 
    - Edges: Roads with weights (distance, traffic congestion, time) 
- Graph algorithms: 
    - Dijkstra's Algorithm: Find shortest path between locations 
    - BFS/DFS: Explore connectivity and reachability 
    - Kruskal's/Prim's Algorithm: Build minimum spanning tree for infrastructure planning 
    - Bellman-Ford: Handle negative weights (e.g., toll discounts) 
    - Topological Sort: For one-way street planning 
==================================================================================== */

#ifndef ROADNETWORK_HPP
#define ROADNETWORK_HPP

#include <unordered_map>
#include <iostream>
#include <vector>

struct Edge {
    int to;
    double weight; // distance, time, congestion level
};

struct Node {
    int id;
    std::vector<Edge> edges;
};

class RoadNetwork{
private: 
    std::unordered_map<int, std::vector<std::pair<int, double>>> roads;
    

public:
    
};

#endif // ROADNETWORK_HPP