#include <gtest/gtest.h>
#include "FastEngine/Systems/PhysicsSystem.h"
#include "FastEngine/World.h"
#include "FastEngine/Entity.h"
#include "FastEngine/Components/RigidBody.h"
#include "FastEngine/Components/Collider.h"
#include "FastEngine/Components/Transform.h"
#include <memory>

using namespace FastEngine;

class PhysicsSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        world = std::make_unique<World>();
        world->Initialize();
        physicsSystem = std::make_unique<PhysicsSystem>();
        physicsSystem->Initialize();
    }
    
    void TearDown() override {
        physicsSystem->Shutdown();
        physicsSystem.reset();
        world.reset();
    }
    
    std::unique_ptr<World> world;
    std::unique_ptr<PhysicsSystem> physicsSystem;
};

TEST_F(PhysicsSystemTest, PhysicsSystemInitialization) {
    EXPECT_TRUE(physicsSystem != nullptr);
}

TEST_F(PhysicsSystemTest, GravitySettings) {
    // Тест установки гравитации
    glm::vec2 gravity(0.0f, -9.81f);
    physicsSystem->SetGravity(gravity);
    
    auto retrievedGravity = physicsSystem->GetGravity();
    EXPECT_FLOAT_EQ(retrievedGravity.x, gravity.x);
    EXPECT_FLOAT_EQ(retrievedGravity.y, gravity.y);
}

TEST_F(PhysicsSystemTest, TimeStepSettings) {
    // Тест установки временного шага
    float timeStep = 1.0f / 60.0f;
    physicsSystem->SetTimeStep(timeStep);
    
    EXPECT_FLOAT_EQ(physicsSystem->GetTimeStep(), timeStep);
}

TEST_F(PhysicsSystemTest, IterationSettings) {
    // Тест установки итераций
    physicsSystem->SetVelocityIterations(8);
    physicsSystem->SetPositionIterations(3);
    
    EXPECT_EQ(physicsSystem->GetVelocityIterations(), 8);
    EXPECT_EQ(physicsSystem->GetPositionIterations(), 3);
}

TEST_F(PhysicsSystemTest, PauseResume) {
    // Тест паузы и возобновления
    physicsSystem->SetPaused(true);
    EXPECT_TRUE(physicsSystem->IsPaused());
    
    physicsSystem->SetPaused(false);
    EXPECT_FALSE(physicsSystem->IsPaused());
}

TEST_F(PhysicsSystemTest, DebugDraw) {
    // Тест отладочной отрисовки
    physicsSystem->SetDebugDraw(true);
    EXPECT_TRUE(physicsSystem->IsDebugDraw());
    
    physicsSystem->SetDebugDraw(false);
    EXPECT_FALSE(physicsSystem->IsDebugDraw());
}

TEST_F(PhysicsSystemTest, EntityRegistration) {
    // Тест регистрации сущности в физической системе
    auto entity = world->CreateEntity();
    auto rigidBody = entity->AddComponent<RigidBody>();
    auto collider = entity->AddComponent<Collider>();
    
    // Сущность должна быть автоматически зарегистрирована
    EXPECT_TRUE(physicsSystem->IsEntityRegistered(entity.get()));
}

TEST_F(PhysicsSystemTest, ForceApplication) {
    // Тест применения силы
    auto entity = world->CreateEntity();
    auto rigidBody = entity->AddComponent<RigidBody>();
    rigidBody->SetMass(1.0f);
    
    glm::vec2 force(100.0f, 0.0f);
    physicsSystem->ApplyForce(entity.get(), force);
    
    // Проверяем, что сила применена
    auto velocity = physicsSystem->GetVelocity(entity.get());
    EXPECT_GT(velocity.x, 0.0f);
}

TEST_F(PhysicsSystemTest, ImpulseApplication) {
    // Тест применения импульса
    auto entity = world->CreateEntity();
    auto rigidBody = entity->AddComponent<RigidBody>();
    rigidBody->SetMass(1.0f);
    
    glm::vec2 impulse(50.0f, 25.0f);
    physicsSystem->ApplyImpulse(entity.get(), impulse);
    
    // Проверяем, что импульс применен
    auto velocity = physicsSystem->GetVelocity(entity.get());
    EXPECT_FLOAT_EQ(velocity.x, 50.0f);
    EXPECT_FLOAT_EQ(velocity.y, 25.0f);
}

