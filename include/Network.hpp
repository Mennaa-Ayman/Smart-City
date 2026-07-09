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
#include <tuple>

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

    // Get every edge currently in the graph as (source, destination, weight)
    std::vector<std::tuple<int, int, double>> getAllEdges() const;

    // Dijkstra's Algorithm
    std::vector<int> shortestPath(int source, int destination);

    // BFS Traversal
    std::vector<int> bfs(Node* startNode);

    // DFS Traversal
    std::vector<int> dfs(Node* startNode);

    // Kruskal's Algorithm: Minimum Spanning Tree for infrastructure planning
    // (treats roads as undirected for the purpose of connectivity planning)
    std::vector<std::tuple<int, int, double>> kruskalMST(const std::vector<int>& allNodeIds);

    // Bellman-Ford Algorithm: shortest paths that can tolerate negative weights
    // (e.g. toll discounts represented as negative edge weights)
    // Returns false if a negative-weight cycle is detected.
    bool bellmanFord(int source, const std::vector<int>& allNodeIds,
                      std::unordered_map<int, double>& distances);

    // Topological Sort (Kahn's Algorithm): ordering for one-way street planning
    // Returns an empty vector if the network contains a cycle (no valid order exists).
    std::vector<int> topologicalSort(const std::vector<int>& allNodeIds);
};

#endif // ROADNETWORK_HPP