#include "FastEngine/Debug/Wireframe.h"
#include "FastEngine/Render/Renderer.h"
#include "FastEngine/Entity.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Components/Collider.h"
#include <cmath>
#include <iostream>

namespace FastEngine {
    Wireframe& Wireframe::GetInstance() {
        static Wireframe instance;
        return instance;
    }
    
    bool Wireframe::Initialize() {
        if (m_initialized) {
            return true;
        }
        
        m_enabled = false;
        m_initialized = true;
        m_lineWidth = 1.0f;
        m_depthTest = true;
        m_culling = false;
        
        return true;
    }
    
    void Wireframe::Shutdown() {
        m_lines.clear();
        m_initialized = false;
    }
    
    void Wireframe::Render(Renderer* renderer) {
        if (!m_enabled || !m_initialized || !renderer) {
            return;
        }
        
        // В реальной реализации здесь бы рендерились линии
        // Для простоты выводим информацию в консоль
        if (!m_lines.empty()) {
            std::cout << "Wireframe: Rendering " << m_lines.size() << " lines" << std::endl;
        }
    }
    
    void Wireframe::Update(float deltaTime) {
        if (!m_enabled || !m_initialized) {
            return;
        }
        
        // Очищаем линии каждый кадр (в реальной реализации они бы сохранялись)
        // m_lines.clear();
    }
    
