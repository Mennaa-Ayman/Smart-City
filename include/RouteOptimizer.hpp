/* =====================================================================
Route Optimization (Greedy & Divide & Conquer)
- Greedy Algorithm: Select best options at each step
- Activity Selection: Choose non-overlapping deliveries
- Fractional Knapsack: Optimize vehicle load capacity
- Merge Sort: Sort deliveries by multiple criteria
- Quick Sort: Fast sorting of large datasets
- Binary Search: Search in sorted delivery schedules
- Closest Pair of Points: Find nearest delivery locations
- Divide & Conquer: Partition delivery zones
===================================================================== */

#ifndef ROUTEOPTIMIZER_HPP
#define ROUTEOPTIMIZER_HPP

#include "Utils.hpp"
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_map>

// ==================== Delivery Item for Knapsack ====================
struct DeliveryItem {
    std::string id;
    double weight;
    double value;  // Priority value
    double profitRatio() const { return value / (weight > 0 ? weight : 1.0); }
};

// ==================== Route Optimizer Class ====================
class RouteOptimizer {
public:
    // ===================== GREEDY ALGORITHMS =====================
    
    // Activity Selection: Select maximum non-overlapping deliveries
    std::vector<Delivery> activitySelection(const std::vector<Delivery>& deliveries);
    
    // Greedy Route Selection: Select best next destination
    int greedyNextDestination(int currentLocation, const std::vector<int>& unvisited,
                              const std::unordered_map<int, std::vector<std::pair<int, double>>>& graph);
    
    // Fractional Knapsack: Optimize package loading
    double fractionalKnapsack(const std::vector<DeliveryItem>& items, double capacity,
                             std::vector<double>& selectedAmounts);
    
    // ===================== SORTING ALGORITHMS =====================
    
    // Merge Sort: Sort deliveries by multiple criteria
    std::vector<Delivery> mergeSort(std::vector<Delivery>& deliveries);
    
    // Quick Sort: Fast sorting of large datasets
    std::vector<Delivery> quickSort(std::vector<Delivery>& deliveries);
    
    // Binary Search: Find delivery in sorted schedule
    int binarySearch(const std::vector<Delivery>& sortedDeliveries, const std::string& deliveryId);
    
    // ===================== GEOMETRIC ALGORITHMS =====================
    
    // Closest Pair of Points: Find nearest delivery locations
    std::pair<Location, Location> closestPairOfPoints(const std::vector<Location>& locations);
    
    double distanceBetweenPoints(double x1, double y1, double x2, double y2);
    
    // ===================== DIVIDE & CONQUER ALGORITHMS =====================
    
    // Partition delivery zones geographically
    std::vector<std::vector<Location>> partitionDeliveryZones(const std::vector<Location>& locations, int numZones);
    
    // Recursively partition a zone
    void recursivePartition(const std::vector<Location>& locations, 
                          std::vector<std::vector<Location>>& zones,
                          int numZones, int depth);

private:
    // Helper functions for sorting
    void mergeSortHelper(std::vector<Delivery>& deliveries, int left, int right);
    void merge(std::vector<Delivery>& deliveries, int left, int mid, int right);
    
    int quickSortPartition(std::vector<Delivery>& deliveries, int low, int high);
    void quickSortHelper(std::vector<Delivery>& deliveries, int low, int high);
    
    // Helper for closest pair
    double closestPairHelper(std::vector<Location>& locations, int left, int right);
    double minDistInStrip(const std::vector<Location>& strip, double minDist);
};

#endif // ROUTEOPTIMIZER_HPP
