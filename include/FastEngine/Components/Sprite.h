#pragma once

#include "Component.h"
#include <string>
#include <glm/glm.hpp>

namespace FastEngine {
    class Texture;
    
    class Sprite : public Component {
    public:
        Sprite(const std::string& texturePath);
        Sprite(Texture* texture);
        
        // Текстура
        Texture* GetTexture() const { return m_texture; }
        void SetTexture(Texture* texture) { m_texture = texture; }
        
        // Размер спрайта
        glm::vec2 GetSize() const { return m_size; }
        void SetSize(float width, float height) { m_size = glm::vec2(width, height); }
        void SetSize(const glm::vec2& size) { m_size = size; }
        
        // Цвет
        glm::vec4 GetColor() const { return m_color; }
        void SetColor(float r, float g, float b, float a = 1.0f) { m_color = glm::vec4(r, g, b, a); }
        void SetColor(const glm::vec4& color) { m_color = color; }
        
        // Видимость
        bool IsVisible() const { return m_visible; }
        void SetVisible(bool visible) { m_visible = visible; }
        
    private:
        Texture* m_texture;
        glm::vec2 m_size;
        glm::vec4 m_color;
        bool m_visible;
    };
}
