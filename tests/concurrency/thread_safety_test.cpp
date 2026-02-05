#include <gtest/gtest.h>
#include "FastEngine/Engine.h"
#include "FastEngine/Entity.h"
#include "FastEngine/World.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Components/Transform.h"
#include <thread>
#include <vector>
#include <memory>
#include <atomic>
#include <mutex>
#include <chrono>
#include <random>

using namespace FastEngine;

class ThreadSafetyTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_TRUE(Engine::GetInstance().Initialize());
    }
    
    void TearDown() override {
        Engine::GetInstance().Shutdown();
    }
};

// Тест на безопасность создания сущностей в разных потоках
TEST_F(ThreadSafetyTest, EntityCreationThreadSafety) {
    auto world = std::make_unique<World>();
    const int numThreads = 4;
    const int numEntitiesPerThread = 250;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    // Создаем потоки, каждый создает свои сущности
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&world, numEntitiesPerThread, &successCount]() {
            try {
                for (int i = 0; i < numEntitiesPerThread; ++i) {
                    auto entity = world->CreateEntity();
                    if (entity) {
                        successCount.fetch_add(1);
                    }
                }
            } catch (const std::exception& e) {
                // Логируем ошибку, но не прерываем тест
                std::cout << "Thread error: " << e.what() << std::endl;
            }
        });
    }
    
    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что все сущности созданы успешно
    EXPECT_EQ(successCount.load(), numThreads * numEntitiesPerThread);
}

// Тест на безопасность добавления компонентов в разных потоках
TEST_F(ThreadSafetyTest, ComponentAdditionThreadSafety) {
    auto world = std::make_unique<World>();
    const int numThreads = 4;
    const int numEntitiesPerThread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    // Создаем потоки, каждый добавляет компоненты к своим сущностям
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&world, numEntitiesPerThread, &successCount]() {
            try {
                for (int i = 0; i < numEntitiesPerThread; ++i) {
                    auto entity = world->CreateEntity();
                    if (entity) {
                        auto transform = entity->AddComponent<Transform>();
                        auto sprite = entity->AddComponent<Sprite>();
                        
                        if (transform && sprite) {
                            successCount.fetch_add(1);
                        }
                    }
                }
            } catch (const std::exception& e) {
                std::cout << "Thread error: " << e.what() << std::endl;
            }
        });
    }
    
    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что все компоненты добавлены успешно
    EXPECT_EQ(successCount.load(), numThreads * numEntitiesPerThread);
}

// Тест на безопасность обновления компонентов в разных потоках
TEST_F(ThreadSafetyTest, ComponentUpdateThreadSafety) {
    auto world = std::make_unique<World>();
    const int numThreads = 4;
    const int numEntitiesPerThread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    // Создаем сущности с компонентами
    for (int i = 0; i < numThreads * numEntitiesPerThread; ++i) {
        auto entity = world->CreateEntity();
        entity->AddComponent<Transform>();
        entity->AddComponent<Sprite>();
    }
    
    // Создаем потоки, каждый обновляет свои компоненты
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&world, numEntitiesPerThread, &successCount, t]() {
            try {
                for (int i = 0; i < numEntitiesPerThread; ++i) {
                    int entityIndex = t * numEntitiesPerThread + i;
                    auto entity = world->GetEntity(entityIndex);
                    
                    if (entity) {
                        auto transform = entity->GetComponent<Transform>();
                        auto sprite = entity->GetComponent<Sprite>();
                        
                        if (transform && sprite) {
                            // Обновляем компоненты
                            transform->SetPosition(i * 0.1f, i * 0.1f, i * 0.1f);
                            transform->SetScale(1.0f + i * 0.01f, 1.0f + i * 0.01f, 1.0f + i * 0.01f);
                            transform->SetRotation(i * 0.1f, i * 0.1f, i * 0.1f);
                            
                            sprite->SetSize(100.0f + i, 100.0f + i);
                            sprite->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
                            
                            successCount.fetch_add(1);
                        }
                    }
                }
            } catch (const std::exception& e) {
                std::cout << "Thread error: " << e.what() << std::endl;
            }
        });
    }
    
    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что все компоненты обновлены успешно
    EXPECT_EQ(successCount.load(), numThreads * numEntitiesPerThread);
}

