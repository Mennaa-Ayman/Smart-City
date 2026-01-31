# Smart City Delivery & Traffic Management System

A comprehensive logistics platform implementing advanced algorithms and data structures for efficient package delivery route optimization across an urban network.

## Table of Contents
- [Implemented Features](#implemented-features)
- [Project Structure](#project-structure)
- [Core Algorithms](#core-algorithms)
- [Data Structures](#data-structures)
- [System Components](#system-components)
- [How to run](#how-to-run)

## Implemented Features

### 1. **Dijkstra's Shortest Path Algorithm**
- O((V+E)log V) complexity using priority queue
- Finds optimal routes between any two city locations
- Handles weighted directed graph with distance and traffic metrics
- Returns complete path from source to destination

### 2. **Graph Algorithms**
- **BFS (Breadth-First Search)**: Explores network connectivity and reachability
- **DFS (Depth-First Search)**: Analyzes network structure

### 3. **Custom Hash Table Implementation**
- Template-based hash table with separate chaining collision resolution
- O(1) average case lookup, insert, and delete operations
- Dynamic resizing at 0.75 load factor
- Stores entities: Locations, Vehicles, Deliveries, Traffic Events
- 10 locations, 10 vehicles, 15 deliveries, 16 traffic events indexed

### 4. **Spatial Indexing with QuadTree**
- 4-way geographic partitioning for efficient location queries
- O(log N) nearest-neighbor searches
- Radius search to find all locations within distance threshold
- Supports dynamic insertion and spatial range queries

### 5. **Priority Queue with Binary Heap**
- Multi-criteria sorting: deadline primary, priority level secondary
- Priority levels: High (3), Medium (2), Low (1)
- Efficient insertion and extraction operations
- Used for delivery scheduling with urgency handling

### 6. **Route Optimization Algorithms**
- **Activity Selection**: Greedy deadline-based delivery selection
- **Fractional Knapsack**: Profit ratio optimization for vehicle load balancing
- **Merge Sort**: Stable O(N log N) sorting by deadline and priority
- **Quick Sort**: Fast average O(N log N) in-place sorting
- **Binary Search**: O(log N) search in sorted delivery schedules
- **Closest Pair of Points**: Divide & Conquer O(N log N) geographic analysis
- **Zone Partitioning**: Recursive geographic divide & conquer splitting

### 7. **Delivery Scheduler**
- Automatic vehicle assignment to pending deliveries
- Capacity validation before assignment
- Route calculation between source and destination
- Real-time delivery management and tracking

### 8. **System Integration & Architecture**
- **SmartCityDeliverySystem**: Unified system orchestration class
- **SmartCitySystem.cpp/hpp**: Business logic implementation (300 lines)
- **Main.cpp**: Clean entry point (11 lines)
- 5-phase delivery optimization pipeline:
  1. Pathfinding demonstration (Dijkstra)
  2. Spatial queries (QuadTree radius search)
  3. Route optimization (sorting + activity selection)
  4. Vehicle assignment (greedy matching)
  5. System analytics and metrics reporting

## Project Structure
```
SmartCity/
├── CMakeLists.txt              # CMake build configuration
├── README.md                   # Project documentation
│
├── Data/                       # Input data files (CSV format)
│   ├── City_map.csv            
│   ├── locations.csv           
│   ├── vehicles.csv            
│   ├── deliveries.csv         
│   └── Traffic_updates.csv     
│
├── src/                        
│   ├── Main.cpp                # Program entry point
│   ├── SmartCitySystem.cpp     # System orchestration 
│   ├── Network.cpp             # Graph algorithms (Dijkstra, BFS, DFS)
│   ├── SpatialIndex.cpp        # QuadTree, BST, SegmentTree
│   ├── DeliveryScheduler.cpp   # Priority queue & assignment logic
│   ├── RouteOptimizer.cpp      # Optimization algorithms 
│   └── Utils.cpp               # CSV file I/O and utilities
│
├── include/                    
│   ├── SmartCitySystem.hpp
│   ├── Network.hpp
│   ├── SpatialIndex.hpp
│   ├── DeliveryScheduler.hpp
│   ├── RouteOptimizer.hpp
│   ├── HashTable.hpp           # Template hash table
│   └── Utils.hpp
│
└── build/                      # Generated build directory
```

## Core Algorithms

| Algorithm | Complexity | Purpose |
|-----------|-----------|---------|
| **Dijkstra's Algorithm** | O((V+E) log V) | Find shortest path between locations |
| **BFS** | O(V+E) | Network connectivity analysis |
| **DFS** | O(V+E) | Network structure exploration |
| **Binary Heap (Priority Queue)** | O(log N) | Delivery scheduling by priority |
| **QuadTree** | O(log N) | Spatial location indexing & queries |
| **Merge Sort** | O(N log N) | Stable multi-criteria sorting |
| **Quick Sort** | O(N log N) avg | Fast in-place sorting |
| **Binary Search** | O(log N) | Search sorted schedules |
| **Closest Pair (D&C)** | O(N log N) | Find nearest delivery locations |
| **Activity Selection** | O(N log N) | Greedy deadline-based selection |
| **Fractional Knapsack** | O(N) | Profit ratio optimization |
| **Zone Partitioning (D&C)** | O(N log N) | Geographic region splitting |

## Data Structures

### Adjacency List Graph
```cpp
unordered_map<int, vector<pair<int, double>>> adjList;
// Weighted directed graph for road network
// Dijkstra, BFS, DFS implementations
```

### Custom Hash Table (Template-based)
```cpp
template<typename K, typename V>
class HashTable {
    vector<list<pair<K, V>>> table;  // Separate chaining
    // O(1) avg: insert, search, delete
};
```

### QuadTree (Spatial Indexing)
```cpp
struct QuadTreeNode {
    double x, y, width, height;
    vector<Location*> locations;
    QuadTreeNode* children[4];  // NW, NE, SW, SE
};
```

### Priority Queue (Binary Heap)
```cpp
template<typename T>
class PriorityQueue {
    vector<T> heap;
    bool (*comparator)(T, T);  // Custom comparator
};
```

## System Components

| Component | Files | Purpose |
|-----------|-------|---------|
| **Network Manager** | Network.hpp/cpp | Graph algorithms, road network |
| **Spatial Indexing** | SpatialIndex.hpp/cpp | QuadTree, nearest neighbor, radius search |
| **Hash Table Registry** | HashTable.hpp | O(1) entity lookup (locations, vehicles, deliveries) |
| **Delivery Scheduler** | DeliveryScheduler.hpp/cpp | Priority queue, vehicle assignment |
| **Route Optimizer** | RouteOptimizer.hpp/cpp | All 8 optimization algorithms |
| **System Integration** | SmartCitySystem.hpp/cpp | 5-phase delivery pipeline orchestration |
| **Utilities** | Utils.hpp/cpp | CSV loading, calculations |

## How to run

### Prerequisites
- **C++ Compiler**: C++17 or higher 
- **CMake**: Version 3.10 or higher

### Build 
```bash
cd d:\Software\Advanced\SmartCity
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Run

```bash
cd d:\Software\Advanced\SmartCity
.\SmartCity.exe
```




