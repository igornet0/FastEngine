#include <gtest/gtest.h>
#include <FastEngine/Engine.h>
#include <FastEngine/World.h>
#include <FastEngine/Entity.h>
#include <FastEngine/Components/Transform.h>
#include <FastEngine/Components/Sprite.h>
#include <FastEngine/Render/Renderer.h>
#include <chrono>
#include <memory>
#include <vector>

class RenderingPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<FastEngine::Engine>();
        engine->Initialize();
        
        world = std::make_unique<FastEngine::World>();
        world->Initialize();
        
        renderer = std::make_unique<FastEngine::Renderer>();
        renderer->Initialize();
    }
    
    void TearDown() override {
        renderer->Shutdown();
        renderer.reset();
        world.reset();
        engine->Shutdown();
        engine.reset();
    }
    
    std::unique_ptr<FastEngine::Engine> engine;
    std::unique_ptr<FastEngine::World> world;
    std::unique_ptr<FastEngine::Renderer> renderer;
};

TEST_F(RenderingPerformanceTest, EntityCreationPerformance) {
    // Тест производительности создания сущностей
    const int entityCount = 1000;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(i % 50 * 20.0f, i / 50 * 20.0f);
        sprite->SetSize(15.0f, 15.0f);
        sprite->SetColor(rand() % 255, rand() % 255, rand() % 255, 255);
        
        entities.push_back(entity);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Проверяем, что создание 1000 сущностей заняло менее 100мс
    EXPECT_LT(duration.count(), 100);
    EXPECT_EQ(entities.size(), entityCount);
}

TEST_F(RenderingPerformanceTest, RenderingPerformance) {
    // Тест производительности рендеринга
    const int entityCount = 500;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    // Создаем сущности
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(i % 25 * 30.0f, i / 25 * 30.0f);
        sprite->SetSize(20.0f, 20.0f);
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
    
    // Проверяем, что рендеринг 500 сущностей занял менее 50мс
    EXPECT_LT(duration.count(), 50);
}

TEST_F(RenderingPerformanceTest, TransformUpdatePerformance) {
    // Тест производительности обновления трансформаций
    const int entityCount = 1000;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    // Создаем сущности
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(0.0f, 0.0f);
        sprite->SetSize(10.0f, 10.0f);
        
        entities.push_back(entity);
    }
    
    // Измеряем время обновления трансформаций
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int frame = 0; frame < 60; ++frame) { // 60 кадров
        for (int i = 0; i < entityCount; ++i) {
            auto transform = entities[i]->GetComponent<FastEngine::Transform>();
            transform->SetPosition(
                transform->GetPosition().x + 1.0f,
                transform->GetPosition().y + 0.5f
            );
            transform->SetRotation(transform->GetRotation() + 1.0f);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Проверяем, что обновление 1000 сущностей за 60 кадров заняло менее 200мс
    EXPECT_LT(duration.count(), 200);
}

TEST_F(RenderingPerformanceTest, MemoryUsagePerformance) {
    // Тест производительности использования памяти
    const int entityCount = 2000;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    // Создаем много сущностей
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(rand() % 1000, rand() % 1000);
        sprite->SetSize(rand() % 50 + 10, rand() % 50 + 10);
        sprite->SetColor(rand() % 255, rand() % 255, rand() % 255, 255);
        
        entities.push_back(entity);
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), entityCount);
    
    // Проверяем, что можем получить доступ ко всем компонентам
    for (const auto& entity : entities) {
        EXPECT_TRUE(entity->HasComponent<FastEngine::Transform>());
        EXPECT_TRUE(entity->HasComponent<FastEngine::Sprite>());
    }
}

TEST_F(RenderingPerformanceTest, BatchRenderingPerformance) {
    // Тест производительности пакетного рендеринга
    const int batchSize = 100;
    const int batchCount = 10;
    
    std::vector<std::vector<std::shared_ptr<FastEngine::Entity>>> batches;
    
    // Создаем пакеты сущностей
    for (int batch = 0; batch < batchCount; ++batch) {
        std::vector<std::shared_ptr<FastEngine::Entity>> batch;
        
        for (int i = 0; i < batchSize; ++i) {
            auto entity = world->CreateEntity();
            auto transform = entity->AddComponent<FastEngine::Transform>();
            auto sprite = entity->AddComponent<FastEngine::Sprite>();
            
            transform->SetPosition(batch * 100 + i % 10 * 10, i / 10 * 10);
            sprite->SetSize(8.0f, 8.0f);
            sprite->SetColor(batch * 25, 255 - batch * 25, 128, 255);
            
            batch.push_back(entity);
        }
        
        batches.push_back(batch);
    }
    
    // Измеряем время пакетного рендеринга
    auto start = std::chrono::high_resolution_clock::now();
    
    renderer->BeginFrame();
    for (const auto& batch : batches) {
        for (const auto& entity : batch) {
            renderer->RenderEntity(entity);
        }
    }
    renderer->EndFrame();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Проверяем, что пакетный рендеринг 1000 сущностей занял менее 100мс
    EXPECT_LT(duration.count(), 100);
}

TEST_F(RenderingPerformanceTest, FrameRateStability) {
    // Тест стабильности частоты кадров
    const int entityCount = 200;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    // Создаем сущности
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(i % 20 * 25.0f, i / 20 * 25.0f);
        sprite->SetSize(20.0f, 20.0f);
        sprite->SetColor(rand() % 255, rand() % 255, rand() % 255, 255);
        
        entities.push_back(entity);
    }
    
    std::vector<long> frameTimes;
    
    // Измеряем время каждого кадра
    for (int frame = 0; frame < 30; ++frame) {
        auto start = std::chrono::high_resolution_clock::now();
        
        renderer->BeginFrame();
        for (const auto& entity : entities) {
            renderer->RenderEntity(entity);
        }
        renderer->EndFrame();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        frameTimes.push_back(duration.count());
    }
    
    // Проверяем, что все кадры отрендерились за разумное время
    for (auto frameTime : frameTimes) {
        EXPECT_LT(frameTime, 16667); // Менее 16.67мс для 60 FPS
    }
}

TEST_F(RenderingPerformanceTest, StressTest) {
    // Стресс-тест рендеринга
    const int entityCount = 5000;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    // Создаем много сущностей
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(rand() % 2000, rand() % 2000);
        sprite->SetSize(rand() % 30 + 5, rand() % 30 + 5);
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
    
    // Проверяем, что стресс-тест прошел успешно
    EXPECT_LT(duration.count(), 500); // Менее 500мс для 5000 сущностей
    EXPECT_EQ(entities.size(), entityCount);
}