TEST_F(PhysicsSystemTest, TorqueApplication) {
    // Тест применения крутящего момента
    auto entity = world->CreateEntity();
    auto rigidBody = entity->AddComponent<RigidBody>();
    rigidBody->SetInertia(1.0f);
    
    float torque = 10.0f;
    physicsSystem->ApplyTorque(entity.get(), torque);
    
    // Проверяем, что крутящий момент применен
    auto angularVelocity = physicsSystem->GetAngularVelocity(entity.get());
    EXPECT_FLOAT_EQ(angularVelocity, 10.0f);
}

TEST_F(PhysicsSystemTest, ForceAtPoint) {
    // Тест применения силы в определенной точке
    auto entity = world->CreateEntity();
    auto rigidBody = entity->AddComponent<RigidBody>();
    rigidBody->SetMass(1.0f);
    rigidBody->SetInertia(1.0f);
    
    glm::vec2 force(100.0f, 0.0f);
    glm::vec2 point(1.0f, 0.0f);
    
    physicsSystem->ApplyForceAtPoint(entity.get(), force, point);
    
    // Проверяем, что сила и крутящий момент применены
    auto velocity = physicsSystem->GetVelocity(entity.get());
    auto angularVelocity = physicsSystem->GetAngularVelocity(entity.get());
    
    EXPECT_FLOAT_EQ(velocity.x, 100.0f);
    EXPECT_FLOAT_EQ(angularVelocity, 100.0f); // Сила * расстояние
}

TEST_F(PhysicsSystemTest, CollisionDetection) {
    // Тест обнаружения коллизий
    auto entity1 = world->CreateEntity();
    auto entity2 = world->CreateEntity();
    
    // Настраиваем сущности
    auto rb1 = entity1->AddComponent<RigidBody>();
    auto rb2 = entity2->AddComponent<RigidBody>();
    auto col1 = entity1->AddComponent<Collider>();
    auto col2 = entity2->AddComponent<Collider>();
    
    auto transform1 = entity1->AddComponent<Transform>();
    auto transform2 = entity2->AddComponent<Transform>();
    
    // Устанавливаем позиции для коллизии
    transform1->SetPosition(glm::vec2(0.0f, 0.0f));
    transform2->SetPosition(glm::vec2(25.0f, 25.0f));
    
    col1->SetType(ColliderType::Box);
    col1->SetSize(glm::vec2(50.0f, 50.0f));
    col2->SetType(ColliderType::Box);
    col2->SetSize(glm::vec2(50.0f, 50.0f));
    
    // Обновляем физическую систему
    physicsSystem->Update(0.016f);
    
    // Проверяем, что коллизия обнаружена
    EXPECT_TRUE(physicsSystem->CheckCollision(entity1.get(), entity2.get()));
}

TEST_F(PhysicsSystemTest, CollisionCallbacks) {
    // Тест callback'ов коллизий
    auto entity1 = world->CreateEntity();
    auto entity2 = world->CreateEntity();
    
    bool onEnterCalled = false;
    bool onExitCalled = false;
    
    physicsSystem->SetOnCollisionEnter([&onEnterCalled](Entity* a, Entity* b) {
        onEnterCalled = true;
    });
    
    physicsSystem->SetOnCollisionExit([&onExitCalled](Entity* a, Entity* b) {
        onExitCalled = true;
    });
    
    // Настраиваем сущности для коллизии
    auto rb1 = entity1->AddComponent<RigidBody>();
    auto rb2 = entity2->AddComponent<RigidBody>();
    auto col1 = entity1->AddComponent<Collider>();
    auto col2 = entity2->AddComponent<Collider>();
    
    auto transform1 = entity1->AddComponent<Transform>();
    auto transform2 = entity2->AddComponent<Transform>();
    
    transform1->SetPosition(glm::vec2(0.0f, 0.0f));
    transform2->SetPosition(glm::vec2(25.0f, 25.0f));
    
    col1->SetType(ColliderType::Box);
    col1->SetSize(glm::vec2(50.0f, 50.0f));
    col2->SetType(ColliderType::Box);
    col2->SetSize(glm::vec2(50.0f, 50.0f));
    
    // Обновляем физическую систему
    physicsSystem->Update(0.016f);
    
    // Проверяем, что callback'и сработали
    EXPECT_TRUE(onEnterCalled);
}

TEST_F(PhysicsSystemTest, MassProperties) {
    // Тест свойств массы
    auto entity = world->CreateEntity();
    auto rigidBody = entity->AddComponent<RigidBody>();
    auto collider = entity->AddComponent<Collider>();
    
    rigidBody->SetMass(10.0f);
    collider->SetType(ColliderType::Box);
    collider->SetSize(glm::vec2(100.0f, 50.0f));
    
    auto mass = physicsSystem->GetMass(entity.get());
    EXPECT_FLOAT_EQ(mass, 10.0f);
}

