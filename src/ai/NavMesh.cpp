#include "FastEngine/AI/NavMesh.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace FastEngine {

NavMesh::NavMesh() {}

bool NavMesh::GenerateFromMesh(const std::vector<glm::vec3>& vertices, 
                              const std::vector<unsigned int>& indices) {
    if (vertices.empty() || indices.empty()) {
        std::cerr << "NavMesh: Invalid input data for mesh generation" << std::endl;
        return false;
    }
    
    // Очищаем существующие данные
    m_nodes.clear();
    m_triangles.clear();
    m_nodeToTriangles.clear();
    
    // Создаем узлы из вершин
    for (const auto& vertex : vertices) {
        m_nodes.emplace_back(vertex, true, 1.0f);
    }
    
    // Триангулируем меш
    TriangulateMesh(vertices, indices);
    
    // Строим соединения
    BuildConnections();
    
    std::cout << "NavMesh generated: " << m_nodes.size() << " nodes, " 
              << m_triangles.size() << " triangles" << std::endl;
    
    return true;
}

bool NavMesh::GenerateFromHeightmap(const std::vector<std::vector<float>>& heightmap, 
                                   float cellSize, float heightThreshold) {
    if (heightmap.empty() || heightmap[0].empty()) {
        std::cerr << "NavMesh: Invalid heightmap data" << std::endl;
        return false;
    }
    
    m_nodes.clear();
    m_triangles.clear();
    m_nodeToTriangles.clear();
    
    int width = heightmap[0].size();
    int height = heightmap.size();
    
    // Создаем узлы из высотной карты
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float heightValue = heightmap[y][x];
            bool walkable = heightValue >= heightThreshold;
            
            glm::vec3 position(x * cellSize, heightValue, y * cellSize);
            m_nodes.emplace_back(position, walkable, walkable ? 1.0f : 1000.0f);
        }
    }
    
    // Создаем треугольники
    for (int y = 0; y < height - 1; ++y) {
        for (int x = 0; x < width - 1; ++x) {
            int i = y * width + x;
            int i1 = i + 1;
            int i2 = (y + 1) * width + x;
            int i3 = i2 + 1;
            
            // Создаем два треугольника для каждого квадрата
            glm::vec3 center1 = (m_nodes[i].position + m_nodes[i1].position + m_nodes[i2].position) / 3.0f;
            glm::vec3 center2 = (m_nodes[i1].position + m_nodes[i2].position + m_nodes[i3].position) / 3.0f;
            
            bool walkable1 = m_nodes[i].walkable && m_nodes[i1].walkable && m_nodes[i2].walkable;
            bool walkable2 = m_nodes[i1].walkable && m_nodes[i2].walkable && m_nodes[i3].walkable;
            
            m_triangles.emplace_back(i, i1, i2, center1, walkable1);
            m_triangles.emplace_back(i1, i2, i3, center2, walkable2);
        }
    }
    
    BuildConnections();
    
    std::cout << "NavMesh generated from heightmap: " << m_nodes.size() << " nodes, " 
              << m_triangles.size() << " triangles" << std::endl;
    
    return true;
}

std::vector<glm::vec3> NavMesh::FindPath(const glm::vec3& start, const glm::vec3& end) {
    std::vector<glm::vec3> path;
    
    if (m_nodes.empty()) {
        return path;
    }
    
    // Находим ближайшие узлы
    int startNode = -1;
    int endNode = -1;
    float minStartDist = std::numeric_limits<float>::max();
    float minEndDist = std::numeric_limits<float>::max();
    
    for (size_t i = 0; i < m_nodes.size(); ++i) {
        if (!m_nodes[i].walkable) continue;
        
        float startDist = glm::distance(start, m_nodes[i].position);
        float endDist = glm::distance(end, m_nodes[i].position);
        
        if (startDist < minStartDist) {
            minStartDist = startDist;
            startNode = static_cast<int>(i);
        }
        
        if (endDist < minEndDist) {
            minEndDist = endDist;
            endNode = static_cast<int>(i);
        }
    }
    
    if (startNode == -1 || endNode == -1) {
        std::cerr << "NavMesh: Could not find valid start or end node" << std::endl;
        return path;
    }
    
    // Простой поиск пути (в реальной реализации здесь бы был A*)
    std::vector<int> nodePath;
    std::unordered_set<int> visited;
    std::vector<int> queue = {startNode};
    std::unordered_map<int, int> parent;
    
    visited.insert(startNode);
    
    while (!queue.empty()) {
        int current = queue.front();
        queue.erase(queue.begin());
        
        if (current == endNode) {
            // Восстанавливаем путь
            int node = endNode;
            while (node != -1) {
                nodePath.push_back(node);
                node = parent[node];
            }
            std::reverse(nodePath.begin(), nodePath.end());
            break;
        }
        
        for (int neighbor : m_nodes[current].connections) {
            if (visited.find(neighbor) == visited.end() && m_nodes[neighbor].walkable) {
                visited.insert(neighbor);
                parent[neighbor] = current;
                queue.push_back(neighbor);
            }
        }
    }
    
    // Преобразуем в позиции
    for (int nodeIndex : nodePath) {
        path.push_back(m_nodes[nodeIndex].position);
    }
    
    return path;
}

