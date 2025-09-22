#include "FastEngine/Components/RigidBody.h"
#include <algorithm>

namespace FastEngine {
    RigidBody::RigidBody() 
        : m_bodyType(BodyType::Dynamic)
        , m_mass(1.0f)
        , m_inertia(1.0f)
        , m_velocity(0.0f)
        , m_angularVelocity(0.0f)
        , m_friction(0.5f)
        , m_restitution(0.0f)
        , m_density(1.0f)
        , m_gravityScale(1.0f)
        , m_fixedRotation(false)
        , m_linearDamping(0.0f)
        , m_angularDamping(0.0f)
        , m_awake(true)
        , m_bullet(false)
        , m_totalForce(0.0f)
        , m_totalTorque(0.0f) {
    }
    
    void RigidBody::SetMass(float mass) {
        m_mass = std::max(0.0f, mass);
        if (m_mass > 0.0f) {
            m_inertia = 1.0f / m_mass;
        } else {
            m_inertia = 0.0f;
        }
    }
    
    void RigidBody::ApplyForce(const glm::vec2& force) {
        if (m_bodyType == BodyType::Dynamic) {
            m_totalForce += force;
        }
    }
    
    void RigidBody::ApplyForceAtPoint(const glm::vec2& force, const glm::vec2& point) {
        if (m_bodyType == BodyType::Dynamic) {
            m_totalForce += force;
            // Для простоты не учитываем момент силы от точки приложения
            // В полной реализации нужно было бы вычислить torque
        }
    }
    
    void RigidBody::ApplyTorque(float torque) {
        if (m_bodyType == BodyType::Dynamic && !m_fixedRotation) {
            m_totalTorque += torque;
        }
    }
    
    void RigidBody::ApplyImpulse(const glm::vec2& impulse) {
        if (m_bodyType == BodyType::Dynamic) {
            m_velocity += impulse * m_inertia;
        }
    }
    
    void RigidBody::ApplyAngularImpulse(float impulse) {
        if (m_bodyType == BodyType::Dynamic && !m_fixedRotation) {
            m_angularVelocity += impulse * m_inertia;
        }
    }
    
    void RigidBody::Update(float deltaTime) {
        if (m_bodyType != BodyType::Dynamic || !m_awake) {
            return;
        }
        
        // Применяем силы к скорости
        m_velocity += m_totalForce * m_inertia * deltaTime;
        if (!m_fixedRotation) {
            m_angularVelocity += m_totalTorque * m_inertia * deltaTime;
        }
        
        // Применяем демпфирование
        m_velocity *= (1.0f - m_linearDamping * deltaTime);
        if (!m_fixedRotation) {
            m_angularVelocity *= (1.0f - m_angularDamping * deltaTime);
        }
        
        // Очищаем силы после применения
        ClearForces();
    }
    
    void RigidBody::ClearForces() {
        m_totalForce = glm::vec2(0.0f);
        m_totalTorque = 0.0f;
    }
    
    void RigidBody::CalculateMass() {
        // В полной реализации здесь бы вычислялась масса на основе плотности и объема
        // Для простоты используем заданную массу
        if (m_mass > 0.0f) {
            m_inertia = 1.0f / m_mass;
        } else {
            m_inertia = 0.0f;
        }
    }
    
    void RigidBody::CalculateInertia() {
        // В полной реализации здесь бы вычислялся момент инерции на основе формы тела
        // Для простоты используем базовое значение
        if (m_mass > 0.0f) {
            m_inertia = m_mass * 0.1f; // Упрощенная формула для прямоугольника
        } else {
            m_inertia = 0.0f;
        }
    }
}

