#include "SmartCitySystem.hpp"
#include <iomanip>
#include <algorithm>
#include <limits>

SmartCityDeliverySystem::SmartCityDeliverySystem() 
    : spatialIndex(nullptr), trafficSegmentTree(nullptr), scheduler(nullptr) {}

SmartCityDeliverySystem::~SmartCityDeliverySystem() {
    if (spatialIndex) delete spatialIndex;
    if (trafficSegmentTree) delete trafficSegmentTree;
    if (scheduler) delete scheduler;
}

std::vector<int> SmartCityDeliverySystem::getAllLocationIds() const {
    std::vector<int> ids;
    for (const auto& loc : locations) ids.push_back(loc.id);
    return ids;
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
    std::cout << " Vehicle Registry: " << vehicleRegistry.size() << " entries" << std::endl;
    
    // Build delivery registry
    for (const auto& delivery : deliveries) {
        deliveryRegistry.insert(delivery.id, delivery);
    }
    std::cout << "  Delivery Registry: " << deliveryRegistry.size() << " entries" << std::endl;
    
    // Build traffic registry
    for (const auto& event : trafficEvents) {
        trafficRegistry.insert(event.road_id, event);
    }
    std::cout << "  Traffic Registry: " << trafficRegistry.size() << " entries" << std::endl;
    
    // Build QuadTree for geographic queries (assuming coords in -90 to 90, -180 to 180)
    spatialIndex = new QuadTree(0, 0, 180, 360);
    for (auto& loc : locations) {
        spatialIndex->insert(&loc);
    }
    std::cout << "  QuadTree Index: Built for " << locations.size() << " locations" << std::endl;
    
    // Build BST for deadline-based queries
    for (const auto& delivery : deliveries) {
        deliveryDeadlineTree.insert(delivery);
    }
    std::cout << "  Deadline BST: Built for " << deliveries.size() << " deliveries" << std::endl;
    
    // Build Segment Tree for traffic density range queries, indexed by (road_id - 1)
    std::vector<int> trafficDensities;
    if (!trafficEvents.empty()) {
        int maxRoadId = 0;
        for (const auto& event : trafficEvents) {
            if (event.road_id > maxRoadId) maxRoadId = event.road_id;
        }
        trafficDensities.assign(maxRoadId, 0);
        for (const auto& event : trafficEvents) {
            trafficDensities[event.road_id - 1] = event.new_weight;
        }
    }
    trafficSegmentTree = new SegmentTree(trafficDensities);
    std::cout << "  ✓ Traffic Segment Tree: Built for " << trafficDensities.size() << " road segments" << std::endl;
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
    
    // Phase 4: Assign deliveries to vehicles (via the Priority Queue / binary heap)
    std::cout << "\n  Phase 4: Vehicle Assignment..." << std::endl;
    
    // Push every selected delivery into the scheduler's priority queue first --
    // assignment always pulls the current highest-priority delivery off the heap.
    for (const auto& d : selectedDeliveries) {
        scheduler->addDelivery(d);
    }
    
    int assignedCount = 0;
    size_t vehicleIdx = 0;
    int totalToAssign = (int)selectedDeliveries.size();
    int attempts = 0;
    int maxAttempts = totalToAssign * 2; // guard against looping if a delivery can't be placed
    
    while (scheduler->getPendingCount() > 0 && vehicleIdx < vehicles.size() && attempts < maxAttempts) {
        bool assigned = scheduler->assignDeliveryToVehicle(
            vehicles[vehicleIdx].id, vehicles, locations
        );
        if (assigned) assignedCount++;
        vehicleIdx++;
        attempts++;
    }
    std::cout << "    ✓ Assigned " << assignedCount << "/" << totalToAssign 
              << " deliveries to vehicles (priority order: deadline + urgency)" << std::endl;
    
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

void SmartCityDeliverySystem::demonstrateGraphTraversal() {
    std::cout << "\n[GRAPH TRAVERSAL: BFS & DFS]" << std::endl;
    
    if (locations.empty()) {
        std::cout << "  No locations to traverse" << std::endl;
        return;
    }
    
    Node startNode;
    startNode.location = locations[0];
    
    std::vector<int> bfsOrder = roadNetwork.bfs(&startNode);
    std::cout << "  BFS from " << locations[0].name << " reached " << bfsOrder.size() << " nodes: ";
    for (size_t i = 0; i < bfsOrder.size(); i++) {
        Location loc;
        if (locationRegistry.find(bfsOrder[i], loc)) std::cout << loc.name;
        if (i + 1 < bfsOrder.size()) std::cout << " -> ";
    }
    std::cout << std::endl;
    
    std::vector<int> dfsOrder = roadNetwork.dfs(&startNode);
    std::cout << "  DFS from " << locations[0].name << " reached " << dfsOrder.size() << " nodes: ";
    for (size_t i = 0; i < dfsOrder.size(); i++) {
        Location loc;
        if (locationRegistry.find(dfsOrder[i], loc)) std::cout << loc.name;
        if (i + 1 < dfsOrder.size()) std::cout << " -> ";
    }
    std::cout << std::endl;
}

void SmartCityDeliverySystem::demonstrateMST() {
    std::cout << "\n[MINIMUM SPANNING TREE: KRUSKAL'S ALGORITHM]" << std::endl;
    
    std::vector<int> allIds = getAllLocationIds();
    if (allIds.size() < 2) {
        std::cout << "  Not enough locations for infrastructure planning" << std::endl;
        return;
    }
    
    auto mst = roadNetwork.kruskalMST(allIds);
    double totalCost = 0.0;
    
    std::cout << "  Minimum-cost road backbone connecting " << allIds.size() 
              << " locations (" << mst.size() << " roads):" << std::endl;
    for (const auto& edge : mst) {
        int u = std::get<0>(edge), v = std::get<1>(edge);
        double w = std::get<2>(edge);
        totalCost += w;
        
        Location locU, locV;
        locationRegistry.find(u, locU);
        locationRegistry.find(v, locV);
        std::cout << "    " << locU.name << " -- " << locV.name 
                  << " (cost " << w << ")" << std::endl;
    }
    std::cout << "    Total infrastructure cost: " << totalCost << std::endl;
}

void SmartCityDeliverySystem::demonstrateBellmanFord() {
    std::cout << "\n[BELLMAN-FORD: TOLL-DISCOUNT (NEGATIVE-WEIGHT) ROUTING]" << std::endl;
    
    if (locations.size() < 2) {
        std::cout << "  Not enough locations for routing" << std::endl;
        return;
    }
    
    int sourceId = locations[0].id;
    int discountTargetId = locations[1].id;
    std::vector<int> allIds = getAllLocationIds();
    
    // Capture the current weight so it can be restored after the demo
    double originalWeight = -1.0;
    bool edgeExists = false;
    for (const auto& e : roadNetwork.getAllEdges()) {
        if (std::get<0>(e) == sourceId && std::get<1>(e) == discountTargetId) {
            originalWeight = std::get<2>(e);
            edgeExists = true;
            break;
        }
    }
    
    if (!edgeExists) {
        std::cout << "  No direct road between " << locations[0].name << " and " 
                  << locations[1].name << " to apply a discount to" << std::endl;
        return;
    }
    
    // Simulate a toll discount that makes this road cheaper than free (negative weight)
    roadNetwork.updateEdgeWeight(sourceId, discountTargetId, -2.0);
    std::cout << "  Applying toll discount: " << locations[0].name << " -> " 
              << locations[1].name << " temporarily weighted -2.0" << std::endl;
    
    std::unordered_map<int, double> distances;
    bool noNegativeCycle = roadNetwork.bellmanFord(sourceId, allIds, distances);
    
    if (!noNegativeCycle) {
        std::cout << "  Negative-weight cycle detected -- routing is unreliable!" << std::endl;
    } else {
        std::cout << "  Shortest distances from " << locations[0].name << " (with discount):" << std::endl;
        for (int id : allIds) {
            Location loc;
            if (!locationRegistry.find(id, loc)) continue;
            if (distances[id] == std::numeric_limits<double>::max()) {
                std::cout << "    " << loc.name << ": unreachable" << std::endl;
            } else {
                std::cout << "    " << loc.name << ": " << distances[id] << std::endl;
            }
        }
    }
    
    // Revert the discount so later demonstrations see normal traffic weights
    roadNetwork.updateEdgeWeight(sourceId, discountTargetId, originalWeight);
    std::cout << "  (Discount reverted, road weight restored to " << originalWeight << ")" << std::endl;
}

void SmartCityDeliverySystem::demonstrateTopologicalSort() {
    std::cout << "\n[TOPOLOGICAL SORT: ONE-WAY STREET PLANNING]" << std::endl;
    
    std::vector<int> allIds = getAllLocationIds();
    auto order = roadNetwork.topologicalSort(allIds);
    
    if (order.empty()) {
        std::cout << "  Road network contains a cycle (loop roads) -- no valid one-way" << std::endl;
        std::cout << "  ordering exists without closing at least one road" << std::endl;
    } else {
        std::cout << "  Valid one-way street ordering: ";
        for (size_t i = 0; i < order.size(); i++) {
            Location loc;
            if (locationRegistry.find(order[i], loc)) std::cout << loc.name;
            if (i + 1 < order.size()) std::cout << " -> ";
        }
        std::cout << std::endl;
    }
}

void SmartCityDeliverySystem::demonstrateDeadlineQueries() {
    std::cout << "\n[DEADLINE BST: URGENCY QUERIES]" << std::endl;
    
    Delivery* earliest = deliveryDeadlineTree.getEarliestDeadline();
    if (earliest) {
        std::cout << "  Most urgent delivery: " << earliest->id 
                  << " (deadline " << earliest->deadline 
                  << ", priority " << earliest->priority << ")" << std::endl;
    } else {
        std::cout << "  No deliveries in the tree" << std::endl;
        return;
    }
    
    // Range search across the earlier half of all deadlines
    std::vector<std::string> deadlines;
    for (const auto& d : deliveries) deadlines.push_back(d.deadline);
    std::sort(deadlines.begin(), deadlines.end());
    
    std::string start = deadlines.front();
    std::string end = deadlines[deadlines.size() / 2];
    
    auto inRange = deliveryDeadlineTree.rangeSearch(start, end);
    std::cout << "  Deliveries due between " << start << " and " << end 
              << ": " << inRange.size() << std::endl;
}

void SmartCityDeliverySystem::demonstrateTrafficAnalysis() {
    std::cout << "\n[SEGMENT TREE: TRAFFIC DENSITY RANGE QUERIES]" << std::endl;
    
    if (!trafficSegmentTree || trafficEvents.empty()) {
        std::cout << "  No traffic data available" << std::endl;
        return;
    }
    
    int maxRoadId = 0;
    for (const auto& event : trafficEvents) {
        if (event.road_id > maxRoadId) maxRoadId = event.road_id;
    }
    int left = 0, right = maxRoadId - 1;
    
    std::cout << "  Across all " << maxRoadId << " monitored roads:" << std::endl;
    std::cout << "    Max congestion:     " << trafficSegmentTree->queryMax(left, right) << std::endl;
    std::cout << "    Min congestion:     " << trafficSegmentTree->queryMin(left, right) << std::endl;
    std::cout << "    Average congestion: " << trafficSegmentTree->queryAverage(left, right) << std::endl;
    
    int mid = right / 2;
    std::cout << "  First half of roads (index 0-" << mid << "):" << std::endl;
    std::cout << "    Max: " << trafficSegmentTree->queryMax(0, mid) 
              << "  Average: " << trafficSegmentTree->queryAverage(0, mid) << std::endl;
    
    // Simulate a live congestion spike and re-query
    trafficSegmentTree->updateTraffic(0, 99);
    std::cout << "  After a simulated congestion spike on road index 0:" << std::endl;
    std::cout << "    New max congestion: " << trafficSegmentTree->queryMax(left, right) << std::endl;
}

void SmartCityDeliverySystem::demonstrateSortingAndSearch() {
    std::cout << "\n[QUICK SORT & BINARY SEARCH]" << std::endl;
    
    if (deliveries.empty()) {
        std::cout << "  No deliveries to sort/search" << std::endl;
        return;
    }
    
    // Quick Sort: sort deliveries by deadline & priority (in-place, divide & conquer)
    std::vector<Delivery> quickSorted = deliveries;
    routeOptimizer.quickSort(quickSorted);
    std::cout << "  Quick Sort: sorted " << quickSorted.size() 
              << " deliveries by deadline & priority" << std::endl;
    std::cout << "    Most urgent after sort: " << quickSorted.front().id 
              << " (deadline " << quickSorted.front().deadline << ")" << std::endl;
    
    // Binary Search requires the array sorted by the search key (delivery ID),
    // so a separate ID-sorted copy is built for this lookup.
    std::vector<Delivery> idSorted = deliveries;
    std::sort(idSorted.begin(), idSorted.end(),
              [](const Delivery& a, const Delivery& b) { return a.id < b.id; });
    
    std::string targetId = idSorted[idSorted.size() / 2].id;
    int index = routeOptimizer.binarySearch(idSorted, targetId);
    if (index != -1) {
        std::cout << "  Binary Search: found delivery " << targetId 
                  << " at index " << index << " in the ID-sorted list" << std::endl;
    } else {
        std::cout << "  Binary Search: delivery " << targetId << " not found" << std::endl;
    }
}

void SmartCityDeliverySystem::demonstrateGreedyAndKnapsack() {
    std::cout << "\n[GREEDY ROUTE SELECTION & FRACTIONAL KNAPSACK]" << std::endl;
    
    if (locations.size() < 2) {
        std::cout << "  Not enough locations for a greedy route" << std::endl;
        return;
    }
    
    // Build an adjacency map from the graph's edges for greedy next-hop selection
    std::unordered_map<int, std::vector<std::pair<int, double>>> graph;
    for (const auto& e : roadNetwork.getAllEdges()) {
        graph[std::get<0>(e)].emplace_back(std::get<1>(e), std::get<2>(e));
    }
    
    int current = locations[0].id;
    std::vector<int> unvisited;
    for (size_t i = 1; i < locations.size(); i++) unvisited.push_back(locations[i].id);
    
    std::cout << "  Greedy nearest-neighbor route from " << locations[0].name << ": " << locations[0].name;
    int steps = 0;
    while (!unvisited.empty() && steps < (int)locations.size()) {
        int next = routeOptimizer.greedyNextDestination(current, unvisited, graph);
        if (next == -1) break;
        
        Location loc;
        if (locationRegistry.find(next, loc)) std::cout << " -> " << loc.name;
        unvisited.erase(std::remove(unvisited.begin(), unvisited.end(), next), unvisited.end());
        current = next;
        steps++;
    }
    std::cout << std::endl;
    
    // Fractional Knapsack: pack deliveries into the first vehicle by weight capacity
    if (!deliveries.empty() && !vehicles.empty()) {
        std::vector<DeliveryItem> items;
        for (const auto& d : deliveries) {
            double value = (d.priority == "High") ? 3.0 : (d.priority == "Medium" ? 2.0 : 1.0);
            items.push_back({d.id, d.weight, value});
        }
        
        double capacity = vehicles[0].capacity;
        std::vector<double> selectedAmounts;
        double totalValue = routeOptimizer.fractionalKnapsack(items, capacity, selectedAmounts);
        
        int loadedCount = 0;
        for (double amount : selectedAmounts) if (amount > 0) loadedCount++;
        
        std::cout << "  Fractional Knapsack: loading vehicle " << vehicles[0].id 
                  << " (capacity " << capacity << " kg)" << std::endl;
        std::cout << "    Deliveries contributing to load: " << loadedCount << "/" << items.size() << std::endl;
        std::cout << "    Total priority value packed: " << totalValue << std::endl;
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
    
    // Graph algorithms
    demonstrateGraphTraversal();       // BFS / DFS
    demonstratePathfinding();          // Dijkstra
    demonstrateMST();                  // Kruskal's Algorithm
    demonstrateBellmanFord();          // Negative-weight / toll-discount routing
    demonstrateTopologicalSort();      // One-way street planning
    
    // Spatial indexing
    demonstrateSpatialQueries();       // QuadTree
    demonstrateDeadlineQueries();      // BST
    demonstrateTrafficAnalysis();      // Segment Tree
    
    // Divide & conquer
    demonstrateClosestPairAnalysis();  // Closest Pair of Points
    demonstrateSortingAndSearch();     // Quick Sort + Binary Search
    
    // Greedy algorithms
    demonstrateGreedyAndKnapsack();    // Greedy next-hop + Fractional Knapsack
    
    // Full integrated pipeline: Merge Sort, Activity Selection, Zone
    // Partitioning, Priority Queue-based vehicle assignment
    runDeliveryOptimization();
    
    displayAnalytics();
    
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "   SYSTEM EXECUTION COMPLETE" << std::endl;
    std::cout << std::string(70, '=') << "\n" << std::endl;
}