TEST_F(PhysicsSystemTest, VelocityProperties) {
    // Тест свойств скорости
    auto entity = world->CreateEntity();
    auto rigidBody = entity->AddComponent<RigidBody>();
    
    rigidBody->SetVelocity(glm::vec2(10.0f, 20.0f));
    rigidBody->SetAngularVelocity(5.0f);
    
    auto velocity = physicsSystem->GetVelocity(entity.get());
    auto angularVelocity = physicsSystem->GetAngularVelocity(entity.get());
    
    EXPECT_FLOAT_EQ(velocity.x, 10.0f);
    EXPECT_FLOAT_EQ(velocity.y, 20.0f);
    EXPECT_FLOAT_EQ(angularVelocity, 5.0f);
}

TEST_F(PhysicsSystemTest, PhysicsUpdate) {
    // Тест обновления физической системы
    auto entity = world->CreateEntity();
    auto rigidBody = entity->AddComponent<RigidBody>();
    auto collider = entity->AddComponent<Collider>();
    
    rigidBody->SetMass(1.0f);
    rigidBody->SetBodyType(BodyType::Dynamic);
    collider->SetType(ColliderType::Box);
    collider->SetSize(glm::vec2(50.0f, 50.0f));
    
    // Применяем силу
    physicsSystem->ApplyForce(entity.get(), glm::vec2(100.0f, 0.0f));
    
    // Обновляем физическую систему
    physicsSystem->Update(0.016f);
    
    // Проверяем, что сущность движется
    auto velocity = physicsSystem->GetVelocity(entity.get());
    EXPECT_GT(velocity.x, 0.0f);
}

TEST_F(PhysicsSystemTest, StaticBody) {
    // Тест статического тела
    auto entity = world->CreateEntity();
    auto rigidBody = entity->AddComponent<RigidBody>();
    auto collider = entity->AddComponent<Collider>();
    
    rigidBody->SetBodyType(BodyType::Static);
    collider->SetType(ColliderType::Box);
    collider->SetSize(glm::vec2(50.0f, 50.0f));
    
    // Применяем силу к статическому телу
    physicsSystem->ApplyForce(entity.get(), glm::vec2(100.0f, 0.0f));
    
    // Обновляем физическую систему
    physicsSystem->Update(0.016f);
    
    // Статическое тело не должно двигаться
    auto velocity = physicsSystem->GetVelocity(entity.get());
    EXPECT_FLOAT_EQ(velocity.x, 0.0f);
    EXPECT_FLOAT_EQ(velocity.y, 0.0f);
}

TEST_F(PhysicsSystemTest, KinematicBody) {
    // Тест кинематического тела
    auto entity = world->CreateEntity();
    auto rigidBody = entity->AddComponent<RigidBody>();
    auto collider = entity->AddComponent<Collider>();
    
    rigidBody->SetBodyType(BodyType::Kinematic);
    collider->SetType(ColliderType::Box);
    collider->SetSize(glm::vec2(50.0f, 50.0f));
    
    // Устанавливаем скорость кинематического тела
    rigidBody->SetVelocity(glm::vec2(10.0f, 0.0f));
    
    // Обновляем физическую систему
    physicsSystem->Update(0.016f);
    
    // Кинематическое тело должно двигаться с заданной скоростью
    auto velocity = physicsSystem->GetVelocity(entity.get());
    EXPECT_FLOAT_EQ(velocity.x, 10.0f);
    EXPECT_FLOAT_EQ(velocity.y, 0.0f);
}

TEST_F(PhysicsSystemTest, CollisionResponse) {
    // Тест отклика на коллизию
    auto entity1 = world->CreateEntity();
    auto entity2 = world->CreateEntity();
    
    auto rb1 = entity1->AddComponent<RigidBody>();
    auto rb2 = entity2->AddComponent<RigidBody>();
    auto col1 = entity1->AddComponent<Collider>();
    auto col2 = entity2->AddComponent<Collider>();
    
    auto transform1 = entity1->AddComponent<Transform>();
    auto transform2 = entity2->AddComponent<Transform>();
    
    // Настраиваем сущности
    rb1->SetMass(1.0f);
    rb2->SetMass(1.0f);
    rb1->SetBodyType(BodyType::Dynamic);
    rb2->SetBodyType(BodyType::Dynamic);
    
    col1->SetType(ColliderType::Box);
    col1->SetSize(glm::vec2(50.0f, 50.0f));
    col2->SetType(ColliderType::Box);
    col2->SetSize(glm::vec2(50.0f, 50.0f));
    
    // Устанавливаем позиции для коллизии
    transform1->SetPosition(glm::vec2(0.0f, 0.0f));
    transform2->SetPosition(glm::vec2(25.0f, 25.0f));
    
    // Применяем силу к первой сущности
    physicsSystem->ApplyForce(entity1.get(), glm::vec2(100.0f, 0.0f));
    
    // Обновляем физическую систему
    physicsSystem->Update(0.016f);
    
    // Проверяем, что обе сущности получили отклик на коллизию
    auto velocity1 = physicsSystem->GetVelocity(entity1.get());
    auto velocity2 = physicsSystem->GetVelocity(entity2.get());
    
    // Скорости должны измениться из-за коллизии
    EXPECT_TRUE(velocity1.x != 100.0f || velocity2.x != 0.0f);
}

