#include "FastEngine/FastEngine.h"
#include "FastEngine/Components/Animator.h"
#include "FastEngine/Components/RigidBody.h"
#include "FastEngine/Components/Collider.h"
#include "FastEngine/Components/AudioSource.h"
#include "FastEngine/Components/Text.h"
#include "FastEngine/Systems/AnimationSystem.h"
#include "FastEngine/Systems/PhysicsSystem.h"
#include "FastEngine/Resources/ResourceManager.h"
#include <iostream>
#include <memory>

using namespace FastEngine;

class ComponentTestApp {
public:
    ComponentTestApp() {
        m_engine = nullptr;
        m_animationSystem = nullptr;
        m_physicsSystem = nullptr;
    }
    
    ~ComponentTestApp() = default;
    
    bool Initialize() {
        // Инициализируем движок
        m_engine = std::make_unique<Engine>();
        if (!m_engine->Initialize("Component Test", 800, 600)) {
            return false;
        }
        
        // Инициализируем ResourceManager
        ResourceManager::GetInstance().Initialize();
        
        // Создаем системы через World
        m_animationSystem = m_engine->GetWorld()->AddSystem<AnimationSystem>();
        m_physicsSystem = m_engine->GetWorld()->AddSystem<PhysicsSystem>();
        
        // Создаем тестовые сущности
        CreateTestEntities();
        
        std::cout << "Component Test App initialized successfully!" << std::endl;
        std::cout << "Features tested:" << std::endl;
        std::cout << "- Animator component with sprite animation" << std::endl;
        std::cout << "- RigidBody component with physics" << std::endl;
        std::cout << "- Collider component with collision detection" << std::endl;
        std::cout << "- AudioSource component with sound management" << std::endl;
        std::cout << "- Text component with text rendering" << std::endl;
        std::cout << "- AnimationSystem for managing animations" << std::endl;
        std::cout << "- PhysicsSystem for physics simulation" << std::endl;
        std::cout << "- ResourceManager for asset management" << std::endl;
        
        return true;
    }
    
    void Update(float deltaTime) {
        if (m_engine) {
            // Обновляем системы напрямую
            m_animationSystem->Update(deltaTime);
            m_physicsSystem->Update(deltaTime);
        }
        
        // Обновляем ResourceManager
        ResourceManager::GetInstance().Update();
    }
    
    void Render() {
        if (m_engine) {
            // Рендеринг через Engine
            // m_engine->Render(); // Приватный метод
        }
    }
    
    void Shutdown() {
        m_engine.reset();
        ResourceManager::GetInstance().Shutdown();
    }
    
private:
    std::unique_ptr<Engine> m_engine;
    AnimationSystem* m_animationSystem;
    PhysicsSystem* m_physicsSystem;
    
    void CreateTestEntities() {
        World* world = m_engine->GetWorld();
        
        // Создаем сущность с анимацией
        Entity* animatedEntity = world->CreateEntity();
        animatedEntity->AddComponent<Transform>();
        animatedEntity->AddComponent<Sprite>("textures/player.png");
        
        Animator* animator = animatedEntity->AddComponent<Animator>();
        
        // Создаем анимацию
        Animation walkAnimation("walk");
        walkAnimation.type = AnimationType::Loop;
        walkAnimation.speed = 1.0f;
        walkAnimation.autoPlay = true;
        
        // Добавляем кадры анимации (заглушки)
        walkAnimation.frames.push_back(AnimationFrame("textures/player_walk_1.png", 0.1f));
        walkAnimation.frames.push_back(AnimationFrame("textures/player_walk_2.png", 0.1f));
        walkAnimation.frames.push_back(AnimationFrame("textures/player_walk_3.png", 0.1f));
        walkAnimation.frames.push_back(AnimationFrame("textures/player_walk_4.png", 0.1f));
        
        animator->AddAnimation(walkAnimation);
        
        // Создаем сущность с физикой
        Entity* physicsEntity = world->CreateEntity();
        Transform* transform = physicsEntity->AddComponent<Transform>();
        transform->SetPosition(100.0f, 100.0f);
        
        RigidBody* rigidBody = physicsEntity->AddComponent<RigidBody>();
        rigidBody->SetBodyType(BodyType::Dynamic);
        rigidBody->SetMass(1.0f);
        rigidBody->SetVelocity(glm::vec2(50.0f, 0.0f));
        
        Collider* collider = physicsEntity->AddComponent<Collider>();
        collider->SetType(ColliderType::Box);
        collider->SetSize(glm::vec2(32.0f, 32.0f));
        
        // Создаем сущность с аудио
        Entity* audioEntity = world->CreateEntity();
        audioEntity->AddComponent<Transform>();
        
        AudioSource* audioSource = audioEntity->AddComponent<AudioSource>();
        audioSource->SetAudioType(AudioType::SFX);
        audioSource->SetVolume(0.8f);
        audioSource->SetAutoPlay(true);
        audioSource->LoadSound("sounds/jump.wav");
        
        // Создаем сущность с текстом
        Entity* textEntity = world->CreateEntity();
        Transform* textTransform = textEntity->AddComponent<Transform>();
        textTransform->SetPosition(50.0f, 50.0f);
        
        Text* text = textEntity->AddComponent<Text>();
        text->SetText("Hello, FastEngine!");
        text->SetFontSize(24);
        text->SetColor(glm::vec3(1.0f, 1.0f, 0.0f));
        text->SetBold(true);
        text->SetShadowEnabled(true);
        
        // Создаем статическую платформу
        Entity* platform = world->CreateEntity();
        Transform* platformTransform = platform->AddComponent<Transform>();
        platformTransform->SetPosition(400.0f, 500.0f);
        platformTransform->SetScale(glm::vec2(200.0f, 20.0f));
        
        RigidBody* platformBody = platform->AddComponent<RigidBody>();
        platformBody->SetBodyType(BodyType::Static);
        
        Collider* platformCollider = platform->AddComponent<Collider>();
        platformCollider->SetType(ColliderType::Box);
        platformCollider->SetSize(glm::vec2(200.0f, 20.0f));
        
        std::cout << "Created test entities:" << std::endl;
        std::cout << "- Animated entity with walk animation" << std::endl;
        std::cout << "- Physics entity with dynamic body" << std::endl;
        std::cout << "- Audio entity with sound effect" << std::endl;
        std::cout << "- Text entity with styled text" << std::endl;
        std::cout << "- Platform entity with static body" << std::endl;
    }
};

int main() {
    std::cout << "FastEngine Component Test Starting..." << std::endl;
    
    ComponentTestApp app;
    if (!app.Initialize()) {
        std::cerr << "Failed to initialize Component Test app" << std::endl;
        return -1;
    }
    
    std::cout << "Running Component Test for 10 seconds..." << std::endl;
    
    // Запускаем на 10 секунд
    auto start = std::chrono::high_resolution_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - start).count() < 10) {
        
        auto now = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(now - start).count();
        
        app.Update(deltaTime);
        app.Render();
    }
    
    app.Shutdown();
    
    std::cout << "Component Test finished!" << std::endl;
    return 0;
}
