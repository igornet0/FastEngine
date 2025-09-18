#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace FastEngine {
    class Texture;
    class Sprite;
    class Camera;
    class Shader;
    
    class Renderer {
    public:
        Renderer();
        ~Renderer();
        
        // Инициализация и завершение работы
        bool Initialize(int width, int height);
        void Shutdown();
        
        // Управление камерой
        void SetCamera(Camera* camera) { m_camera = camera; }
        Camera* GetCamera() const { return m_camera; }
        
        // Очистка экрана
        void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
        void Present();
        
        // Отрисовка спрайтов
        void DrawSprite(Sprite* sprite, const glm::mat4& transform);
        void DrawTexture(Texture* texture, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f));
        
        // Управление состоянием
        void SetViewport(int x, int y, int width, int height);
        void SetBlendMode(bool enabled);
        
        // Получение информации
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        
    private:
        void SetupOpenGL();
        void CreateQuad();
        
        int m_width;
        int m_height;
        Camera* m_camera;
        
        // OpenGL объекты
        unsigned int m_quadVAO;
        unsigned int m_quadVBO;
        unsigned int m_quadEBO;
        
        // Шейдеры
        std::unique_ptr<Shader> m_spriteShader;
        
        bool m_initialized;
    };
}
