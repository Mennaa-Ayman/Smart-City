#include "Utils.hpp"

// ================ File Loading Functions ================ //
std::vector<std::string> FileUtils::parseCSVLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<Edge> FileUtils::loadCityMap() {
    std::vector<Edge> edges;
    std::ifstream file("../Data/City_map.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening City_map.csv" << std::endl;
        return edges;
    }
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        auto tokens = parseCSVLine(line);
            Edge edge;
            edge.source = tokens[0];
            edge.destination = tokens[1];
            edge.distance = std::stod(tokens[2]);
            edge.traffic_congestion = std::stod(tokens[3]);
            edge.traffic_time = std::stod(tokens[4]);
            edges.push_back(edge);
    }
    return edges;
}

std::vector<Location> FileUtils::loadLocations() {
    std::vector<Location> locations;
    std::ifstream file("../Data/locations.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening locations.csv" << std::endl;
        return locations;
    }   
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        auto tokens = parseCSVLine(line);
            Location location;
            location.id = std::stoi(tokens[0]);
            location.name = tokens[1];
            location.x = std::stod(tokens[2]);
            location.y = std::stod(tokens[3]);
            location.type = tokens[4];
            locations.push_back(location);
    }
    return locations;
}

std::vector<Vehicle> FileUtils::loadVehicles() {
    std::vector<Vehicle> vehicles;
    std::ifstream file("../Data/vehicles.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening vehicles.csv" << std::endl;
        return vehicles;
    }   
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        auto tokens = parseCSVLine(line);
            Vehicle vehicle;
            vehicle.id = tokens[0];
            vehicle.capacity = std::stod(tokens[1]);
            vehicle.speed = std::stod(tokens[2]);
            vehicle.start_location = tokens[3];
            vehicles.push_back(vehicle);
    }
    return vehicles;
}

std::vector<Delivery> FileUtils::loadDeliveries() {
    std::vector<Delivery> deliveries;
    std::ifstream file("../Data/deliveries.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening deliveries.csv" << std::endl;
        return deliveries;
    }   
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        auto tokens = parseCSVLine(line);
            Delivery delivery;
            delivery.id = tokens[0];
            delivery.source = tokens[1];
            delivery.destination = tokens[2];
            delivery.deadline = tokens[3];
            delivery.priority = tokens[4];
            delivery.weight = std::stod(tokens[5]);
            deliveries.push_back(delivery);
    }
    return deliveries;
}

std::vector<TrafficEvent> FileUtils::loadTrafficUpdates() {
    std::vector<TrafficEvent> events;
    std::ifstream file("../Data/traffic_updates.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening traffic_updates.csv" << std::endl;
        return events;
    }   
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        auto tokens = parseCSVLine(line);
            TrafficEvent event;
            event.road_id = std::stoi(tokens[0]);
            event.new_weight = std::stoi(tokens[1]);
            event.timestamp = tokens[2];
            events.push_back(event);
    }
    return events;
}