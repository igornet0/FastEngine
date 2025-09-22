#include <gtest/gtest.h>
#include "FastEngine/Components/RigidBody.h"
#include "FastEngine/Entity.h"
#include "FastEngine/World.h"
#include <memory>

using namespace FastEngine;

class RigidBodyTest : public ::testing::Test {
protected:
    void SetUp() override {
        world = std::make_unique<World>();
        world->Initialize();
        entity = world->CreateEntity();
        rigidBody = entity->AddComponent<RigidBody>();
    }
    
    void TearDown() override {
        rigidBody = nullptr;
        entity = nullptr;
        world.reset();
    }
    
    std::unique_ptr<World> world;
    std::shared_ptr<Entity> entity;
    std::shared_ptr<RigidBody> rigidBody;
};

TEST_F(RigidBodyTest, RigidBodyCreation) {
    EXPECT_TRUE(rigidBody != nullptr);
    EXPECT_TRUE(entity->HasComponent<RigidBody>());
}

TEST_F(RigidBodyTest, MassProperties) {
    // Тест установки массы
    rigidBody->SetMass(10.0f);
    EXPECT_FLOAT_EQ(rigidBody->GetMass(), 10.0f);
    
    // Тест установки инерции
    rigidBody->SetInertia(5.0f);
    EXPECT_FLOAT_EQ(rigidBody->GetInertia(), 5.0f);
    
    // Тест установки центра масс
    rigidBody->SetCenterOfMass(glm::vec2(2.0f, 3.0f));
    auto com = rigidBody->GetCenterOfMass();
    EXPECT_FLOAT_EQ(com.x, 2.0f);
    EXPECT_FLOAT_EQ(com.y, 3.0f);
}

TEST_F(RigidBodyTest, VelocityProperties) {
    // Тест установки линейной скорости
    rigidBody->SetVelocity(glm::vec2(10.0f, 20.0f));
    auto velocity = rigidBody->GetVelocity();
    EXPECT_FLOAT_EQ(velocity.x, 10.0f);
    EXPECT_FLOAT_EQ(velocity.y, 20.0f);
    
    // Тест установки угловой скорости
    rigidBody->SetAngularVelocity(5.0f);
    EXPECT_FLOAT_EQ(rigidBody->GetAngularVelocity(), 5.0f);
}

TEST_F(RigidBodyTest, ForceApplication) {
    // Тест применения силы
    rigidBody->SetMass(1.0f);
    rigidBody->ApplyForce(glm::vec2(100.0f, 0.0f));
    
    // Проверяем, что сила применена
    auto forces = rigidBody->GetForces();
    EXPECT_FLOAT_EQ(forces.x, 100.0f);
    EXPECT_FLOAT_EQ(forces.y, 0.0f);
}

TEST_F(RigidBodyTest, ImpulseApplication) {
    // Тест применения импульса
    rigidBody->SetMass(1.0f);
    rigidBody->ApplyImpulse(glm::vec2(50.0f, 25.0f));
    
    // Проверяем, что импульс применен
    auto velocity = rigidBody->GetVelocity();
    EXPECT_FLOAT_EQ(velocity.x, 50.0f);
    EXPECT_FLOAT_EQ(velocity.y, 25.0f);
}

TEST_F(RigidBodyTest, TorqueApplication) {
    // Тест применения крутящего момента
    rigidBody->SetInertia(1.0f);
    rigidBody->ApplyTorque(10.0f);
    
    // Проверяем, что крутящий момент применен
    auto torques = rigidBody->GetTorques();
    EXPECT_FLOAT_EQ(torques, 10.0f);
}

TEST_F(RigidBodyTest, BodyType) {
    // Тест установки типа тела
    rigidBody->SetBodyType(BodyType::Dynamic);
    EXPECT_EQ(rigidBody->GetBodyType(), BodyType::Dynamic);
    
    rigidBody->SetBodyType(BodyType::Static);
    EXPECT_EQ(rigidBody->GetBodyType(), BodyType::Static);
    
    rigidBody->SetBodyType(BodyType::Kinematic);
    EXPECT_EQ(rigidBody->GetBodyType(), BodyType::Kinematic);
}

TEST_F(RigidBodyTest, Damping) {
    // Тест установки затухания
    rigidBody->SetLinearDamping(0.5f);
    rigidBody->SetAngularDamping(0.3f);
    
    EXPECT_FLOAT_EQ(rigidBody->GetLinearDamping(), 0.5f);
    EXPECT_FLOAT_EQ(rigidBody->GetAngularDamping(), 0.3f);
}

TEST_F(RigidBodyTest, GravityScale) {
    // Тест установки масштаба гравитации
    rigidBody->SetGravityScale(2.0f);
    EXPECT_FLOAT_EQ(rigidBody->GetGravityScale(), 2.0f);
}

TEST_F(RigidBodyTest, SleepProperties) {
    // Тест установки сна тела
    rigidBody->SetAwake(true);
    EXPECT_TRUE(rigidBody->IsAwake());
    
    rigidBody->SetAwake(false);
    EXPECT_FALSE(rigidBody->IsAwake());
}

