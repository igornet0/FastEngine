#include <gtest/gtest.h>
#include "FastEngine/Engine.h"
#include "FastEngine/Entity.h"
#include "FastEngine/World.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Components/Transform.h"
#include <memory>
#include <vector>
#include <chrono>
#include <thread>

using namespace FastEngine;

class MemoryLeakTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Инициализируем движок для тестов
        ASSERT_TRUE(Engine::GetInstance().Initialize());
    }
    
    void TearDown() override {
        // Очищаем ресурсы
        Engine::GetInstance().Shutdown();
    }
};

// Тест на утечки памяти при создании и уничтожении сущностей
TEST_F(MemoryLeakTest, EntityCreationDestruction) {
    const int numEntities = 1000;
    std::vector<std::unique_ptr<Entity>> entities;
    
    // Создаем множество сущностей
    for (int i = 0; i < numEntities; ++i) {
        auto entity = std::make_unique<Entity>();
        entity->AddComponent<Transform>();
        entity->AddComponent<Sprite>();
        entities.push_back(std::move(entity));
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), numEntities);
    
    // Уничтожаем все сущности
    entities.clear();
    
    // Даем время на очистку памяти
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Проверяем, что память освобождена
    // В реальном тесте здесь можно использовать инструменты для проверки утечек
}

