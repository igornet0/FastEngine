#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>

namespace FastEngine {

/**
 * Узел навигационной сетки
 */
struct NavNode {
    glm::vec3 position;
    std::vector<int> connections; // Индексы связанных узлов
    bool walkable;
    float cost; // Стоимость прохождения через узел
    
    NavNode() : position(0.0f), walkable(true), cost(1.0f) {}
    NavNode(const glm::vec3& pos, bool walk = true, float c = 1.0f) 
        : position(pos), walkable(walk), cost(c) {}
};

/**
 * Треугольник навигационной сетки
 */
struct NavTriangle {
    int vertices[3]; // Индексы вершин треугольника
    glm::vec3 center; // Центр треугольника
    bool walkable;
    
    NavTriangle() : walkable(true) {
        vertices[0] = vertices[1] = vertices[2] = -1;
    }
    
    NavTriangle(int v0, int v1, int v2, const glm::vec3& c, bool walk = true)
        : center(c), walkable(walk) {
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
    }
};

/**
 * Навигационная сетка
 */
class NavMesh {
public:
    NavMesh();
    ~NavMesh() = default;
    
    // Генерация навигационной сетки
    bool GenerateFromMesh(const std::vector<glm::vec3>& vertices, 
                         const std::vector<unsigned int>& indices);
    bool GenerateFromHeightmap(const std::vector<std::vector<float>>& heightmap, 
                               float cellSize, float heightThreshold);
    
    // Поиск пути
    std::vector<glm::vec3> FindPath(const glm::vec3& start, const glm::vec3& end);
    bool IsWalkable(const glm::vec3& position) const;
    glm::vec3 GetNearestWalkablePoint(const glm::vec3& position) const;
    
    // Получение информации о сетке
    const std::vector<NavNode>& GetNodes() const { return m_nodes; }
    const std::vector<NavTriangle>& GetTriangles() const { return m_triangles; }
    bool IsValid() const { return !m_nodes.empty() && !m_triangles.empty(); }
    
    // Сериализация
    std::string Serialize() const;
    void Deserialize(const std::string& data);
    
private:
    std::vector<NavNode> m_nodes;
    std::vector<NavTriangle> m_triangles;
    std::unordered_map<int, std::vector<int>> m_nodeToTriangles; // Узел -> треугольники
    
    // Вспомогательные методы
    void TriangulateMesh(const std::vector<glm::vec3>& vertices, 
                        const std::vector<unsigned int>& indices);
    void BuildConnections();
    bool IsPointInTriangle(const glm::vec3& point, const NavTriangle& triangle) const;
    glm::vec3 GetTriangleCenter(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) const;
};

/**
 * Генератор навигационной сетки
 */
class NavMeshGenerator {
public:
    NavMeshGenerator();
    ~NavMeshGenerator() = default;
    
    // Настройки генерации
    void SetCellSize(float size) { m_cellSize = size; }
    void SetHeightThreshold(float threshold) { m_heightThreshold = threshold; }
    void SetAgentRadius(float radius) { m_agentRadius = radius; }
    void SetMaxSlope(float slope) { m_maxSlope = slope; }
    
    // Генерация из различных источников
    std::unique_ptr<NavMesh> GenerateFromMesh(const std::vector<glm::vec3>& vertices,
                                              const std::vector<unsigned int>& indices);
    std::unique_ptr<NavMesh> GenerateFromHeightmap(const std::vector<std::vector<float>>& heightmap);
    std::unique_ptr<NavMesh> GenerateFromColliders(const std::vector<glm::vec3>& colliderPositions,
                                                   const std::vector<glm::vec3>& colliderSizes);
    
private:
    float m_cellSize;
    float m_heightThreshold;
    float m_agentRadius;
    float m_maxSlope;
    
    // Алгоритмы генерации
    void DelaunayTriangulation(std::vector<glm::vec3>& points, 
                              std::vector<NavTriangle>& triangles);
    void FilterWalkableAreas(NavMesh& navMesh);
    void SimplifyMesh(NavMesh& navMesh);
};

} // namespace FastEngine

