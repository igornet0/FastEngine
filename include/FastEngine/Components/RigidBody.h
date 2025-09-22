#pragma once

#include "FastEngine/Component.h"
#include <glm/glm.hpp>

namespace FastEngine {
    enum class BodyType {
        Static,     // Статическое тело (не движется)
        Dynamic,    // Динамическое тело (подвержено физике)
        Kinematic   // Кинематическое тело (движется, но не подвержено силам)
    };
    
    class RigidBody : public Component {
    public:
        RigidBody();
        ~RigidBody() override = default;
        
        // Основные свойства
        void SetBodyType(BodyType type) { m_bodyType = type; }
        BodyType GetBodyType() const { return m_bodyType; }
        
        // Масса и инерция
        void SetMass(float mass);
        float GetMass() const { return m_mass; }
        
        void SetInertia(float inertia) { m_inertia = inertia; }
        float GetInertia() const { return m_inertia; }
        
        // Скорость
        void SetVelocity(const glm::vec2& velocity) { m_velocity = velocity; }
        glm::vec2 GetVelocity() const { return m_velocity; }
        
        void SetAngularVelocity(float angularVelocity) { m_angularVelocity = angularVelocity; }
        float GetAngularVelocity() const { return m_angularVelocity; }
        
        // Силы
        void ApplyForce(const glm::vec2& force);
        void ApplyForceAtPoint(const glm::vec2& force, const glm::vec2& point);
        void ApplyTorque(float torque);
        void ApplyImpulse(const glm::vec2& impulse);
        void ApplyAngularImpulse(float impulse);
        
        // Свойства материала
        void SetFriction(float friction) { m_friction = friction; }
        float GetFriction() const { return m_friction; }
        
        void SetRestitution(float restitution) { m_restitution = restitution; }
        float GetRestitution() const { return m_restitution; }
        
        void SetDensity(float density) { m_density = density; }
        float GetDensity() const { return m_density; }
        
        // Гравитация
        void SetGravityScale(float scale) { m_gravityScale = scale; }
        float GetGravityScale() const { return m_gravityScale; }
        
        // Ограничения движения
        void SetFixedRotation(bool fixed) { m_fixedRotation = fixed; }
        bool IsFixedRotation() const { return m_fixedRotation; }
        
        void SetLinearDamping(float damping) { m_linearDamping = damping; }
        float GetLinearDamping() const { return m_linearDamping; }
        
        void SetAngularDamping(float damping) { m_angularDamping = damping; }
        float GetAngularDamping() const { return m_angularDamping; }
        
        // Состояние
        bool IsAwake() const { return m_awake; }
        void SetAwake(bool awake) { m_awake = awake; }
        
        bool IsBullet() const { return m_bullet; }
        void SetBullet(bool bullet) { m_bullet = bullet; }
        
        // Обновление (вызывается PhysicsSystem)
        void Update(float deltaTime) override;
        
        // Сброс сил
        void ClearForces();
        
        // Получение результирующих сил
        glm::vec2 GetTotalForce() const { return m_totalForce; }
        float GetTotalTorque() const { return m_totalTorque; }
        
    private:
        BodyType m_bodyType;
        float m_mass;
        float m_inertia;
        glm::vec2 m_velocity;
        float m_angularVelocity;
        
        // Материальные свойства
        float m_friction;
        float m_restitution;
        float m_density;
        float m_gravityScale;
        
        // Ограничения
        bool m_fixedRotation;
        float m_linearDamping;
        float m_angularDamping;
        
        // Состояние
        bool m_awake;
        bool m_bullet;
        
        // Накопленные силы
        glm::vec2 m_totalForce;
        float m_totalTorque;
        
        // Вспомогательные методы
        void CalculateMass();
        void CalculateInertia();
    };
}