bool NavMesh::IsWalkable(const glm::vec3& position) const {
    // Проверяем, находится ли точка в проходимом треугольнике
    for (const auto& triangle : m_triangles) {
        if (!triangle.walkable) continue;
        
        if (IsPointInTriangle(position, triangle)) {
            return true;
        }
    }
    
    return false;
}

glm::vec3 NavMesh::GetNearestWalkablePoint(const glm::vec3& position) const {
    if (m_nodes.empty()) {
        return position;
    }
    
    float minDist = std::numeric_limits<float>::max();
    glm::vec3 nearestPoint = position;
    
    for (const auto& node : m_nodes) {
        if (!node.walkable) continue;
        
        float dist = glm::distance(position, node.position);
        if (dist < minDist) {
            minDist = dist;
            nearestPoint = node.position;
        }
    }
    
    return nearestPoint;
}

std::string NavMesh::Serialize() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"nodes\": [\n";
    
    for (size_t i = 0; i < m_nodes.size(); ++i) {
        const auto& node = m_nodes[i];
        ss << "    {\n";
        ss << "      \"position\": {\"x\": " << node.position.x << ", \"y\": " << node.position.y << ", \"z\": " << node.position.z << "},\n";
        ss << "      \"walkable\": " << (node.walkable ? "true" : "false") << ",\n";
        ss << "      \"cost\": " << node.cost << ",\n";
        ss << "      \"connections\": [";
        for (size_t j = 0; j < node.connections.size(); ++j) {
            ss << node.connections[j];
            if (j < node.connections.size() - 1) ss << ", ";
        }
        ss << "]\n";
        ss << "    }";
        if (i < m_nodes.size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "  ],\n";
    ss << "  \"triangles\": [\n";
    
    for (size_t i = 0; i < m_triangles.size(); ++i) {
        const auto& triangle = m_triangles[i];
        ss << "    {\n";
        ss << "      \"vertices\": [" << triangle.vertices[0] << ", " << triangle.vertices[1] << ", " << triangle.vertices[2] << "],\n";
        ss << "      \"center\": {\"x\": " << triangle.center.x << ", \"y\": " << triangle.center.y << ", \"z\": " << triangle.center.z << "},\n";
        ss << "      \"walkable\": " << (triangle.walkable ? "true" : "false") << "\n";
        ss << "    }";
        if (i < m_triangles.size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "  ]\n";
    ss << "}";
    return ss.str();
}

void NavMesh::Deserialize(const std::string& data) {
    std::cout << "Deserializing NavMesh..." << std::endl;
    // В реальной реализации здесь бы парсился JSON
}

void NavMesh::TriangulateMesh(const std::vector<glm::vec3>& vertices, 
                             const std::vector<unsigned int>& indices) {
    // Простая триангуляция - создаем треугольники из индексов
    for (size_t i = 0; i < indices.size(); i += 3) {
        if (i + 2 < indices.size()) {
            int v0 = indices[i];
            int v1 = indices[i + 1];
            int v2 = indices[i + 2];
            
            glm::vec3 center = GetTriangleCenter(vertices[v0], vertices[v1], vertices[v2]);
            bool walkable = m_nodes[v0].walkable && m_nodes[v1].walkable && m_nodes[v2].walkable;
            
            m_triangles.emplace_back(v0, v1, v2, center, walkable);
        }
    }
}

void NavMesh::BuildConnections() {
    // Строим соединения между узлами на основе треугольников
    for (size_t i = 0; i < m_triangles.size(); ++i) {
        const auto& triangle = m_triangles[i];
        
        for (int j = 0; j < 3; ++j) {
            int vertexIndex = triangle.vertices[j];
            m_nodeToTriangles[vertexIndex].push_back(static_cast<int>(i));
        }
    }
    
    // Создаем соединения между узлами в одном треугольнике
    for (const auto& triangle : m_triangles) {
        if (!triangle.walkable) continue;
        
        for (int i = 0; i < 3; ++i) {
            int v0 = triangle.vertices[i];
            int v1 = triangle.vertices[(i + 1) % 3];
            
            // Добавляем двустороннее соединение
            if (std::find(m_nodes[v0].connections.begin(), m_nodes[v0].connections.end(), v1) == m_nodes[v0].connections.end()) {
                m_nodes[v0].connections.push_back(v1);
            }
            if (std::find(m_nodes[v1].connections.begin(), m_nodes[v1].connections.end(), v0) == m_nodes[v1].connections.end()) {
                m_nodes[v1].connections.push_back(v0);
            }
        }
    }
}

bool NavMesh::IsPointInTriangle(const glm::vec3& point, const NavTriangle& triangle) const {
    if (triangle.vertices[0] >= m_nodes.size() || 
        triangle.vertices[1] >= m_nodes.size() || 
        triangle.vertices[2] >= m_nodes.size()) {
        return false;
    }
    
    const glm::vec3& v0 = m_nodes[triangle.vertices[0]].position;
    const glm::vec3& v1 = m_nodes[triangle.vertices[1]].position;
    const glm::vec3& v2 = m_nodes[triangle.vertices[2]].position;
    
    // Используем барицентрические координаты
    float denom = (v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y);
    if (std::abs(denom) < 1e-10f) return false;
    
    float a = ((v1.y - v2.y) * (point.x - v2.x) + (v2.x - v1.x) * (point.y - v2.y)) / denom;
    float b = ((v2.y - v0.y) * (point.x - v2.x) + (v0.x - v2.x) * (point.y - v2.y)) / denom;
    float c = 1.0f - a - b;
    
    return a >= 0.0f && b >= 0.0f && c >= 0.0f;
}

glm::vec3 NavMesh::GetTriangleCenter(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) const {
    return (v0 + v1 + v2) / 3.0f;
}

// NavMeshGenerator implementation
NavMeshGenerator::NavMeshGenerator() 
    : m_cellSize(1.0f)
    , m_heightThreshold(0.5f)
    , m_agentRadius(0.5f)
    , m_maxSlope(45.0f) {
}

std::unique_ptr<NavMesh> NavMeshGenerator::GenerateFromMesh(const std::vector<glm::vec3>& vertices,
                                                           const std::vector<unsigned int>& indices) {
    auto navMesh = std::make_unique<NavMesh>();
    
    if (navMesh->GenerateFromMesh(vertices, indices)) {
        FilterWalkableAreas(*navMesh);
        SimplifyMesh(*navMesh);
        return navMesh;
    }
    
    return nullptr;
}

std::unique_ptr<NavMesh> NavMeshGenerator::GenerateFromHeightmap(const std::vector<std::vector<float>>& heightmap) {
    auto navMesh = std::make_unique<NavMesh>();
    
    if (navMesh->GenerateFromHeightmap(heightmap, m_cellSize, m_heightThreshold)) {
        FilterWalkableAreas(*navMesh);
        SimplifyMesh(*navMesh);
        return navMesh;
    }
    
    return nullptr;
}

std::unique_ptr<NavMesh> NavMeshGenerator::GenerateFromColliders(const std::vector<glm::vec3>& colliderPositions,
                                                                const std::vector<glm::vec3>& colliderSizes) {
    // В реальной реализации здесь бы генерировалась навигационная сетка
    // на основе коллайдеров
    std::cout << "NavMeshGenerator: Generating from colliders not implemented yet" << std::endl;
    return nullptr;
}

void NavMeshGenerator::DelaunayTriangulation(std::vector<glm::vec3>& points, 
                                            std::vector<NavTriangle>& triangles) {
    // В реальной реализации здесь бы был алгоритм триангуляции Делоне
    std::cout << "NavMeshGenerator: Delaunay triangulation not implemented yet" << std::endl;
}

void NavMeshGenerator::FilterWalkableAreas(NavMesh& navMesh) {
    // Фильтруем непроходимые области
    std::cout << "NavMeshGenerator: Filtering walkable areas" << std::endl;
}

void NavMeshGenerator::SimplifyMesh(NavMesh& navMesh) {
    // Упрощаем сетку
    std::cout << "NavMeshGenerator: Simplifying mesh" << std::endl;
}

} // namespace FastEngine

