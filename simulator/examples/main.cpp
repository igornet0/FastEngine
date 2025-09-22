#include "ProjectSimulator/ProjectSimulator.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    std::cout << "=== FastEngine Project Simulator ===" << std::endl;
    std::cout << "Version 1.0.0" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        // Создаем симулятор
        ProjectSimulator::ProjectSimulator simulator;
        
        // Инициализируем
        if (!simulator.Initialize(1024, 768)) {
            std::cerr << "Failed to initialize Project Simulator" << std::endl;
            return -1;
        }
        
        std::cout << "Project Simulator initialized successfully!" << std::endl;
        std::cout << "Starting main loop..." << std::endl;
        
        // Запускаем главный цикл
        simulator.Run();
        
        std::cout << "Project Simulator shutdown complete" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return -1;
    }
    
    return 0;
}

