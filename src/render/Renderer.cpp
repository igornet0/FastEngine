#include "FastEngine/Render/Renderer.h"
#include "FastEngine/Render/Shader.h"
#include "FastEngine/Render/Texture.h"
#include "FastEngine/Render/Camera.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Platform/Platform.h"
#include "FastEngine/Platform/FileSystem.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif
#else
#include <GL/gl.h>
#endif

#include <iostream>

namespace FastEngine {
    Renderer::Renderer() 
        : m_width(0)
        , m_height(0)
        , m_camera(nullptr)
        , m_quadVAO(0)
        , m_quadVBO(0)
        , m_quadEBO(0)
        , m_initialized(false) {
    }
    
    Renderer::~Renderer() {
        Shutdown();
    }
    
    bool Renderer::Initialize(int width, int height) {
        if (m_initialized) {
            return true;
        }
        
        m_width = width;
        m_height = height;
        
        // Проверяем, что OpenGL контекст создан
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL context not available: " << error << std::endl;
            return false;
        }
        
        // Настройка OpenGL
        SetupOpenGL();
        
        // Создание геометрии для отрисовки спрайтов
        CreateQuad();
        
        // Загрузка шейдеров
        m_spriteShader = std::make_unique<Shader>();
        
        // Загружаем шейдеры из файлов
        std::string vertexPath = Platform::GetInstance().GetFileSystem()->GetResourcePath("shaders/sprite.vert");
        std::string fragmentPath = Platform::GetInstance().GetFileSystem()->GetResourcePath("shaders/sprite.frag");
        
        if (!m_spriteShader->LoadFromFiles(vertexPath, fragmentPath)) {
            // Если не удалось загрузить из файлов, используем встроенные шейдеры
            std::string vertexSource = R"(
#version 120

attribute vec3 aPos;
attribute vec2 aTexCoord;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

varying vec2 TexCoord;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
            )";
            
