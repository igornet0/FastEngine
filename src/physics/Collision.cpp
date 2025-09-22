#include "FastEngine/Physics/Collision.h"
#include <algorithm>
#include <cmath>

namespace FastEngine {
    // AABB методы
    bool AABB::Contains(const glm::vec2& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y;
    }
    
    // Circle методы
    bool Circle::Contains(const glm::vec2& point) const {
        float distance = glm::length(point - center);
        return distance <= radius;
    }
    
    // Polygon методы
    bool Polygon::Contains(const glm::vec2& point) const {
        if (vertices.size() < 3) return false;
        
        bool inside = false;
        for (size_t i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
            if (((vertices[i].y > point.y) != (vertices[j].y > point.y)) &&
                (point.x < (vertices[j].x - vertices[i].x) * (point.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x)) {
                inside = !inside;
            }
        }
        return inside;
    }
    
    glm::vec2 Polygon::GetCenter() const {
        if (vertices.empty()) return glm::vec2(0.0f);
        
        glm::vec2 center(0.0f);
        for (const auto& vertex : vertices) {
            center += vertex;
        }
        return center / static_cast<float>(vertices.size());
    }
    
    AABB Polygon::GetAABB() const {
        if (vertices.empty()) return AABB();
        
        glm::vec2 min = vertices[0];
        glm::vec2 max = vertices[0];
        
        for (const auto& vertex : vertices) {
            min = glm::min(min, vertex);
            max = glm::max(max, vertex);
        }
        
        return AABB(min, max);
    }
    
    // CollisionSystem реализация
    CollisionSystem::CollisionSystem() = default;
    CollisionSystem::~CollisionSystem() = default;
    
    bool CollisionSystem::AABBvsAABB(const AABB& a, const AABB& b) {
        return a.min.x <= b.max.x && a.max.x >= b.min.x &&
               a.min.y <= b.max.y && a.max.y >= b.min.y;
    }
    
    CollisionInfo CollisionSystem::AABBvsAABBInfo(const AABB& a, const AABB& b) {
        CollisionInfo info;
        
        if (!AABBvsAABB(a, b)) {
            return info;
        }
        
        info.collided = true;
        
        // Вычисляем перекрытие по осям
        float overlapX = std::min(a.max.x - b.min.x, b.max.x - a.min.x);
        float overlapY = std::min(a.max.y - b.min.y, b.max.y - a.min.y);
        
        // Определяем нормаль и проникновение
        if (overlapX < overlapY) {
            info.penetration = overlapX;
            info.normal = glm::vec2(a.min.x < b.min.x ? -1.0f : 1.0f, 0.0f);
        } else {
            info.penetration = overlapY;
            info.normal = glm::vec2(0.0f, a.min.y < b.min.y ? -1.0f : 1.0f);
        }
        
        // Вычисляем точку контакта
        info.contactPoint = (a.GetCenter() + b.GetCenter()) * 0.5f;
        
        return info;
    }
    
    bool CollisionSystem::CirclevsCircle(const Circle& a, const Circle& b) {
        float distance = glm::length(a.center - b.center);
        return distance <= (a.radius + b.radius);
    }
    
    CollisionInfo CollisionSystem::CirclevsCircleInfo(const Circle& a, const Circle& b) {
        CollisionInfo info;
        
        glm::vec2 distance = a.center - b.center;
        float length = glm::length(distance);
        float totalRadius = a.radius + b.radius;
        
        if (length > totalRadius) {
            return info;
        }
        
        info.collided = true;
        info.penetration = totalRadius - length;
        info.normal = length > 0.0f ? glm::normalize(distance) : glm::vec2(1.0f, 0.0f);
        info.contactPoint = a.center - info.normal * a.radius;
        
        return info;
    }
    
    bool CollisionSystem::AABBvsCircle(const AABB& aabb, const Circle& circle) {
        // Находим ближайшую точку на AABB к центру круга
        glm::vec2 closestPoint;
        closestPoint.x = std::max(aabb.min.x, std::min(circle.center.x, aabb.max.x));
        closestPoint.y = std::max(aabb.min.y, std::min(circle.center.y, aabb.max.y));
        
        // Проверяем расстояние
        float distance = glm::length(circle.center - closestPoint);
        return distance <= circle.radius;
    }
    
    CollisionInfo CollisionSystem::AABBvsCircleInfo(const AABB& aabb, const Circle& circle) {
        CollisionInfo info;
        
        // Находим ближайшую точку на AABB к центру круга
        glm::vec2 closestPoint;
        closestPoint.x = std::max(aabb.min.x, std::min(circle.center.x, aabb.max.x));
        closestPoint.y = std::max(aabb.min.y, std::min(circle.center.y, aabb.max.y));
        
        float distance = glm::length(circle.center - closestPoint);
        
        if (distance > circle.radius) {
            return info;
        }
        
        info.collided = true;
        info.penetration = circle.radius - distance;
        info.normal = distance > 0.0f ? glm::normalize(circle.center - closestPoint) : glm::vec2(1.0f, 0.0f);
        info.contactPoint = closestPoint;
        
        return info;
    }
    
    bool CollisionSystem::SATvsSAT(const Polygon& a, const Polygon& b) {
        auto axes = GetAxes(a);
        auto axesB = GetAxes(b);
        axes.insert(axes.end(), axesB.begin(), axesB.end());
        
        for (const auto& axis : axes) {
            auto projA = ProjectPolygon(a, axis);
            auto projB = ProjectPolygon(b, axis);
            
            if (!Overlap(projA, projB)) {
                return false;
            }
        }
        
        return true;
    }
    
    CollisionInfo CollisionSystem::SATvsSATInfo(const Polygon& a, const Polygon& b) {
        CollisionInfo info;
        
        if (!SATvsSAT(a, b)) {
            return info;
        }
        
        info.collided = true;
        
        // Упрощенная реализация - в реальном проекте нужен более сложный алгоритм
        // для определения нормали и проникновения
        info.normal = glm::vec2(1.0f, 0.0f);
        info.penetration = 0.1f;
        info.contactPoint = (a.GetCenter() + b.GetCenter()) * 0.5f;
        
        return info;
    }
    
    AABB CollisionSystem::TransformAABB(const AABB& aabb, const glm::mat3& transform) {
        glm::vec2 corners[4] = {
            glm::vec2(aabb.min.x, aabb.min.y),
            glm::vec2(aabb.max.x, aabb.min.y),
            glm::vec2(aabb.max.x, aabb.max.y),
            glm::vec2(aabb.min.x, aabb.max.y)
        };
        
        glm::vec2 min = glm::vec2(transform * glm::vec3(corners[0], 1.0f));
        glm::vec2 max = min;
        
        for (int i = 1; i < 4; i++) {
            glm::vec2 transformed = glm::vec2(transform * glm::vec3(corners[i], 1.0f));
            min = glm::min(min, transformed);
            max = glm::max(max, transformed);
        }
        
        return AABB(min, max);
    }
    
    Circle CollisionSystem::TransformCircle(const Circle& circle, const glm::mat3& transform) {
        glm::vec2 newCenter = glm::vec2(transform * glm::vec3(circle.center, 1.0f));
        
        // Упрощенное масштабирование радиуса
        float scale = std::max(transform[0][0], transform[1][1]);
        
        return Circle(newCenter, circle.radius * scale);
    }
    
    Polygon CollisionSystem::TransformPolygon(const Polygon& polygon, const glm::mat3& transform) {
        Polygon result;
        result.vertices.reserve(polygon.vertices.size());
        
        for (const auto& vertex : polygon.vertices) {
            glm::vec2 transformed = glm::vec2(transform * glm::vec3(vertex, 1.0f));
            result.vertices.push_back(transformed);
        }
        
        return result;
    }
    
    std::vector<glm::vec2> CollisionSystem::GetAxes(const Polygon& polygon) {
        std::vector<glm::vec2> axes;
        
        for (size_t i = 0; i < polygon.vertices.size(); i++) {
            size_t j = (i + 1) % polygon.vertices.size();
            glm::vec2 edge = polygon.vertices[j] - polygon.vertices[i];
            glm::vec2 normal = glm::vec2(-edge.y, edge.x);
            axes.push_back(glm::normalize(normal));
        }
        
        return axes;
    }
    
    glm::vec2 CollisionSystem::ProjectPolygon(const Polygon& polygon, const glm::vec2& axis) {
        if (polygon.vertices.empty()) return glm::vec2(0.0f);
        
        float min = glm::dot(polygon.vertices[0], axis);
        float max = min;
        
        for (const auto& vertex : polygon.vertices) {
            float projection = glm::dot(vertex, axis);
            min = std::min(min, projection);
            max = std::max(max, projection);
        }
        
        return glm::vec2(min, max);
    }
    
    bool CollisionSystem::Overlap(const glm::vec2& projection1, const glm::vec2& projection2) {
        return projection1.x <= projection2.y && projection2.x <= projection1.y;
    }
}

