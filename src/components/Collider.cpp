#include "FastEngine/Components/Collider.h"
#include "FastEngine/Entity.h"
#include <cmath>

namespace FastEngine {
    Collider::Collider() 
        : m_type(ColliderType::Box)
        , m_size(1.0f)
        , m_radius(0.5f)
        , m_offset(0.0f)
        , m_isTrigger(false)
        , m_sensor(false)
        , m_collisionLayer(0)
        , m_collisionMask(0xFFFFFFFF)
        , m_friction(0.5f)
        , m_restitution(0.0f)
        , m_density(1.0f) {
    }
    
    AABB Collider::GetAABB(const glm::vec2& position, float rotation) const {
        glm::vec2 center = position + m_offset;
        glm::vec2 halfSize = m_size * 0.5f;
        
        if (rotation != 0.0f) {
            // Для повернутых коллайдеров создаем AABB, охватывающий всю область
            float cos_r = std::cos(rotation);
            float sin_r = std::sin(rotation);
            
            glm::vec2 corners[4] = {
                glm::vec2(-halfSize.x, -halfSize.y),
                glm::vec2(halfSize.x, -halfSize.y),
                glm::vec2(halfSize.x, halfSize.y),
                glm::vec2(-halfSize.x, halfSize.y)
            };
            
            glm::vec2 min = center + RotatePoint(corners[0], glm::vec2(0.0f), rotation);
            glm::vec2 max = min;
            
            for (int i = 1; i < 4; i++) {
                glm::vec2 rotated = center + RotatePoint(corners[i], glm::vec2(0.0f), rotation);
                min = glm::min(min, rotated);
                max = glm::max(max, rotated);
            }
            
            return AABB(min, max);
        } else {
            return AABB(center - halfSize, center + halfSize);
        }
    }
    
    Circle Collider::GetCircle(const glm::vec2& position) const {
        return Circle(position + m_offset, m_radius);
    }
    
    Polygon Collider::GetPolygon(const glm::vec2& position, float rotation) const {
        if (m_vertices.empty()) {
            return Polygon();
        }
        
        std::vector<glm::vec2> transformedVertices;
        transformedVertices.reserve(m_vertices.size());
        
        glm::vec2 center = position + m_offset;
        
        for (const auto& vertex : m_vertices) {
            glm::vec2 transformed = center + RotatePoint(vertex, glm::vec2(0.0f), rotation);
            transformedVertices.push_back(transformed);
        }
        
        return Polygon(transformedVertices);
    }
    
    bool Collider::CheckCollision(const Collider& other, const glm::vec2& thisPos, const glm::vec2& otherPos) const {
        // Проверяем слои коллизий
        if (!(m_collisionMask & (1 << other.m_collisionLayer))) {
            return false;
        }
        
        // Получаем геометрию коллайдеров
        if (m_type == ColliderType::Box && other.m_type == ColliderType::Box) {
            AABB thisAABB = GetAABB(thisPos);
            AABB otherAABB = other.GetAABB(otherPos);
            return CollisionSystem::AABBvsAABB(thisAABB, otherAABB);
        }
        else if (m_type == ColliderType::Circle && other.m_type == ColliderType::Circle) {
            Circle thisCircle = GetCircle(thisPos);
            Circle otherCircle = other.GetCircle(otherPos);
            return CollisionSystem::CirclevsCircle(thisCircle, otherCircle);
        }
        else if (m_type == ColliderType::Box && other.m_type == ColliderType::Circle) {
            AABB thisAABB = GetAABB(thisPos);
            Circle otherCircle = other.GetCircle(otherPos);
            return CollisionSystem::AABBvsCircle(thisAABB, otherCircle);
        }
        else if (m_type == ColliderType::Circle && other.m_type == ColliderType::Box) {
            Circle thisCircle = GetCircle(thisPos);
            AABB otherAABB = other.GetAABB(otherPos);
            return CollisionSystem::AABBvsCircle(otherAABB, thisCircle);
        }
        else if (m_type == ColliderType::Polygon && other.m_type == ColliderType::Polygon) {
            Polygon thisPolygon = GetPolygon(thisPos);
            Polygon otherPolygon = other.GetPolygon(otherPos);
            return CollisionSystem::SATvsSAT(thisPolygon, otherPolygon);
        }
        
        return false;
    }
    
