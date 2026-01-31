#include "RouteOptimizer.hpp"
#include <algorithm>
#include <numeric>

// ===================== GREEDY ALGORITHMS =====================

// Activity Selection: Select maximum non-overlapping deliveries
std::vector<Delivery> RouteOptimizer::activitySelection(const std::vector<Delivery>& deliveries) {
    std::vector<Delivery> selected;
    
    if (deliveries.empty()) {
        return selected;
    }
    
    // Create a copy and sort by deadline
    std::vector<Delivery> sortedDeliveries = deliveries;
    std::sort(sortedDeliveries.begin(), sortedDeliveries.end(),
              [](const Delivery& a, const Delivery& b) { return a.deadline < b.deadline; });
    
    // Select first activity
    selected.push_back(sortedDeliveries[0]);
    
    // Greedily select non-overlapping activities
    for (size_t i = 1; i < sortedDeliveries.size(); i++) {
        // For simplicity, consider deliveries non-overlapping if they have different deadlines
        if (sortedDeliveries[i].deadline != selected.back().deadline) {
            selected.push_back(sortedDeliveries[i]);
        }
    }
    
    return selected;
}

// Greedy Route Selection: Select best next destination
int RouteOptimizer::greedyNextDestination(int currentLocation, const std::vector<int>& unvisited,
                                           const std::unordered_map<int, std::vector<std::pair<int, double>>>& graph) {
    if (unvisited.empty()) {
        return -1;
    }
    
    int bestNext = unvisited[0];
    double minDistance = std::numeric_limits<double>::max();
    
    // Find the nearest unvisited location
    if (graph.count(currentLocation)) {
        for (const auto& [neighbor, weight] : graph.at(currentLocation)) {
            // Check if neighbor is in unvisited list
            if (std::find(unvisited.begin(), unvisited.end(), neighbor) != unvisited.end()) {
                if (weight < minDistance) {
                    minDistance = weight;
                    bestNext = neighbor;
                }
            }
        }
    }
    
    return bestNext;
}

// Fractional Knapsack: Optimize package loading
double RouteOptimizer::fractionalKnapsack(const std::vector<DeliveryItem>& items, double capacity,
                                          std::vector<double>& selectedAmounts) {
    selectedAmounts.resize(items.size(), 0.0);
    
    // Create items with indices
    std::vector<std::pair<DeliveryItem, int>> indexedItems;
    for (size_t i = 0; i < items.size(); i++) {
        indexedItems.push_back({items[i], i});
    }
    
    // Sort by profit ratio (value/weight) in descending order
    std::sort(indexedItems.begin(), indexedItems.end(),
              [](const auto& a, const auto& b) { return a.first.profitRatio() > b.first.profitRatio(); });
    
    double totalValue = 0.0;
    double remainingCapacity = capacity;
    
    // Greedily pick items
    for (const auto& [item, originalIndex] : indexedItems) {
        if (remainingCapacity >= item.weight) {
            // Take the whole item
            selectedAmounts[originalIndex] = item.weight;
            totalValue += item.value;
            remainingCapacity -= item.weight;
        } else {
            // Take fraction of item
            double fraction = remainingCapacity / item.weight;
            selectedAmounts[originalIndex] = remainingCapacity;
            totalValue += item.value * fraction;
            remainingCapacity = 0;
            break;
        }
    }
    
    return totalValue;
}

// ===================== SORTING ALGORITHMS =====================

// Merge Sort: Sort deliveries by multiple criteria
std::vector<Delivery> RouteOptimizer::mergeSort(std::vector<Delivery>& deliveries) {
    if (deliveries.size() <= 1) {
        return deliveries;
    }
    
    mergeSortHelper(deliveries, 0, deliveries.size() - 1);
    return deliveries;
}

void RouteOptimizer::mergeSortHelper(std::vector<Delivery>& deliveries, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        // Sort left half
        mergeSortHelper(deliveries, left, mid);
        // Sort right half
        mergeSortHelper(deliveries, mid + 1, right);
        // Merge halves
        merge(deliveries, left, mid, right);
    }
}

