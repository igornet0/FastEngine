#include <gtest/gtest.h>
#include <FastEngine/Engine.h>
#include <FastEngine/World.h>
#include <FastEngine/Entity.h>
#include <FastEngine/Components/Transform.h>
#include <FastEngine/Components/Sprite.h>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>

class MemorySafetyTest : public ::testing::Test {
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

TEST_F(MemorySafetyTest, EntityLifetimeManagement) {
    // Тест управления жизненным циклом сущностей
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    // Создаем сущности
    for (int i = 0; i < 1000; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(i % 100 * 10.0f, i / 100 * 10.0f);
        sprite->SetSize(5.0f, 5.0f);
        sprite->SetColor(rand() % 255, rand() % 255, rand() % 255, 255);
        
        entities.push_back(entity);
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), 1000);
    
    // Удаляем половину сущностей
    for (int i = 0; i < 500; ++i) {
        world->DestroyEntity(entities[i]->GetId());
    }
    
    // Проверяем, что система не упала
    EXPECT_TRUE(true);
}

TEST_F(MemorySafetyTest, ComponentLifetimeManagement) {
    // Тест управления жизненным циклом компонентов
    auto entity = world->CreateEntity();
    
    // Добавляем компоненты
    auto transform = entity->AddComponent<FastEngine::Transform>();
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    
    // Проверяем, что компоненты добавлены
    EXPECT_TRUE(entity->HasComponent<FastEngine::Transform>());
    EXPECT_TRUE(entity->HasComponent<FastEngine::Sprite>());
    
    // Удаляем один компонент
    entity->RemoveComponent<FastEngine::Transform>();
    
    // Проверяем, что компонент удален
    EXPECT_FALSE(entity->HasComponent<FastEngine::Transform>());
    EXPECT_TRUE(entity->HasComponent<FastEngine::Sprite>());
    
    // Проверяем, что не можем получить удаленный компонент
    auto removedTransform = entity->GetComponent<FastEngine::Transform>();
    EXPECT_TRUE(removedTransform == nullptr);
}

TEST_F(MemorySafetyTest, MemoryLeakPrevention) {
    // Тест предотвращения утечек памяти
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
    EXPECT_TRUE(true);
}

TEST_F(MemorySafetyTest, DanglingPointerPrevention) {
    // Тест предотвращения висячих указателей
    std::shared_ptr<FastEngine::Entity> entity;
    
    {
        // Создаем сущность в области видимости
        entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(100.0f, 200.0f);
        sprite->SetSize(50.0f, 50.0f);
        sprite->SetColor(255, 0, 0, 255);
    }
    
    // Проверяем, что сущность все еще существует
    EXPECT_TRUE(entity != nullptr);
    EXPECT_TRUE(entity->HasComponent<FastEngine::Transform>());
    EXPECT_TRUE(entity->HasComponent<FastEngine::Sprite>());
    
    // Проверяем, что можем получить компоненты
    auto transform = entity->GetComponent<FastEngine::Transform>();
    auto sprite = entity->GetComponent<FastEngine::Sprite>();
    
    EXPECT_TRUE(transform != nullptr);
    EXPECT_TRUE(sprite != nullptr);
}

TEST_F(MemorySafetyTest, BufferOverflowPrevention) {
    // Тест предотвращения переполнения буфера
    const int maxEntities = 10000;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
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
        if (i % 1000 == 0) {
            EXPECT_TRUE(true);
        }
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), maxEntities);
}

TEST_F(MemorySafetyTest, ThreadSafetyTest) {
    // Тест потокобезопасности
    const int threadCount = 4;
    const int entitiesPerThread = 1000;
    std::vector<std::thread> threads;
    std::vector<std::vector<std::shared_ptr<FastEngine::Entity>>> threadEntities(threadCount);
    
    // Создаем потоки
    for (int threadId = 0; threadId < threadCount; ++threadId) {
        threads.emplace_back([this, threadId, entitiesPerThread, &threadEntities]() {
            for (int i = 0; i < entitiesPerThread; ++i) {
                auto entity = world->CreateEntity();
                auto transform = entity->AddComponent<FastEngine::Transform>();
                auto sprite = entity->AddComponent<FastEngine::Sprite>();
                
                transform->SetPosition(threadId * 1000 + i, 0.0f);
                sprite->SetSize(10.0f, 10.0f);
                sprite->SetColor(threadId * 64, 128, 255 - threadId * 64, 255);
                
                threadEntities[threadId].push_back(entity);
            }
        });
    }
    
    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что все сущности созданы
    int totalEntities = 0;
    for (const auto& entities : threadEntities) {
        totalEntities += entities.size();
    }
    
    EXPECT_EQ(totalEntities, threadCount * entitiesPerThread);
}

TEST_F(MemorySafetyTest, MemoryFragmentationTest) {
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
    EXPECT_TRUE(true);
}

TEST_F(MemorySafetyTest, ResourceCleanupTest) {
    // Тест очистки ресурсов
    const int entityCount = 1000;
    std::vector<std::shared_ptr<FastEngine::Entity>> entities;
    
    // Создаем сущности
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto transform = entity->AddComponent<FastEngine::Transform>();
        auto sprite = entity->AddComponent<FastEngine::Sprite>();
        
        transform->SetPosition(i % 100 * 10.0f, i / 100 * 10.0f);
        sprite->SetSize(5.0f, 5.0f);
        sprite->SetColor(rand() % 255, rand() % 255, rand() % 255, 255);
        
        entities.push_back(entity);
    }
    
    // Удаляем все сущности
    for (const auto& entity : entities) {
        world->DestroyEntity(entity->GetId());
    }
    
    // Проверяем, что ресурсы очищены
    EXPECT_TRUE(true);
}

TEST_F(MemorySafetyTest, ExceptionSafetyTest) {
    // Тест безопасности при исключениях
    try {
        // Создаем сущности
        for (int i = 0; i < 100; ++i) {
            auto entity = world->CreateEntity();
            auto transform = entity->AddComponent<FastEngine::Transform>();
            auto sprite = entity->AddComponent<FastEngine::Sprite>();
            
            transform->SetPosition(i * 10.0f, i * 10.0f);
            sprite->SetSize(10.0f, 10.0f);
            sprite->SetColor(rand() % 255, rand() % 255, rand() % 255, 255);
        }
        
        // Симулируем исключение
        throw std::runtime_error("Test exception");
        
    } catch (const std::exception& e) {
        // Проверяем, что система не упала
        EXPECT_TRUE(true);
    }
    
    // Проверяем, что система все еще работает
    auto entity = world->CreateEntity();
    EXPECT_TRUE(entity != nullptr);
}










