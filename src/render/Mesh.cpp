#include "FastEngine/Render/Mesh.h"

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
#include <cmath>

namespace FastEngine {
    Mesh::Mesh() 
        : m_VAO(0)
        , m_VBO(0)
        , m_EBO(0)
        , m_vertexCount(0)
        , m_indexCount(0)
        , m_loaded(false) {
    }
    
    Mesh::~Mesh() {
        Destroy();
    }
    
    bool Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
        if (m_loaded) {
            Destroy();
        }
        
        m_vertices = vertices;
        m_indices = indices;
        m_vertexCount = vertices.size();
        m_indexCount = indices.size();
        
        SetupMesh();
        m_loaded = true;
        
        return true;
    }
    
    bool Mesh::LoadFromFile(const std::string& filePath) {
        // Простая реализация загрузки OBJ файла
        // В реальном проекте лучше использовать библиотеку типа Assimp
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Заглушка - создаем куб по умолчанию
        std::cout << "Mesh loading from file not implemented yet, creating default cube" << std::endl;
        *this = CreateCube();
        
        return m_loaded;
    }
    
    void Mesh::Destroy() {
        if (m_VAO != 0) {
#ifdef __APPLE__
#if TARGET_OS_IPHONE
            glDeleteVertexArrays(1, &m_VAO);
#else
            glDeleteVertexArraysAPPLE(1, &m_VAO);
#endif
#else
            glDeleteVertexArrays(1, &m_VAO);
#endif
            m_VAO = 0;
        }
        
        if (m_VBO != 0) {
            glDeleteBuffers(1, &m_VBO);
            m_VBO = 0;
        }
        
        if (m_EBO != 0) {
            glDeleteBuffers(1, &m_EBO);
            m_EBO = 0;
        }
        
        m_loaded = false;
        m_vertexCount = 0;
        m_indexCount = 0;
    }
    
    void Mesh::Draw() const {
        if (!m_loaded) {
            return;
        }
        
#ifdef __APPLE__
#if TARGET_OS_IPHONE
        glBindVertexArray(m_VAO);
#else
        glBindVertexArrayAPPLE(m_VAO);
#endif
#else
        glBindVertexArray(m_VAO);
#endif
        
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
        
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
    
    void Mesh::SetupMesh() {
        // Создание VAO
#ifdef __APPLE__
#if TARGET_OS_IPHONE
        glGenVertexArrays(1, &m_VAO);
#else
        glGenVertexArraysAPPLE(1, &m_VAO);
#endif
#else
        glGenVertexArrays(1, &m_VAO);
#endif
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
        
#ifdef __APPLE__
#if TARGET_OS_IPHONE
        glBindVertexArray(m_VAO);
#else
        glBindVertexArrayAPPLE(m_VAO);
#endif
#else
        glBindVertexArray(m_VAO);
#endif
        
        // Загрузка вершин
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
        
        // Загрузка индексов
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);
        
        // Настройка атрибутов вершин
        // Позиция
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Нормаль
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);
        
        // Текстурные координаты
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(2);
        
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
    
    Mesh Mesh::CreateCube(float size) {
        std::vector<Vertex> vertices = {
            // Передняя грань
            Vertex(glm::vec3(-size, -size,  size), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3( size, -size,  size), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3( size,  size,  size), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
            Vertex(glm::vec3(-size,  size,  size), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
            
            // Задняя грань
            Vertex(glm::vec3(-size, -size, -size), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3(-size,  size, -size), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)),
            Vertex(glm::vec3( size,  size, -size), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)),
            Vertex(glm::vec3( size, -size, -size), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)),
            
            // Левая грань
            Vertex(glm::vec3(-size, -size, -size), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3(-size, -size,  size), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3(-size,  size,  size), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
            Vertex(glm::vec3(-size,  size, -size), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
            
            // Правая грань
            Vertex(glm::vec3( size, -size, -size), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3( size,  size, -size), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
            Vertex(glm::vec3( size,  size,  size), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
            Vertex(glm::vec3( size, -size,  size), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
            
            // Верхняя грань
            Vertex(glm::vec3(-size,  size, -size), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
            Vertex(glm::vec3(-size,  size,  size), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3( size,  size,  size), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3( size,  size, -size), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
            
            // Нижняя грань
            Vertex(glm::vec3(-size, -size, -size), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
            Vertex(glm::vec3( size, -size, -size), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
            Vertex(glm::vec3( size, -size,  size), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3(-size, -size,  size), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f))
        };
        
        std::vector<unsigned int> indices = {
            // Передняя грань
            0, 1, 2, 2, 3, 0,
            // Задняя грань
            4, 5, 6, 6, 7, 4,
            // Левая грань
            8, 9, 10, 10, 11, 8,
            // Правая грань
            12, 13, 14, 14, 15, 12,
            // Верхняя грань
            16, 17, 18, 18, 19, 16,
            // Нижняя грань
            20, 21, 22, 22, 23, 20
        };
        
        Mesh mesh;
        mesh.Create(vertices, indices);
        return mesh;
    }
    
    Mesh Mesh::CreateSphere(float radius, int segments) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Создаем вершины сферы
        for (int i = 0; i <= segments; i++) {
            float lat = M_PI * i / segments;
            for (int j = 0; j <= segments; j++) {
                float lon = 2.0f * M_PI * j / segments;
                
                float x = radius * sin(lat) * cos(lon);
                float y = radius * cos(lat);
                float z = radius * sin(lat) * sin(lon);
                
                glm::vec3 position(x, y, z);
                glm::vec3 normal = glm::normalize(position);
                glm::vec2 texCoords(j / (float)segments, i / (float)segments);
                
                vertices.push_back(Vertex(position, normal, texCoords));
            }
        }
        
        // Создаем индексы
        for (int i = 0; i < segments; i++) {
            for (int j = 0; j < segments; j++) {
                int first = i * (segments + 1) + j;
                int second = first + segments + 1;
                
                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);
                
                indices.push_back(first + 1);
                indices.push_back(second);
                indices.push_back(second + 1);
            }
        }
        
        Mesh mesh;
        mesh.Create(vertices, indices);
        return mesh;
    }
    
    Mesh Mesh::CreatePlane(float width, float height) {
        std::vector<Vertex> vertices = {
            Vertex(glm::vec3(-width/2, 0.0f, -height/2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3( width/2, 0.0f, -height/2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3( width/2, 0.0f,  height/2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
            Vertex(glm::vec3(-width/2, 0.0f,  height/2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f))
        };
        
        std::vector<unsigned int> indices = {
            0, 1, 2, 2, 3, 0
        };
        
        Mesh mesh;
        mesh.Create(vertices, indices);
        return mesh;
    }
    
    Mesh Mesh::CreateCylinder(float radius, float height, int segments) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Создаем вершины цилиндра
        for (int i = 0; i <= segments; i++) {
            float angle = 2.0f * M_PI * i / segments;
            float x = radius * cos(angle);
            float z = radius * sin(angle);
            
            // Верхняя грань
            vertices.push_back(Vertex(glm::vec3(x, height/2, z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(i / (float)segments, 0.0f)));
            // Нижняя грань
            vertices.push_back(Vertex(glm::vec3(x, -height/2, z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(i / (float)segments, 1.0f)));
        }
        
        // Создаем индексы для боковых граней
        for (int i = 0; i < segments; i++) {
            int first = i * 2;
            int second = first + 1;
            int third = (i + 1) % (segments + 1) * 2;
            int fourth = third + 1;
            
            // Боковая грань
            indices.push_back(first);
            indices.push_back(third);
            indices.push_back(second);
            
            indices.push_back(second);
            indices.push_back(third);
            indices.push_back(fourth);
        }
        
        Mesh mesh;
        mesh.Create(vertices, indices);
        return mesh;
    }
}