    void Wireframe::AddLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color) {
        if (!m_initialized) {
            return;
        }
        
        m_lines.emplace_back(start, end, color);
    }
    
    void Wireframe::AddBox(const glm::vec3& center, const glm::vec3& size, const glm::vec4& color) {
        AddBoxLines(center, size, color);
    }
    
    void Wireframe::AddCircle(const glm::vec3& center, float radius, const glm::vec4& color) {
        AddCircleLines(center, radius, color);
    }
    
    void Wireframe::AddSphere(const glm::vec3& center, float radius, const glm::vec4& color) {
        AddSphereLines(center, radius, color);
    }
    
    void Wireframe::AddCylinder(const glm::vec3& center, float radius, float height, const glm::vec4& color) {
        AddCylinderLines(center, radius, height, color);
    }
    
    void Wireframe::Clear() {
        m_lines.clear();
    }
    
    void Wireframe::ClearLines() {
        m_lines.clear();
    }
    
    void Wireframe::AddEntityWireframe(Entity* entity, const glm::vec4& color) {
        if (!entity) {
            return;
        }
        
        Transform* transform = entity->GetComponent<Transform>();
        if (!transform) {
            return;
        }
        
        glm::vec2 pos2d = transform->GetPosition();
        glm::vec2 scale2d = transform->GetScale();
        glm::vec3 position = glm::vec3(pos2d.x, pos2d.y, 0.0f);
        glm::vec3 scale = glm::vec3(scale2d.x, scale2d.y, 1.0f);
        
        // Добавляем wireframe для коллайдера, если есть
        Collider* collider = entity->GetComponent<Collider>();
        if (collider) {
            switch (collider->GetType()) {
                case ColliderType::Box:
                    {
                        glm::vec2 size2d = collider->GetSize();
                        glm::vec3 size3d = glm::vec3(size2d.x, size2d.y, 1.0f);
                        AddBox(position, size3d * scale, color);
                    }
                    break;
                case ColliderType::Circle:
                    AddCircle(position, collider->GetRadius() * scale.x, color);
                    break;
                case ColliderType::Polygon:
                    // Для полигона добавляем простой box
                    AddBox(position, glm::vec3(1.0f) * scale, color);
                    break;
            }
        } else {
            // Если нет коллайдера, добавляем простой box
            AddBox(position, scale, color);
        }
    }
    
    void Wireframe::AddAllEntitiesWireframes() {
        // В реальной реализации здесь бы получались все сущности из World
        // Для простоты пропускаем
    }
    
    void Wireframe::AddBoxLines(const glm::vec3& center, const glm::vec3& size, const glm::vec4& color) {
        glm::vec3 halfSize = size * 0.5f;
        
        // 8 вершин куба
        glm::vec3 vertices[8] = {
            center + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z), // 0
            center + glm::vec3( halfSize.x, -halfSize.y, -halfSize.z), // 1
            center + glm::vec3( halfSize.x,  halfSize.y, -halfSize.z), // 2
            center + glm::vec3(-halfSize.x,  halfSize.y, -halfSize.z), // 3
            center + glm::vec3(-halfSize.x, -halfSize.y,  halfSize.z), // 4
            center + glm::vec3( halfSize.x, -halfSize.y,  halfSize.z), // 5
            center + glm::vec3( halfSize.x,  halfSize.y,  halfSize.z), // 6
            center + glm::vec3(-halfSize.x,  halfSize.y,  halfSize.z)  // 7
        };
        
        // 12 рёбер куба
        int edges[12][2] = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Нижняя грань
            {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Верхняя грань
            {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Вертикальные рёбра
        };
        
        for (int i = 0; i < 12; i++) {
            AddLine(vertices[edges[i][0]], vertices[edges[i][1]], color);
        }
    }
    
    void Wireframe::AddCircleLines(const glm::vec3& center, float radius, const glm::vec4& color) {
        const int segments = 32;
        const float angleStep = 2.0f * M_PI / segments;
        
        for (int i = 0; i < segments; i++) {
            float angle1 = i * angleStep;
            float angle2 = (i + 1) * angleStep;
            
            glm::vec3 p1 = center + glm::vec3(
                radius * std::cos(angle1),
                radius * std::sin(angle1),
                0.0f
            );
            
            glm::vec3 p2 = center + glm::vec3(
                radius * std::cos(angle2),
                radius * std::sin(angle2),
                0.0f
            );
            
            AddLine(p1, p2, color);
        }
    }
    
    void Wireframe::AddSphereLines(const glm::vec3& center, float radius, const glm::vec4& color) {
        const int rings = 8;
        const int segments = 16;
        
        // Горизонтальные кольца
        for (int ring = 0; ring < rings; ring++) {
            float phi = ring * M_PI / (rings - 1);
            float y = radius * std::cos(phi);
            float ringRadius = radius * std::sin(phi);
            
            for (int segment = 0; segment < segments; segment++) {
                float theta1 = segment * 2.0f * M_PI / segments;
                float theta2 = (segment + 1) * 2.0f * M_PI / segments;
                
                glm::vec3 p1 = center + glm::vec3(
                    ringRadius * std::cos(theta1),
                    y,
                    ringRadius * std::sin(theta1)
                );
                
                glm::vec3 p2 = center + glm::vec3(
                    ringRadius * std::cos(theta2),
                    y,
                    ringRadius * std::sin(theta2)
                );
                
                AddLine(p1, p2, color);
            }
        }
        
        // Вертикальные линии
        for (int segment = 0; segment < segments; segment++) {
            float theta = segment * 2.0f * M_PI / segments;
            
            for (int ring = 0; ring < rings - 1; ring++) {
                float phi1 = ring * M_PI / (rings - 1);
                float phi2 = (ring + 1) * M_PI / (rings - 1);
                
                glm::vec3 p1 = center + glm::vec3(
                    radius * std::sin(phi1) * std::cos(theta),
                    radius * std::cos(phi1),
                    radius * std::sin(phi1) * std::sin(theta)
                );
                
                glm::vec3 p2 = center + glm::vec3(
                    radius * std::sin(phi2) * std::cos(theta),
                    radius * std::cos(phi2),
                    radius * std::sin(phi2) * std::sin(theta)
                );
                
                AddLine(p1, p2, color);
            }
        }
    }
    
    void Wireframe::AddCylinderLines(const glm::vec3& center, float radius, float height, const glm::vec4& color) {
        const int segments = 16;
        const float angleStep = 2.0f * M_PI / segments;
        
        // Нижнее кольцо
        for (int i = 0; i < segments; i++) {
            float angle1 = i * angleStep;
            float angle2 = (i + 1) * angleStep;
            
            glm::vec3 p1 = center + glm::vec3(
                radius * std::cos(angle1),
                -height * 0.5f,
                radius * std::sin(angle1)
            );
            
            glm::vec3 p2 = center + glm::vec3(
                radius * std::cos(angle2),
                -height * 0.5f,
                radius * std::sin(angle2)
            );
            
            AddLine(p1, p2, color);
        }
        
        // Верхнее кольцо
        for (int i = 0; i < segments; i++) {
            float angle1 = i * angleStep;
            float angle2 = (i + 1) * angleStep;
            
            glm::vec3 p1 = center + glm::vec3(
                radius * std::cos(angle1),
                height * 0.5f,
                radius * std::sin(angle1)
            );
            
            glm::vec3 p2 = center + glm::vec3(
                radius * std::cos(angle2),
                height * 0.5f,
                radius * std::sin(angle2)
            );
            
            AddLine(p1, p2, color);
        }
        
        // Вертикальные линии
        for (int i = 0; i < segments; i++) {
            float angle = i * angleStep;
            
            glm::vec3 bottom = center + glm::vec3(
                radius * std::cos(angle),
                -height * 0.5f,
                radius * std::sin(angle)
            );
            
            glm::vec3 top = center + glm::vec3(
                radius * std::cos(angle),
                height * 0.5f,
                radius * std::sin(angle)
            );
            
            AddLine(bottom, top, color);
        }
    }
}
