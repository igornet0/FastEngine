#pragma once

#include "FastEngine/Component.h"
#include "FastEngine/Physics/Collision.h"
#include "FastEngine/Entity.h"
#include <glm/glm.hpp>
#include <functional>

namespace FastEngine {
    enum class ColliderType {
        Box,        // AABB коллайдер
        Circle,     // Круговой коллайдер
        Polygon     // Полигональный коллайдер
    };
    
    class Collider : public Component {
    public:
        Collider();
        ~Collider() override = default;
        
        // Тип коллайдера
        void SetType(ColliderType type) { m_type = type; }
        ColliderType GetType() const { return m_type; }
        
        // Размеры (для Box)
        void SetSize(const glm::vec2& size) { m_size = size; }
        glm::vec2 GetSize() const { return m_size; }
        
        // Радиус (для Circle)
        void SetRadius(float radius) { m_radius = radius; }
        float GetRadius() const { return m_radius; }
        
        // Вершины (для Polygon)
        void SetVertices(const std::vector<glm::vec2>& vertices) { m_vertices = vertices; }
        const std::vector<glm::vec2>& GetVertices() const { return m_vertices; }
        
        // Смещение от центра объекта
        void SetOffset(const glm::vec2& offset) { m_offset = offset; }
        glm::vec2 GetOffset() const { return m_offset; }
        
        // Свойства коллизии
        void SetIsTrigger(bool trigger) { m_isTrigger = trigger; }
        bool IsTrigger() const { return m_isTrigger; }
        
        void SetSensor(bool sensor) { m_sensor = sensor; }
        bool IsSensor() const { return m_sensor; }
        
        // Слои коллизий
        void SetCollisionLayer(int layer) { m_collisionLayer = layer; }
        int GetCollisionLayer() const { return m_collisionLayer; }
        
        void SetCollisionMask(int mask) { m_collisionMask = mask; }
        int GetCollisionMask() const { return m_collisionMask; }
        
        // Материал
        void SetFriction(float friction) { m_friction = friction; }
        float GetFriction() const { return m_friction; }
        
        void SetRestitution(float restitution) { m_restitution = restitution; }
        float GetRestitution() const { return m_restitution; }
        
        void SetDensity(float density) { m_density = density; }
        float GetDensity() const { return m_density; }
        
        // Получение геометрии коллайдера
        AABB GetAABB(const glm::vec2& position, float rotation = 0.0f) const;
        Circle GetCircle(const glm::vec2& position) const;
        Polygon GetPolygon(const glm::vec2& position, float rotation = 0.0f) const;
        
        // Проверка коллизий
        bool CheckCollision(const Collider& other, const glm::vec2& thisPos, const glm::vec2& otherPos) const;
        CollisionInfo GetCollisionInfo(const Collider& other, const glm::vec2& thisPos, const glm::vec2& otherPos) const;
        
        // События коллизий
        void SetOnCollisionEnter(std::function<void(Entity*, const CollisionInfo&)> callback);
        void SetOnCollisionExit(std::function<void(Entity*)> callback);
        void SetOnTriggerEnter(std::function<void(Entity*)> callback);
        void SetOnTriggerExit(std::function<void(Entity*)> callback);
        
        // Вызов событий (вызывается CollisionSystem)
        void OnCollisionEnter(Entity* other, const CollisionInfo& info);
        void OnCollisionExit(Entity* other);
        void OnTriggerEnter(Entity* other);
        void OnTriggerExit(Entity* other);
        
        // Обновление коллайдера
        void Update();
        
        // Валидация
        bool IsValid() const;
        
    private:
        ColliderType m_type;
        glm::vec2 m_size;           // Для Box
        float m_radius;             // Для Circle
        std::vector<glm::vec2> m_vertices; // Для Polygon
        glm::vec2 m_offset;
        
        // Свойства
        bool m_isTrigger;
        bool m_sensor;
        int m_collisionLayer;
        int m_collisionMask;
        
        // Материал
        float m_friction;
        float m_restitution;
        float m_density;
        
        // События
        std::function<void(Entity*, const CollisionInfo&)> m_onCollisionEnter;
        std::function<void(Entity*)> m_onCollisionExit;
        std::function<void(Entity*)> m_onTriggerEnter;
        std::function<void(Entity*)> m_onTriggerExit;
        
        // Вспомогательные методы
        glm::vec2 RotatePoint(const glm::vec2& point, const glm::vec2& center, float angle) const;
        void UpdateVertices();
    };
}
