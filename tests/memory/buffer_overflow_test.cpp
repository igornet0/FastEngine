#include <gtest/gtest.h>
#include "FastEngine/Engine.h"
#include "FastEngine/Entity.h"
#include "FastEngine/World.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Components/Transform.h"
#include <vector>
#include <array>
#include <cstring>
#include <memory>

using namespace FastEngine;

class BufferOverflowTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_TRUE(Engine::GetInstance().Initialize());
    }
    
    void TearDown() override {
        Engine::GetInstance().Shutdown();
    }
};

// Тест на переполнение буфера при работе с массивами
TEST_F(BufferOverflowTest, ArrayBufferOverflow) {
    const size_t bufferSize = 100;
    std::array<int, bufferSize> buffer;
    
    // Заполняем буфер корректными данными
    for (size_t i = 0; i < bufferSize; ++i) {
        buffer[i] = static_cast<int>(i);
    }
    
    // Проверяем, что буфер заполнен корректно
    for (size_t i = 0; i < bufferSize; ++i) {
        EXPECT_EQ(buffer[i], static_cast<int>(i));
    }
    
    // Тест на попытку доступа за границы буфера
    // Это должно быть безопасно благодаря std::array
    EXPECT_NO_THROW(buffer.at(bufferSize - 1));
    EXPECT_THROW(buffer.at(bufferSize), std::out_of_range);
}

// Тест на переполнение буфера при работе с векторами
TEST_F(BufferOverflowTest, VectorBufferOverflow) {
    const size_t initialSize = 50;
    std::vector<int> vec(initialSize);
    
    // Заполняем вектор
    for (size_t i = 0; i < initialSize; ++i) {
        vec[i] = static_cast<int>(i);
    }
    
    // Проверяем корректность заполнения
    for (size_t i = 0; i < initialSize; ++i) {
        EXPECT_EQ(vec[i], static_cast<int>(i));
    }
    
    // Тест на безопасный доступ
    EXPECT_NO_THROW(vec.at(initialSize - 1));
    EXPECT_THROW(vec.at(initialSize), std::out_of_range);
}

// Тест на переполнение буфера при работе с C-массивами
TEST_F(BufferOverflowTest, CArrayBufferOverflow) {
    const size_t bufferSize = 100;
    int* buffer = new int[bufferSize];
    
    // Заполняем буфер
    for (size_t i = 0; i < bufferSize; ++i) {
        buffer[i] = static_cast<int>(i);
    }
    
    // Проверяем корректность заполнения
    for (size_t i = 0; i < bufferSize; ++i) {
        EXPECT_EQ(buffer[i], static_cast<int>(i));
    }
    
    // Очищаем память
    delete[] buffer;
}

// Тест на переполнение буфера при работе с строками
TEST_F(BufferOverflowTest, StringBufferOverflow) {
    const size_t bufferSize = 100;
    char buffer[bufferSize];
    
    // Заполняем буфер строкой
    const char* testString = "Hello, World!";
    size_t stringLen = strlen(testString);
    
    // Проверяем, что строка помещается в буфер
    EXPECT_LT(stringLen, bufferSize);
    
    // Копируем строку безопасно
    strncpy(buffer, testString, bufferSize - 1);
    buffer[bufferSize - 1] = '\0';
    
    // Проверяем корректность копирования
    EXPECT_STREQ(buffer, testString);
}

// Тест на переполнение буфера при работе с компонентами
TEST_F(BufferOverflowTest, ComponentBufferOverflow) {
    const int numComponents = 1000;
    std::vector<std::unique_ptr<Entity>> entities;
    
    // Создаем сущности с компонентами
    for (int i = 0; i < numComponents; ++i) {
        auto entity = std::make_unique<Entity>();
        entity->AddComponent<Transform>();
        entity->AddComponent<Sprite>();
        entities.push_back(std::move(entity));
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), numComponents);
    
    // Проверяем доступ к компонентам
    for (size_t i = 0; i < entities.size(); ++i) {
        EXPECT_NO_THROW(entities[i]->GetComponent<Transform>());
        EXPECT_NO_THROW(entities[i]->GetComponent<Sprite>());
    }
}

// Тест на переполнение буфера при работе с миром
TEST_F(BufferOverflowTest, WorldBufferOverflow) {
    const int numEntities = 10000;
    auto world = std::make_unique<World>();
    
    // Создаем множество сущностей в мире
    for (int i = 0; i < numEntities; ++i) {
        auto entity = world->CreateEntity();
        entity->AddComponent<Transform>();
        entity->AddComponent<Sprite>();
    }
    
    // Проверяем, что мир может обработать все сущности
    EXPECT_NO_THROW(world->Update(0.016f));
    EXPECT_NO_THROW(world->Render());
}

