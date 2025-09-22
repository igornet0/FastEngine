#include "FastEngine/Render/Texture.h"
#include "FastEngine/Render/ImageLoader.h"
#include "FastEngine/Platform/Platform.h"
#include "FastEngine/Platform/FileSystem.h"

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#else
#include <OpenGL/gl.h>
#endif
#else
#include <GL/gl.h>
#endif

#include <iostream>

namespace FastEngine {
    Texture::Texture() 
        : m_textureID(0)
        , m_width(0)
        , m_height(0)
        , m_loaded(false) {
    }
    
    Texture::~Texture() {
        Destroy();
    }
    
    bool Texture::LoadFromFile(const std::string& filePath) {
        if (m_loaded) {
            Destroy();
        }
        
        // Загружаем изображение через ImageLoader
        ImageData image = ImageLoader::LoadFromFile(filePath);
        
        // Если не удалось загрузить, создаем fallback текстуру
        if (!image.data) {
            std::cout << "Failed to load texture: " << filePath << ", creating fallback" << std::endl;
            image = ImageLoader::CreateCheckerboard(64, 64, 8); // Шахматная доска для отсутствующих текстур
        }
        
        // Создаем OpenGL текстуру
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        
        // Загружаем данные текстуры
        GLenum format = (image.channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data);
        
        // Генерируем мипмапы
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // Устанавливаем параметры фильтрации
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        m_width = image.width;
        m_height = image.height;
        m_loaded = true;
        
        return true;
    }
    
    bool Texture::Create(int width, int height, const unsigned char* data) {
        if (m_loaded) {
            Destroy();
        }
        
        // Создаем OpenGL текстуру
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        
        if (data) {
            // Загружаем предоставленные данные
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        } else {
            // Создаем пустую текстуру
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        }
        
        // Устанавливаем параметры фильтрации
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        m_width = width;
        m_height = height;
        m_loaded = true;
        
        return true;
    }
    
    void Texture::Destroy() {
        if (m_textureID != 0) {
            glDeleteTextures(1, &m_textureID);
            m_textureID = 0;
        }
        m_loaded = false;
        m_width = 0;
        m_height = 0;
    }
    
    void Texture::SetFiltering(bool linear) {
        if (!m_loaded) return;
        
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        GLint filter = linear ? GL_LINEAR : GL_NEAREST;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    }
    
    void Texture::SetWrapping(bool repeat) {
        if (!m_loaded) return;
        
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        GLint wrap = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    }
    
    void Texture::Bind(int unit) const {
        if (!m_loaded) return;
        
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
    }
    
    void Texture::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
