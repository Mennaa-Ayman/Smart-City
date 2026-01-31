#include "SmartCitySystem.hpp"
#include <iomanip>

SmartCityDeliverySystem::SmartCityDeliverySystem() 
    : spatialIndex(nullptr), scheduler(nullptr) {}

SmartCityDeliverySystem::~SmartCityDeliverySystem() {
    if (spatialIndex) delete spatialIndex;
    if (scheduler) delete scheduler;
}

void SmartCityDeliverySystem::initialize() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "   SMART CITY DELIVERY & TRAFFIC MANAGEMENT SYSTEM" << std::endl;
    std::cout << std::string(70, '=') << "\n" << std::endl;
    
    // Load all data
    loadData();
    
    // Initialize spatial indexing
    initializeSpatialIndex();
    
    // Initialize scheduler
    scheduler = new DeliveryScheduler(&roadNetwork, spatialIndex);
    
    // Build the road network graph
    buildRoadNetwork();
}

void SmartCityDeliverySystem::loadData() {
    std::cout << "[LOADING DATA]" << std::endl;
    
    locations = FileUtils::loadLocations();
    vehicles = FileUtils::loadVehicles();
    deliveries = FileUtils::loadDeliveries();
    trafficEvents = FileUtils::loadTrafficUpdates();
    
    std::cout << "  ✓ Locations: " << locations.size() << std::endl;
    std::cout << "  ✓ Vehicles: " << vehicles.size() << std::endl;
    std::cout << "  ✓ Deliveries: " << deliveries.size() << std::endl;
    std::cout << "  ✓ Traffic Events: " << trafficEvents.size() << std::endl;
}

void SmartCityDeliverySystem::initializeSpatialIndex() {
    std::cout << "\n[INITIALIZING SPATIAL INDEXING]" << std::endl;
    
    // Build location registry (O(1) lookup)
    for (const auto& loc : locations) {
        locationRegistry.insert(loc.id, loc);
    }
    std::cout << "  ✓ Location Registry: " << locationRegistry.size() << " entries" << std::endl;
    
    // Build vehicle registry
    for (const auto& vehicle : vehicles) {
        vehicleRegistry.insert(vehicle.id, vehicle);
    }
    std::cout << "  ✓ Vehicle Registry: " << vehicleRegistry.size() << " entries" << std::endl;
    
    // Build delivery registry
    for (const auto& delivery : deliveries) {
        deliveryRegistry.insert(delivery.id, delivery);
    }
    std::cout << "  ✓ Delivery Registry: " << deliveryRegistry.size() << " entries" << std::endl;
    
    // Build traffic registry
    for (const auto& event : trafficEvents) {
        trafficRegistry.insert(event.road_id, event);
    }
    std::cout << "  ✓ Traffic Registry: " << trafficRegistry.size() << " entries" << std::endl;
    
    // Build QuadTree for geographic queries (assuming coords in -90 to 90, -180 to 180)
    spatialIndex = new QuadTree(0, 0, 180, 360);
    for (auto& loc : locations) {
        spatialIndex->insert(&loc);
    }
    std::cout << "  ✓ QuadTree Index: Built for " << locations.size() << " locations" << std::endl;
    
    // Build BST for deadline-based queries
    for (const auto& delivery : deliveries) {
        deliveryDeadlineTree.insert(delivery);
    }
    std::cout << "  ✓ Deadline BST: Built for " << deliveries.size() << " deliveries" << std::endl;
}

void SmartCityDeliverySystem::buildRoadNetwork() {
    std::cout << "\n[BUILDING ROAD NETWORK GRAPH]" << std::endl;
    
    auto cityMap = FileUtils::loadCityMap();
    
    // Add edges to graph
    for (const auto& edge : cityMap) {
        int srcId = roadNetwork.getLocationIdByName(edge.source, locations);
        int destId = roadNetwork.getLocationIdByName(edge.destination, locations);
        
        if (srcId != -1 && destId != -1) {
            // Use weighted average of distance and traffic time
            double weight = (edge.distance + edge.traffic_time) / 2.0;
            roadNetwork.addEdge(srcId, destId, weight);
        }
    }
    
    std::cout << "  ✓ Road Network: " << cityMap.size() << " roads added" << std::endl;
}

void SmartCityDeliverySystem::runDeliveryOptimization() {
    std::cout << "\n[DELIVERY OPTIMIZATION & ASSIGNMENT]" << std::endl;
    
    if (deliveries.empty()) {
        std::cout << "  No deliveries to process" << std::endl;
        return;
    }
    
    // Phase 1: Sort deliveries using merge sort (by deadline and priority)
    std::cout << "\n  Phase 1: Sorting Deliveries..." << std::endl;
    std::vector<Delivery> sortedDeliveries = deliveries;
    sortedDeliveries = routeOptimizer.mergeSort(sortedDeliveries);
    std::cout << "    ✓ Sorted " << sortedDeliveries.size() << " deliveries by deadline & priority" << std::endl;
    
    // Phase 2: Activity Selection - select non-conflicting deliveries
    std::cout << "\n  Phase 2: Activity Selection..." << std::endl;
    std::vector<Delivery> selectedDeliveries = routeOptimizer.activitySelection(sortedDeliveries);
    std::cout << "    ✓ Selected " << selectedDeliveries.size() << " non-overlapping deliveries" << std::endl;
    
    // Phase 3: Partition delivery zones using divide & conquer
    std::cout << "\n  Phase 3: Geographic Zone Partitioning..." << std::endl;
    int numZones = (vehicles.size() > 0) ? vehicles.size() : 1;
    auto zones = routeOptimizer.partitionDeliveryZones(locations, numZones);
    std::cout << "    ✓ Partitioned " << locations.size() << " locations into " << zones.size() << " zones" << std::endl;
    
    // Phase 4: Assign deliveries to vehicles
    std::cout << "\n  Phase 4: Vehicle Assignment..." << std::endl;
    int assignedCount = 0;
    for (size_t i = 0; i < selectedDeliveries.size() && i < vehicles.size(); i++) {
        bool assigned = scheduler->assignDeliveryToVehicle(
            selectedDeliveries[i].id,
            vehicles[i].id,
            vehicles,
            locations
        );
        if (assigned) assignedCount++;
    }
    std::cout << "    ✓ Assigned " << assignedCount << "/" << selectedDeliveries.size() 
              << " deliveries to vehicles" << std::endl;
    
    // Phase 5: Display pending deliveries
    std::cout << "\n  Phase 5: Pending Deliveries Summary..." << std::endl;
    scheduler->displayPendingDeliveries();
}

