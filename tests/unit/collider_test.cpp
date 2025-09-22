#include <gtest/gtest.h>
#include "FastEngine/Components/Collider.h"
#include "FastEngine/Entity.h"
#include "FastEngine/World.h"
#include <memory>

using namespace FastEngine;

class ColliderTest : public ::testing::Test {
protected:
    void SetUp() override {
        world = std::make_unique<World>();
        world->Initialize();
        entity = world->CreateEntity();
        collider = entity->AddComponent<Collider>();
    }
    
    void TearDown() override {
        collider = nullptr;
        entity = nullptr;
        world.reset();
    }
    
    std::unique_ptr<World> world;
    std::shared_ptr<Entity> entity;
    std::shared_ptr<Collider> collider;
};

TEST_F(ColliderTest, ColliderCreation) {
    EXPECT_TRUE(collider != nullptr);
    EXPECT_TRUE(entity->HasComponent<Collider>());
}

TEST_F(ColliderTest, ColliderType) {
    // Тест установки типа коллайдера
    collider->SetType(ColliderType::Box);
    EXPECT_EQ(collider->GetType(), ColliderType::Box);
    
    collider->SetType(ColliderType::Circle);
    EXPECT_EQ(collider->GetType(), ColliderType::Circle);
    
    collider->SetType(ColliderType::Polygon);
    EXPECT_EQ(collider->GetType(), ColliderType::Polygon);
}

TEST_F(ColliderTest, BoxCollider) {
    // Тест боксового коллайдера
    collider->SetType(ColliderType::Box);
    collider->SetSize(glm::vec2(100.0f, 50.0f));
    
    auto size = collider->GetSize();
    EXPECT_FLOAT_EQ(size.x, 100.0f);
    EXPECT_FLOAT_EQ(size.y, 50.0f);
}

TEST_F(ColliderTest, CircleCollider) {
    // Тест кругового коллайдера
    collider->SetType(ColliderType::Circle);
    collider->SetRadius(25.0f);
    
    EXPECT_FLOAT_EQ(collider->GetRadius(), 25.0f);
}

TEST_F(ColliderTest, PolygonCollider) {
    // Тест полигонального коллайдера
    collider->SetType(ColliderType::Polygon);
    
    std::vector<glm::vec2> vertices = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(50.0f, 0.0f),
        glm::vec2(50.0f, 50.0f),
        glm::vec2(0.0f, 50.0f)
    };
    
    collider->SetVertices(vertices);
    auto retrievedVertices = collider->GetVertices();
    
    EXPECT_EQ(retrievedVertices.size(), vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i) {
        EXPECT_FLOAT_EQ(retrievedVertices[i].x, vertices[i].x);
        EXPECT_FLOAT_EQ(retrievedVertices[i].y, vertices[i].y);
    }
}

TEST_F(ColliderTest, Offset) {
    // Тест смещения коллайдера
    collider->SetOffset(glm::vec2(10.0f, 20.0f));
    auto offset = collider->GetOffset();
    
    EXPECT_FLOAT_EQ(offset.x, 10.0f);
    EXPECT_FLOAT_EQ(offset.y, 20.0f);
}

TEST_F(ColliderTest, IsTrigger) {
    // Тест триггерного коллайдера
    collider->SetIsTrigger(true);
    EXPECT_TRUE(collider->IsTrigger());
    
    collider->SetIsTrigger(false);
    EXPECT_FALSE(collider->IsTrigger());
}

TEST_F(ColliderTest, MaterialProperties) {
    // Тест свойств материала
    collider->SetFriction(0.5f);
    collider->SetRestitution(0.8f);
    collider->SetDensity(2.0f);
    
    EXPECT_FLOAT_EQ(collider->GetFriction(), 0.5f);
    EXPECT_FLOAT_EQ(collider->GetRestitution(), 0.8f);
    EXPECT_FLOAT_EQ(collider->GetDensity(), 2.0f);
}

TEST_F(ColliderTest, CollisionLayers) {
    // Тест слоев коллизий
    collider->SetCollisionLayer(1);
    collider->SetCollisionMask(0xFFFF);
    
    EXPECT_EQ(collider->GetCollisionLayer(), 1);
    EXPECT_EQ(collider->GetCollisionMask(), 0xFFFF);
}

