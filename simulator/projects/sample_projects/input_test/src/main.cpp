#include "FastEngine/Engine.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Input/InputManager.h"
#include <iostream>
#include <vector>
#include <string>

class InputTest {
public:
    InputTest(FastEngine::Engine* engine) 
        : m_engine(engine)
        , m_world(nullptr)
        , m_testEntity(nullptr)
        , m_inputLog()
        , m_maxLogEntries(20) {
    }

    void Initialize() {
        m_world = m_engine->GetWorld();
        
        // Создаем тестовую сущность
        m_testEntity = m_world->CreateEntity();
        m_testEntity->AddComponent<FastEngine::Transform>(400.0f, 300.0f);
        auto* sprite = m_testEntity->AddComponent<FastEngine::Sprite>("test.png");
        sprite->SetSize(32.0f, 32.0f);
        sprite->SetColor(1.0f, 1.0f, 0.0f, 1.0f); // Желтый
        
        AddLogMessage("Input Test initialized. Use WASD/Arrow keys to move, Space to log input state.");
    }

    void Update(float deltaTime) {
        if (!m_testEntity) return;
        
        auto* inputManager = m_engine->GetInputManager();
        auto* transform = m_testEntity->GetComponent<FastEngine::Transform>();
        
        if (!transform) return;
        
        // Обработка клавиатуры
        glm::vec2 movement(0.0f);
        bool keyPressed = false;
        
        if (inputManager->IsKeyPressed(87) || inputManager->IsKeyPressed(265)) { // W или стрелка вверх
            movement.y -= 1.0f;
            keyPressed = true;
        }
        if (inputManager->IsKeyPressed(83) || inputManager->IsKeyPressed(264)) { // S или стрелка вниз
            movement.y += 1.0f;
            keyPressed = true;
        }
        if (inputManager->IsKeyPressed(65) || inputManager->IsKeyPressed(263)) { // A или стрелка влево
            movement.x -= 1.0f;
            keyPressed = true;
        }
        if (inputManager->IsKeyPressed(68) || inputManager->IsKeyPressed(262)) { // D или стрелка вправо
            movement.x += 1.0f;
            keyPressed = true;
        }
        
        // Логируем нажатия клавиш
        if (keyPressed) {
            AddLogMessage("Keyboard input detected: " + 
                         std::to_string(movement.x) + ", " + std::to_string(movement.y));
        }
        
        // Обработка касаний
        const auto& touches = inputManager->GetActiveTouches();
        for (const auto& touch : touches) {
            if (touch.pressed) {
                AddLogMessage("Touch detected at: " + 
                             std::to_string(touch.position.x) + ", " + std::to_string(touch.position.y));
                
                // Движение к точке касания
                glm::vec2 touchPos = touch.position;
                glm::vec2 currentPos = transform->GetPosition();
                glm::vec2 direction = touchPos - currentPos;
                
                if (glm::length(direction) > 10.0f) {
                    direction = glm::normalize(direction);
                    movement += direction * 0.5f;
                }
            }
        }
        
        // Обновляем позицию
        if (glm::length(movement) > 0.0f) {
            movement = glm::normalize(movement);
            glm::vec2 currentPos = transform->GetPosition();
            glm::vec2 newPos = currentPos + movement * 200.0f * deltaTime;
            
            // Ограничиваем движение границами экрана
            newPos.x = glm::clamp(newPos.x, 16.0f, 784.0f);
            newPos.y = glm::clamp(newPos.y, 16.0f, 584.0f);
            
            transform->SetPosition(newPos);
        }
        
        // Проверяем специальные клавиши
        if (inputManager->IsKeyPressed(32)) { // Пробел
            AddLogMessage("Space key pressed - Current position: " + 
                         std::to_string(transform->GetPosition().x) + ", " + 
                         std::to_string(transform->GetPosition().y));
        }
        
        if (inputManager->IsKeyPressed(27)) { // Escape
            AddLogMessage("Escape key pressed - Exiting test");
            m_engine->Stop();
        }
        
        // Логируем состояние ввода каждые 2 секунды
        static float logTimer = 0.0f;
        logTimer += deltaTime;
        if (logTimer >= 2.0f) {
            LogInputState();
            logTimer = 0.0f;
        }
    }

    void LogInputState() {
        auto* inputManager = m_engine->GetInputManager();
        
        std::string state = "Input State - ";
        
        // Проверяем основные клавиши
        if (inputManager->IsKeyPressed(87)) state += "W ";
        if (inputManager->IsKeyPressed(83)) state += "S ";
        if (inputManager->IsKeyPressed(65)) state += "A ";
        if (inputManager->IsKeyPressed(68)) state += "D ";
        if (inputManager->IsKeyPressed(32)) state += "SPACE ";
        if (inputManager->IsKeyPressed(27)) state += "ESC ";
        
        // Проверяем касания
        const auto& touches = inputManager->GetActiveTouches();
        if (!touches.empty()) {
            state += "TOUCH(" + std::to_string(touches.size()) + ") ";
        }
        
        AddLogMessage(state);
    }

    void AddLogMessage(const std::string& message) {
        m_inputLog.push_back(message);
        
        // Ограничиваем размер лога
        if (m_inputLog.size() > m_maxLogEntries) {
            m_inputLog.erase(m_inputLog.begin());
        }
        
        // Выводим в консоль
        std::cout << "[InputTest] " << message << std::endl;
    }

    void Render() {
        // Отрисовка обрабатывается RenderSystem
        // Здесь можно добавить отрисовку лога на экран
    }

    void Shutdown() {
        m_testEntity = nullptr;
        m_world = nullptr;
        m_inputLog.clear();
    }

private:
    FastEngine::Engine* m_engine;
    FastEngine::World* m_world;
    FastEngine::Entity* m_testEntity;
    std::vector<std::string> m_inputLog;
    size_t m_maxLogEntries;
};

int main() {
    // Создаем движок
    FastEngine::Engine engine;
    
    // Инициализируем
    if (!engine.Initialize("Input Test", 800, 600)) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return -1;
    }
    
    // Создаем тест
    InputTest test(&engine);
    test.Initialize();
    
    // Главный цикл
    while (engine.IsRunning()) {
        float deltaTime = engine.GetDeltaTime();
        test.Update(deltaTime);
        engine.Update();
    }
    
    test.Shutdown();
    return 0;
}

