#include "FastEngine/Engine.h"
#include <iostream>

int main() {
    std::cout << "FastEngine Simple Test Starting..." << std::endl;
    
    try {
        FastEngine::Engine engine;
        
        std::cout << "Platform: " << engine.GetPlatformName() << std::endl;
        
        if (engine.Initialize("FastEngine Test", 800, 600)) {
            std::cout << "Engine initialized successfully!" << std::endl;
            
            // Простой тест - запускаем на 1 секунду
            auto start = std::chrono::high_resolution_clock::now();
            while (std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - start).count() < 1) {
                engine.Run();
            }
            
            std::cout << "Test completed successfully!" << std::endl;
        } else {
            std::cout << "Failed to initialize engine!" << std::endl;
        }
        
        engine.Shutdown();
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "FastEngine Simple Test Finished." << std::endl;
    return 0;
}
