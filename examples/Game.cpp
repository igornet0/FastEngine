#include "Game.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Components/Sprite.h"

Game::Game(FastEngine::Engine* engine) 
    : m_engine(engine)
    , m_world(nullptr)
    , m_player(nullptr)
    , m_background(nullptr) {
}

Game::~Game() = default;

void Game::Initialize() {
    m_world = m_engine->GetWorld();
    
    // Создаем фоновый спрайт
    m_background = m_world->CreateEntity();
    m_background->AddComponent<FastEngine::Transform>(0.0f, 0.0f);
    m_background->AddComponent<FastEngine::Sprite>("background.png");
    
    // Создаем игрока
    m_player = m_world->CreateEntity();
    m_player->AddComponent<FastEngine::Transform>(400.0f, 300.0f);
    m_player->AddComponent<FastEngine::Sprite>("player.png");
}

void Game::Update(float deltaTime) {
    // Здесь будет игровая логика
    // Например, движение игрока, обработка столкновений и т.д.
}

void Game::Render() {
    // Отрисовка обрабатывается автоматически движком
    // Здесь можно добавить дополнительную логику отрисовки
}
