#include "FastEngine/Systems/PhysicsSystem.h"
#include "FastEngine/Entity.h"
#include "FastEngine/World.h"
#include "FastEngine/Components/Collider.h"
#include "FastEngine/Physics/Collision.h"
#include <algorithm>

namespace FastEngine {
    PhysicsSystem::PhysicsSystem() 
        : System(nullptr)
        , m_gravity(0.0f, -9.81f)
        , m_timeStep(1.0f / 60.0f)
        , m_velocityIterations(6)
        , m_positionIterations(2)
        , m_paused(false)
        , m_debugDraw(false)
        , m_accumulator(0.0f) {
    }
    
    void PhysicsSystem::Update(float deltaTime) {
        if (m_paused) {
            return;
        }
        
        m_accumulator += deltaTime;
        
        // Используем фиксированные временные шаги для стабильности физики
        while (m_accumulator >= m_timeStep) {
            // Обновляем все RigidBody компоненты
            auto entities = m_world->GetEntitiesWithComponents<RigidBody>();
            
            for (auto* entity : entities) {
                Integrate(entity, m_timeStep);
            }
            
            // Проверяем коллизии
            CheckCollisions();
            
            m_accumulator -= m_timeStep;
        }
    }
    
    void PhysicsSystem::ApplyForce(Entity* entity, const glm::vec2& force) {
        RigidBody* rigidBody = GetRigidBody(entity);
        if (rigidBody) {
            rigidBody->ApplyForce(force);
        }
    }
    
    void PhysicsSystem::ApplyForceAtPoint(Entity* entity, const glm::vec2& force, const glm::vec2& point) {
        RigidBody* rigidBody = GetRigidBody(entity);
        if (rigidBody) {
            rigidBody->ApplyForceAtPoint(force, point);
        }
    }
    
    void PhysicsSystem::ApplyImpulse(Entity* entity, const glm::vec2& impulse) {
        RigidBody* rigidBody = GetRigidBody(entity);
        if (rigidBody) {
            rigidBody->ApplyImpulse(impulse);
        }
    }
    
    void PhysicsSystem::ApplyTorque(Entity* entity, float torque) {
        RigidBody* rigidBody = GetRigidBody(entity);
        if (rigidBody) {
            rigidBody->ApplyTorque(torque);
        }
    }
    
    glm::vec2 PhysicsSystem::GetVelocity(Entity* entity) const {
        RigidBody* rigidBody = GetRigidBody(entity);
        return rigidBody ? rigidBody->GetVelocity() : glm::vec2(0.0f);
    }
    
    float PhysicsSystem::GetAngularVelocity(Entity* entity) const {
        RigidBody* rigidBody = GetRigidBody(entity);
        return rigidBody ? rigidBody->GetAngularVelocity() : 0.0f;
    }
    
    float PhysicsSystem::GetMass(Entity* entity) const {
        RigidBody* rigidBody = GetRigidBody(entity);
        return rigidBody ? rigidBody->GetMass() : 0.0f;
    }
    
    void PhysicsSystem::SetOnCollisionEnter(std::function<void(Entity*, Entity*)> callback) {
        m_onCollisionEnter = callback;
    }
    
    void PhysicsSystem::SetOnCollisionExit(std::function<void(Entity*, Entity*)> callback) {
        m_onCollisionExit = callback;
    }
    
    void PhysicsSystem::Integrate(Entity* entity, float deltaTime) {
        RigidBody* rigidBody = GetRigidBody(entity);
        Transform* transform = GetTransform(entity);
        
        if (!rigidBody || !transform) {
            return;
        }
        
        // Применяем гравитацию
        if (rigidBody->GetBodyType() == BodyType::Dynamic) {
            glm::vec2 gravityForce = m_gravity * rigidBody->GetMass() * rigidBody->GetGravityScale();
            rigidBody->ApplyForce(gravityForce);
        }
        
        // Обновляем физику
        rigidBody->Update(deltaTime);
        
        // Обновляем позицию через Transform
        glm::vec2 velocity = rigidBody->GetVelocity();
        glm::vec2 currentPosition = transform->GetPosition();
        transform->SetPosition(currentPosition + velocity * deltaTime);
        
        // Обновляем вращение
        if (!rigidBody->IsFixedRotation()) {
            float angularVelocity = rigidBody->GetAngularVelocity();
            float currentRotation = transform->GetRotation();
            transform->SetRotation(currentRotation + angularVelocity * deltaTime);
        }
    }
    
    void PhysicsSystem::CheckCollisions() {
        auto entities = m_world->GetEntitiesWithComponents<Collider>();
        
        // Простая проверка коллизий между всеми парами сущностей
        for (size_t i = 0; i < entities.size(); ++i) {
            for (size_t j = i + 1; j < entities.size(); ++j) {
                Entity* entityA = entities[i];
                Entity* entityB = entities[j];
                
                Collider* colliderA = entityA->GetComponent<Collider>();
                Collider* colliderB = entityB->GetComponent<Collider>();
                
                if (!colliderA || !colliderB) {
                    continue;
                }
                
                Transform* transformA = entityA->GetComponent<Transform>();
                Transform* transformB = entityB->GetComponent<Transform>();
                
                if (!transformA || !transformB) {
                    continue;
                }
                
                // Проверяем коллизию
                if (colliderA->CheckCollision(*colliderB, transformA->GetPosition(), transformB->GetPosition())) {
                    ResolveCollision(entityA, entityB);
                    
                    // Вызываем события
                    if (m_onCollisionEnter) {
                        m_onCollisionEnter(entityA, entityB);
                    }
                }
            }
        }
    }
    
    void PhysicsSystem::ResolveCollision(Entity* entityA, Entity* entityB) {
        RigidBody* rigidBodyA = GetRigidBody(entityA);
        RigidBody* rigidBodyB = GetRigidBody(entityB);
        Transform* transformA = GetTransform(entityA);
        Transform* transformB = GetTransform(entityB);
        
        if (!rigidBodyA || !rigidBodyB || !transformA || !transformB) {
            return;
        }
        
        // Упрощенное разрешение коллизий
        // В полной реализации здесь бы использовались импульсы и массы
        
        glm::vec2 velocityA = rigidBodyA->GetVelocity();
        glm::vec2 velocityB = rigidBodyB->GetVelocity();
        
        // Простое отталкивание
        glm::vec2 direction = glm::normalize(transformB->GetPosition() - transformA->GetPosition());
        
        if (rigidBodyA->GetBodyType() == BodyType::Dynamic) {
            rigidBodyA->ApplyImpulse(-direction * 0.1f);
        }
        
        if (rigidBodyB->GetBodyType() == BodyType::Dynamic) {
            rigidBodyB->ApplyImpulse(direction * 0.1f);
        }
    }
    
    RigidBody* PhysicsSystem::GetRigidBody(Entity* entity) const {
        return entity->GetComponent<RigidBody>();
    }
    
    Transform* PhysicsSystem::GetTransform(Entity* entity) const {
        return entity->GetComponent<Transform>();
    }
}
