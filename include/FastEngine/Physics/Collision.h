#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace FastEngine {
    enum class CollisionShape {
        AABB,    // Axis-Aligned Bounding Box
        Circle,  // Circle
        Polygon  // Convex polygon
    };
    
    struct AABB {
        glm::vec2 min;
        glm::vec2 max;
        
        AABB() : min(0.0f), max(0.0f) {}
        AABB(const glm::vec2& min, const glm::vec2& max) : min(min), max(max) {}
        
        glm::vec2 GetCenter() const { return (min + max) * 0.5f; }
        glm::vec2 GetSize() const { return max - min; }
        bool Contains(const glm::vec2& point) const;
    };
    
    struct Circle {
        glm::vec2 center;
        float radius;
        
        Circle() : center(0.0f), radius(0.0f) {}
        Circle(const glm::vec2& center, float radius) : center(center), radius(radius) {}
        
        bool Contains(const glm::vec2& point) const;
    };
    
    struct Polygon {
        std::vector<glm::vec2> vertices;
        
        Polygon() = default;
        Polygon(const std::vector<glm::vec2>& vertices) : vertices(vertices) {}
        
        bool Contains(const glm::vec2& point) const;
        glm::vec2 GetCenter() const;
        AABB GetAABB() const;
    };
    
    struct CollisionInfo {
        bool collided;
        glm::vec2 normal;
        float penetration;
        glm::vec2 contactPoint;
        
        CollisionInfo() : collided(false), normal(0.0f), penetration(0.0f), contactPoint(0.0f) {}
    };
    
    class CollisionSystem {
    public:
        CollisionSystem();
        ~CollisionSystem();
        
        // AABB коллизии
        static bool AABBvsAABB(const AABB& a, const AABB& b);
        static CollisionInfo AABBvsAABBInfo(const AABB& a, const AABB& b);
        
        // Circle коллизии
        static bool CirclevsCircle(const Circle& a, const Circle& b);
        static CollisionInfo CirclevsCircleInfo(const Circle& a, const Circle& b);
        
        // AABB vs Circle коллизии
        static bool AABBvsCircle(const AABB& aabb, const Circle& circle);
        static CollisionInfo AABBvsCircleInfo(const AABB& aabb, const Circle& circle);
        
        // SAT (Separating Axis Theorem) для полигонов
        static bool SATvsSAT(const Polygon& a, const Polygon& b);
        static CollisionInfo SATvsSATInfo(const Polygon& a, const Polygon& b);
        
        // Утилиты
        static AABB TransformAABB(const AABB& aabb, const glm::mat3& transform);
        static Circle TransformCircle(const Circle& circle, const glm::mat3& transform);
        static Polygon TransformPolygon(const Polygon& polygon, const glm::mat3& transform);
        
    private:
        // Вспомогательные функции для SAT
        static std::vector<glm::vec2> GetAxes(const Polygon& polygon);
        static glm::vec2 ProjectPolygon(const Polygon& polygon, const glm::vec2& axis);
        static bool Overlap(const glm::vec2& projection1, const glm::vec2& projection2);
    };
}

