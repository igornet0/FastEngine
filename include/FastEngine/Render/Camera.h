#pragma once

#include <glm/glm.hpp>

namespace FastEngine {
    enum class ProjectionType {
        Orthographic,
        Perspective
    };
    
    class Camera {
    public:
        Camera();
        ~Camera();
        
        // 2D позиция камеры (для обратной совместимости)
        glm::vec2 GetPosition() const { return glm::vec2(m_position3D.x, m_position3D.y); }
        void SetPosition(float x, float y) { m_position3D = glm::vec3(x, y, m_position3D.z); }
        void SetPosition(const glm::vec2& position) { m_position3D = glm::vec3(position, m_position3D.z); }
        
        // 3D позиция камеры
        glm::vec3 GetPosition3D() const { return m_position3D; }
        void SetPosition3D(float x, float y, float z) { m_position3D = glm::vec3(x, y, z); }
        void SetPosition3D(const glm::vec3& position) { m_position3D = position; }
        
        // Размер камеры (для 2D)
        glm::vec2 GetSize() const { return m_size; }
        void SetSize(float width, float height) { m_size = glm::vec2(width, height); }
        void SetSize(const glm::vec2& size) { m_size = size; }
        
        // Зум
        float GetZoom() const { return m_zoom; }
        void SetZoom(float zoom) { m_zoom = zoom; }
        
        // Поворот (для 2D)
        float GetRotation() const { return m_rotation; }
        void SetRotation(float rotation) { m_rotation = rotation; }
        
        // 3D поворот
        glm::vec3 GetRotation3D() const { return m_rotation3D; }
        void SetRotation3D(float pitch, float yaw, float roll) { m_rotation3D = glm::vec3(pitch, yaw, roll); }
        void SetRotation3D(const glm::vec3& rotation) { m_rotation3D = rotation; }
        
        // Направление взгляда (для 3D)
        glm::vec3 GetForward() const;
        glm::vec3 GetRight() const;
        glm::vec3 GetUp() const;
        
        // Точка фокуса (для 3D)
        glm::vec3 GetTarget() const { return m_target; }
        void SetTarget(const glm::vec3& target) { m_target = target; }
        void LookAt(const glm::vec3& target);
        
        // Тип проекции
        ProjectionType GetProjectionType() const { return m_projectionType; }
        void SetProjectionType(ProjectionType type) { m_projectionType = type; }
        
        // Параметры перспективной проекции
        float GetFOV() const { return m_fov; }
        void SetFOV(float fov) { m_fov = fov; }
        
        float GetNearPlane() const { return m_nearPlane; }
        void SetNearPlane(float nearPlane) { m_nearPlane = nearPlane; }
        
        float GetFarPlane() const { return m_farPlane; }
        void SetFarPlane(float farPlane) { m_farPlane = farPlane; }
        
        // Матрицы
        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;
        glm::mat4 GetViewProjectionMatrix() const;
        
        // Преобразование координат
        glm::vec2 ScreenToWorld(const glm::vec2& screenPos) const;
        glm::vec2 WorldToScreen(const glm::vec2& worldPos) const;
        glm::vec3 ScreenToWorld3D(const glm::vec2& screenPos, float depth = 0.0f) const;
        glm::vec2 WorldToScreen3D(const glm::vec3& worldPos) const;
        
        // Движение камеры
        void Move(const glm::vec3& offset);
        void Rotate(float pitch, float yaw);
        
    private:
        // 2D параметры
        glm::vec2 m_size;
        float m_zoom;
        float m_rotation;
        
        // 3D параметры
        glm::vec3 m_position3D;
        glm::vec3 m_rotation3D;
        glm::vec3 m_target;
        
        // Параметры проекции
        ProjectionType m_projectionType;
        float m_fov;
        float m_nearPlane;
        float m_farPlane;
    };
}
