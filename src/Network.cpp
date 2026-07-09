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

// ================ Get All Edges ================= //
std::vector<std::tuple<int, int, double>> RoadNetwork::getAllEdges() const {
    std::vector<std::tuple<int, int, double>> edges;
    for (const auto& [src, neighbors] : adjList) {
        for (const auto& [dest, weight] : neighbors) {
            edges.emplace_back(src, dest, weight);
        }
    }
    return edges;
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
// ================ Kruskal's Algorithm (Minimum Spanning Tree) ================= //
namespace {
    // Simple Disjoint Set Union (Union-Find), local to this translation unit
    struct DSU {
        std::unordered_map<int, int> parent;
        std::unordered_map<int, int> rank_;

        void makeSet(int x) {
            if (parent.find(x) == parent.end()) {
                parent[x] = x;
                rank_[x] = 0;
            }
        }

        int find(int x) {
            makeSet(x);
            if (parent[x] != x) {
                parent[x] = find(parent[x]); // path compression
            }
            return parent[x];
        }

        // Returns true if x and y were in different sets (i.e. the union happened)
        bool unite(int x, int y) {
            int rootX = find(x);
            int rootY = find(y);
            if (rootX == rootY) return false;

            if (rank_[rootX] < rank_[rootY]) std::swap(rootX, rootY);
            parent[rootY] = rootX;
            if (rank_[rootX] == rank_[rootY]) rank_[rootX]++;
            return true;
        }
    };
}

std::vector<std::tuple<int, int, double>> RoadNetwork::kruskalMST(const std::vector<int>& allNodeIds) {
    // Treat the road network as undirected: dedupe (u,v) and (v,u) pairs,
    // keeping the cheaper weight, since infrastructure planning cares about
    // whether two locations are connected, not travel direction.
    std::unordered_map<long long, double> undirectedEdges;
    auto edges = getAllEdges();
    for (const auto& e : edges) {
        int u = std::get<0>(e), v = std::get<1>(e);
        double w = std::get<2>(e);
        int a = std::min(u, v), b = std::max(u, v);
        long long key = (long long)a * 1000000LL + b;
        auto it = undirectedEdges.find(key);
        if (it == undirectedEdges.end() || w < it->second) {
            undirectedEdges[key] = w;
        }
    }

    std::vector<std::tuple<int, int, double>> candidateEdges;
    for (const auto& kv : undirectedEdges) {
        int a = (int)(kv.first / 1000000LL);
        int b = (int)(kv.first % 1000000LL);
        candidateEdges.emplace_back(a, b, kv.second);
    }

    // Sort edges by weight ascending (Kruskal's greedy choice)
    std::sort(candidateEdges.begin(), candidateEdges.end(),
              [](const auto& e1, const auto& e2) { return std::get<2>(e1) < std::get<2>(e2); });

    DSU dsu;
    for (int id : allNodeIds) dsu.makeSet(id);

    std::vector<std::tuple<int, int, double>> mst;
    for (const auto& e : candidateEdges) {
        int u = std::get<0>(e), v = std::get<1>(e);
        double w = std::get<2>(e);
        if (dsu.unite(u, v)) {
            mst.emplace_back(u, v, w);
            if (mst.size() == allNodeIds.size() - 1) break; // MST complete
        }
    }
    return mst;
}

// ================ Bellman-Ford Algorithm ================= //
bool RoadNetwork::bellmanFord(int source, const std::vector<int>& allNodeIds,
                               std::unordered_map<int, double>& distances) {
    const double INF = std::numeric_limits<double>::max();

    for (int id : allNodeIds) distances[id] = INF;
    distances[source] = 0.0;

    int V = (int)allNodeIds.size();

    // Relax all edges V-1 times
    for (int i = 0; i < V - 1; i++) {
        bool changed = false;
        for (const auto& kv : adjList) {
            int src = kv.first;
            if (distances[src] == INF) continue;
            for (const auto& edge : kv.second) {
                int dest = edge.first;
                double weight = edge.second;
                if (distances[src] + weight < distances[dest]) {
                    distances[dest] = distances[src] + weight;
                    changed = true;
                }
            }
        }
        if (!changed) break; // early exit: no more relaxations possible
    }

    // Check for negative-weight cycles
    for (const auto& kv : adjList) {
        int src = kv.first;
        if (distances[src] == INF) continue;
        for (const auto& edge : kv.second) {
            int dest = edge.first;
            double weight = edge.second;
            if (distances[src] + weight < distances[dest]) {
                return false; // negative cycle detected
            }
        }
    }

    return true;
}

// ================ Topological Sort (Kahn's Algorithm) ================= //
std::vector<int> RoadNetwork::topologicalSort(const std::vector<int>& allNodeIds) {
    std::unordered_map<int, int> inDegree;
    for (int id : allNodeIds) inDegree[id] = 0;

    for (const auto& kv : adjList) {
        for (const auto& edge : kv.second) {
            inDegree[edge.first]++;
        }
    }

    std::queue<int> q;
    for (int id : allNodeIds) {
        if (inDegree[id] == 0) q.push(id);
    }

    std::vector<int> order;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        order.push_back(u);

        if (adjList.count(u)) {
            for (const auto& edge : adjList[u]) {
                int v = edge.first;
                if (--inDegree[v] == 0) {
                    q.push(v);
                }
            }
        }
    }

    if (order.size() != allNodeIds.size()) {
        // Cycle detected: no valid topological order exists for one-way planning
        return {};
    }

    return order;
}
