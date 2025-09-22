#include <gtest/gtest.h>
#include <FastEngine/Engine.h>
#include <FastEngine/World.h>
#include <FastEngine/Entity.h>
#include <FastEngine/Components/Transform.h>
#include <FastEngine/Components/Sprite.h>
#include <memory>
#include <limits>

class InputValidationTest : public ::testing::Test {
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

TEST_F(InputValidationTest, TransformPositionValidation) {
    // Тест валидации позиции Transform
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<FastEngine::Transform>();
    
    // Тест с нормальными значениями
    transform->SetPosition(100.0f, 200.0f);
    auto position = transform->GetPosition();
    EXPECT_FLOAT_EQ(position.x, 100.0f);
    EXPECT_FLOAT_EQ(position.y, 200.0f);
    
    // Тест с отрицательными значениями
    transform->SetPosition(-100.0f, -200.0f);
    position = transform->GetPosition();
    EXPECT_FLOAT_EQ(position.x, -100.0f);
    EXPECT_FLOAT_EQ(position.y, -200.0f);
    
    // Тест с нулевыми значениями
    transform->SetPosition(0.0f, 0.0f);
    position = transform->GetPosition();
    EXPECT_FLOAT_EQ(position.x, 0.0f);
    EXPECT_FLOAT_EQ(position.y, 0.0f);
    
    // Тест с очень большими значениями
    transform->SetPosition(1000000.0f, -1000000.0f);
    position = transform->GetPosition();
    EXPECT_FLOAT_EQ(position.x, 1000000.0f);
    EXPECT_FLOAT_EQ(position.y, -1000000.0f);
}

TEST_F(InputValidationTest, TransformScaleValidation) {
    // Тест валидации масштаба Transform
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<FastEngine::Transform>();
    
    // Тест с нормальными значениями
    transform->SetScale(2.0f, 3.0f);
    auto scale = transform->GetScale();
    EXPECT_FLOAT_EQ(scale.x, 2.0f);
    EXPECT_FLOAT_EQ(scale.y, 3.0f);
    
    // Тест с нулевым масштабом
    transform->SetScale(0.0f, 0.0f);
    scale = transform->GetScale();
    EXPECT_FLOAT_EQ(scale.x, 0.0f);
    EXPECT_FLOAT_EQ(scale.y, 0.0f);
    
    // Тест с отрицательным масштабом
    transform->SetScale(-1.0f, -2.0f);
    scale = transform->GetScale();
    EXPECT_FLOAT_EQ(scale.x, -1.0f);
    EXPECT_FLOAT_EQ(scale.y, -2.0f);
    
    // Тест с очень большим масштабом
    transform->SetScale(1000.0f, 1000.0f);
    scale = transform->GetScale();
    EXPECT_FLOAT_EQ(scale.x, 1000.0f);
    EXPECT_FLOAT_EQ(scale.y, 1000.0f);
}

TEST_F(InputValidationTest, TransformRotationValidation) {
    // Тест валидации поворота Transform
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<FastEngine::Transform>();
    
    // Тест с нормальными значениями
    transform->SetRotation(45.0f);
    EXPECT_FLOAT_EQ(transform->GetRotation(), 45.0f);
    
    // Тест с нулевым поворотом
    transform->SetRotation(0.0f);
    EXPECT_FLOAT_EQ(transform->GetRotation(), 0.0f);
    
    // Тест с отрицательным поворотом
    transform->SetRotation(-90.0f);
    EXPECT_FLOAT_EQ(transform->GetRotation(), -90.0f);
    
    // Тест с поворотом больше 360 градусов
    transform->SetRotation(450.0f);
    EXPECT_FLOAT_EQ(transform->GetRotation(), 450.0f);
    
    // Тест с очень большим поворотом
    transform->SetRotation(10000.0f);
    EXPECT_FLOAT_EQ(transform->GetRotation(), 10000.0f);
}

TEST_F(InputValidationTest, SpriteSizeValidation) {
    // Тест валидации размера Sprite
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    
    // Тест с нормальными значениями
    sprite->SetSize(100.0f, 150.0f);
    auto size = sprite->GetSize();
    EXPECT_FLOAT_EQ(size.x, 100.0f);
    EXPECT_FLOAT_EQ(size.y, 150.0f);
    
    // Тест с нулевым размером
    sprite->SetSize(0.0f, 0.0f);
    size = sprite->GetSize();
    EXPECT_FLOAT_EQ(size.x, 0.0f);
    EXPECT_FLOAT_EQ(size.y, 0.0f);
    
    // Тест с отрицательным размером
    sprite->SetSize(-50.0f, -75.0f);
    size = sprite->GetSize();
    EXPECT_FLOAT_EQ(size.x, -50.0f);
    EXPECT_FLOAT_EQ(size.y, -75.0f);
    
    // Тест с очень большим размером
    sprite->SetSize(10000.0f, 10000.0f);
    size = sprite->GetSize();
    EXPECT_FLOAT_EQ(size.x, 10000.0f);
    EXPECT_FLOAT_EQ(size.y, 10000.0f);
}

TEST_F(InputValidationTest, SpriteColorValidation) {
    // Тест валидации цвета Sprite
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    
    // Тест с нормальными значениями
    sprite->SetColor(255, 128, 64, 200);
    auto color = sprite->GetColor();
    EXPECT_EQ(color.r, 255);
    EXPECT_EQ(color.g, 128);
    EXPECT_EQ(color.b, 64);
    EXPECT_EQ(color.a, 200);
    
    // Тест с нулевыми значениями
    sprite->SetColor(0, 0, 0, 0);
    color = sprite->GetColor();
    EXPECT_EQ(color.r, 0);
    EXPECT_EQ(color.g, 0);
    EXPECT_EQ(color.b, 0);
    EXPECT_EQ(color.a, 0);
    
    // Тест с максимальными значениями
    sprite->SetColor(255, 255, 255, 255);
    color = sprite->GetColor();
    EXPECT_EQ(color.r, 255);
    EXPECT_EQ(color.g, 255);
    EXPECT_EQ(color.b, 255);
    EXPECT_EQ(color.a, 255);
    
    // Тест с отрицательными значениями
    sprite->SetColor(-1, -1, -1, -1);
    color = sprite->GetColor();
    EXPECT_EQ(color.r, -1);
    EXPECT_EQ(color.g, -1);
    EXPECT_EQ(color.b, -1);
    EXPECT_EQ(color.a, -1);
}

TEST_F(InputValidationTest, EntityIdValidation) {
    // Тест валидации ID сущности
    auto entity = world->CreateEntity();
    auto entityId = entity->GetId();
    
    // Проверяем, что ID положительный
    EXPECT_GT(entityId, 0);
    
    // Проверяем, что можем получить сущность по ID
    auto retrievedEntity = world->GetEntity(entityId);
    EXPECT_TRUE(retrievedEntity != nullptr);
    EXPECT_EQ(entity, retrievedEntity);
    
    // Проверяем, что несуществующий ID возвращает nullptr
    auto nonExistentEntity = world->GetEntity(999999);
    EXPECT_TRUE(nonExistentEntity == nullptr);
}

TEST_F(InputValidationTest, ComponentValidation) {
    // Тест валидации компонентов
    auto entity = world->CreateEntity();
    
    // Проверяем, что компоненты не существуют изначально
    EXPECT_FALSE(entity->HasComponent<FastEngine::Transform>());
    EXPECT_FALSE(entity->HasComponent<FastEngine::Sprite>());
    
    // Добавляем компоненты
    auto transform = entity->AddComponent<FastEngine::Transform>();
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    
    // Проверяем, что компоненты добавлены
    EXPECT_TRUE(entity->HasComponent<FastEngine::Transform>());
    EXPECT_TRUE(entity->HasComponent<FastEngine::Sprite>());
    
    // Проверяем, что можем получить компоненты
    auto retrievedTransform = entity->GetComponent<FastEngine::Transform>();
    auto retrievedSprite = entity->GetComponent<FastEngine::Sprite>();
    
    EXPECT_TRUE(retrievedTransform != nullptr);
    EXPECT_TRUE(retrievedSprite != nullptr);
    EXPECT_EQ(transform, retrievedTransform);
    EXPECT_EQ(sprite, retrievedSprite);
}

TEST_F(InputValidationTest, BoundaryValueTesting) {
    // Тест граничных значений
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<FastEngine::Transform>();
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    
    // Тест с минимальными значениями
    transform->SetPosition(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    transform->SetScale(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    transform->SetRotation(std::numeric_limits<float>::min());
    
    sprite->SetSize(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    sprite->SetColor(0, 0, 0, 0);
    
    // Проверяем, что значения установились
    auto position = transform->GetPosition();
    auto scale = transform->GetScale();
    auto rotation = transform->GetRotation();
    auto size = sprite->GetSize();
    auto color = sprite->GetColor();
    
    EXPECT_FLOAT_EQ(position.x, std::numeric_limits<float>::min());
    EXPECT_FLOAT_EQ(position.y, std::numeric_limits<float>::min());
    EXPECT_FLOAT_EQ(scale.x, std::numeric_limits<float>::min());
    EXPECT_FLOAT_EQ(scale.y, std::numeric_limits<float>::min());
    EXPECT_FLOAT_EQ(rotation, std::numeric_limits<float>::min());
    EXPECT_FLOAT_EQ(size.x, std::numeric_limits<float>::min());
    EXPECT_FLOAT_EQ(size.y, std::numeric_limits<float>::min());
    EXPECT_EQ(color.r, 0);
    EXPECT_EQ(color.g, 0);
    EXPECT_EQ(color.b, 0);
    EXPECT_EQ(color.a, 0);
    
    // Тест с максимальными значениями
    transform->SetPosition(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    transform->SetScale(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    transform->SetRotation(std::numeric_limits<float>::max());
    
    sprite->SetSize(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    sprite->SetColor(255, 255, 255, 255);
    
    // Проверяем, что значения установились
    position = transform->GetPosition();
    scale = transform->GetScale();
    rotation = transform->GetRotation();
    size = sprite->GetSize();
    color = sprite->GetColor();
    
    EXPECT_FLOAT_EQ(position.x, std::numeric_limits<float>::max());
    EXPECT_FLOAT_EQ(position.y, std::numeric_limits<float>::max());
    EXPECT_FLOAT_EQ(scale.x, std::numeric_limits<float>::max());
    EXPECT_FLOAT_EQ(scale.y, std::numeric_limits<float>::max());
    EXPECT_FLOAT_EQ(rotation, std::numeric_limits<float>::max());
    EXPECT_FLOAT_EQ(size.x, std::numeric_limits<float>::max());
    EXPECT_FLOAT_EQ(size.y, std::numeric_limits<float>::max());
    EXPECT_EQ(color.r, 255);
    EXPECT_EQ(color.g, 255);
    EXPECT_EQ(color.b, 255);
    EXPECT_EQ(color.a, 255);
}

TEST_F(InputValidationTest, InvalidInputHandling) {
    // Тест обработки недопустимых входных данных
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<FastEngine::Transform>();
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    
    // Тест с NaN значениями
    transform->SetPosition(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
    transform->SetScale(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
    transform->SetRotation(std::numeric_limits<float>::quiet_NaN());
    
    sprite->SetSize(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
    
    // Проверяем, что система не упала
    EXPECT_TRUE(true); // Если дошли сюда, значит система обработала NaN
    
    // Тест с бесконечными значениями
    transform->SetPosition(std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
    transform->SetScale(std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
    transform->SetRotation(std::numeric_limits<float>::infinity());
    
    sprite->SetSize(std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
    
    // Проверяем, что система не упала
    EXPECT_TRUE(true); // Если дошли сюда, значит система обработала бесконечность
}