#include "FastEngine/Engine.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Input/InputManager.h"
#include <iostream>
#include <vector>
#include <cmath>

class SpriteDemo {
public:
    SpriteDemo(FastEngine::Engine* engine) 
        : m_engine(engine)
        , m_world(nullptr)
        , m_sprites()
        , m_time(0.0f) {
    }

    void Initialize() {
        m_world = m_engine->GetWorld();
        
        // Создаем фоновый спрайт
        auto* background = m_world->CreateEntity();
        background->AddComponent<FastEngine::Transform>(512.0f, 384.0f);
        auto* bgSprite = background->AddComponent<FastEngine::Sprite>("background.png");
        bgSprite->SetSize(1024.0f, 768.0f);
        bgSprite->SetColor(0.2f, 0.3f, 0.8f, 1.0f);
        
        // Создаем несколько спрайтов для демонстрации
        CreateSprite("sprite1.png", 200.0f, 200.0f, 1.0f, 0.0f, 0.0f, 1.0f); // Красный
        CreateSprite("sprite2.png", 400.0f, 200.0f, 0.0f, 1.0f, 0.0f, 1.0f); // Зеленый
        CreateSprite("sprite3.png", 600.0f, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f); // Синий
        CreateSprite("sprite1.png", 200.0f, 400.0f, 1.0f, 1.0f, 0.0f, 1.0f); // Желтый
        CreateSprite("sprite2.png", 400.0f, 400.0f, 1.0f, 0.0f, 1.0f, 1.0f); // Пурпурный
        CreateSprite("sprite3.png", 600.0f, 400.0f, 0.0f, 1.0f, 1.0f, 1.0f); // Голубой
        
        std::cout << "Sprite Demo initialized with " << m_sprites.size() << " sprites!" << std::endl;
    }

    void CreateSprite(const std::string& texture, float x, float y, 
                     float r, float g, float b, float a) {
        auto* entity = m_world->CreateEntity();
        entity->AddComponent<FastEngine::Transform>(x, y);
        auto* sprite = entity->AddComponent<FastEngine::Sprite>(texture);
        sprite->SetSize(64.0f, 64.0f);
        sprite->SetColor(r, g, b, a);
        
        m_sprites.push_back(entity);
    }

    void Update(float deltaTime) {
        m_time += deltaTime;
        
        // Анимируем спрайты
        for (size_t i = 0; i < m_sprites.size(); ++i) {
            auto* transform = m_sprites[i]->GetComponent<FastEngine::Transform>();
            if (transform) {
                // Круговое движение
                float radius = 50.0f + i * 20.0f;
                float angle = m_time * (1.0f + i * 0.2f);
                
                float x = 512.0f + std::cos(angle) * radius;
                float y = 384.0f + std::sin(angle) * radius;
                
                transform->SetPosition(x, y);
                transform->SetRotation(angle * 180.0f / M_PI);
                
                // Пульсация размера
                float scale = 1.0f + 0.2f * std::sin(m_time * 2.0f + i);
                transform->SetScale(scale, scale);
            }
        }
        
        // Обработка ввода для изменения скорости
        auto* inputManager = m_engine->GetInputManager();
        if (inputManager->IsKeyPressed(32)) { // Пробел - пауза
            // Можно добавить логику паузы
        }
    }

    void Render() {
        // Отрисовка обрабатывается RenderSystem
    }

    void Shutdown() {
        m_sprites.clear();
        m_world = nullptr;
    }

private:
    FastEngine::Engine* m_engine;
    FastEngine::World* m_world;
    std::vector<FastEngine::Entity*> m_sprites;
    float m_time;
};

int main() {
    // Создаем движок
    FastEngine::Engine engine;
    
    // Инициализируем
    if (!engine.Initialize("Sprite Demo", 1024, 768)) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return -1;
    }
    
    // Создаем демо
    SpriteDemo demo(&engine);
    demo.Initialize();
    
    // Главный цикл
    while (engine.IsRunning()) {
        float deltaTime = engine.GetDeltaTime();
        demo.Update(deltaTime);
        engine.Update();
    }
    
    demo.Shutdown();
    return 0;
}

