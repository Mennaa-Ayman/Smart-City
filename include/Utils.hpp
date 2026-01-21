#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

// System Components
struct Location {
    int id;
    std::string name;
    double x, y;
    std::string type;
};

struct Vehicle {
    std::string id;
    double capacity;
    double speed;
    std::string start_location;
};

struct Delivery {
    std::string id;
    std::string source, destination;
    double weight;
    std::string deadline;
    std::string priority;
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