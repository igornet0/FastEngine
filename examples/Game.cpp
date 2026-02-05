#include "Game.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Input/InputManager.h"
#include "FastEngine/Render/Camera.h"
#include "FastEngine/Render/Renderer.h"
#include "FastEngine/Systems/RenderSystem.h"
#include <iostream>

Game::Game(FastEngine::Engine* engine) 
    : m_engine(engine)
    , m_world(nullptr)
    , m_player(nullptr)
    , m_background(nullptr)
    , m_camera(nullptr)
    , m_playerSpeed(200.0f)
    , m_rotationSpeed(90.0f) {
}

Game::~Game() = default;

void Game::Initialize() {
    m_world = m_engine->GetWorld();
    
    // Создаем камеру
    m_camera = new FastEngine::Camera();
    m_camera->SetSize(800.0f, 600.0f);
    m_camera->SetPosition(400.0f, 300.0f);
    
    // Устанавливаем камеру в систему рендеринга
    auto* renderSystem = m_engine->GetRenderSystem();
    if (renderSystem) {
        renderSystem->SetCamera(m_camera);
    }
    
    // Создаем фоновый спрайт (центр 400,300 чтобы покрыть видимую область 0..800, 0..600)
    m_background = m_world->CreateEntity();
    m_background->AddComponent<FastEngine::Transform>(400.0f, 300.0f);
    auto* bgSprite = m_background->AddComponent<FastEngine::Sprite>("textures/background.bmp");
    bgSprite->SetSize(800.0f, 600.0f);
    bgSprite->SetColor(0.2f, 0.3f, 0.8f, 1.0f); // Синий фон
    
    // Создаем игрока
    m_player = m_world->CreateEntity();
    m_player->AddComponent<FastEngine::Transform>(400.0f, 300.0f);
    auto* playerSprite = m_player->AddComponent<FastEngine::Sprite>("textures/player.bmp");
    playerSprite->SetSize(64.0f, 64.0f);
    playerSprite->SetColor(1.0f, 0.5f, 0.0f, 1.0f); // Оранжевый игрок
    
    // Создаем несколько врагов
    for (int i = 0; i < 5; ++i) {
        auto* enemy = m_world->CreateEntity();
        float x = 100.0f + i * 150.0f;
        float y = 100.0f + (i % 2) * 200.0f;
        enemy->AddComponent<FastEngine::Transform>(x, y);
        auto* enemySprite = enemy->AddComponent<FastEngine::Sprite>("textures/enemy.bmp");
        enemySprite->SetSize(48.0f, 48.0f);
        enemySprite->SetColor(1.0f, 0.0f, 0.0f, 1.0f); // Красные враги
        m_enemies.push_back(enemy);
    }
    
    std::cout << "Game initialized successfully!" << std::endl;
}

void Game::Update(float deltaTime) {
    if (!m_player) return;
    
    auto* inputManager = m_engine->GetInputManager();
    auto* playerTransform = m_player->GetComponent<FastEngine::Transform>();
    
    if (!playerTransform) return;
    
    // Обработка ввода с клавиатуры
    glm::vec2 movement(0.0f);
    
    if (inputManager->IsKeyPressed(87) || inputManager->IsKeyPressed(265)) { // W или стрелка вверх
        movement.y -= 1.0f;
    }
    if (inputManager->IsKeyPressed(83) || inputManager->IsKeyPressed(264)) { // S или стрелка вниз
        movement.y += 1.0f;
    }
    if (inputManager->IsKeyPressed(65) || inputManager->IsKeyPressed(263)) { // A или стрелка влево
        movement.x -= 1.0f;
    }
    if (inputManager->IsKeyPressed(68) || inputManager->IsKeyPressed(262)) { // D или стрелка вправо
        movement.x += 1.0f;
    }
    
    // Обработка касаний (для мобильных устройств)
    const auto& touches = inputManager->GetActiveTouches();
    for (const auto& touch : touches) {
        if (touch.pressed) {
            // Простое управление касанием - движение к точке касания
            glm::vec2 touchPos = touch.position;
            glm::vec2 playerPos = playerTransform->GetPosition();
            glm::vec2 direction = touchPos - playerPos;
            
            if (glm::length(direction) > 10.0f) { // Минимальное расстояние для движения
                direction = glm::normalize(direction);
                movement += direction * 0.5f; // Замедляем движение касанием
            }
        }
    }
    
    // Нормализуем движение по диагонали
    if (glm::length(movement) > 0.0f) {
        movement = glm::normalize(movement);
    }
    
    // Обновляем позицию игрока
    glm::vec2 currentPos = playerTransform->GetPosition();
    glm::vec2 newPos = currentPos + movement * m_playerSpeed * deltaTime;
    
    // Ограничиваем движение границами экрана
    newPos.x = glm::clamp(newPos.x, 32.0f, 768.0f);
    newPos.y = glm::clamp(newPos.y, 32.0f, 568.0f);
    
    playerTransform->SetPosition(newPos);
    
    // Поворот игрока в направлении движения
    if (glm::length(movement) > 0.0f) {
        float targetRotation = glm::degrees(glm::atan(movement.y, movement.x));
        playerTransform->SetRotation(targetRotation);
    }
    
    // Обновляем врагов (простое движение по кругу)
    for (size_t i = 0; i < m_enemies.size(); ++i) {
        auto* enemyTransform = m_enemies[i]->GetComponent<FastEngine::Transform>();
        if (enemyTransform) {
            float time = m_engine->GetDeltaTime() * 0.5f + i;
            float radius = 100.0f + i * 20.0f;
            float x = 400.0f + glm::cos(time) * radius;
            float y = 300.0f + glm::sin(time) * radius;
            enemyTransform->SetPosition(x, y);
            enemyTransform->SetRotation(time * 30.0f); // Медленный поворот
        }
    }
    
    // Обновляем камеру (следим за игроком)
    if (m_camera) {
        glm::vec2 playerPos = playerTransform->GetPosition();
        glm::vec2 cameraPos = m_camera->GetPosition();
        
        // Плавное следование камеры за игроком
        glm::vec2 targetPos = playerPos;
        glm::vec2 newCameraPos = cameraPos + (targetPos - cameraPos) * 2.0f * deltaTime;
        m_camera->SetPosition(newCameraPos);
    }
}

void Game::Render() {
    // Отрисовка теперь полностью обрабатывается RenderSystem
    // Этот метод оставлен для совместимости и может использоваться
    // для дополнительной отрисовки UI элементов, дебаг информации и т.д.
}

void Game::Shutdown() {
    if (m_camera) {
        delete m_camera;
        m_camera = nullptr;
    }
    
    m_enemies.clear();
    m_player = nullptr;
    m_background = nullptr;
    m_world = nullptr;
}
