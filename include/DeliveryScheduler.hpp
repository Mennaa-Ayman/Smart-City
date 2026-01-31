/* =====================================================
Responsibilities:
- Maintain priority queue of pending deliveries 
- Assign deliveries to vehicles based on urgency 
- Handle real-time delivery requests 
- Reorder queue when priorities change
======================================================== */

#ifndef DELIVERYSCHEDULER_HPP
#define DELIVERYSCHEDULER_HPP

#include <vector>
#include <functional>
#include <iostream>
#include "Utils.hpp"
#include "HashTable.hpp"
#include "SpatialIndex.hpp"
#include "Network.hpp"

// ==================== Priority Queue Template ====================
template<typename T> 
class PriorityQueue { 
private:
    std::vector<T> heap; 
    std::function<bool(const T&, const T&)> comparator; 
    
    // Restore heap property by moving element up
    void heapifyUp(int index) { 
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (comparator(heap[index], heap[parent])) {
                std::swap(heap[index], heap[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }
    
    // Restore heap property by moving element down
    void heapifyDown(int index) { 
        int size = heap.size();
        while (2 * index + 1 < size) {
            int smallest = index;
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            
            if (left < size && comparator(heap[left], heap[smallest])) {
                smallest = left;
            }
            if (right < size && comparator(heap[right], heap[smallest])) {
                smallest = right;
            }
            
            if (smallest != index) {
                std::swap(heap[index], heap[smallest]);
                index = smallest;
            } else {
                break;
            }
        }
    }

public:
    // Constructor with comparator
    PriorityQueue(std::function<bool(const T&, const T&)> cmp) 
        : comparator(cmp) {}
    
    // Insert element into priority queue
    void push(const T& element) {
        heap.push_back(element);
        heapifyUp(heap.size() - 1);
    }
    
    // Remove and return the top (highest priority) element
    T pop() {
        if (heap.empty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        T top = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            heapifyDown(0);
        }
        return top;
    }
    
    // Return top element without removing
    const T& top() const {
        if (heap.empty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        return heap[0];
    }
    
    // Check if queue is empty
    bool empty() const {
        return heap.empty();
    }
    
    // Get size of queue
    int size() const {
        return heap.size();
    }
    
    // Clear all elements
    void clear() {
        heap.clear();
    }
};

// ==================== Delivery Assignment Record ====================
struct DeliveryAssignment {
    std::string deliveryId;
    int vehicleId;
    std::vector<int> route;  // ordered list of location IDs
    double estimatedTime;
    std::string status;      
    
    DeliveryAssignment() : vehicleId(0), estimatedTime(0.0) {}
};

// ==================== Delivery Scheduler Class ====================
class DeliveryScheduler {
private:
    // Priority queue for pending deliveries
    PriorityQueue<Delivery> pendingDeliveries;
    
    // Hash table to track assigned deliveries
    HashTable<std::string, DeliveryAssignment> assignedDeliveries;
    
    // Hash table for vehicle assignments
    HashTable<int, std::vector<std::string>> vehicleDeliveries;
    
    // Reference to road network for pathfinding
    RoadNetwork* roadNetwork;
    
    // Reference to spatial index for nearest location queries
    QuadTree* spatialIndex;
    
    // Priority comparator
    static bool compareDeliveries(const Delivery& d1, const Delivery& d2) {
        // Priority levels: "High" > "Medium" > "Low"
        auto getPriorityLevel = [](const std::string& p) -> int {
            if (p == "High") return 3;
            if (p == "Medium") return 2;
            return 1;  // Low
        };
        
        int p1 = getPriorityLevel(d1.priority);
        int p2 = getPriorityLevel(d2.priority);
        
        if (p1 != p2) return p1 > p2;
        return d1.deadline < d2.deadline;  // Earlier deadline has higher priority
    }
    
    // Check if vehicle has capacity for delivery
    bool hasCapacity(const Vehicle& vehicle, const Delivery& delivery) const {
        return vehicle.capacity >= delivery.weight;
    }
    
    // Calculate estimated delivery time
    double calculateEstimatedTime(const std::vector<int>& route);

public:
    // Constructor
    DeliveryScheduler(RoadNetwork* network, QuadTree* spatial)
        : pendingDeliveries(compareDeliveries), roadNetwork(network), spatialIndex(spatial) {}
    
    // Add new delivery to queue
    void addDelivery(const Delivery& delivery);
    
    // Get next highest priority delivery
    Delivery getNextDelivery();
    
    // Assign delivery to vehicle
    bool assignDeliveryToVehicle(const std::string& deliveryId, int vehicleId, 
                                  const std::vector<Vehicle>& vehicles,
                                  const std::vector<Location>& locations);
    
    // Get route for a delivery (find optimal path)
    std::vector<int> calculateOptimalRoute(const std::string& source, 
                                           const std::string& destination,
                                           const std::vector<Location>& locations);
    
    // Reassign delivery (handles priority changes)
    void reassignDelivery(const std::string& deliveryId, const Delivery& updatedDelivery);
    
    // Mark delivery as completed
    void completeDelivery(const std::string& deliveryId);
    
    // Get assignment details
    DeliveryAssignment* getAssignment(const std::string& deliveryId);
    
    // Get all deliveries for a vehicle
    std::vector<std::string> getVehicleDeliveries(int vehicleId);
    
    // Get pending deliveries count
    int getPendingCount() const;
    
    // Display all pending deliveries
    void displayPendingDeliveries() const;
};

#endif // DELIVERYSCHEDULER_HPP