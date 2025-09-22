#include <gtest/gtest.h>
#include "FastEngine/Engine.h"
#include "FastEngine/World.h"
#include "FastEngine/Entity.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Components/Animator.h"
#include "FastEngine/Components/RigidBody.h"
#include "FastEngine/Components/Collider.h"
#include "FastEngine/Components/AudioSource.h"
#include "FastEngine/Components/Text.h"
#include <memory>

using namespace FastEngine;

class EngineIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<Engine>();
        engine->Initialize("Test Game", 800, 600);
        world = engine->GetWorld();
    }
    
    void TearDown() override {
        engine->Shutdown();
        engine.reset();
    }
    
    std::unique_ptr<Engine> engine;
    World* world;
};

TEST_F(EngineIntegrationTest, EngineInitialization) {
    EXPECT_TRUE(engine != nullptr);
    EXPECT_TRUE(world != nullptr);
    EXPECT_TRUE(engine->IsRunning());
}

TEST_F(EngineIntegrationTest, EntityCreation) {
    // Тест создания сущности
    auto entity = world->CreateEntity();
    EXPECT_TRUE(entity != nullptr);
    EXPECT_GT(entity->GetId(), 0);
}

TEST_F(EngineIntegrationTest, ComponentSystem) {
    // Тест системы компонентов
    auto entity = world->CreateEntity();
    
    // Добавляем компоненты
    auto transform = entity->AddComponent<Transform>();
    auto sprite = entity->AddComponent<Sprite>();
    auto animator = entity->AddComponent<Animator>();
    
    EXPECT_TRUE(transform != nullptr);
    EXPECT_TRUE(sprite != nullptr);
    EXPECT_TRUE(animator != nullptr);
    
    // Проверяем, что компоненты добавлены
    EXPECT_TRUE(entity->HasComponent<Transform>());
    EXPECT_TRUE(entity->HasComponent<Sprite>());
    EXPECT_TRUE(entity->HasComponent<Animator>());
}

TEST_F(EngineIntegrationTest, TransformIntegration) {
    // Тест интеграции трансформации
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<Transform>();
    
    transform->SetPosition(glm::vec2(100.0f, 200.0f));
    transform->SetRotation(45.0f);
    transform->SetScale(glm::vec2(2.0f, 1.5f));
    
    auto position = transform->GetPosition();
    auto rotation = transform->GetRotation();
    auto scale = transform->GetScale();
    
    EXPECT_FLOAT_EQ(position.x, 100.0f);
    EXPECT_FLOAT_EQ(position.y, 200.0f);
    EXPECT_FLOAT_EQ(rotation, 45.0f);
    EXPECT_FLOAT_EQ(scale.x, 2.0f);
    EXPECT_FLOAT_EQ(scale.y, 1.5f);
}

TEST_F(EngineIntegrationTest, SpriteIntegration) {
    // Тест интеграции спрайта
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    
    sprite->SetSize(glm::vec2(100.0f, 100.0f));
    sprite->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    sprite->SetVisible(true);
    
    auto size = sprite->GetSize();
    auto color = sprite->GetColor();
    
    EXPECT_FLOAT_EQ(size.x, 100.0f);
    EXPECT_FLOAT_EQ(size.y, 100.0f);
    EXPECT_FLOAT_EQ(color.r, 1.0f);
    EXPECT_FLOAT_EQ(color.g, 0.0f);
    EXPECT_FLOAT_EQ(color.b, 0.0f);
    EXPECT_FLOAT_EQ(color.a, 1.0f);
    EXPECT_TRUE(sprite->IsVisible());
}

TEST_F(EngineIntegrationTest, AnimationIntegration) {
    // Тест интеграции анимации
    auto entity = world->CreateEntity();
    auto animator = entity->AddComponent<Animator>();
    
    // Создаем анимацию
    Animation walkAnim("walk");
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    walkAnim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
    walkAnim.frames.push_back(AnimationFrame("frame3.png", 0.1f));
    
    animator->AddAnimation(walkAnim);
    animator->Play("walk");
    
    EXPECT_TRUE(animator->IsPlaying());
    EXPECT_EQ(animator->GetCurrentAnimation(), "walk");
}

TEST_F(EngineIntegrationTest, PhysicsIntegration) {
    // Тест интеграции физики
    auto entity = world->CreateEntity();
    auto rigidBody = entity->AddComponent<RigidBody>();
    auto collider = entity->AddComponent<Collider>();
    
    rigidBody->SetMass(1.0f);
    rigidBody->SetBodyType(BodyType::Dynamic);
    
    collider->SetType(ColliderType::Box);
    collider->SetSize(glm::vec2(50.0f, 50.0f));
    
    EXPECT_FLOAT_EQ(rigidBody->GetMass(), 1.0f);
    EXPECT_EQ(rigidBody->GetBodyType(), BodyType::Dynamic);
    EXPECT_EQ(collider->GetType(), ColliderType::Box);
}

TEST_F(EngineIntegrationTest, AudioIntegration) {
    // Тест интеграции аудио
    auto entity = world->CreateEntity();
    auto audioSource = entity->AddComponent<AudioSource>();
    
    audioSource->SetAudioClip("test_sound.wav");
    audioSource->SetVolume(0.5f);
    audioSource->SetLoop(true);
    
    EXPECT_EQ(audioSource->GetAudioClip(), "test_sound.wav");
    EXPECT_FLOAT_EQ(audioSource->GetVolume(), 0.5f);
    EXPECT_TRUE(audioSource->IsLooping());
}

