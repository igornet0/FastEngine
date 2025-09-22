#include "FastEngine/Engine.h"
#include "Game.h"
#include <iostream>

int main() {
    std::cout << "FastEngine Example Starting..." << std::endl;
    
    // Создаем движок
    FastEngine::Engine engine;
    
    // Инициализируем движок
    if (!engine.Initialize("FastEngine Example", 800, 600)) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return -1;
    }
    
    std::cout << "Engine initialized successfully!" << std::endl;
    std::cout << "Platform: " << engine.GetPlatformName() << std::endl;
    
    // Создаем игру
    Game game(&engine);
    game.Initialize();
    
    std::cout << "Game initialized! Starting main loop..." << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD or Arrow Keys - Move player" << std::endl;
    std::cout << "  Touch - Move to touch position (mobile)" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;
    
    // Запускаем основной цикл
    engine.Run();
    
    // Очищаем ресурсы
    game.Shutdown();
    
    std::cout << "Game finished!" << std::endl;
    return 0;
}
