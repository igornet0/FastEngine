#include "GameUpdateSystem.h"
#include "Game.h"

GameUpdateSystem::GameUpdateSystem(FastEngine::World* world, Game* game)
    : FastEngine::System(world)
    , m_game(game) {
}

void GameUpdateSystem::Update(float deltaTime) {
    if (m_game) {
        m_game->Update(deltaTime);
    }
}
