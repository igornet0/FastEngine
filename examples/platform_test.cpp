#include "FastEngine/Platform/Platform.h"
#include "FastEngine/Platform/Window.h"
#include "FastEngine/Platform/FileSystem.h"
#include "FastEngine/Platform/Timer.h"
#include <iostream>

int main() {
    std::cout << "FastEngine Platform Test Starting..." << std::endl;
    
    try {
        FastEngine::Platform& platform = FastEngine::Platform::GetInstance();
        
        if (platform.Initialize()) {
            std::cout << "Platform initialized successfully!" << std::endl;
            std::cout << "Platform: " << platform.GetPlatformName() << std::endl;
            std::cout << "Version: " << platform.GetVersion() << std::endl;
            
            // Тестируем FileSystem
            if (platform.GetFileSystem()) {
                std::cout << "FileSystem available" << std::endl;
                std::cout << "Resource path: " << platform.GetFileSystem()->GetResourcePath("test.txt") << std::endl;
            }
            
            // Тестируем Timer
            if (platform.GetTimer()) {
                std::cout << "Timer available" << std::endl;
                std::cout << "Time: " << platform.GetTimer()->GetTime() << std::endl;
            }
            
            // Тестируем Window
            if (platform.GetWindow()) {
                std::cout << "Window available" << std::endl;
            }
            
            platform.Shutdown();
            std::cout << "Platform shutdown successfully!" << std::endl;
        } else {
            std::cout << "Failed to initialize platform!" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "FastEngine Platform Test Finished." << std::endl;
    return 0;
}
