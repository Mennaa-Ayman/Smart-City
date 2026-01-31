#include "DeliveryScheduler.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>

// Add new delivery to the priority queue
void DeliveryScheduler::addDelivery(const Delivery& delivery) {
    pendingDeliveries.push(delivery);
    std::cout << "Delivery added: " << delivery.id 
              << " (Priority: " << delivery.priority << ")" << std::endl;
}

// Get next highest priority delivery
Delivery DeliveryScheduler::getNextDelivery() {
    if (pendingDeliveries.empty()) {
        throw std::runtime_error("No pending deliveries");
    }
    return pendingDeliveries.top();
}

// Calculate optimal route between source and destination
std::vector<int> DeliveryScheduler::calculateOptimalRoute(const std::string& source, 
                                                          const std::string& destination,
                                                          const std::vector<Location>& locations) {
    std::vector<int> route;
    
    // Find location IDs for source and destination
    int sourceId = -1, destId = -1;
    
    for (const auto& loc : locations) {
        if (loc.name == source) sourceId = loc.id;
        if (loc.name == destination) destId = loc.id;
    }
    
    if (sourceId == -1 || destId == -1) {
        std::cerr << "Invalid source or destination location" << std::endl;
        return route;
    }
    
    // Use road network to find shortest path
    try {
        route = roadNetwork->shortestPath(sourceId, destId);
    } catch (const std::exception& e) {
        std::cerr << "Error calculating route: " << e.what() << std::endl;
        // Fallback: direct route
        route = {sourceId, destId};
    }
    
    return route;
}

// Calculate estimated delivery time for a route
double DeliveryScheduler::calculateEstimatedTime(const std::vector<int>& route) {
    double totalTime = 0.0;
    
    for (size_t i = 0; i + 1 < route.size(); i++) {
        totalTime += 15.0;  
    }
    
    return totalTime;
}

bool DeliveryScheduler::assignDeliveryToVehicle(const std::string& deliveryId, 
                                                 int vehicleId,
                                                 const std::vector<Vehicle>& vehicles,
                                                 const std::vector<Location>& locations) {
    // Find the delivery in pending queue
    if (pendingDeliveries.empty()) {
        std::cerr << "No pending deliveries to assign" << std::endl;
        return false;
    }
    
    // Get the delivery
    Delivery delivery = pendingDeliveries.pop();
    
    if (delivery.id != deliveryId) {
        std::cerr << "Delivery ID mismatch" << std::endl;
        // Re-add the delivery back
        pendingDeliveries.push(delivery);
        return false;
    }
    
    // Find vehicle
    Vehicle* assignedVehicle = nullptr;
    for (const auto& v : vehicles) {
        if (v.id == vehicleId) {
            assignedVehicle = const_cast<Vehicle*>(&v);
            break;
        }
    }
    
    if (!assignedVehicle) {
        std::cerr << "Vehicle not found" << std::endl;
        pendingDeliveries.push(delivery);
        return false;
    }
    
    // Check capacity
    if (!hasCapacity(*assignedVehicle, delivery)) {
        std::cerr << "Vehicle capacity insufficient" << std::endl;
        pendingDeliveries.push(delivery);
        return false;
    }
    
    // Calculate route
    std::vector<int> route = calculateOptimalRoute(delivery.source, delivery.destination, locations);
    
    if (route.empty()) {
        std::cerr << "Cannot calculate route" << std::endl;
        pendingDeliveries.push(delivery);
        return false;
    }
    
    // Create assignment record
    DeliveryAssignment assignment;
    assignment.deliveryId = deliveryId;
    assignment.vehicleId = vehicleId;
    assignment.route = route;
    assignment.estimatedTime = calculateEstimatedTime(route);
    assignment.status = "in-transit";
    
    // Store assignment
    assignedDeliveries.insert(deliveryId, assignment);
    
    // Add to vehicle's delivery list
    std::vector<std::string> vehicleDeliv;
    if (!vehicleDeliveries.find(vehicleId, vehicleDeliv)) {
        vehicleDeliv.clear();
    }
    vehicleDeliv.push_back(deliveryId);
    vehicleDeliveries.insert(vehicleId, vehicleDeliv);
    
    std::cout << "Delivery " << deliveryId << " assigned to vehicle " << vehicleId 
              << " with estimated time: " << assignment.estimatedTime << " minutes" << std::endl;
    
    return true;
}

// Reassign delivery (when priority changes)
void DeliveryScheduler::reassignDelivery(const std::string& deliveryId, 
                                         const Delivery& updatedDelivery) {
    // Remove current assignment
    DeliveryAssignment* current = getAssignment(deliveryId);
    if (current) {
        current->status = "reassigned";
    }
    
    addDelivery(updatedDelivery);
    
    std::cout << "Delivery " << deliveryId << " reassigned with new priority: " 
              << updatedDelivery.priority << std::endl;
}

// Mark delivery as completed
void DeliveryScheduler::completeDelivery(const std::string& deliveryId) {
    DeliveryAssignment* assignment = getAssignment(deliveryId);
    if (assignment) {
        assignment->status = "completed";
        std::cout << "Delivery " << deliveryId << " marked as completed" << std::endl;
    } else {
        std::cerr << "Delivery not found: " << deliveryId << std::endl;
    }
}

// Get assignment details
DeliveryAssignment* DeliveryScheduler::getAssignment(const std::string& deliveryId) {
    DeliveryAssignment dummy;
    if (assignedDeliveries.find(deliveryId, dummy)) {
        static DeliveryAssignment lastFound;
        lastFound = dummy;
        return &lastFound;
    }
    return nullptr;
}

std::vector<std::string> DeliveryScheduler::getVehicleDeliveries(int vehicleId) {
    std::vector<std::string> deliveries;
    vehicleDeliveries.find(vehicleId, deliveries);
    return deliveries;
}

int DeliveryScheduler::getPendingCount() const {
    return pendingDeliveries.size();
}

// Display all pending deliveries
void DeliveryScheduler::displayPendingDeliveries() const {
    std::cout << "\n===== Pending Deliveries =====" << std::endl;
    std::cout << "Total: " << pendingDeliveries.size() << std::endl;
    
    if (!pendingDeliveries.empty()) {
        try {
            const Delivery& top = pendingDeliveries.top();
            std::cout << "\nHighest Priority Delivery:" << std::endl;
            std::cout << "  ID: " << top.id << std::endl;
            std::cout << "  From: " << top.source << " To: " << top.destination << std::endl;
            std::cout << "  Weight: " << top.weight << " kg" << std::endl;
            std::cout << "  Priority: " << top.priority << std::endl;
            std::cout << "  Deadline: " << top.deadline << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error displaying deliveries: " << e.what() << std::endl;
        }
    }
}

