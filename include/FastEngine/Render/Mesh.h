#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace FastEngine {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
        
        Vertex() = default;
        Vertex(const glm::vec3& pos, const glm::vec3& norm = glm::vec3(0.0f), const glm::vec2& tex = glm::vec2(0.0f))
            : position(pos), normal(norm), texCoords(tex) {}
    };
    
    class Mesh {
    public:
        Mesh();
        ~Mesh();
        
        // Создание меша
        bool Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        bool LoadFromFile(const std::string& filePath);
        
        // Уничтожение меша
        void Destroy();
        
        // Отрисовка
        void Draw() const;
        
        // Получение информации
        size_t GetVertexCount() const { return m_vertexCount; }
        size_t GetIndexCount() const { return m_indexCount; }
        bool IsLoaded() const { return m_loaded; }
        
        // Создание примитивных мешей
        static Mesh CreateCube(float size = 1.0f);
        static Mesh CreateSphere(float radius = 1.0f, int segments = 32);
        static Mesh CreatePlane(float width = 1.0f, float height = 1.0f);
        static Mesh CreateCylinder(float radius = 1.0f, float height = 1.0f, int segments = 32);
        
    private:
        void SetupMesh();
        
        unsigned int m_VAO;
        unsigned int m_VBO;
        unsigned int m_EBO;
        
        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
        
        size_t m_vertexCount;
        size_t m_indexCount;
        bool m_loaded;
    };
}