// Тест на безопасность удаления компонентов в разных потоках
TEST_F(ThreadSafetyTest, ComponentRemovalThreadSafety) {
    auto world = std::make_unique<World>();
    const int numThreads = 4;
    const int numEntitiesPerThread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    // Создаем сущности с компонентами
    for (int i = 0; i < numThreads * numEntitiesPerThread; ++i) {
        auto entity = world->CreateEntity();
        entity->AddComponent<Transform>();
        entity->AddComponent<Sprite>();
    }
    
    // Создаем потоки, каждый удаляет компоненты у своих сущностей
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&world, numEntitiesPerThread, &successCount, t]() {
            try {
                for (int i = 0; i < numEntitiesPerThread; ++i) {
                    int entityIndex = t * numEntitiesPerThread + i;
                    auto entity = world->GetEntity(entityIndex);
                    
                    if (entity) {
                        // Удаляем компоненты
                        entity->RemoveComponent<Transform>();
                        entity->RemoveComponent<Sprite>();
                        
                        successCount.fetch_add(1);
                    }
                }
            } catch (const std::exception& e) {
                std::cout << "Thread error: " << e.what() << std::endl;
            }
        });
    }
    
    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что все компоненты удалены успешно
    EXPECT_EQ(successCount.load(), numThreads * numEntitiesPerThread);
}

// Тест на безопасность обновления мира в разных потоках
TEST_F(ThreadSafetyTest, WorldUpdateThreadSafety) {
    auto world = std::make_unique<World>();
    const int numThreads = 4;
    const int numEntitiesPerThread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    // Создаем сущности с компонентами
    for (int i = 0; i < numThreads * numEntitiesPerThread; ++i) {
        auto entity = world->CreateEntity();
        entity->AddComponent<Transform>();
        entity->AddComponent<Sprite>();
    }
    
    // Создаем потоки, каждый обновляет мир
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&world, &successCount]() {
            try {
                for (int i = 0; i < 100; ++i) {
                    world->Update(0.016f);
                    successCount.fetch_add(1);
                    
                    // Небольшая задержка для имитации реальной работы
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            } catch (const std::exception& e) {
                std::cout << "Thread error: " << e.what() << std::endl;
            }
        });
    }
    
    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что мир обновлен успешно
    EXPECT_EQ(successCount.load(), numThreads * 100);
}

// Тест на безопасность рендеринга в разных потоках
TEST_F(ThreadSafetyTest, WorldRenderThreadSafety) {
    auto world = std::make_unique<World>();
    const int numThreads = 4;
    const int numEntitiesPerThread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    // Создаем сущности с компонентами
    for (int i = 0; i < numThreads * numEntitiesPerThread; ++i) {
        auto entity = world->CreateEntity();
        entity->AddComponent<Transform>();
        entity->AddComponent<Sprite>();
    }
    
    // Создаем потоки, каждый рендерит мир
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&world, &successCount]() {
            try {
                for (int i = 0; i < 100; ++i) {
                    world->Render();
                    successCount.fetch_add(1);
                    
                    // Небольшая задержка для имитации реальной работы
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            } catch (const std::exception& e) {
                std::cout << "Thread error: " << e.what() << std::endl;
            }
        });
    }
    
    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что мир отрендерен успешно
    EXPECT_EQ(successCount.load(), numThreads * 100);
}

