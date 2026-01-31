#include "SmartCitySystem.hpp"

int main() {
    try {
        SmartCityDeliverySystem system;
        system.runFullSystem();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}