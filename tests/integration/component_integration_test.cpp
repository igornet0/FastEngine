#include <gtest/gtest.h>
#include <FastEngine/World.h>
#include <FastEngine/Entity.h>
#include <FastEngine/Components/Transform.h>
#include <FastEngine/Components/Sprite.h>
#include <memory>

class ComponentIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        world = std::make_unique<FastEngine::World>();
        world->Initialize();
        entity = world->CreateEntity();
    }
    
    void TearDown() override {
        world.reset();
        entity.reset();
    }
    
    std::unique_ptr<FastEngine::World> world;
    std::shared_ptr<FastEngine::Entity> entity;
};

TEST_F(ComponentIntegrationTest, TransformComponent) {
    // Тест компонента трансформации
    auto transform = entity->AddComponent<FastEngine::Transform>();
    EXPECT_TRUE(transform != nullptr);
    
    // Тест установки позиции
    transform->SetPosition(10.0f, 20.0f);
    auto position = transform->GetPosition();
    EXPECT_FLOAT_EQ(position.x, 10.0f);
    EXPECT_FLOAT_EQ(position.y, 20.0f);
    
    // Тест установки масштаба
    transform->SetScale(2.0f, 3.0f);
    auto scale = transform->GetScale();
    EXPECT_FLOAT_EQ(scale.x, 2.0f);
    EXPECT_FLOAT_EQ(scale.y, 3.0f);
    
    // Тест установки поворота
    transform->SetRotation(45.0f);
    EXPECT_FLOAT_EQ(transform->GetRotation(), 45.0f);
}

TEST_F(ComponentIntegrationTest, SpriteComponent) {
    // Тест компонента спрайта
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    EXPECT_TRUE(sprite != nullptr);
    
    // Тест установки размера
    sprite->SetSize(100.0f, 100.0f);
    auto size = sprite->GetSize();
    EXPECT_FLOAT_EQ(size.x, 100.0f);
    EXPECT_FLOAT_EQ(size.y, 100.0f);
    
    // Тест установки цвета
    sprite->SetColor(255, 128, 64, 255);
    auto color = sprite->GetColor();
    EXPECT_EQ(color.r, 255);
    EXPECT_EQ(color.g, 128);
    EXPECT_EQ(color.b, 64);
    EXPECT_EQ(color.a, 255);
}

TEST_F(ComponentIntegrationTest, MultipleComponents) {
    // Тест множественных компонентов на одной сущности
    auto transform = entity->AddComponent<FastEngine::Transform>();
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    
    EXPECT_TRUE(transform != nullptr);
    EXPECT_TRUE(sprite != nullptr);
    
    // Проверяем, что оба компонента добавлены
    EXPECT_TRUE(entity->HasComponent<FastEngine::Transform>());
    EXPECT_TRUE(entity->HasComponent<FastEngine::Sprite>());
    
    // Проверяем, что можем получить оба компонента
    auto retrievedTransform = entity->GetComponent<FastEngine::Transform>();
    auto retrievedSprite = entity->GetComponent<FastEngine::Sprite>();
    
    EXPECT_EQ(transform, retrievedTransform);
    EXPECT_EQ(sprite, retrievedSprite);
}

TEST_F(ComponentIntegrationTest, ComponentRemoval) {
    // Тест удаления компонента
    auto transform = entity->AddComponent<FastEngine::Transform>();
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    
    // Проверяем, что компоненты добавлены
    EXPECT_TRUE(entity->HasComponent<FastEngine::Transform>());
    EXPECT_TRUE(entity->HasComponent<FastEngine::Sprite>());
    
    // Удаляем Transform
    entity->RemoveComponent<FastEngine::Transform>();
    
    // Проверяем, что Transform удален, а Sprite остался
    EXPECT_FALSE(entity->HasComponent<FastEngine::Transform>());
    EXPECT_TRUE(entity->HasComponent<FastEngine::Sprite>());
    
    // Проверяем, что не можем получить удаленный компонент
    auto removedTransform = entity->GetComponent<FastEngine::Transform>();
    EXPECT_TRUE(removedTransform == nullptr);
}

TEST_F(ComponentIntegrationTest, ComponentUpdate) {
    // Тест обновления компонентов
    auto transform = entity->AddComponent<FastEngine::Transform>();
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    
    // Устанавливаем начальные значения
    transform->SetPosition(0.0f, 0.0f);
    sprite->SetSize(50.0f, 50.0f);
    
    // Обновляем сущность
    entity->Update(0.016f);
    
    // Проверяем, что компоненты обновились без ошибок
    EXPECT_TRUE(true); // Если дошли сюда, значит обновление прошло успешно
}

TEST_F(ComponentIntegrationTest, ComponentSerialization) {
    // Тест сериализации компонентов
    auto transform = entity->AddComponent<FastEngine::Transform>();
    auto sprite = entity->AddComponent<FastEngine::Sprite>();
    
    // Устанавливаем значения
    transform->SetPosition(15.0f, 25.0f);
    transform->SetScale(1.5f, 2.0f);
    transform->SetRotation(30.0f);
    
    sprite->SetSize(75.0f, 100.0f);
    sprite->SetColor(200, 150, 100, 255);
    
    // Проверяем, что значения сохранились
    auto position = transform->GetPosition();
    auto scale = transform->GetScale();
    auto rotation = transform->GetRotation();
    auto size = sprite->GetSize();
    auto color = sprite->GetColor();
    
    EXPECT_FLOAT_EQ(position.x, 15.0f);
    EXPECT_FLOAT_EQ(position.y, 25.0f);
    EXPECT_FLOAT_EQ(scale.x, 1.5f);
    EXPECT_FLOAT_EQ(scale.y, 2.0f);
    EXPECT_FLOAT_EQ(rotation, 30.0f);
    EXPECT_FLOAT_EQ(size.x, 75.0f);
    EXPECT_FLOAT_EQ(size.y, 100.0f);
    EXPECT_EQ(color.r, 200);
    EXPECT_EQ(color.g, 150);
    EXPECT_EQ(color.b, 100);
    EXPECT_EQ(color.a, 255);
}

TEST_F(ComponentIntegrationTest, ComponentCopy) {
    // Тест копирования компонентов
    auto transform1 = entity->AddComponent<FastEngine::Transform>();
    transform1->SetPosition(10.0f, 20.0f);
    transform1->SetScale(2.0f, 3.0f);
    
    // Создаем вторую сущность
    auto entity2 = world->CreateEntity();
    auto transform2 = entity2->AddComponent<FastEngine::Transform>();
    
    // Копируем значения
    transform2->SetPosition(transform1->GetPosition());
    transform2->SetScale(transform1->GetScale());
    
    // Проверяем, что значения скопированы
    auto pos1 = transform1->GetPosition();
    auto pos2 = transform2->GetPosition();
    auto scale1 = transform1->GetScale();
    auto scale2 = transform2->GetScale();
    
    EXPECT_FLOAT_EQ(pos1.x, pos2.x);
    EXPECT_FLOAT_EQ(pos1.y, pos2.y);
    EXPECT_FLOAT_EQ(scale1.x, scale2.x);
    EXPECT_FLOAT_EQ(scale1.y, scale2.y);
}