TEST_F(ColliderTest, SensorProperty) {
    // Тест сенсорного свойства
    collider->SetSensor(true);
    EXPECT_TRUE(collider->IsSensor());
    
    collider->SetSensor(false);
    EXPECT_FALSE(collider->IsSensor());
}

TEST_F(ColliderTest, ActiveProperty) {
    // Тест активности коллайдера
    collider->SetActive(true);
    EXPECT_TRUE(collider->IsActive());
    
    collider->SetActive(false);
    EXPECT_FALSE(collider->IsActive());
}

TEST_F(ColliderTest, AABB) {
    // Тест ограничивающего прямоугольника
    collider->SetType(ColliderType::Box);
    collider->SetSize(glm::vec2(100.0f, 50.0f));
    collider->SetOffset(glm::vec2(10.0f, 20.0f));
    
    auto aabb = collider->GetAABB();
    
    // Проверяем, что AABB не пустой
    EXPECT_TRUE(aabb.min.x < aabb.max.x);
    EXPECT_TRUE(aabb.min.y < aabb.max.y);
}

TEST_F(ColliderTest, CircleColliderBounds) {
    // Тест границ кругового коллайдера
    collider->SetType(ColliderType::Circle);
    collider->SetRadius(25.0f);
    collider->SetOffset(glm::vec2(50.0f, 50.0f));
    
    auto aabb = collider->GetAABB();
    
    // Для круга AABB должен быть квадратом
    float expectedSize = 50.0f; // 2 * radius
    EXPECT_FLOAT_EQ(aabb.max.x - aabb.min.x, expectedSize);
    EXPECT_FLOAT_EQ(aabb.max.y - aabb.min.y, expectedSize);
}

TEST_F(ColliderTest, PolygonColliderBounds) {
    // Тест границ полигонального коллайдера
    collider->SetType(ColliderType::Polygon);
    
    std::vector<glm::vec2> vertices = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(100.0f, 0.0f),
        glm::vec2(100.0f, 100.0f),
        glm::vec2(0.0f, 100.0f)
    };
    
    collider->SetVertices(vertices);
    auto aabb = collider->GetAABB();
    
    EXPECT_FLOAT_EQ(aabb.min.x, 0.0f);
    EXPECT_FLOAT_EQ(aabb.min.y, 0.0f);
    EXPECT_FLOAT_EQ(aabb.max.x, 100.0f);
    EXPECT_FLOAT_EQ(aabb.max.y, 100.0f);
}

TEST_F(ColliderTest, CollisionCallbacks) {
    // Тест callback'ов коллизий
    bool onEnterCalled = false;
    bool onExitCalled = false;
    
    collider->SetOnCollisionEnter([&onEnterCalled](Entity* other) {
        onEnterCalled = true;
    });
    
    collider->SetOnCollisionExit([&onExitCalled](Entity* other) {
        onExitCalled = true;
    });
    
    // Симулируем коллизию
    auto otherEntity = world->CreateEntity();
    collider->OnCollisionEnter(otherEntity.get());
    collider->OnCollisionExit(otherEntity.get());
    
    EXPECT_TRUE(onEnterCalled);
    EXPECT_TRUE(onExitCalled);
}

TEST_F(ColliderTest, CollisionFiltering) {
    // Тест фильтрации коллизий
    collider->SetCollisionLayer(1);
    collider->SetCollisionMask(0x02); // Только слой 1
    
    auto otherEntity = world->CreateEntity();
    auto otherCollider = otherEntity->AddComponent<Collider>();
    otherCollider->SetCollisionLayer(2);
    otherCollider->SetCollisionMask(0x01); // Только слой 0
    
    // Эти коллайдеры не должны сталкиваться
    EXPECT_FALSE(collider->CanCollideWith(otherCollider.get()));
    
    // Изменяем маску для возможности коллизии
    otherCollider->SetCollisionMask(0x01 | 0x02);
    EXPECT_TRUE(collider->CanCollideWith(otherCollider.get()));
}

