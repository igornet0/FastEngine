#include "FastEngine/Engine.h"
#include "Game.h"
#include <iostream>

int main() {
    std::cout << "Snake (snake_cros_app) starting..." << std::endl;

    FastEngine::Engine engine;

    if (!engine.Initialize("Snake", 800, 600)) {
        std::cerr << "Failed to initialize engine." << std::endl;
        return -1;
    }

    std::cout << "Platform: " << engine.GetPlatformName() << std::endl;

    Game game(&engine);
    game.Initialize();

    std::cout << "Controls: Arrow Keys or WASD - Move | Space - Pause | R - Restart | ESC - Quit" << std::endl;

    engine.SetRenderCallback([&game]() { game.Render(); });
    engine.Run();

    game.Shutdown();

    std::cout << "Snake finished." << std::endl;
    return 0;
}
