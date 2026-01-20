# Smart City System

This project simulates a real-world logistics and traffic coordination platform. The system manages package deliveries across an urban network by optimizing delivery routes, handling dynamic traffic conditions, and coordinating multiple delivery vehicles in real-time.

## Table of Contents
- [System Components](#system-components)
- [Features and Algorithms Integration](#features-and-algorithms-integration)
- [Project Structure](#project-structure)
- [How to Run](#how-to-run)

## System Components
### 1.City Map Manager 
- Load city graph from file
- Add/remove roads (edges) 
- Update traffic conditions (edge weights)
- Find shortest paths between any two points 
- Detect road closures and suggest alternatives

### 2. Location & Entity Manager
- Register new locations, vehicles, customers
- Fast O(1) lookup operations 
- Update entity information 
- Generate unique IDs with hash distribution

### 3. Spatial Query Engine
- Build QuadTree/KD-Tree from location coordinates
- Find all locations within radius 
- Nearest neighbor search for vehicle assignment 
- Range queries for delivery zones

### 4. Delivery Scheduler
- Maintain priority queue of pending deliveries 
- Assign deliveries to vehicles based on urgency 
- Handle real-time delivery requests
- Reorder queue when priorities change 

### 5. Route Optimizer
- Generate optimal routes for vehicles 
- Select most profitable deliveries first 
- Balance load across vehicles 
- Minimize empty travel distance 

### 6.Data Processing Module
- Sort and filter large datasets 
- Partition delivery zones geographically 
- Merge sorted results from different vehicles 
- Binary search in sorted schedules

## Features and Algorithms Integration
### 1. Graph Algorithms
- **Dijkstra's Algorithm**: Find shortest paths between locations O(V log V)
- **Breadth-First Search (BFS)**: Explore graph connectivity and reachability
- **Depth-First Search (DFS)**: Detect cycles and strongly connected components
- **Bellman-Ford**: Handle negative weights (e.g., toll discounts)
- **Topological Sort**: Plan one-way street networks

### 2. Hash-based Lookups
- O(1) average-case vehicle and location searches
- Hash map implementation for fast entity registration
- Collision handling with chaining or open addressing
- Unique ID generation with hash distribution

### 3. Priority Queues
- Binary heap-based delivery scheduling
- Dynamic priority updates for urgent deliveries
- O(log n) insertion and extraction
- Handles real-time delivery request reordering

### 4. Greedy Optimization
- Vehicle assignment based on proximity and capacity
- Delivery selection by urgency and profitability
- Load balancing across fleet
- Minimize empty travel distance

### 5. Spatial Tree Structures
- **QuadTree/KD-Tree** for 2D location indexing
- Nearest neighbor search for vehicle assignments
- Range queries for delivery zone analysis
- O(log n) spatial lookups

### 6. Sorting and Searching
- **Merge Sort**: Sort deliveries by deadline or priority O(n log n)
- **Binary Search**: Find schedule positions in sorted delivery lists
- **Divide-and-Conquer**: Partition delivery zones geographically
- Filter and merge sorted results from different vehicles  


## Project Structure 
```
SmartCity/
├── src/                              
│   ├── Network.cpp                   # Dijkstra, BFS/DFS, Bellman-Ford
│   ├── HashTable.cpp                 # Hash map implementation
│   └── Main.cpp                      # Main application entry
|
├── include/                         
│   ├── Network.hpp                   # Road network graph & pathfinding
│   ├── HashTable.hpp                 # Fast O(1) lookups for entities
│
├── Data/                             
│   ├── city_map.csv                  # Road network: source, destination, distance, traffic_time
│   ├── locations.csv                 # Locations: id, name, latitude, longitude, type
│   ├── vehicles.csv                  # Fleet: vehicle_id, capacity, speed, location, status
│   ├── deliveries.csv                # Requests: id, source, dest, weight, deadline, priority
│   └── traffic_updates.csv           # Dynamic: road_id, new_weight, timestamp, congestion
│
├── CMakeLists.txt                    # CMake build configuration
└── README.md                         # Project documentation
```


## How to Run
### Prerequisites
- C++17
- CMake 3.10+

### Quick Start
**1. Create a build directory and generate build files**
```
mkdir build
cd build
cmake ..
```

**2. Build the project**
```
cmake --build . --config Release
```

**3. Run the executable**
```
cd ..
.\SmartCity.exe
```

