#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Render/Texture.h"
#include "FastEngine/Platform/Platform.h"

namespace FastEngine {
    Sprite::Sprite(const std::string& texturePath) 
        : m_texture(nullptr)
        , m_size(64.0f, 64.0f)
        , m_color(1.0f, 1.0f, 1.0f, 1.0f)
        , m_visible(true) {
        // Загружаем текстуру
        m_texture = new Texture();
        if (!m_texture->LoadFromFile(texturePath)) {
            // Если не удалось загрузить, создаем цветную текстуру
            m_texture->Create(64, 64);
        }
    }
    
    Sprite::Sprite(Texture* texture) 
        : m_texture(texture)
        , m_size(64.0f, 64.0f)
        , m_color(1.0f, 1.0f, 1.0f, 1.0f)
        , m_visible(true) {
        // Если текстура не предоставлена, создаем цветную
        if (!m_texture) {
            m_texture = new Texture();
            m_texture->Create(64, 64);
        }
    }
    
    Sprite::~Sprite() = default;
}
