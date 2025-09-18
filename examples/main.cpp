#include "FastEngine/Engine.h"
#include "Game.h"

int main() {
    // Создаем движок
    FastEngine::Engine engine;
    
    // Инициализируем движок
    if (!engine.Initialize("FastEngine Example", 800, 600)) {
        return -1;
    }
    
    // Создаем игру
    Game game(&engine);
    game.Initialize();
    
    // Запускаем основной цикл
    engine.Run();
    
    return 0;
}