TEST_F(ColliderTest, TransformIntegration) {
    // Тест интеграции с трансформацией
    auto transform = entity->AddComponent<Transform>();
    transform->SetPosition(glm::vec2(100.0f, 200.0f));
    transform->SetRotation(45.0f);
    transform->SetScale(glm::vec2(2.0f, 2.0f));
    
    collider->SetType(ColliderType::Box);
    collider->SetSize(glm::vec2(50.0f, 25.0f));
    
    // Обновляем коллайдер с учетом трансформации
    collider->UpdateTransform();
    
    auto aabb = collider->GetAABB();
    
    // AABB должен учитывать позицию, поворот и масштаб
    EXPECT_TRUE(aabb.min.x >= 0.0f);
    EXPECT_TRUE(aabb.min.y >= 0.0f);
}

TEST_F(ColliderTest, EdgeCollider) {
    // Тест коллайдера-края
    collider->SetType(ColliderType::Edge);
    
    std::vector<glm::vec2> points = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(100.0f, 0.0f)
    };
    
    collider->SetEdgePoints(points);
    auto edgePoints = collider->GetEdgePoints();
    
    EXPECT_EQ(edgePoints.size(), 2);
    EXPECT_FLOAT_EQ(edgePoints[0].x, 0.0f);
    EXPECT_FLOAT_EQ(edgePoints[0].y, 0.0f);
    EXPECT_FLOAT_EQ(edgePoints[1].x, 100.0f);
    EXPECT_FLOAT_EQ(edgePoints[1].y, 0.0f);
}

TEST_F(ColliderTest, ChainCollider) {
    // Тест цепочного коллайдера
    collider->SetType(ColliderType::Chain);
    
    std::vector<glm::vec2> chain = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(50.0f, 0.0f),
        glm::vec2(100.0f, 50.0f),
        glm::vec2(100.0f, 100.0f)
    };
    
    collider->SetChainPoints(chain);
    auto chainPoints = collider->GetChainPoints();
    
    EXPECT_EQ(chainPoints.size(), 4);
    for (size_t i = 0; i < chain.size(); ++i) {
        EXPECT_FLOAT_EQ(chainPoints[i].x, chain[i].x);
        EXPECT_FLOAT_EQ(chainPoints[i].y, chain[i].y);
    }
}

TEST_F(ColliderTest, CollisionShape) {
    // Тест получения формы коллизии
    collider->SetType(ColliderType::Box);
    collider->SetSize(glm::vec2(100.0f, 50.0f));
    
    auto shape = collider->GetCollisionShape();
    EXPECT_TRUE(shape != nullptr);
}

TEST_F(ColliderTest, MassProperties) {
    // Тест свойств массы
    collider->SetType(ColliderType::Box);
    collider->SetSize(glm::vec2(100.0f, 50.0f));
    collider->SetDensity(2.0f);
    
    auto mass = collider->GetMass();
    auto inertia = collider->GetInertia();
    
    // Масса должна быть больше 0
    EXPECT_GT(mass, 0.0f);
    EXPECT_GT(inertia, 0.0f);
}

TEST_F(ColliderTest, CollisionDetection) {
    // Тест обнаружения коллизий
    collider->SetType(ColliderType::Box);
    collider->SetSize(glm::vec2(50.0f, 50.0f);
    
    auto otherEntity = world->CreateEntity();
    auto otherCollider = otherEntity->AddComponent<Collider>();
    otherCollider->SetType(ColliderType::Box);
    otherCollider->SetSize(glm::vec2(50.0f, 50.0f));
    
    // Устанавливаем позиции для коллизии
    auto transform1 = entity->AddComponent<Transform>();
    auto transform2 = otherEntity->AddComponent<Transform>();
    
    transform1->SetPosition(glm::vec2(0.0f, 0.0f));
    transform2->SetPosition(glm::vec2(25.0f, 25.0f)); // Перекрытие
    
    // Обновляем коллайдеры
    collider->UpdateTransform();
    otherCollider->UpdateTransform();
    
    // Проверяем коллизию
    EXPECT_TRUE(collider->CheckCollision(otherCollider.get()));
}