TEST_F(PhysicsSystemTest, Damping) {
    // Тест затухания
    auto entity = world->CreateEntity();
    auto rigidBody = entity->AddComponent<RigidBody>();
    auto collider = entity->AddComponent<Collider>();
    
    rigidBody->SetMass(1.0f);
    rigidBody->SetBodyType(BodyType::Dynamic);
    rigidBody->SetLinearDamping(0.5f);
    rigidBody->SetAngularDamping(0.3f);
    
    collider->SetType(ColliderType::Box);
    collider->SetSize(glm::vec2(50.0f, 50.0f));
    
    // Устанавливаем начальную скорость
    rigidBody->SetVelocity(glm::vec2(100.0f, 0.0f));
    rigidBody->SetAngularVelocity(10.0f);
    
    // Обновляем физическую систему несколько раз
    for (int i = 0; i < 10; ++i) {
        physicsSystem->Update(0.016f);
    }
    
    // Скорость должна уменьшиться из-за затухания
    auto velocity = physicsSystem->GetVelocity(entity.get());
    auto angularVelocity = physicsSystem->GetAngularVelocity(entity.get());
    
    EXPECT_LT(velocity.x, 100.0f);
    EXPECT_LT(angularVelocity, 10.0f);
}

TEST_F(PhysicsSystemTest, Gravity) {
    // Тест гравитации
    auto entity = world->CreateEntity();
    auto rigidBody = entity->AddComponent<RigidBody>();
    auto collider = entity->AddComponent<Collider>();
    
    rigidBody->SetMass(1.0f);
    rigidBody->SetBodyType(BodyType::Dynamic);
    rigidBody->SetGravityScale(1.0f);
    
    collider->SetType(ColliderType::Box);
    collider->SetSize(glm::vec2(50.0f, 50.0f));
    
    // Устанавливаем гравитацию
    physicsSystem->SetGravity(glm::vec2(0.0f, -9.81f));
    
    // Обновляем физическую систему
    physicsSystem->Update(0.016f);
    
    // Сущность должна падать под действием гравитации
    auto velocity = physicsSystem->GetVelocity(entity.get());
    EXPECT_LT(velocity.y, 0.0f);
}

TEST_F(PhysicsSystemTest, CollisionFiltering) {
    // Тест фильтрации коллизий
    auto entity1 = world->CreateEntity();
    auto entity2 = world->CreateEntity();
    
    auto rb1 = entity1->AddComponent<RigidBody>();
    auto rb2 = entity2->AddComponent<RigidBody>();
    auto col1 = entity1->AddComponent<Collider>();
    auto col2 = entity2->AddComponent<Collider>();
    
    auto transform1 = entity1->AddComponent<Transform>();
    auto transform2 = entity2->AddComponent<Transform>();
    
    // Настраиваем слои коллизий
    col1->SetCollisionLayer(1);
    col1->SetCollisionMask(0x01); // Только слой 0
    col2->SetCollisionLayer(2);
    col2->SetCollisionMask(0x02); // Только слой 1
    
    // Устанавливаем позиции для коллизии
    transform1->SetPosition(glm::vec2(0.0f, 0.0f));
    transform2->SetPosition(glm::vec2(25.0f, 25.0f));
    
    col1->SetType(ColliderType::Box);
    col1->SetSize(glm::vec2(50.0f, 50.0f));
    col2->SetType(ColliderType::Box);
    col2->SetSize(glm::vec2(50.0f, 50.0f));
    
    // Обновляем физическую систему
    physicsSystem->Update(0.016f);
    
    // Эти сущности не должны сталкиваться из-за фильтрации
    EXPECT_FALSE(physicsSystem->CheckCollision(entity1.get(), entity2.get()));
}