            std::string fragmentSource = R"(
#version 120

varying vec2 TexCoord;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform bool uUseTexture;

void main() {
    if (uUseTexture) {
        gl_FragColor = texture2D(uTexture, TexCoord) * uColor;
    } else {
        gl_FragColor = uColor;
    }
}
            )";
            
            if (!m_spriteShader->LoadFromSource(vertexSource, fragmentSource)) {
                std::cerr << "Failed to load sprite shader" << std::endl;
                return false;
            }
        }
        
        m_initialized = true;
        return true;
    }
    
    void Renderer::Shutdown() {
        if (!m_initialized) {
            return;
        }
        
        // Удаление OpenGL объектов
        if (m_quadEBO != 0) {
            glDeleteBuffers(1, &m_quadEBO);
            m_quadEBO = 0;
        }
        
        if (m_quadVBO != 0) {
            glDeleteBuffers(1, &m_quadVBO);
            m_quadVBO = 0;
        }
        
        if (m_quadVAO != 0) {
#ifdef __APPLE__
#if TARGET_OS_IPHONE
            glDeleteVertexArrays(1, &m_quadVAO);
#else
            glDeleteVertexArraysAPPLE(1, &m_quadVAO);
#endif
#else
            glDeleteVertexArrays(1, &m_quadVAO);
#endif
            m_quadVAO = 0;
        }
        
        m_spriteShader.reset();
        m_initialized = false;
    }
    
    void Renderer::Clear(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    void Renderer::Present() {
        // На мобильных платформах буферизация обрабатывается автоматически
        // На Desktop нужно вызвать SwapBuffers
#ifdef __APPLE__
#if !TARGET_OS_IPHONE
        // Для macOS используем SDL
        // SDL_GL_SwapWindow будет вызван в Platform
#endif
#endif
    }
    
    void Renderer::DrawSprite(Sprite* sprite, const glm::mat4& transform) {
        if (!sprite || !sprite->IsVisible()) {
            return;
        }
        
        m_spriteShader->Use();
        
        // Установка матриц
        if (m_camera) {
            m_spriteShader->SetMat4("uProjection", m_camera->GetProjectionMatrix());
            m_spriteShader->SetMat4("uView", m_camera->GetViewMatrix());
        } else {
            // Матрица проекции по умолчанию (исправлена для правильной ориентации)
            glm::mat4 projection = glm::ortho(0.0f, (float)m_width, 0.0f, (float)m_height, -1.0f, 1.0f);
            m_spriteShader->SetMat4("uProjection", projection);
            m_spriteShader->SetMat4("uView", glm::mat4(1.0f));
        }
        
        m_spriteShader->SetMat4("uModel", transform);
        
        // Установка цвета
        glm::vec4 color = sprite->GetColor();
        m_spriteShader->SetVec4("uColor", color);
        
        // Установка текстуры
        if (sprite->GetTexture()) {
            sprite->GetTexture()->Bind(0);
            m_spriteShader->SetBool("uUseTexture", true);
        } else {
            m_spriteShader->SetBool("uUseTexture", false);
        }
        
        // Отрисовка
#ifdef __APPLE__
#if TARGET_OS_IPHONE
        glBindVertexArray(m_quadVAO);
#else
        glBindVertexArrayAPPLE(m_quadVAO);
#endif
#else
        glBindVertexArray(m_quadVAO);
#endif
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
#ifdef __APPLE__
#if TARGET_OS_IPHONE
        glBindVertexArray(0);
#else
        glBindVertexArrayAPPLE(0);
#endif
#else
        glBindVertexArray(0);
#endif
    }
    
    void Renderer::DrawTexture(Texture* texture, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
        if (!texture) {
            return;
        }
        
        // Создаем матрицу трансформации
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(position, 0.0f));
        transform = glm::scale(transform, glm::vec3(size, 1.0f));
        
        // Создаем временный спрайт
        Sprite tempSprite(texture);
        tempSprite.SetSize(size);
        tempSprite.SetColor(color);
        
        DrawSprite(&tempSprite, transform);
    }
    
    void Renderer::SetViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
        m_width = width;
        m_height = height;
    }
    
    void Renderer::SetBlendMode(bool enabled) {
        if (enabled) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        } else {
            glDisable(GL_BLEND);
        }
    }
    
    void Renderer::SetupOpenGL() {
        // Включение смешивания для прозрачности
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Отключение теста глубины для 2D
        glDisable(GL_DEPTH_TEST);
        
        // Включение сглаживания (если поддерживается)
        GLint samples = 0;
        glGetIntegerv(GL_SAMPLES, &samples);
        if (samples > 0) {
            glEnable(GL_MULTISAMPLE);
        }
        
        // Настройка для OpenGL ES
#ifdef GL_ES
        // Включение теста глубины для 3D (если понадобится)
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        
        // Настройка viewport
        glViewport(0, 0, m_width, m_height);
#endif
        
        // Проверка ошибок OpenGL
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL setup error: " << error << std::endl;
        }
    }
    
    void Renderer::CreateQuad() {
        // Вершины для отрисовки спрайта (квадрат)
        float vertices[] = {
            // Позиция        // Текстурные координаты
             0.5f,  0.5f, 0.0f,  1.0f, 1.0f,  // Верхний правый
             0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  // Нижний правый
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  // Нижний левый
            -0.5f,  0.5f, 0.0f,  0.0f, 1.0f   // Верхний левый
        };
        
        // Индексы для отрисовки двух треугольников
        unsigned int indices[] = {
            0, 1, 3,  // Первый треугольник
            1, 2, 3   // Второй треугольник
        };
        
        // Создание VAO
#ifdef __APPLE__
#if TARGET_OS_IPHONE
        glGenVertexArrays(1, &m_quadVAO);
#else
        glGenVertexArraysAPPLE(1, &m_quadVAO);
#endif
#else
        glGenVertexArrays(1, &m_quadVAO);
#endif
        glGenBuffers(1, &m_quadVBO);
        glGenBuffers(1, &m_quadEBO);
        
#ifdef __APPLE__
#if TARGET_OS_IPHONE
        glBindVertexArray(m_quadVAO);
#else
        glBindVertexArrayAPPLE(m_quadVAO);
#endif
#else
        glBindVertexArray(m_quadVAO);
#endif
        
        // Загрузка вершин
        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        // Загрузка индексов
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        // Настройка атрибутов вершин для OpenGL 2.1
        // Позиция
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Текстурные координаты
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
#ifdef __APPLE__
#if TARGET_OS_IPHONE
        glBindVertexArray(0);
#else
        glBindVertexArrayAPPLE(0);
#endif
#else
        glBindVertexArray(0);
#endif
    }
}
