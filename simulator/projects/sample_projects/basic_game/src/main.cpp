#include "FastEngine/Engine.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Input/InputManager.h"
#include "FastEngine/Render/Camera.h"
#include <iostream>
#include <vector>
#include <random>

class BasicGame {
public:
    BasicGame(FastEngine::Engine* engine) 
        : m_engine(engine)
        , m_world(nullptr)
        , m_player(nullptr)
        , m_enemies()
        , m_camera(nullptr)
        , m_playerSpeed(200.0f)
        , m_enemySpeed(100.0f)
        , m_spawnTimer(0.0f)
        , m_spawnInterval(2.0f)
        , m_running(true) {
    }

    void Initialize() {
        m_world = m_engine->GetWorld();
        
        // Создаем камеру
        m_camera = new FastEngine::Camera();
        m_camera->SetSize(800.0f, 600.0f);
        m_camera->SetPosition(400.0f, 300.0f);
        
        // Создаем игрока
        m_player = m_world->CreateEntity();
        m_player->AddComponent<FastEngine::Transform>(400.0f, 500.0f);
        auto* playerSprite = m_player->AddComponent<FastEngine::Sprite>("player.png");
        playerSprite->SetSize(32.0f, 32.0f);
        playerSprite->SetColor(0.0f, 1.0f, 0.0f, 1.0f); // Зеленый игрок
        
        std::cout << "Basic Game initialized!" << std::endl;
    }

    void Update(float deltaTime) {
        if (!m_player) return;
        
        auto* inputManager = m_engine->GetInputManager();
        auto* playerTransform = m_player->GetComponent<FastEngine::Transform>();
        
        if (!playerTransform) return;
        
        // Обработка ввода
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
        
        // Обработка касаний
        const auto& touches = inputManager->GetActiveTouches();
        for (const auto& touch : touches) {
            if (touch.pressed) {
                glm::vec2 touchPos = touch.position;
                glm::vec2 playerPos = playerTransform->GetPosition();
                glm::vec2 direction = touchPos - playerPos;
                
                if (glm::length(direction) > 10.0f) {
                    direction = glm::normalize(direction);
                    movement += direction * 0.5f;
                }
            }
        }
        
        // Нормализуем движение
        if (glm::length(movement) > 0.0f) {
            movement = glm::normalize(movement);
        }
        
        // Обновляем позицию игрока
        glm::vec2 currentPos = playerTransform->GetPosition();
        glm::vec2 newPos = currentPos + movement * m_playerSpeed * deltaTime;
        
        // Ограничиваем движение границами экрана
        newPos.x = glm::clamp(newPos.x, 16.0f, 784.0f);
        newPos.y = glm::clamp(newPos.y, 16.0f, 584.0f);
        
        playerTransform->SetPosition(newPos);
        
        // Спавн врагов
        m_spawnTimer += deltaTime;
        if (m_spawnTimer >= m_spawnInterval) {
            SpawnEnemy();
            m_spawnTimer = 0.0f;
        }
        
        // Обновляем врагов
        UpdateEnemies(deltaTime);
        
        // Обновляем камеру
        if (m_camera) {
            glm::vec2 playerPos = playerTransform->GetPosition();
            glm::vec2 cameraPos = m_camera->GetPosition();
            glm::vec2 newCameraPos = cameraPos + (playerPos - cameraPos) * 2.0f * deltaTime;
            m_camera->SetPosition(newCameraPos);
        }
    }

    void SpawnEnemy() {
        auto* enemy = m_world->CreateEntity();
        
        // Случайная позиция сверху экрана
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(50.0f, 750.0f);
        
        float x = dis(gen);
        float y = -50.0f;
        
        enemy->AddComponent<FastEngine::Transform>(x, y);
        auto* enemySprite = enemy->AddComponent<FastEngine::Sprite>("enemy.png");
        enemySprite->SetSize(24.0f, 24.0f);
        enemySprite->SetColor(1.0f, 0.0f, 0.0f, 1.0f); // Красный враг
        
        m_enemies.push_back(enemy);
    }

    void UpdateEnemies(float deltaTime) {
        for (auto it = m_enemies.begin(); it != m_enemies.end();) {
            auto* enemyTransform = (*it)->GetComponent<FastEngine::Transform>();
            if (enemyTransform) {
                glm::vec2 pos = enemyTransform->GetPosition();
                pos.y += m_enemySpeed * deltaTime;
                enemyTransform->SetPosition(pos);
                
                // Удаляем врагов, которые вышли за экран
                if (pos.y > 650.0f) {
                    m_world->DestroyEntity(*it);
                    it = m_enemies.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }

    void Render() {
        // Отрисовка обрабатывается RenderSystem
    }

    void Shutdown() {
        if (m_camera) {
            delete m_camera;
            m_camera = nullptr;
        }
        
        m_enemies.clear();
        m_player = nullptr;
        m_world = nullptr;
    }

private:
    FastEngine::Engine* m_engine;
    FastEngine::World* m_world;
    FastEngine::Entity* m_player;
    std::vector<FastEngine::Entity*> m_enemies;
    FastEngine::Camera* m_camera;
    
    float m_playerSpeed;
    float m_enemySpeed;
    float m_spawnTimer;
    float m_spawnInterval;
    bool m_running;
};

int main() {
    // Создаем движок
    FastEngine::Engine engine;
    
    // Инициализируем
    if (!engine.Initialize("Basic Game", 800, 600)) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return -1;
    }
    
    // Создаем игру
    BasicGame game(&engine);
    game.Initialize();
    
    // Главный цикл
    while (engine.IsRunning()) {
        float deltaTime = engine.GetDeltaTime();
        game.Update(deltaTime);
        engine.Update();
    }
    
    game.Shutdown();
    return 0;
}

