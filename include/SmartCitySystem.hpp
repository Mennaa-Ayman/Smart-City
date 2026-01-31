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
    
    // Demonstration methods
    void runDeliveryOptimization();
    void demonstratePathfinding();
    void demonstrateSpatialQueries();
    void demonstrateClosestPairAnalysis();
    void displayAnalytics();
};

#endif // SMARTCITYSYSTEM_HPP