TEST_F(RigidBodyTest, FixedRotation) {
    // Тест фиксированного поворота
    rigidBody->SetFixedRotation(true);
    EXPECT_TRUE(rigidBody->IsFixedRotation());
    
    rigidBody->SetFixedRotation(false);
    EXPECT_FALSE(rigidBody->IsFixedRotation());
}

TEST_F(RigidBodyTest, BulletProperty) {
    // Тест свойства "пуля" (для быстрых объектов)
    rigidBody->SetBullet(true);
    EXPECT_TRUE(rigidBody->IsBullet());
    
    rigidBody->SetBullet(false);
    EXPECT_FALSE(rigidBody->IsBullet());
}

TEST_F(RigidBodyTest, ActiveProperty) {
    // Тест активности тела
    rigidBody->SetActive(true);
    EXPECT_TRUE(rigidBody->IsActive());
    
    rigidBody->SetActive(false);
    EXPECT_FALSE(rigidBody->IsActive());
}

TEST_F(RigidBodyTest, ForceAtPoint) {
    // Тест применения силы в определенной точке
    rigidBody->SetMass(1.0f);
    rigidBody->SetInertia(1.0f);
    
    rigidBody->ApplyForceAtPoint(glm::vec2(100.0f, 0.0f), glm::vec2(1.0f, 0.0f));
    
    // Проверяем, что сила и крутящий момент применены
    auto forces = rigidBody->GetForces();
    auto torques = rigidBody->GetTorques();
    
    EXPECT_FLOAT_EQ(forces.x, 100.0f);
    EXPECT_FLOAT_EQ(forces.y, 0.0f);
    EXPECT_FLOAT_EQ(torques, 100.0f); // Сила * расстояние
}

TEST_F(RigidBodyTest, ClearForces) {
    // Тест очистки сил
    rigidBody->ApplyForce(glm::vec2(100.0f, 50.0f));
    rigidBody->ApplyTorque(25.0f);
    
    auto forces = rigidBody->GetForces();
    auto torques = rigidBody->GetTorques();
    
    EXPECT_FLOAT_EQ(forces.x, 100.0f);
    EXPECT_FLOAT_EQ(torques, 25.0f);
    
    rigidBody->ClearForces();
    
    forces = rigidBody->GetForces();
    torques = rigidBody->GetTorques();
    
    EXPECT_FLOAT_EQ(forces.x, 0.0f);
    EXPECT_FLOAT_EQ(forces.y, 0.0f);
    EXPECT_FLOAT_EQ(torques, 0.0f);
}

TEST_F(RigidBodyTest, PositionAndRotation) {
    // Тест установки позиции и поворота
    rigidBody->SetPosition(glm::vec2(100.0f, 200.0f));
    rigidBody->SetRotation(45.0f);
    
    auto position = rigidBody->GetPosition();
    auto rotation = rigidBody->GetRotation();
    
    EXPECT_FLOAT_EQ(position.x, 100.0f);
    EXPECT_FLOAT_EQ(position.y, 200.0f);
    EXPECT_FLOAT_EQ(rotation, 45.0f);
}

TEST_F(RigidBodyTest, TransformMatrix) {
    // Тест получения матрицы преобразования
    rigidBody->SetPosition(glm::vec2(10.0f, 20.0f));
    rigidBody->SetRotation(30.0f);
    
    auto transform = rigidBody->GetTransform();
    
    // Проверяем, что матрица не нулевая
    EXPECT_FALSE(glm::length(glm::vec2(transform[3])) < 0.001f);
}

TEST_F(RigidBodyTest, CollisionLayers) {
    // Тест слоев коллизий
    rigidBody->SetCollisionLayer(1);
    rigidBody->SetCollisionMask(0xFFFF);
    
    EXPECT_EQ(rigidBody->GetCollisionLayer(), 1);
    EXPECT_EQ(rigidBody->GetCollisionMask(), 0xFFFF);
}

TEST_F(RigidBodyTest, Restitution) {
    // Тест упругости
    rigidBody->SetRestitution(0.8f);
    EXPECT_FLOAT_EQ(rigidBody->GetRestitution(), 0.8f);
}

TEST_F(RigidBodyTest, Friction) {
    // Тест трения
    rigidBody->SetFriction(0.5f);
    EXPECT_FLOAT_EQ(rigidBody->GetFriction(), 0.5f);
}

TEST_F(RigidBodyTest, Density) {
    // Тест плотности
    rigidBody->SetDensity(2.0f);
    EXPECT_FLOAT_EQ(rigidBody->GetDensity(), 2.0f);
}

TEST_F(RigidBodyTest, SensorProperty) {
    // Тест сенсорного свойства
    rigidBody->SetSensor(true);
    EXPECT_TRUE(rigidBody->IsSensor());
    
    rigidBody->SetSensor(false);
    EXPECT_FALSE(rigidBody->IsSensor());
}
