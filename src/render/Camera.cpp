#include "FastEngine/Render/Camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace FastEngine {
    Camera::Camera() 
        : m_size(800.0f, 600.0f)
        , m_zoom(1.0f)
        , m_rotation(0.0f)
        , m_position3D(0.0f, 0.0f, 0.0f)
        , m_rotation3D(0.0f, 0.0f, 0.0f)
        , m_target(0.0f, 0.0f, -1.0f)
        , m_projectionType(ProjectionType::Orthographic)
        , m_fov(45.0f)
        , m_nearPlane(0.1f)
        , m_farPlane(1000.0f) {
    }
    
    Camera::~Camera() = default;
    
    glm::mat4 Camera::GetViewMatrix() const {
        if (m_projectionType == ProjectionType::Perspective) {
            // 3D камера
            glm::mat4 view = glm::mat4(1.0f);
            
            // Применяем поворот по осям
            view = glm::rotate(view, glm::radians(m_rotation3D.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
            view = glm::rotate(view, glm::radians(m_rotation3D.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
            view = glm::rotate(view, glm::radians(m_rotation3D.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll
            
            // Применяем позицию (смещение в обратную сторону)
            view = glm::translate(view, -m_position3D);
            
            return view;
        } else {
            // 2D камера (обратная совместимость)
            glm::mat4 view = glm::mat4(1.0f);
            
            // Применяем поворот
            if (m_rotation != 0.0f) {
                view = glm::rotate(view, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
            }
            
            // Применяем зум
            if (m_zoom != 1.0f) {
                view = glm::scale(view, glm::vec3(m_zoom, m_zoom, 1.0f));
            }
            
            // Применяем позицию (смещение в обратную сторону)
            view = glm::translate(view, glm::vec3(-m_position3D.x, -m_position3D.y, 0.0f));
            
            return view;
        }
    }
    
    glm::mat4 Camera::GetProjectionMatrix() const {
        if (m_projectionType == ProjectionType::Perspective) {
            // Перспективная проекция
            float aspect = m_size.x / m_size.y;
            return glm::perspective(glm::radians(m_fov), aspect, m_nearPlane, m_farPlane);
        } else {
            // Ортогональная проекция
            float left = -m_size.x * 0.5f;
            float right = m_size.x * 0.5f;
            float bottom = -m_size.y * 0.5f;
            float top = m_size.y * 0.5f;
            
            return glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        }
    }
    
    glm::mat4 Camera::GetViewProjectionMatrix() const {
        return GetProjectionMatrix() * GetViewMatrix();
    }
    
    glm::vec2 Camera::ScreenToWorld(const glm::vec2& screenPos) const {
        // Преобразуем координаты экрана в координаты мира
        glm::vec2 normalizedPos = (screenPos / m_size) * 2.0f - 1.0f;
        normalizedPos.y = -normalizedPos.y; // Инвертируем Y
        
        // Применяем обратную проекцию
        glm::mat4 invProjection = glm::inverse(GetProjectionMatrix());
        glm::vec4 worldPos = invProjection * glm::vec4(normalizedPos, 0.0f, 1.0f);
        
        // Применяем обратную матрицу вида
        glm::mat4 invView = glm::inverse(GetViewMatrix());
        worldPos = invView * worldPos;
        
        return glm::vec2(worldPos.x, worldPos.y);
    }
    
    glm::vec2 Camera::WorldToScreen(const glm::vec2& worldPos) const {
        // Преобразуем координаты мира в координаты экрана
        glm::vec4 screenPos = GetViewProjectionMatrix() * glm::vec4(worldPos, 0.0f, 1.0f);
        
        // Нормализуем координаты
        screenPos /= screenPos.w;
        
        // Преобразуем в координаты экрана
        glm::vec2 result = glm::vec2(screenPos.x, screenPos.y);
        result = (result + 1.0f) * 0.5f * m_size;
        result.y = m_size.y - result.y; // Инвертируем Y
        
        return result;
    }
    
    glm::vec3 Camera::ScreenToWorld3D(const glm::vec2& screenPos, float depth) const {
        // Преобразуем координаты экрана в координаты мира 3D
        glm::vec2 normalizedPos = (screenPos / m_size) * 2.0f - 1.0f;
        normalizedPos.y = -normalizedPos.y; // Инвертируем Y
        
        glm::vec4 clipPos = glm::vec4(normalizedPos, depth, 1.0f);
        
        // Применяем обратную проекцию
        glm::mat4 invProjection = glm::inverse(GetProjectionMatrix());
        glm::vec4 viewPos = invProjection * clipPos;
        
        // Применяем обратную матрицу вида
        glm::mat4 invView = glm::inverse(GetViewMatrix());
        glm::vec4 worldPos = invView * viewPos;
        
        return glm::vec3(worldPos.x, worldPos.y, worldPos.z);
    }
    
    glm::vec2 Camera::WorldToScreen3D(const glm::vec3& worldPos) const {
        // Преобразуем координаты мира 3D в координаты экрана
        glm::vec4 screenPos = GetViewProjectionMatrix() * glm::vec4(worldPos, 1.0f);
        
        // Нормализуем координаты
        screenPos /= screenPos.w;
        
        // Преобразуем в координаты экрана
        glm::vec2 result = glm::vec2(screenPos.x, screenPos.y);
        result = (result + 1.0f) * 0.5f * m_size;
        result.y = m_size.y - result.y; // Инвертируем Y
        
        return result;
    }
    
    glm::vec3 Camera::GetForward() const {
        glm::mat4 rotationMatrix = glm::eulerAngleYXZ(glm::radians(m_rotation3D.y), 
                                                     glm::radians(m_rotation3D.x), 
                                                     glm::radians(m_rotation3D.z));
        return glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    }
    
    glm::vec3 Camera::GetRight() const {
        glm::mat4 rotationMatrix = glm::eulerAngleYXZ(glm::radians(m_rotation3D.y), 
                                                     glm::radians(m_rotation3D.x), 
                                                     glm::radians(m_rotation3D.z));
        return glm::vec3(rotationMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
    }
    
    glm::vec3 Camera::GetUp() const {
        glm::mat4 rotationMatrix = glm::eulerAngleYXZ(glm::radians(m_rotation3D.y), 
                                                     glm::radians(m_rotation3D.x), 
                                                     glm::radians(m_rotation3D.z));
        return glm::vec3(rotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    }
    
    void Camera::LookAt(const glm::vec3& target) {
        m_target = target;
        glm::vec3 direction = glm::normalize(target - m_position3D);
        
        // Вычисляем углы поворота
        float yaw = glm::degrees(atan2(direction.x, direction.z));
        float pitch = glm::degrees(asin(-direction.y));
        
        m_rotation3D = glm::vec3(pitch, yaw, 0.0f);
    }
    
    void Camera::Move(const glm::vec3& offset) {
        m_position3D += offset;
    }
    
    void Camera::Rotate(float pitch, float yaw) {
        m_rotation3D.x += pitch;
        m_rotation3D.y += yaw;
        
        // Ограничиваем pitch
        m_rotation3D.x = glm::clamp(m_rotation3D.x, -89.0f, 89.0f);
    }
}
