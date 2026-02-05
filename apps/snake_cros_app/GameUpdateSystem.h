#pragma once

#include "FastEngine/System.h"

class Game;

class GameUpdateSystem : public FastEngine::System {
public:
    GameUpdateSystem(FastEngine::World* world, Game* game);
    void Update(float deltaTime) override;

private:
    Game* m_game;
};
