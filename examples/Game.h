#pragma once

#include "FastEngine/Engine.h"
#include "FastEngine/World.h"
#include "FastEngine/Entity.h"
#include "FastEngine/Render/Camera.h"
#include <vector>

class Game {
public:
    Game(FastEngine::Engine* engine);
    ~Game();
    
    void Initialize();
    void Update(float deltaTime);
    void Render();
    void Shutdown();
    
private:
    FastEngine::Engine* m_engine;
    FastEngine::World* m_world;
    
    // Игровые объекты
    FastEngine::Entity* m_player;
    FastEngine::Entity* m_background;
    std::vector<FastEngine::Entity*> m_enemies;
    
    // Камера
    FastEngine::Camera* m_camera;
    
    // Параметры игры
    float m_playerSpeed;
    float m_rotationSpeed;
};
