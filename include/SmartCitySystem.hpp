/* =====================================================================
Smart City Delivery & Traffic Management System
Integrated platform for logistics and traffic coordination

Components:
- Graph algorithms (Dijkstra, BFS, DFS)
- Hash tables for fast lookups
- Spatial indexing (QuadTree, BST, SegmentTree)
- Route optimization (Greedy, D&C, Sorting)
- Priority queue scheduling
- Real-time analytics
===================================================================== */

#ifndef SMARTCITYSYSTEM_HPP
#define SMARTCITYSYSTEM_HPP

#include "Utils.hpp"
#include "Network.hpp"
#include "HashTable.hpp"
#include "SpatialIndex.hpp"
#include "DeliveryScheduler.hpp"
#include "RouteOptimizer.hpp"
#include <vector>
#include <iostream>

class SmartCityDeliverySystem {
private:
    RoadNetwork roadNetwork;
    RouteOptimizer routeOptimizer;
    
    // Hash tables for fast entity lookup
    HashTable<int, Location> locationRegistry;
    HashTable<int, Vehicle> vehicleRegistry;
    HashTable<std::string, Delivery> deliveryRegistry;
    HashTable<int, TrafficEvent> trafficRegistry;
    
    // Spatial indexing
    QuadTree* spatialIndex;
    BST deliveryDeadlineTree;
    SegmentTree* trafficSegmentTree;
    
    // Delivery management
    DeliveryScheduler* scheduler;
    
    // Data storage
    std::vector<Location> locations;
    std::vector<Vehicle> vehicles;
    std::vector<Delivery> deliveries;
    std::vector<TrafficEvent> trafficEvents;
    
public:
    SmartCityDeliverySystem();
    ~SmartCityDeliverySystem();
    
    // Initialize the entire system
    void initialize();
    
    // Run the full system demonstration
    void runFullSystem();
    
private:
    // Helper initialization methods
    void loadData();
    void initializeSpatialIndex();
    void buildRoadNetwork();
    std::vector<int> getAllLocationIds() const;
    
    // Demonstration methods
    void runDeliveryOptimization();
    void demonstratePathfinding();
    void demonstrateGraphTraversal();        // BFS / DFS
    void demonstrateMST();                   // Kruskal's Algorithm
    void demonstrateBellmanFord();           // Toll-discount / negative-weight routing
    void demonstrateTopologicalSort();       // One-way street planning
    void demonstrateSpatialQueries();
    void demonstrateDeadlineQueries();       // BST earliest-deadline / range search
    void demonstrateTrafficAnalysis();       // Segment Tree range queries
    void demonstrateClosestPairAnalysis();
    void demonstrateSortingAndSearch();      // Quick Sort + Binary Search
    void demonstrateGreedyAndKnapsack();     // Greedy next-hop + Fractional Knapsack
    void displayAnalytics();
};

#endif // SMARTCITYSYSTEM_HPP
