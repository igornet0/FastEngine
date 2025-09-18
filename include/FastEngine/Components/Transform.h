#pragma once

#include "Component.h"
#include <glm/glm.hpp>

namespace FastEngine {
    class Transform : public Component {
    public:
        Transform(float x = 0.0f, float y = 0.0f, float rotation = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f);
        
        // Позиция
        glm::vec2 GetPosition() const { return m_position; }
        void SetPosition(float x, float y) { m_position = glm::vec2(x, y); }
        void SetPosition(const glm::vec2& position) { m_position = position; }
        
        // Поворот
        float GetRotation() const { return m_rotation; }
        void SetRotation(float rotation) { m_rotation = rotation; }
        
        // Масштаб
        glm::vec2 GetScale() const { return m_scale; }
        void SetScale(float scaleX, float scaleY) { m_scale = glm::vec2(scaleX, scaleY); }
        void SetScale(const glm::vec2& scale) { m_scale = scale; }
        
        // Матрица преобразования
        glm::mat4 GetTransformMatrix() const;
        
    private:
        glm::vec2 m_position;
        float m_rotation;
        glm::vec2 m_scale;
    };
}