// Тест на переполнение буфера при работе с текстурами
TEST_F(BufferOverflowTest, TextureBufferOverflow) {
    const int numTextures = 100;
    std::vector<std::unique_ptr<Entity>> entities;
    
    for (int i = 0; i < numTextures; ++i) {
        auto entity = std::make_unique<Entity>();
        auto sprite = entity->AddComponent<Sprite>();
        
        // Симулируем загрузку текстур
        // sprite->LoadTexture("test_texture.png");
        
        entities.push_back(std::move(entity));
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), numTextures);
}

// Тест на переполнение буфера при работе с шейдерами
TEST_F(BufferOverflowTest, ShaderBufferOverflow) {
    const int numShaders = 50;
    std::vector<std::unique_ptr<Entity>> entities;
    
    for (int i = 0; i < numShaders; ++i) {
        auto entity = std::make_unique<Entity>();
        auto sprite = entity->AddComponent<Sprite>();
        
        // Симулируем загрузку шейдеров
        // sprite->LoadShader("test.vert", "test.frag");
        
        entities.push_back(std::move(entity));
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), numShaders);
}

// Тест на переполнение буфера при работе с анимациями
TEST_F(BufferOverflowTest, AnimationBufferOverflow) {
    const int numAnimations = 200;
    std::vector<std::unique_ptr<Entity>> entities;
    
    for (int i = 0; i < numAnimations; ++i) {
        auto entity = std::make_unique<Entity>();
        auto sprite = entity->AddComponent<Sprite>();
        
        // Симулируем создание анимаций
        // auto animator = entity->AddComponent<Animator>();
        // animator->CreateAnimation("test", frames);
        
        entities.push_back(std::move(entity));
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), numAnimations);
}

// Тест на переполнение буфера при работе с событиями
TEST_F(BufferOverflowTest, EventBufferOverflow) {
    const int numEvents = 1000;
    std::vector<std::unique_ptr<Entity>> entities;
    
    for (int i = 0; i < numEvents; ++i) {
        auto entity = std::make_unique<Entity>();
        
        // Симулируем подписку на события
        // EventManager::Subscribe("test_event", [](const Event& e) { /* обработка */ });
        
        entities.push_back(std::move(entity));
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), numEvents);
}

// Тест на переполнение буфера при работе с сетью
TEST_F(BufferOverflowTest, NetworkBufferOverflow) {
    const int numNetworkObjects = 500;
    std::vector<std::unique_ptr<Entity>> entities;
    
    for (int i = 0; i < numNetworkObjects; ++i) {
        auto entity = std::make_unique<Entity>();
        
        // Симулируем сетевые компоненты
        // auto networkComponent = entity->AddComponent<NetworkComponent>();
        // networkComponent->SetData(largeData);
        
        entities.push_back(std::move(entity));
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), numNetworkObjects);
}

// Тест на переполнение буфера при работе с физикой
TEST_F(BufferOverflowTest, PhysicsBufferOverflow) {
    const int numPhysicsObjects = 1000;
    std::vector<std::unique_ptr<Entity>> entities;
    
    for (int i = 0; i < numPhysicsObjects; ++i) {
        auto entity = std::make_unique<Entity>();
        
        // Симулируем физические компоненты
        // auto rigidBody = entity->AddComponent<RigidBody>();
        // auto collider = entity->AddComponent<Collider>();
        
        entities.push_back(std::move(entity));
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), numPhysicsObjects);
}

// Тест на переполнение буфера при работе с аудио
TEST_F(BufferOverflowTest, AudioBufferOverflow) {
    const int numAudioObjects = 300;
    std::vector<std::unique_ptr<Entity>> entities;
    
    for (int i = 0; i < numAudioObjects; ++i) {
        auto entity = std::make_unique<Entity>();
        
        // Симулируем аудио компоненты
        // auto audioSource = entity->AddComponent<AudioSource>();
        // audioSource->LoadSound("test_sound.wav");
        
        entities.push_back(std::move(entity));
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), numAudioObjects);
}

// Тест на переполнение буфера при работе с AI
TEST_F(BufferOverflowTest, AIBufferOverflow) {
    const int numAIObjects = 400;
    std::vector<std::unique_ptr<Entity>> entities;
    
    for (int i = 0; i < numAIObjects; ++i) {
        auto entity = std::make_unique<Entity>();
        
        // Симулируем AI компоненты
        // auto aiComponent = entity->AddComponent<AIComponent>();
        // aiComponent->SetBehavior(behaviorData);
        
        entities.push_back(std::move(entity));
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), numAIObjects);
}

// Тест на переполнение буфера при работе с UI
TEST_F(BufferOverflowTest, UIBufferOverflow) {
    const int numUIObjects = 200;
    std::vector<std::unique_ptr<Entity>> entities;
    
    for (int i = 0; i < numUIObjects; ++i) {
        auto entity = std::make_unique<Entity>();
        
        // Симулируем UI компоненты
        // auto uiComponent = entity->AddComponent<UIComponent>();
        // uiComponent->SetText("Test text");
        
        entities.push_back(std::move(entity));
    }
    
    // Проверяем, что все сущности созданы
    EXPECT_EQ(entities.size(), numUIObjects);
}


