#include "Utils.hpp"

// ================ File Loading Functions ================ //
std::vector<std::string> FileUtils::parseCSVLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ',')) {
        // Trim whitespace from token
        size_t start = token.find_first_not_of(" \t\r\n");
        size_t end = token.find_last_not_of(" \t\r\n");
        if (start != std::string::npos && end != std::string::npos) {
            tokens.push_back(token.substr(start, end - start + 1));
        } else if (start == std::string::npos) {
            tokens.push_back("");  // Empty field
        } else {
            tokens.push_back(token.substr(start, end - start + 1));
        }
    }
    return tokens;
}

std::vector<Edge> FileUtils::loadCityMap() {
    std::vector<Edge> edges;
    std::ifstream file("Data/City_map.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening City_map.csv" << std::endl;
        return edges;
    }
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        auto tokens = parseCSVLine(line);
        if (tokens.size() >= 5) {
            Edge edge;
            try {
                edge.source = tokens[0];
                edge.destination = tokens[1];
                edge.distance = std::stod(tokens[2]);
                edge.traffic_congestion = std::stod(tokens[3]);
                edge.traffic_time = std::stod(tokens[4]);
                edges.push_back(edge);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing city map line: " << line << std::endl;
            }
        }
    }
    return edges;
}

std::vector<Location> FileUtils::loadLocations() {
    std::vector<Location> locations;
    std::ifstream file("Data/locations.csv");

    if (!file.is_open()) {
        std::cerr << "Error opening locations.csv" << std::endl;
        return locations;
    }   
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        auto tokens = parseCSVLine(line);
        if (tokens.size() >= 4) {
            Location location;
            try {
                location.id = std::stoi(tokens[0]);
                location.name = tokens[1];
                location.latitude = std::stod(tokens[2]);
                location.longitude = std::stod(tokens[3]);
                location.details = (tokens.size() > 4) ? tokens[4] : "";
                locations.push_back(location);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing location line: " << line << std::endl;
            }
        }
    }
    return locations;
}

std::vector<Vehicle> FileUtils::loadVehicles() {
    std::vector<Vehicle> vehicles;
    std::ifstream file("Data/vehicles.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening vehicles.csv" << std::endl;
        return vehicles;
    }   
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        auto tokens = parseCSVLine(line);
        if (tokens.size() >= 4) {
            Vehicle vehicle;
            try {
                // Parse vehicle_id (e.g., "V001")
                std::string id_str = tokens[0];
                // Extract numeric part from vehicle ID
                size_t pos = 0;
                while (pos < id_str.size() && !std::isdigit(id_str[pos])) pos++;
                if (pos < id_str.size()) {
                    vehicle.id = std::stoi(id_str.substr(pos));
                } else {
                    vehicle.id = 0;
                }
                vehicle.capacity = std::stod(tokens[1]);
                vehicle.type = tokens[2];  // avg_speed_kmh can be stored as type for now
                vehicle.licensePlate = tokens[3];  // start_location stored as licensePlate
                vehicle.status = "Active";
                vehicles.push_back(vehicle);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing vehicle line: " << line << std::endl;
            }
        }
    }
    return vehicles;
}

std::vector<Delivery> FileUtils::loadDeliveries() {
    std::vector<Delivery> deliveries;
    std::ifstream file("Data/deliveries.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening deliveries.csv" << std::endl;
        return deliveries;
    }   
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        auto tokens = parseCSVLine(line);
        if (tokens.size() >= 5) {
            Delivery delivery;
            try {
                delivery.id = tokens[0];
                delivery.source = tokens[1];
                delivery.destination = tokens[2];
                delivery.deadline = tokens[3];
                delivery.priority = tokens[4];
                delivery.weight = (tokens.size() > 5 && !tokens[5].empty()) ? std::stod(tokens[5]) : 0.0;
                deliveries.push_back(delivery);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing delivery line: " << line << std::endl;
            }
        }
    }
    return deliveries;
}

std::vector<TrafficEvent> FileUtils::loadTrafficUpdates() {
    std::vector<TrafficEvent> events;
    std::ifstream file("Data/Traffic_updates.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening Traffic_updates.csv" << std::endl;
        return events;
    }   
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        auto tokens = parseCSVLine(line);
        if (tokens.size() >= 3) {
            TrafficEvent event;
            try {
                event.road_id = std::stoi(tokens[0]);
                event.new_weight = std::stoi(tokens[1]);
                event.timestamp = tokens[2];
                events.push_back(event);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing traffic event line: " << line << std::endl;
            }
        }
    }
    return events;
}

// ================ Calculate Distance Function ================ //
double FileUtils::calculateDistance(double x1, double y1, double x2, double y2) {
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}