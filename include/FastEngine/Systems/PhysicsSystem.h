#pragma once

#include "FastEngine/System.h"
#include "FastEngine/Components/RigidBody.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Entity.h"
#include <glm/glm.hpp>
#include <functional>

namespace FastEngine {
    class PhysicsSystem : public System {
    public:
        PhysicsSystem();
        ~PhysicsSystem() override = default;
        
        // Обновление системы
        void Update(float deltaTime) override;
        
        // Настройки физики
        void SetGravity(const glm::vec2& gravity) { m_gravity = gravity; }
        glm::vec2 GetGravity() const { return m_gravity; }
        
        void SetTimeStep(float timeStep) { m_timeStep = timeStep; }
        float GetTimeStep() const { return m_timeStep; }
        
        void SetVelocityIterations(int iterations) { m_velocityIterations = iterations; }
        int GetVelocityIterations() const { return m_velocityIterations; }
        
        void SetPositionIterations(int iterations) { m_positionIterations = iterations; }
        int GetPositionIterations() const { return m_positionIterations; }
        
        // Управление симуляцией
        void SetPaused(bool paused) { m_paused = paused; }
        bool IsPaused() const { return m_paused; }
        
        void SetDebugDraw(bool enabled) { m_debugDraw = enabled; }
        bool IsDebugDraw() const { return m_debugDraw; }
        
        // Применение сил
        void ApplyForce(Entity* entity, const glm::vec2& force);
        void ApplyForceAtPoint(Entity* entity, const glm::vec2& force, const glm::vec2& point);
        void ApplyImpulse(Entity* entity, const glm::vec2& impulse);
        void ApplyTorque(Entity* entity, float torque);
        
        // Получение информации
        glm::vec2 GetVelocity(Entity* entity) const;
        float GetAngularVelocity(Entity* entity) const;
        float GetMass(Entity* entity) const;
        
        // События
        void SetOnCollisionEnter(std::function<void(Entity*, Entity*)> callback);
        void SetOnCollisionExit(std::function<void(Entity*, Entity*)> callback);
        
    private:
        glm::vec2 m_gravity;
        float m_timeStep;
        int m_velocityIterations;
        int m_positionIterations;
        bool m_paused;
        bool m_debugDraw;
        
        // Накопленное время для фиксированных шагов
        float m_accumulator;
        
        // События
        std::function<void(Entity*, Entity*)> m_onCollisionEnter;
        std::function<void(Entity*, Entity*)> m_onCollisionExit;
        
        // Вспомогательные методы
        void Integrate(Entity* entity, float deltaTime);
        void CheckCollisions();
        void ResolveCollision(Entity* entityA, Entity* entityB);
        RigidBody* GetRigidBody(Entity* entity) const;
        Transform* GetTransform(Entity* entity) const;
    };
}