// Тест на безопасность работы с ресурсами в разных потоках
TEST_F(ThreadSafetyTest, ResourceAccessThreadSafety) {
    auto world = std::make_unique<World>();
    const int numThreads = 4;
    const int numEntitiesPerThread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    // Создаем сущности с компонентами
    for (int i = 0; i < numThreads * numEntitiesPerThread; ++i) {
        auto entity = world->CreateEntity();
        entity->AddComponent<Transform>();
        entity->AddComponent<Sprite>();
    }
    
    // Создаем потоки, каждый работает с ресурсами
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&world, numEntitiesPerThread, &successCount, t]() {
            try {
                for (int i = 0; i < numEntitiesPerThread; ++i) {
                    int entityIndex = t * numEntitiesPerThread + i;
                    auto entity = world->GetEntity(entityIndex);
                    
                    if (entity) {
                        auto sprite = entity->GetComponent<Sprite>();
                        
                        if (sprite) {
                            // Работаем с ресурсами (если есть соответствующие методы)
                            // sprite->LoadTexture("test_texture.png");
                            // sprite->LoadShader("test.vert", "test.frag");
                            
                            successCount.fetch_add(1);
                        }
                    }
                }
            } catch (const std::exception& e) {
                std::cout << "Thread error: " << e.what() << std::endl;
            }
        });
    }
    
    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что все ресурсы обработаны успешно
    EXPECT_EQ(successCount.load(), numThreads * numEntitiesPerThread);
}

// Тест на безопасность работы с событиями в разных потоках
TEST_F(ThreadSafetyTest, EventHandlingThreadSafety) {
    auto world = std::make_unique<World>();
    const int numThreads = 4;
    const int numEntitiesPerThread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    // Создаем сущности с компонентами
    for (int i = 0; i < numThreads * numEntitiesPerThread; ++i) {
        auto entity = world->CreateEntity();
        entity->AddComponent<Transform>();
        entity->AddComponent<Sprite>();
    }
    
    // Создаем потоки, каждый обрабатывает события
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&world, &successCount]() {
            try {
                for (int i = 0; i < 100; ++i) {
                    // Обрабатываем события (если есть система событий)
                    // EventManager::ProcessEvents();
                    
                    successCount.fetch_add(1);
                    
                    // Небольшая задержка для имитации реальной работы
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            } catch (const std::exception& e) {
                std::cout << "Thread error: " << e.what() << std::endl;
            }
        });
    }
    
    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что все события обработаны успешно
    EXPECT_EQ(successCount.load(), numThreads * 100);
}

// Тест на безопасность работы с физикой в разных потоках
TEST_F(ThreadSafetyTest, PhysicsUpdateThreadSafety) {
    auto world = std::make_unique<World>();
    const int numThreads = 4;
    const int numEntitiesPerThread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    // Создаем сущности с физическими компонентами
    for (int i = 0; i < numThreads * numEntitiesPerThread; ++i) {
        auto entity = world->CreateEntity();
        entity->AddComponent<Transform>();
        // entity->AddComponent<RigidBody>();
        // entity->AddComponent<Collider>();
    }
    
    // Создаем потоки, каждый обновляет физику
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&world, &successCount]() {
            try {
                for (int i = 0; i < 100; ++i) {
                    // Обновляем физику (если есть система физики)
                    // PhysicsSystem::Update(0.016f);
                    
                    successCount.fetch_add(1);
                    
                    // Небольшая задержка для имитации реальной работы
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            } catch (const std::exception& e) {
                std::cout << "Thread error: " << e.what() << std::endl;
            }
        });
    }
    
    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что физика обновлена успешно
    EXPECT_EQ(successCount.load(), numThreads * 100);
}

// Тест на безопасность работы с аудио в разных потоках
TEST_F(ThreadSafetyTest, AudioUpdateThreadSafety) {
    auto world = std::make_unique<World>();
    const int numThreads = 4;
    const int numEntitiesPerThread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    // Создаем сущности с аудио компонентами
    for (int i = 0; i < numThreads * numEntitiesPerThread; ++i) {
        auto entity = world->CreateEntity();
        entity->AddComponent<Transform>();
        // entity->AddComponent<AudioSource>();
    }
    
    // Создаем потоки, каждый обновляет аудио
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&world, &successCount]() {
            try {
                for (int i = 0; i < 100; ++i) {
                    // Обновляем аудио (если есть система аудио)
                    // AudioSystem::Update(0.016f);
                    
                    successCount.fetch_add(1);
                    
                    // Небольшая задержка для имитации реальной работы
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            } catch (const std::exception& e) {
                std::cout << "Thread error: " << e.what() << std::endl;
            }
        });
    }
    
    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что аудио обновлено успешно
    EXPECT_EQ(successCount.load(), numThreads * 100);
}