void SmartCityDeliverySystem::demonstratePathfinding() {
    std::cout << "\n[PATHFINDING DEMONSTRATION]" << std::endl;
    
    if (locations.size() < 2) {
        std::cout << "  Insufficient locations for pathfinding" << std::endl;
        return;
    }
    
    // Find shortest path between first two locations
    int srcId = locations[0].id;
    int destId = locations[1].id;
    
    std::cout << "  Finding shortest path from " << locations[0].name 
              << " to " << locations[1].name << "..." << std::endl;
    
    std::vector<int> path = roadNetwork.shortestPath(srcId, destId);
    
    if (!path.empty()) {
        std::cout << "    ✓ Path found with " << path.size() << " nodes: ";
        for (size_t i = 0; i < path.size(); i++) {
            Location loc;
            if (locationRegistry.find(path[i], loc)) {
                std::cout << loc.name;
                if (i < path.size() - 1) std::cout << " → ";
            }
        }
        std::cout << std::endl;
    } else {
        std::cout << "    ✗ No path found" << std::endl;
    }
}

void SmartCityDeliverySystem::demonstrateSpatialQueries() {
    std::cout << "\n[SPATIAL QUERY DEMONSTRATION]" << std::endl;
    
    if (locations.empty()) return;
    
    // Radius search
    double searchX = locations[0].longitude;
    double searchY = locations[0].latitude;
    double radius = 10.0;
    
    std::cout << "  Radius search at (" << searchX << ", " << searchY << ") with radius " 
              << radius << "..." << std::endl;
    
    auto nearby = spatialIndex->radiusSearch(searchX, searchY, radius);
    std::cout << "    ✓ Found " << nearby.size() << " locations within radius" << std::endl;
    
    // Nearest neighbor search
    Location* nearest = spatialIndex->findNearest(searchX, searchY);
    if (nearest) {
        std::cout << "  Nearest location: " << nearest->name << std::endl;
    }
}

void SmartCityDeliverySystem::demonstrateClosestPairAnalysis() {
    std::cout << "\n[CLOSEST PAIR ANALYSIS]" << std::endl;
    
    if (locations.size() < 2) {
        std::cout << "  Insufficient locations for analysis" << std::endl;
        return;
    }
    
    auto [loc1, loc2] = routeOptimizer.closestPairOfPoints(locations);
    
    if (loc1.id != 0 || loc2.id != 0) {
        double dist = routeOptimizer.distanceBetweenPoints(
            loc1.longitude, loc1.latitude,
            loc2.longitude, loc2.latitude
        );
        std::cout << "  Closest pair of delivery locations:" << std::endl;
        std::cout << "    " << loc1.name << " ↔ " << loc2.name << std::endl;
        std::cout << "    Distance: " << std::fixed << std::setprecision(4) << dist << " units" << std::endl;
    }
}

void SmartCityDeliverySystem::displayAnalytics() {
    std::cout << "\n[SYSTEM ANALYTICS & METRICS]" << std::endl;
    
    std::cout << "\n  Fleet Statistics:" << std::endl;
    std::cout << "    Total Vehicles: " << vehicles.size() << std::endl;
    double totalCapacity = 0;
    for (const auto& v : vehicles) {
        totalCapacity += v.capacity;
    }
    std::cout << "    Total Capacity: " << totalCapacity << " kg" << std::endl;
    std::cout << "    Average Capacity: " << (vehicles.empty() ? 0 : totalCapacity / vehicles.size()) 
              << " kg/vehicle" << std::endl;
    
    std::cout << "\n  Delivery Statistics:" << std::endl;
    std::cout << "    Total Deliveries: " << deliveries.size() << std::endl;
    double totalWeight = 0;
    int highPriority = 0;
    for (const auto& d : deliveries) {
        totalWeight += d.weight;
        if (d.priority == "High") highPriority++;
    }
    std::cout << "    Total Weight: " << totalWeight << " kg" << std::endl;
    std::cout << "    High Priority: " << highPriority << std::endl;
    std::cout << "    Average Weight: " << (deliveries.empty() ? 0 : totalWeight / deliveries.size()) 
              << " kg/delivery" << std::endl;
    
    std::cout << "\n  Network Statistics:" << std::endl;
    std::cout << "    Total Locations: " << locations.size() << std::endl;
    std::cout << "    Location Registry Size: " << locationRegistry.size() << std::endl;
    
    std::cout << "\n  Pending Deliveries:" << std::endl;
    std::cout << "    Count: " << scheduler->getPendingCount() << std::endl;
}

void SmartCityDeliverySystem::runFullSystem() {
    initialize();
    demonstratePathfinding();
    demonstrateSpatialQueries();
    demonstrateClosestPairAnalysis();
    runDeliveryOptimization();
    displayAnalytics();
    
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "   SYSTEM EXECUTION COMPLETE" << std::endl;
    std::cout << std::string(70, '=') << "\n" << std::endl;
}
