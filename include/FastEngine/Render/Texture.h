#pragma once

#include <string>
#include <glm/glm.hpp>

namespace FastEngine {
    class Texture {
    public:
        Texture();
        ~Texture();
        
        // Загрузка и создание текстуры
        bool LoadFromFile(const std::string& filePath);
        bool Create(int width, int height, const unsigned char* data = nullptr);
        
        // Уничтожение текстуры
        void Destroy();
        
        // Получение информации
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        unsigned int GetID() const { return m_textureID; }
        
        // Установка параметров
        void SetFiltering(bool linear);
        void SetWrapping(bool repeat);
        
        // Активация текстуры
        void Bind(int unit = 0) const;
        void Unbind() const;
        
    private:
        unsigned int m_textureID;
        int m_width;
        int m_height;
        bool m_loaded;
    };
}