TEST_F(EngineIntegrationTest, TextIntegration) {
    // Тест интеграции текста
    auto entity = world->CreateEntity();
    auto text = entity->AddComponent<Text>();
    
    text->SetText("Hello, World!");
    text->SetFontSize(24);
    text->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    EXPECT_EQ(text->GetText(), "Hello, World!");
    EXPECT_EQ(text->GetFontSize(), 24);
    
    auto color = text->GetColor();
    EXPECT_FLOAT_EQ(color.r, 1.0f);
    EXPECT_FLOAT_EQ(color.g, 1.0f);
    EXPECT_FLOAT_EQ(color.b, 1.0f);
    EXPECT_FLOAT_EQ(color.a, 1.0f);
}

TEST_F(EngineIntegrationTest, MultipleEntities) {
    // Тест множественных сущностей
    const int entityCount = 100;
    std::vector<std::shared_ptr<Entity>> entities;
    
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<Transform>();
        auto sprite = entity->AddComponent<Sprite>();
        
        transform->SetPosition(glm::vec2(i * 10.0f, i * 10.0f));
        sprite->SetSize(glm::vec2(50.0f, 50.0f));
        sprite->SetColor(glm::vec4(i / 100.0f, 0.0f, 1.0f - i / 100.0f, 1.0f));
        
        entities.push_back(entity);
    }
    
    EXPECT_EQ(entities.size(), entityCount);
    
    // Проверяем, что все сущности созданы корректно
    for (const auto& entity : entities) {
        EXPECT_TRUE(entity->HasComponent<Transform>());
        EXPECT_TRUE(entity->HasComponent<Sprite>());
    }
}

TEST_F(EngineIntegrationTest, EntityDestruction) {
    // Тест уничтожения сущностей
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<Transform>();
    auto sprite = entity->AddComponent<Sprite>();
    
    EXPECT_TRUE(entity->HasComponent<Transform>());
    EXPECT_TRUE(entity->HasComponent<Sprite>());
    
    // Удаляем сущность
    world->DestroyEntity(entity->GetId());
    
    // Сущность должна быть удалена
    auto retrievedEntity = world->GetEntity(entity->GetId());
    EXPECT_TRUE(retrievedEntity == nullptr);
}

TEST_F(EngineIntegrationTest, ComponentRemoval) {
    // Тест удаления компонентов
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<Transform>();
    auto sprite = entity->AddComponent<Sprite>();
    
    EXPECT_TRUE(entity->HasComponent<Transform>());
    EXPECT_TRUE(entity->HasComponent<Sprite>());
    
    // Удаляем компонент
    entity->RemoveComponent<Transform>();
    
    EXPECT_FALSE(entity->HasComponent<Transform>());
    EXPECT_TRUE(entity->HasComponent<Sprite>());
}

TEST_F(EngineIntegrationTest, EntityUpdate) {
    // Тест обновления сущностей
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<Transform>();
    auto sprite = entity->AddComponent<Sprite>();
    
    transform->SetPosition(glm::vec2(0.0f, 0.0f));
    sprite->SetSize(glm::vec2(100.0f, 100.0f));
    
    // Обновляем сущность
    entity->Update(0.016f);
    
    // Проверяем, что обновление прошло без ошибок
    EXPECT_TRUE(true);
}

TEST_F(EngineIntegrationTest, EngineUpdate) {
    // Тест обновления движка
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<Transform>();
    auto sprite = entity->AddComponent<Sprite>();
    
    transform->SetPosition(glm::vec2(100.0f, 100.0f));
    sprite->SetSize(glm::vec2(50.0f, 50.0f));
    
    // Обновляем движок
    engine->Update(0.016f);
    
    // Проверяем, что обновление прошло без ошибок
    EXPECT_TRUE(true);
}

TEST_F(EngineIntegrationTest, EngineRender) {
    // Тест рендеринга движка
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<Transform>();
    auto sprite = entity->AddComponent<Sprite>();
    
    transform->SetPosition(glm::vec2(200.0f, 200.0f));
    sprite->SetSize(glm::vec2(100.0f, 100.0f));
    sprite->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    
    // Рендерим движок
    engine->Render();
    
    // Проверяем, что рендеринг прошел без ошибок
    EXPECT_TRUE(true);
}

TEST_F(EngineIntegrationTest, EnginePerformance) {
    // Тест производительности движка
    const int entityCount = 1000;
    std::vector<std::shared_ptr<Entity>> entities;
    
    // Создаем много сущностей
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<Transform>();
        auto sprite = entity->AddComponent<Sprite>();
        
        transform->SetPosition(glm::vec2(i % 50 * 20.0f, i / 50 * 20.0f));
        sprite->SetSize(glm::vec2(15.0f, 15.0f));
        sprite->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        
        entities.push_back(entity);
    }
    
    // Измеряем время обновления
    auto start = std::chrono::high_resolution_clock::now();
    
    engine->Update(0.016f);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Обновление 1000 сущностей должно занять менее 100мс
    EXPECT_LT(duration.count(), 100);
}

TEST_F(EngineIntegrationTest, EngineShutdown) {
    // Тест завершения работы движка
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<Transform>();
    auto sprite = entity->AddComponent<Sprite>();
    
    transform->SetPosition(glm::vec2(100.0f, 100.0f));
    sprite->SetSize(glm::vec2(50.0f, 50.0f));
    
    // Завершаем работу движка
    engine->Shutdown();
    
    // Проверяем, что движок остановлен
    EXPECT_FALSE(engine->IsRunning());
}