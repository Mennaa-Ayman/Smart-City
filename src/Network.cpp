#include "Network.hpp"

// ================ Get Location ID by Name Function ================ //
int RoadNetwork::getLocationIdByName(const std::string& name, const std::vector<Location>& locations) {
    for (const auto& loc : locations) {
        if (loc.name == name) {
            return loc.id;
        }
    }
    return -1; 
}
// ================ Basic Graph Operations ================= //

void RoadNetwork::addEdge(int src, int dest, double weight) {
    adjList[src].emplace_back(dest, weight);
}
void RoadNetwork::removeEdge(int nodeId) {
    adjList.erase(nodeId);
}
void RoadNetwork::updateEdgeWeight(int src, int dest, double newWeight) {
    for (auto& neighbor : adjList[src]) {
        if (neighbor.first == dest) {
            neighbor.second = newWeight;
            break;
        }
    }
}

// ================ Dijkstra's Algorithm ================= //
std::vector<int> RoadNetwork::shortestPath(int source, int destination) {
    
    return {};
}

// ================ BFS Traversal ================= //
std::vector<int> RoadNetwork::bfs(Node* startNode) {
    std::queue<int> q;
    std::vector<int> r;
    std::unordered_set<int> visited;

    // Initialize queue with start node
    q.push(startNode->location.id);
    visited.insert(startNode->location.id);

    while(!q.empty()) {
        int current = q.front();
        q.pop();
        r.push_back(current);

        // Enqueue unvisited neighbors
        for (const auto& neighbor : adjList[current]) {
            if (visited.find(neighbor.first) == visited.end()) {
                visited.insert(neighbor.first);
                q.push(neighbor.first);
            }
        }
    }
    return r;
}   
// ================ DFS Traversal ================= //
std::vector<int> RoadNetwork::dfs(Node* startNode) {
    std::stack <int> s;
    std::vector<int> r;
    std::unordered_set<int> visited;

    // Initialize stack with start node
    s.push(startNode->location.id);
    visited.insert(startNode->location.id);

    while(!s.empty()) {
        int current = s.top();
        s.pop();
        
        // Process current node
        r.push_back(current);

        // Push unvisited neighbors
        for (const auto& neighbor : adjList[current]) {
            if (visited.find(neighbor.first) == visited.end()) {
                visited.insert(neighbor.first);
                s.push(neighbor.first);
            }
        }
    }
    return r;
}