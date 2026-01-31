#include "Network.hpp"
#include <limits>
#include <algorithm>

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
    // Initialize distances and parent tracking
    std::unordered_map<int, double> distance;
    std::unordered_map<int, int> parent;
    std::unordered_set<int> visited;
    
    // Priority queue: (distance, node)
    auto cmp = [](const std::pair<double, int>& a, const std::pair<double, int>& b) {
        return a.first > b.first;  // Min-heap
    };
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, 
                       decltype(cmp)> pq(cmp);
    
    // Initialize all distances to infinity
    for (const auto& node : adjList) {
        distance[node.first] = std::numeric_limits<double>::max();
        parent[node.first] = -1;
    }
    
    // Set source distance to 0
    distance[source] = 0.0;
    pq.push({0.0, source});
    
    // Dijkstra's main loop
    while (!pq.empty()) {
        auto [curr_dist, curr_node] = pq.top();
        pq.pop();
        
        // Skip if already visited
        if (visited.count(curr_node)) {
            continue;
        }
        
        visited.insert(curr_node);
        
        // If we reached destination, we can stop early
        if (curr_node == destination) {
            break;
        }
        
        // Skip if this distance is outdated
        if (curr_dist > distance[curr_node]) {
            continue;
        }
        
        // Relax all neighbors
        if (adjList.count(curr_node)) {
            for (const auto& [neighbor, weight] : adjList[curr_node]) {
                double newDist = distance[curr_node] + weight;
                
                // Found shorter path
                if (newDist < distance[neighbor]) {
                    distance[neighbor] = newDist;
                    parent[neighbor] = curr_node;
                    pq.push({newDist, neighbor});
                }
            }
        }
    }
    
    // Reconstruct path from destination to source
    std::vector<int> path;
    if (distance[destination] == std::numeric_limits<double>::max()) {
        // No path exists
        return path;
    }
    
    int curr = destination;
    while (curr != -1) {
        path.push_back(curr);
        curr = parent[curr];
    }
    
    // Reverse to get path from source to destination
    std::reverse(path.begin(), path.end());
    
    return path;
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