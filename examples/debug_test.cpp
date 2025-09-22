#include "FastEngine/Engine.h"
#include "FastEngine/Platform/Platform.h"
#include "FastEngine/Platform/FileSystem.h"
#include <iostream>

int main() {
    std::cout << "FastEngine Debug Test Starting..." << std::endl;
    
    try {
        // Инициализируем платформу
        auto& platform = FastEngine::Platform::GetInstance();
        if (!platform.Initialize()) {
            std::cout << "Failed to initialize platform!" << std::endl;
            return 1;
        }
        
        // Проверяем пути к ресурсам
        auto* fileSystem = platform.GetFileSystem();
        if (fileSystem) {
            std::string shaderPath = fileSystem->GetResourcePath("shaders/sprite.vert");
            std::cout << "Shader path: " << shaderPath << std::endl;
            
            std::string content = fileSystem->ReadFile(shaderPath);
            if (!content.empty()) {
                std::cout << "Shader content length: " << content.length() << std::endl;
                std::cout << "First 200 chars: " << content.substr(0, 200) << std::endl;
            } else {
                std::cout << "Failed to read shader file" << std::endl;
            }
        }
        
        platform.Shutdown();
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "FastEngine Debug Test Finished." << std::endl;
    return 0;
}