    CollisionInfo Collider::GetCollisionInfo(const Collider& other, const glm::vec2& thisPos, const glm::vec2& otherPos) const {
        // Проверяем слои коллизий
        if (!(m_collisionMask & (1 << other.m_collisionLayer))) {
            return CollisionInfo();
        }
        
        // Получаем информацию о коллизии
        if (m_type == ColliderType::Box && other.m_type == ColliderType::Box) {
            AABB thisAABB = GetAABB(thisPos);
            AABB otherAABB = other.GetAABB(otherPos);
            return CollisionSystem::AABBvsAABBInfo(thisAABB, otherAABB);
        }
        else if (m_type == ColliderType::Circle && other.m_type == ColliderType::Circle) {
            Circle thisCircle = GetCircle(thisPos);
            Circle otherCircle = other.GetCircle(otherPos);
            return CollisionSystem::CirclevsCircleInfo(thisCircle, otherCircle);
        }
        else if (m_type == ColliderType::Box && other.m_type == ColliderType::Circle) {
            AABB thisAABB = GetAABB(thisPos);
            Circle otherCircle = other.GetCircle(otherPos);
            return CollisionSystem::AABBvsCircleInfo(thisAABB, otherCircle);
        }
        else if (m_type == ColliderType::Circle && other.m_type == ColliderType::Box) {
            Circle thisCircle = GetCircle(thisPos);
            AABB otherAABB = other.GetAABB(otherPos);
            return CollisionSystem::AABBvsCircleInfo(otherAABB, thisCircle);
        }
        else if (m_type == ColliderType::Polygon && other.m_type == ColliderType::Polygon) {
            Polygon thisPolygon = GetPolygon(thisPos);
            Polygon otherPolygon = other.GetPolygon(otherPos);
            return CollisionSystem::SATvsSATInfo(thisPolygon, otherPolygon);
        }
        
        return CollisionInfo();
    }
    
    void Collider::SetOnCollisionEnter(std::function<void(Entity*, const CollisionInfo&)> callback) {
        m_onCollisionEnter = callback;
    }
    
    void Collider::SetOnCollisionExit(std::function<void(Entity*)> callback) {
        m_onCollisionExit = callback;
    }
    
    void Collider::SetOnTriggerEnter(std::function<void(Entity*)> callback) {
        m_onTriggerEnter = callback;
    }
    
    void Collider::SetOnTriggerExit(std::function<void(Entity*)> callback) {
        m_onTriggerExit = callback;
    }
    
    void Collider::OnCollisionEnter(Entity* other, const CollisionInfo& info) {
        if (m_onCollisionEnter) {
            m_onCollisionEnter(other, info);
        }
    }
    
    void Collider::OnCollisionExit(Entity* other) {
        if (m_onCollisionExit) {
            m_onCollisionExit(other);
        }
    }
    
    void Collider::OnTriggerEnter(Entity* other) {
        if (m_onTriggerEnter) {
            m_onTriggerEnter(other);
        }
    }
    
    void Collider::OnTriggerExit(Entity* other) {
        if (m_onTriggerExit) {
            m_onTriggerExit(other);
        }
    }
    
    void Collider::Update() {
        UpdateVertices();
    }
    
    bool Collider::IsValid() const {
        switch (m_type) {
            case ColliderType::Box:
                return m_size.x > 0.0f && m_size.y > 0.0f;
            case ColliderType::Circle:
                return m_radius > 0.0f;
            case ColliderType::Polygon:
                return m_vertices.size() >= 3;
            default:
                return false;
        }
    }
    
    glm::vec2 Collider::RotatePoint(const glm::vec2& point, const glm::vec2& center, float angle) const {
        float cos_a = std::cos(angle);
        float sin_a = std::sin(angle);
        
        glm::vec2 translated = point - center;
        return glm::vec2(
            translated.x * cos_a - translated.y * sin_a,
            translated.x * sin_a + translated.y * cos_a
        ) + center;
    }
    
    void Collider::UpdateVertices() {
        if (m_type == ColliderType::Polygon && m_vertices.empty()) {
            // Создаем прямоугольник по умолчанию
            glm::vec2 halfSize = m_size * 0.5f;
            m_vertices = {
                glm::vec2(-halfSize.x, -halfSize.y),
                glm::vec2(halfSize.x, -halfSize.y),
                glm::vec2(halfSize.x, halfSize.y),
                glm::vec2(-halfSize.x, halfSize.y)
            };
        }
    }
}