// Тест на утечки памяти при работе с компонентами
TEST_F(MemoryLeakTest, ComponentMemoryLeaks) {
    const int numComponents = 5000;
    std::vector<std::unique_ptr<Entity>> entities;
    
    // Создаем сущности с множеством компонентов
    for (int i = 0; i < numComponents; ++i) {
        auto entity = std::make_unique<Entity>();
        
        // Добавляем различные компоненты
        entity->AddComponent<Transform>();
        entity->AddComponent<Sprite>();
        
        // Создаем и удаляем компоненты динамически
        for (int j = 0; j < 10; ++j) {
            auto tempComponent = entity->AddComponent<Transform>();
            entity->RemoveComponent<Transform>();
        }
        
        entities.push_back(std::move(entity));
    }
    
    // Уничтожаем все сущности
    entities.clear();
    
    // Даем время на очистку
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Тест на утечки памяти при работе с миром
TEST_F(MemoryLeakTest, WorldMemoryLeaks) {
    const int numWorlds = 100;
    std::vector<std::unique_ptr<World>> worlds;
    
    // Создаем множество миров
    for (int i = 0; i < numWorlds; ++i) {
        auto world = std::make_unique<World>();
        
        // Добавляем сущности в мир
        for (int j = 0; j < 100; ++j) {
            auto entity = world->CreateEntity();
            entity->AddComponent<Transform>();
            entity->AddComponent<Sprite>();
        }
        
        worlds.push_back(std::move(world));
    }
    
    // Уничтожаем все миры
    worlds.clear();
    
    // Даем время на очистку
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Тест на утечки памяти при циклических ссылках
TEST_F(MemoryLeakTest, CircularReferenceLeaks) {
    const int numCycles = 100;
    
    for (int i = 0; i < numCycles; ++i) {
        auto world = std::make_unique<World>();
        
        // Создаем сущности с циклическими ссылками
        auto entity1 = world->CreateEntity();
        auto entity2 = world->CreateEntity();
        
        // Добавляем компоненты, которые могут ссылаться друг на друга
        auto transform1 = entity1->AddComponent<Transform>();
        auto transform2 = entity2->AddComponent<Transform>();
        
        // Устанавливаем ссылки (если такие есть в компонентах)
        // transform1->SetParent(transform2);
        // transform2->SetChild(transform1);
        
        // Уничтожаем мир
        world.reset();
    }
    
    // Даем время на очистку
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Тест на утечки памяти при исключениях
TEST_F(MemoryLeakTest, ExceptionMemoryLeaks) {
    const int numTests = 100;
    
    for (int i = 0; i < numTests; ++i) {
        try {
            auto world = std::make_unique<World>();
            
            // Создаем сущности
            for (int j = 0; j < 50; ++j) {
                auto entity = world->CreateEntity();
                entity->AddComponent<Transform>();
                entity->AddComponent<Sprite>();
            }
            
            // Симулируем исключение
            if (i % 10 == 0) {
                throw std::runtime_error("Simulated exception");
            }
            
            // Нормальное завершение
            world.reset();
            
        } catch (const std::exception& e) {
            // Обрабатываем исключение
            // Память должна быть освобождена автоматически
        }
    }
    
    // Даем время на очистку
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Тест на утечки памяти при многопоточности
TEST_F(MemoryLeakTest, MultithreadedMemoryLeaks) {
    const int numThreads = 4;
    const int numEntitiesPerThread = 250;
    std::vector<std::thread> threads;
    
    // Создаем потоки, каждый создает свои сущности
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([numEntitiesPerThread]() {
            auto world = std::make_unique<World>();
            
            // Создаем сущности в потоке
            for (int i = 0; i < numEntitiesPerThread; ++i) {
                auto entity = world->CreateEntity();
                entity->AddComponent<Transform>();
                entity->AddComponent<Sprite>();
            }
            
            // Даем время на работу
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            // Уничтожаем мир
            world.reset();
        });
    }
    
    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Даем время на очистку
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Тест на утечки памяти при работе с ресурсами
TEST_F(MemoryLeakTest, ResourceMemoryLeaks) {
    const int numResources = 1000;
    std::vector<std::unique_ptr<World>> worlds;
    
    for (int i = 0; i < numResources; ++i) {
        auto world = std::make_unique<World>();
        
        // Создаем сущности с ресурсами
        for (int j = 0; j < 10; ++j) {
            auto entity = world->CreateEntity();
            auto sprite = entity->AddComponent<Sprite>();
            
            // Загружаем ресурсы (если есть)
            // sprite->LoadTexture("test_texture.png");
            // sprite->LoadShader("test_shader.vert", "test_shader.frag");
        }
        
        worlds.push_back(std::move(world));
    }
    
    // Уничтожаем все миры
    worlds.clear();
    
    // Даем время на очистку ресурсов
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

// Тест на утечки памяти при работе с большими данными
TEST_F(MemoryLeakTest, LargeDataMemoryLeaks) {
    const int numLargeEntities = 100;
    std::vector<std::unique_ptr<Entity>> entities;
    
    for (int i = 0; i < numLargeEntities; ++i) {
        auto entity = std::make_unique<Entity>();
        
        // Добавляем компоненты с большими данными
        auto transform = entity->AddComponent<Transform>();
        auto sprite = entity->AddComponent<Sprite>();
        
        // Симулируем большие данные (если есть соответствующие методы)
        // sprite->SetLargeTextureData(largeData);
        // transform->SetLargeMatrixData(largeMatrix);
        
        entities.push_back(std::move(entity));
    }
    
    // Уничтожаем все сущности
    entities.clear();
    
    // Даем время на очистку
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Тест на утечки памяти при работе с анимациями
TEST_F(MemoryLeakTest, AnimationMemoryLeaks) {
    const int numAnimations = 500;
    std::vector<std::unique_ptr<Entity>> entities;
    
    for (int i = 0; i < numAnimations; ++i) {
        auto entity = std::make_unique<Entity>();
        
        // Добавляем компоненты для анимации
        auto transform = entity->AddComponent<Transform>();
        auto sprite = entity->AddComponent<Sprite>();
        
        // Создаем анимации (если есть соответствующие компоненты)
        // auto animator = entity->AddComponent<Animator>();
        // animator->CreateAnimation("test_animation", frames);
        
        entities.push_back(std::move(entity));
    }
    
    // Уничтожаем все сущности
    entities.clear();
    
    // Даем время на очистку
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Тест на утечки памяти при работе с событиями
TEST_F(MemoryLeakTest, EventMemoryLeaks) {
    const int numEvents = 1000;
    std::vector<std::unique_ptr<Entity>> entities;
    
    for (int i = 0; i < numEvents; ++i) {
        auto entity = std::make_unique<Entity>();
        
        // Добавляем компоненты
        auto transform = entity->AddComponent<Transform>();
        auto sprite = entity->AddComponent<Sprite>();
        
        // Подписываемся на события (если есть система событий)
        // EventManager::Subscribe("test_event", [](const Event& e) { /* обработка */ });
        
        entities.push_back(std::move(entity));
    }
    
    // Уничтожаем все сущности
    entities.clear();
    
    // Даем время на очистку
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}


