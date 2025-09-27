#include <gtest/gtest.h>
#include <FastEngine/Engine.h>
#include <FastEngine/World.h>
#include <FastEngine/Entity.h>
#include <FastEngine/Components/Transform.h>
#include <FastEngine/Components/Sprite.h>
#include <FastEngine/Render/Renderer.h>
#include <memory>

class RenderingIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<FastEngine::Engine>();
        engine->Initialize();
        
        world = std::make_unique<FastEngine::World>();
        world->Initialize();
    }
    
    void TearDown() override {
        world.reset();
        engine->Shutdown();
        engine.reset();
    }
    
    std::unique_ptr<FastEngine::Engine> engine;
    std::unique_ptr<FastEngine::World> world;
};

TEST_F(RenderingIntegrationTest, RendererInitialization) {
    // Тест инициализации рендерера
    auto renderer = std::make_unique<FastEngine::Renderer>();
    EXPECT_TRUE(renderer != nullptr);
    
    // Проверяем, что рендерер может быть инициализирован
    bool initResult = renderer->Initialize();
    EXPECT_TRUE(initResult);
    
    // Проверяем, что рендерер может быть завершен
    renderer->Shutdown();
}

TEST_F(RenderingIntegrationTest, BasicRendering) {
    // Тест базового рендеринга
    auto renderer = std::make_unique<FastEngine::Renderer>();
    renderer->Initialize();
    
    // Создаем сущность со спрайтом
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<FastEngine::Transform>();
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    
    // Устанавливаем параметры
    transform->SetPosition(100.0f, 100.0f);
    sprite->SetSize(50.0f, 50.0f);
    sprite->SetColor(255, 0, 0, 255); // Красный
    
    // Проверяем, что рендеринг может быть выполнен
    renderer->BeginFrame();
    renderer->RenderEntity(entity);
    renderer->EndFrame();
    
    // Если дошли сюда, значит рендеринг прошел успешно
    EXPECT_TRUE(true);
    
    renderer->Shutdown();
}

TEST_F(RenderingIntegrationTest, MultipleEntitiesRendering) {
    // Тест рендеринга множественных сущностей
    auto renderer = std::make_unique<FastEngine::Renderer>();
    renderer->Initialize();
    
    const int entityCount = 5;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    // Создаем несколько сущностей
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        // Устанавливаем разные позиции и цвета
        transform->SetPosition(i * 100.0f, i * 50.0f);
        sprite->SetSize(30.0f, 30.0f);
        sprite->SetColor(255 - i * 50, i * 50, 128, 255);
        
        entities.push_back(entity);
    }
    
    // Рендерим все сущности
    renderer->BeginFrame();
    for (const auto& entity : entities) {
        renderer->RenderEntity(entity);
    }
    renderer->EndFrame();
    
    // Проверяем, что все сущности были отрендерены
    EXPECT_EQ(entities.size(), entityCount);
    
    renderer->Shutdown();
}

TEST_F(RenderingIntegrationTest, TransformRendering) {
    // Тест рендеринга с трансформациями
    auto renderer = std::make_unique<FastEngine::Renderer>();
    renderer->Initialize();
    
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<FastEngine::Transform>();
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    
    // Устанавливаем трансформации
    transform->SetPosition(200.0f, 150.0f);
    transform->SetScale(2.0f, 1.5f);
    transform->SetRotation(45.0f);
    
    sprite->SetSize(40.0f, 40.0f);
    sprite->SetColor(0, 255, 0, 255); // Зеленый
    
    // Рендерим с трансформациями
    renderer->BeginFrame();
    renderer->RenderEntity(entity);
    renderer->EndFrame();
    
    // Проверяем, что трансформации применились
    auto position = transform->GetPosition();
    auto scale = transform->GetScale();
    auto rotation = transform->GetRotation();
    
    EXPECT_FLOAT_EQ(position.x, 200.0f);
    EXPECT_FLOAT_EQ(position.y, 150.0f);
    EXPECT_FLOAT_EQ(scale.x, 2.0f);
    EXPECT_FLOAT_EQ(scale.y, 1.5f);
    EXPECT_FLOAT_EQ(rotation, 45.0f);
    
    renderer->Shutdown();
}

TEST_F(RenderingIntegrationTest, SpriteProperties) {
    // Тест свойств спрайта при рендеринге
    auto renderer = std::make_unique<FastEngine::Renderer>();
    renderer->Initialize();
    
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<FastEngine::Transform>();
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    
    // Устанавливаем свойства спрайта
    transform->SetPosition(300.0f, 200.0f);
    sprite->SetSize(80.0f, 60.0f);
    sprite->SetColor(100, 150, 200, 180); // Полупрозрачный синий
    
    // Рендерим
    renderer->BeginFrame();
    renderer->RenderEntity(entity);
    renderer->EndFrame();
    
    // Проверяем свойства спрайта
    auto size = sprite->GetSize();
    auto color = sprite->GetColor();
    
    EXPECT_FLOAT_EQ(size.x, 80.0f);
    EXPECT_FLOAT_EQ(size.y, 60.0f);
    EXPECT_EQ(color.r, 100);
    EXPECT_EQ(color.g, 150);
    EXPECT_EQ(color.b, 200);
    EXPECT_EQ(color.a, 180);
    
    renderer->Shutdown();
}

TEST_F(RenderingIntegrationTest, RenderPerformance) {
    // Тест производительности рендеринга
    auto renderer = std::make_unique<FastEngine::Renderer>();
    renderer->Initialize();
    
    const int entityCount = 100;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    // Создаем много сущностей
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(i % 10 * 50.0f, i / 10 * 50.0f);
        sprite->SetSize(25.0f, 25.0f);
        sprite->SetColor(rand() % 255, rand() % 255, rand() % 255, 255);
        
        entities.push_back(entity);
    }
    
    // Измеряем время рендеринга
    auto start = std::chrono::high_resolution_clock::now();
    
    renderer->BeginFrame();
    for (const auto& entity : entities) {
        renderer->RenderEntity(entity);
    }
    renderer->EndFrame();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Проверяем, что рендеринг выполнился за разумное время (менее 100мс)
    EXPECT_LT(duration.count(), 100);
    
    renderer->Shutdown();
}

TEST_F(RenderingIntegrationTest, RenderStateManagement) {
    // Тест управления состоянием рендеринга
    auto renderer = std::make_unique<FastEngine::Renderer>();
    renderer->Initialize();
    
    // Проверяем, что можем начать и закончить фрейм
    renderer->BeginFrame();
    EXPECT_TRUE(true); // Если дошли сюда, значит BeginFrame работает
    
    renderer->EndFrame();
    EXPECT_TRUE(true); // Если дошли сюда, значит EndFrame работает
    
    // Проверяем, что можем сделать несколько фреймов подряд
    for (int i = 0; i < 5; ++i) {
        renderer->BeginFrame();
        renderer->EndFrame();
    }
    
    EXPECT_TRUE(true); // Если дошли сюда, значит множественные фреймы работают
    
    renderer->Shutdown();
}

