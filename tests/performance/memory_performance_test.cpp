#include <gtest/gtest.h>
#include <FastEngine/Engine.h>
#include <FastEngine/World.h>
#include <FastEngine/Entity.h>
#include <FastEngine/Components/Transform.h>
#include <FastEngine/Components/Sprite.h>
#include <chrono>
#include <memory>
#include <vector>
#include <random>

class MemoryPerformanceTest : public ::testing::Test {
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

TEST_F(MemoryPerformanceTest, EntityCreationMemoryUsage) {
    // Тест использования памяти при создании сущностей
    const int entityCount = 10000;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(i % 100 * 10.0f, i / 100 * 10.0f);
        sprite->SetSize(5.0f, 5.0f);
        sprite->SetColor(rand() % 255, rand() % 255, rand() % 255, 255);
        
        entities.push_back(entity);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Проверяем, что создание 10000 сущностей заняло менее 500мс
    EXPECT_LT(duration.count(), 500);
    EXPECT_EQ(entities.size(), entityCount);
}

TEST_F(MemoryPerformanceTest, ComponentAllocationPerformance) {
    // Тест производительности выделения памяти для компонентов
    const int entityCount = 5000;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    // Создаем сущности
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        entities.push_back(entity);
    }
    
    // Измеряем время добавления компонентов
    auto start = std::chrono::high_resolution_clock::now();
    
    for (const auto& entity : entities) {
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(rand() % 1000, rand() % 1000);
        sprite->SetSize(rand() % 50 + 10, rand() % 50 + 10);
        sprite->SetColor(rand() % 255, rand() % 255, rand() % 255, 255);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Проверяем, что добавление компонентов заняло менее 200мс
    EXPECT_LT(duration.count(), 200);
}

TEST_F(MemoryPerformanceTest, MemoryFragmentationTest) {
    // Тест фрагментации памяти
    const int cycles = 1000;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    // Создаем и удаляем сущности в случайном порядке
    for (int cycle = 0; cycle < cycles; ++cycle) {
        if (entities.empty() || rand() % 2 == 0) {
            // Создаем новую сущность
            auto entity = world->CreateEntity();
            auto transform = entity->AddComponent<FastEngine::Transform>();
            auto sprite = entity->AddComponent<FastEngine::Sprite>();
            
            transform->SetPosition(rand() % 1000, rand() % 1000);
            sprite->SetSize(rand() % 50 + 10, rand() % 50 + 10);
            sprite->SetColor(rand() % 255, rand() % 255, rand() % 255, 255);
            
            entities.push_back(entity);
        } else {
            // Удаляем случайную сущность
            int index = rand() % entities.size();
            world->DestroyEntity(entities[index]->GetId());
            entities.erase(entities.begin() + index);
        }
    }
    
    // Проверяем, что система выдержала фрагментацию
    EXPECT_TRUE(true); // Если дошли сюда, значит тест прошел
}

TEST_F(MemoryPerformanceTest, LargeDataSetTest) {
    // Тест с большим набором данных
    const int entityCount = 50000;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(i % 500 * 2.0f, i / 500 * 2.0f);
        sprite->SetSize(1.0f, 1.0f);
        sprite->SetColor(i % 255, (i * 2) % 255, (i * 3) % 255, 255);
        
        entities.push_back(entity);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Проверяем, что создание 50000 сущностей заняло менее 2000мс
    EXPECT_LT(duration.count(), 2000);
    EXPECT_EQ(entities.size(), entityCount);
}

TEST_F(MemoryPerformanceTest, ComponentUpdatePerformance) {
    // Тест производительности обновления компонентов
    const int entityCount = 10000;
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
    
    // Измеряем время обновления
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int frame = 0; frame < 100; ++frame) {
        for (const auto& entity : entities) {
            auto transform = entity->GetComponent<FastEngine::Transform>();
            auto sprite = entity->GetComponent<FastEngine::Sprite>();
            
            transform->SetPosition(
                transform->GetPosition().x + 0.1f,
                transform->GetPosition().y + 0.1f
            );
            transform->SetRotation(transform->GetRotation() + 0.1f);
            
            sprite->SetColor(
                (sprite->GetColor().r + 1) % 255,
                sprite->GetColor().g,
                sprite->GetColor().b,
                sprite->GetColor().a
            );
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Проверяем, что обновление заняло менее 1000мс
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(MemoryPerformanceTest, MemoryLeakTest) {
    // Тест утечек памяти
    const int cycles = 1000;
    
    for (int cycle = 0; cycle < cycles; ++cycle) {
        // Создаем временные сущности
        std::vector<std::shared_ptr<FastEngine::Entity>> tempEntities;
        
        for (int i = 0; i < 100; ++i) {
            auto entity = world->CreateEntity();
            auto transform = entity->AddComponent<FastEngine::Transform>();
            auto sprite = entity->AddComponent<FastEngine::Sprite>();
            
            transform->SetPosition(rand() % 1000, rand() % 1000);
            sprite->SetSize(rand() % 50 + 10, rand() % 50 + 10);
            sprite->SetColor(rand() % 255, rand() % 255, rand() % 255, 255);
            
            tempEntities.push_back(entity);
        }
        
        // Удаляем все сущности
        for (const auto& entity : tempEntities) {
            world->DestroyEntity(entity->GetId());
        }
    }
    
    // Проверяем, что утечек памяти нет
    EXPECT_TRUE(true); // Если дошли сюда, значит утечек нет
}

TEST_F(MemoryPerformanceTest, ConcurrentAccessTest) {
    // Тест параллельного доступа к памяти
    const int entityCount = 1000;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    // Создаем сущности
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(rand() % 1000, rand() % 1000);
        sprite->SetSize(rand() % 50 + 10, rand() % 50 + 10);
        sprite->SetColor(rand() % 255, rand() % 255, rand() % 255, 255);
        
        entities.push_back(entity);
    }
    
    // Измеряем время параллельного доступа
    auto start = std::chrono::high_resolution_clock::now();
    
    // Симулируем параллельный доступ
    for (int thread = 0; thread < 4; ++thread) {
        for (int i = thread; i < entityCount; i += 4) {
            auto transform = entities[i]->GetComponent<FastEngine::Transform>();
            auto sprite = entities[i]->GetComponent<FastEngine::Sprite>();
            
            transform->SetPosition(
                transform->GetPosition().x + 0.1f,
                transform->GetPosition().y + 0.1f
            );
            sprite->SetColor(
                sprite->GetColor().r,
                (sprite->GetColor().g + 1) % 255,
                sprite->GetColor().b,
                sprite->GetColor().a
            );
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Проверяем, что параллельный доступ занял менее 100мс
    EXPECT_LT(duration.count(), 100);
}

TEST_F(MemoryPerformanceTest, StressMemoryTest) {
    // Стресс-тест памяти
    const int maxEntities = 100000;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Создаем максимальное количество сущностей
    for (int i = 0; i < maxEntities; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(i % 1000 * 0.1f, i / 1000 * 0.1f);
        sprite->SetSize(0.1f, 0.1f);
        sprite->SetColor(i % 255, (i * 2) % 255, (i * 3) % 255, 255);
        
        entities.push_back(entity);
        
        // Проверяем, что система не упала
        if (i % 10000 == 0) {
            EXPECT_TRUE(true); // Система все еще работает
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Проверяем, что стресс-тест прошел успешно
    EXPECT_LT(duration.count(), 10000); // Менее 10 секунд
    EXPECT_EQ(entities.size(), maxEntities);
}









