#include "FastEngine/Components/Transform.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>

namespace FastEngine {
    Transform::Transform(float x, float y, float rotation, float scaleX, float scaleY)
        : m_position(x, y)
        , m_rotation(rotation)
        , m_scale(scaleX, scaleY) {
    }
    
    glm::mat4 Transform::GetTransformMatrix() const {
        // Создаем матрицу трансформации
        glm::mat4 transform = glm::mat4(1.0f);
        
        // Применяем масштаб
        transform = glm::scale(transform, glm::vec3(m_scale, 1.0f));
        
        // Применяем поворот
        if (m_rotation != 0.0f) {
            transform = glm::rotate(transform, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        
        // Применяем позицию
        transform = glm::translate(transform, glm::vec3(m_position, 0.0f));
        
        return transform;
    }
}
