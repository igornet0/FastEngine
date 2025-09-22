#include "FastEngine/Platform/Platform.h"
#include <iostream>

int main() {
    std::cout << "FastEngine Minimal Test Starting..." << std::endl;
    
    try {
        FastEngine::Platform& platform = FastEngine::Platform::GetInstance();
        
        if (platform.Initialize()) {
            std::cout << "Platform initialized successfully!" << std::endl;
            std::cout << "Platform: " << platform.GetPlatformName() << std::endl;
            platform.Shutdown();
            std::cout << "Platform shutdown successfully!" << std::endl;
        } else {
            std::cout << "Failed to initialize platform!" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "FastEngine Minimal Test Finished." << std::endl;
    return 0;
}
