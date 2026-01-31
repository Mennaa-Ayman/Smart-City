#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

// ==================== Data Structures ==================== //

// Location structure
struct Location {
    int id;
    std::string name;
    double latitude;
    double longitude;
    std::string details;
    
    Location() : id(0), latitude(0.0), longitude(0.0) {}
    Location(int id, const std::string& n, double lat, double lon, const std::string& det)
        : id(id), name(n), latitude(lat), longitude(lon), details(det) {}
};

// Vehicle structure
struct Vehicle {
    int id;
    std::string licensePlate;
    std::string type;
    double capacity;
    std::string status;
    
    Vehicle() : id(0), capacity(0.0) {}
    Vehicle(int id, const std::string& plate, const std::string& t, double cap, const std::string& stat)
        : id(id), licensePlate(plate), type(t), capacity(cap), status(stat) {}
};

// Package Tracking structure
struct Package {
    std::string trackingNumber;
    std::string senderName;
    std::string recipientName;
    double weight;
    std::string status;
    int currentLocationId;
    
    Package() : weight(0.0), currentLocationId(0) {}
    Package(const std::string& track, const std::string& sender, const std::string& recipient,
            double w, const std::string& stat, int locID)
        : trackingNumber(track), senderName(sender), recipientName(recipient), 
          weight(w), status(stat), currentLocationId(locID) {}
};

// Delivery structure
struct Delivery {
    std::string id;
    std::string source, destination;
    double weight;
    std::string deadline;
    std::string priority;
};

// Customer structure
struct Customer {
    int id;
    std::string name;
    std::string email;
    std::string phone;
    std::string address;
    double totalSpending;
    
    Customer() : id(0), totalSpending(0.0) {}
    Customer(int id, const std::string& n, const std::string& e, const std::string& phone,
             const std::string& addr, double spent)
        : id(id), name(n), email(e), phone(phone), address(addr), totalSpending(spent) {}
};


struct Edge {
    std::string source, destination;
    double distance, traffic_congestion, traffic_time;
};

struct Node {
    Location location;
    std::vector<Edge> edges;
};

struct TrafficEvent {
    std::string timestamp;
    int road_id;
    int new_weight;
};

class FileUtils {
public:
    static std::vector<std::string> parseCSVLine(const std::string& line);
    static std::vector<Edge> loadCityMap();    
    static std::vector<Location> loadLocations();
    static std::vector<Vehicle> loadVehicles();
    static std::vector<Delivery> loadDeliveries();
    static std::vector<TrafficEvent> loadTrafficUpdates();

    double calculateDistance(double x1, double y1, double x2, double y2);
};

#endif // UTILS_HPP