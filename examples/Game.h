#pragma once

#include "FastEngine/Engine.h"
#include "FastEngine/World.h"
#include "FastEngine/Entity.h"

class Game {
public:
    Game(FastEngine::Engine* engine);
    ~Game();
    
    void Initialize();
    void Update(float deltaTime);
    void Render();
    
private:
    FastEngine::Engine* m_engine;
    FastEngine::World* m_world;
    
    // Игровые объекты
    FastEngine::Entity* m_player;
    FastEngine::Entity* m_background;
};
