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

#include "Utils.hpp"
#include <unordered_map>
#include <queue>
#include <stack>
#include <unordered_set>

class RoadNetwork{
private: 
    std::unordered_map<int, std::vector<std::pair<int, double>>> adjList; // <neighbor, weight>
    std::unordered_map<int, Location> nodes;

public:
    // Helper Function to get Location ID by Name
    int getLocationIdByName(const std::string& name, const std::vector<Location>& locations);

    // Basic Graph Operations
    void addEdge(int src, int dest, double weight);
    void removeEdge(int nodeId);
    void updateEdgeWeight(int src, int dest, double newWeight);

    // Dijkstra's Algorithm
    std::vector<int> shortestPath(int source, int destination);

    // BFS Traversal
    std::vector<int> bfs(Node* startNode);

    // DFS Traversal
    std::vector<int> dfs(Node* startNode);

    
};

#endif // ROADNETWORK_HPP