void RouteOptimizer::merge(std::vector<Delivery>& deliveries, int left, int mid, int right) {
    std::vector<Delivery> temp;
    int i = left, j = mid + 1;
    
    // Merge based on deadline (primary) and priority (secondary)
    while (i <= mid && j <= right) {
        auto priorityValue = [](const std::string& p) -> int {
            if (p == "High") return 3;
            if (p == "Medium") return 2;
            return 1;
        };
        
        if (deliveries[i].deadline < deliveries[j].deadline ||
            (deliveries[i].deadline == deliveries[j].deadline && 
             priorityValue(deliveries[i].priority) > priorityValue(deliveries[j].priority))) {
            temp.push_back(deliveries[i++]);
        } else {
            temp.push_back(deliveries[j++]);
        }
    }
    
    // Copy remaining elements
    while (i <= mid) temp.push_back(deliveries[i++]);
    while (j <= right) temp.push_back(deliveries[j++]);
    
    // Copy back to original array
    for (int i = 0; i < temp.size(); i++) {
        deliveries[left + i] = temp[i];
    }
}

// Quick Sort: Fast sorting of large datasets
std::vector<Delivery> RouteOptimizer::quickSort(std::vector<Delivery>& deliveries) {
    if (deliveries.empty()) {
        return deliveries;
    }
    
    quickSortHelper(deliveries, 0, deliveries.size() - 1);
    return deliveries;
}

int RouteOptimizer::quickSortPartition(std::vector<Delivery>& deliveries, int low, int high) {
    auto priorityValue = [](const std::string& p) -> int {
        if (p == "High") return 3;
        if (p == "Medium") return 2;
        return 1;
    };
    
    // Choose last element as pivot
    Delivery pivot = deliveries[high];
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        // Compare by deadline first, then priority
        if (deliveries[j].deadline < pivot.deadline ||
            (deliveries[j].deadline == pivot.deadline && 
             priorityValue(deliveries[j].priority) > priorityValue(pivot.priority))) {
            i++;
            std::swap(deliveries[i], deliveries[j]);
        }
    }
    
    std::swap(deliveries[i + 1], deliveries[high]);
    return i + 1;
}

void RouteOptimizer::quickSortHelper(std::vector<Delivery>& deliveries, int low, int high) {
    if (low < high) {
        int pi = quickSortPartition(deliveries, low, high);
        quickSortHelper(deliveries, low, pi - 1);
        quickSortHelper(deliveries, pi + 1, high);
    }
}

// Binary Search: Find delivery in sorted schedule
int RouteOptimizer::binarySearch(const std::vector<Delivery>& sortedDeliveries, const std::string& deliveryId) {
    int left = 0, right = sortedDeliveries.size() - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (sortedDeliveries[mid].id == deliveryId) {
            return mid;
        } else if (sortedDeliveries[mid].id < deliveryId) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return -1;  // Not found
}

// ===================== GEOMETRIC ALGORITHMS =====================

// Helper: Calculate distance between two points
double RouteOptimizer::distanceBetweenPoints(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

// Closest Pair of Points: Find nearest delivery locations
std::pair<Location, Location> RouteOptimizer::closestPairOfPoints(const std::vector<Location>& locations) {
    std::pair<Location, Location> result;
    
    if (locations.size() < 2) {
        return result;
    }
    
    std::vector<Location> sortedLocations = locations;
    
    // Sort by x-coordinate
    std::sort(sortedLocations.begin(), sortedLocations.end(),
              [](const Location& a, const Location& b) { return a.longitude < b.longitude; });
    
    closestPairHelper(sortedLocations, 0, sortedLocations.size() - 1);
    
    // Find the closest pair
    double minDist = std::numeric_limits<double>::max();
    for (size_t i = 0; i < locations.size(); i++) {
        for (size_t j = i + 1; j < locations.size(); j++) {
            double dist = distanceBetweenPoints(locations[i].longitude, locations[i].latitude,
                                               locations[j].longitude, locations[j].latitude);
            if (dist < minDist) {
                minDist = dist;
                result = {locations[i], locations[j]};
            }
        }
    }
    
    return result;
}

double RouteOptimizer::closestPairHelper(std::vector<Location>& locations, int left, int right) {
    // Use brute force for small arrays
    if (right - left <= 2) {
        double minDist = std::numeric_limits<double>::max();
        for (int i = left; i <= right; i++) {
            for (int j = i + 1; j <= right; j++) {
                double dist = distanceBetweenPoints(locations[i].longitude, locations[i].latitude,
                                                   locations[j].longitude, locations[j].latitude);
                minDist = std::min(minDist, dist);
            }
        }
        return minDist;
    }
    
    int mid = left + (right - left) / 2;
    double midX = locations[mid].longitude;
    
    double leftDist = closestPairHelper(locations, left, mid);
    double rightDist = closestPairHelper(locations, mid + 1, right);
    
    double minDist = std::min(leftDist, rightDist);
    
    // Check strip
    std::vector<Location> strip;
    for (int i = left; i <= right; i++) {
        if (std::abs(locations[i].longitude - midX) < minDist) {
            strip.push_back(locations[i]);
        }
    }
    
    return std::min(minDist, minDistInStrip(strip, minDist));
}

double RouteOptimizer::minDistInStrip(const std::vector<Location>& strip, double minDist) {
    double min = minDist;
    
    for (size_t i = 0; i < strip.size(); i++) {
        for (size_t j = i + 1; j < strip.size() && (strip[j].latitude - strip[i].latitude) < min; j++) {
            double dist = distanceBetweenPoints(strip[i].longitude, strip[i].latitude,
                                               strip[j].longitude, strip[j].latitude);
            min = std::min(min, dist);
        }
    }
    
    return min;
}

// ===================== DIVIDE & CONQUER ALGORITHMS =====================

// Partition delivery zones geographically
std::vector<std::vector<Location>> RouteOptimizer::partitionDeliveryZones(const std::vector<Location>& locations, int numZones) {
    std::vector<std::vector<Location>> zones(numZones);
    
    if (locations.empty() || numZones <= 0) {
        return zones;
    }
    
    // Simple grid-based partitioning
    if (locations.size() <= static_cast<size_t>(numZones)) {
        // One location per zone
        for (size_t i = 0; i < locations.size(); i++) {
            zones[i].push_back(locations[i]);
        }
        return zones;
    }
    
    // Find bounding box
    double minLat = locations[0].latitude, maxLat = locations[0].latitude;
    double minLon = locations[0].longitude, maxLon = locations[0].longitude;
    
    for (const auto& loc : locations) {
        minLat = std::min(minLat, loc.latitude);
        maxLat = std::max(maxLat, loc.latitude);
        minLon = std::min(minLon, loc.longitude);
        maxLon = std::max(maxLon, loc.longitude);
    }
    
    // Create grid
    int gridSize = static_cast<int>(std::ceil(std::sqrt(numZones)));
    double latStep = (maxLat - minLat) / gridSize;
    double lonStep = (maxLon - minLon) / gridSize;
    
    // Assign locations to zones
    for (const auto& loc : locations) {
        int latZone = static_cast<int>((loc.latitude - minLat) / (latStep + 1e-9));
        int lonZone = static_cast<int>((loc.longitude - minLon) / (lonStep + 1e-9));
        
        latZone = std::min(latZone, gridSize - 1);
        lonZone = std::min(lonZone, gridSize - 1);
        
        int zoneIdx = latZone * gridSize + lonZone;
        zoneIdx = std::min(zoneIdx, numZones - 1);
        
        zones[zoneIdx].push_back(loc);
    }
    
    return zones;
}

// Recursively partition a zone
void RouteOptimizer::recursivePartition(const std::vector<Location>& locations,
                                         std::vector<std::vector<Location>>& zones,
                                         int numZones, int depth) {
    if (depth <= 0 || locations.size() <= static_cast<size_t>(numZones)) {
        zones.push_back(locations);
        return;
    }
    
    // Partition along x-axis (longitude)
    std::vector<Location> sorted = locations;
    std::sort(sorted.begin(), sorted.end(),
              [](const Location& a, const Location& b) { return a.longitude < b.longitude; });
    
    size_t mid = sorted.size() / 2;
    std::vector<Location> left(sorted.begin(), sorted.begin() + mid);
    std::vector<Location> right(sorted.begin() + mid, sorted.end());
    
    recursivePartition(left, zones, numZones / 2, depth - 1);
    recursivePartition(right, zones, numZones / 2, depth - 1);
}
