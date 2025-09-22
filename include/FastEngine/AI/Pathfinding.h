#pragma once

#include "FastEngine/AI/NavMesh.h"
#include <vector>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace FastEngine {

/**
 * Узел для поиска пути
 */
struct PathNode {
    int nodeIndex;
    float gCost; // Стоимость от старта
    float hCost; // Эвристическая стоимость до цели
    float fCost; // Общая стоимость (g + h)
    int parentIndex; // Родительский узел
    
    PathNode() : nodeIndex(-1), gCost(0.0f), hCost(0.0f), fCost(0.0f), parentIndex(-1) {}
    PathNode(int index, float g, float h, int parent = -1) 
        : nodeIndex(index), gCost(g), hCost(h), fCost(g + h), parentIndex(parent) {}
    
    bool operator>(const PathNode& other) const {
        return fCost > other.fCost;
    }
};

/**
 * Результат поиска пути
 */
struct PathfindingResult {
    bool success;
    std::vector<glm::vec3> path;
    float totalCost;
    int nodesExplored;
    
    PathfindingResult() : success(false), totalCost(0.0f), nodesExplored(0) {}
};

/**
 * Алгоритм поиска пути A*
 */
class AStarPathfinding {
public:
    AStarPathfinding();
    ~AStarPathfinding() = default;
    
    // Настройки алгоритма
    void SetHeuristic(std::function<float(const glm::vec3&, const glm::vec3&)> heuristic);
    void SetMaxIterations(int maxIter) { m_maxIterations = maxIter; }
    void SetSmoothing(bool smooth) { m_smoothPath = smooth; }
    
    // Поиск пути
    PathfindingResult FindPath(const NavMesh& navMesh, 
                              const glm::vec3& start, 
                              const glm::vec3& end);
    
    // Получение статистики
    int GetLastNodesExplored() const { return m_lastNodesExplored; }
    float GetLastPathCost() const { return m_lastPathCost; }
    
private:
    std::function<float(const glm::vec3&, const glm::vec3&)> m_heuristic;
    int m_maxIterations;
    bool m_smoothPath;
    int m_lastNodesExplored;
    float m_lastPathCost;
    
    // Вспомогательные методы
    float CalculateHeuristic(const glm::vec3& start, const glm::vec3& end) const;
    std::vector<int> ReconstructPath(const std::unordered_map<int, int>& cameFrom, 
                                    int start, int end) const;
    std::vector<glm::vec3> SmoothPath(const std::vector<glm::vec3>& path) const;
    bool IsValidMove(const NavMesh& navMesh, int from, int to) const;
};

/**
 * Менеджер поиска пути
 */
class PathfindingManager {
public:
    PathfindingManager();
    ~PathfindingManager() = default;
    
    // Инициализация
    bool Initialize();
    void Shutdown();
    
    // Управление навигационными сетками
    void AddNavMesh(const std::string& name, std::shared_ptr<NavMesh> navMesh);
    void RemoveNavMesh(const std::string& name);
    std::shared_ptr<NavMesh> GetNavMesh(const std::string& name) const;
    
    // Поиск пути
    PathfindingResult FindPath(const std::string& navMeshName,
                              const glm::vec3& start,
                              const glm::vec3& end);
    
    // Настройки
    void SetDefaultHeuristic(std::function<float(const glm::vec3&, const glm::vec3&)> heuristic);
    void SetMaxIterations(int maxIter) { m_maxIterations = maxIter; }
    
    // Обновление
    void Update(float deltaTime);
    
    // Статистика
    int GetTotalPathsFound() const { return m_totalPathsFound; }
    float GetAveragePathCost() const { return m_averagePathCost; }
    
private:
    std::unordered_map<std::string, std::shared_ptr<NavMesh>> m_navMeshes;
    std::unique_ptr<AStarPathfinding> m_pathfinder;
    int m_maxIterations;
    int m_totalPathsFound;
    float m_averagePathCost;
    
    // Обновление статистики
    void UpdateStatistics(const PathfindingResult& result);
};

/**
 * Компонент для поиска пути
 */
class PathfindingComponent {
public:
    PathfindingComponent();
    ~PathfindingComponent() = default;
    
    // Настройки
    void SetNavMeshName(const std::string& name) { m_navMeshName = name; }
    void SetTargetPosition(const glm::vec3& target) { m_target = target; }
    void SetSpeed(float speed) { m_speed = speed; }
    void SetArrivalDistance(float distance) { m_arrivalDistance = distance; }
    
    // Состояние
    bool HasPath() const { return !m_path.empty(); }
    bool IsMoving() const { return m_isMoving; }
    glm::vec3 GetNextWaypoint() const;
    
    // Управление
    void StartPathfinding();
    void StopPathfinding();
    void Update(float deltaTime);
    
    // Получение данных
    const std::vector<glm::vec3>& GetPath() const { return m_path; }
    const glm::vec3& GetTarget() const { return m_target; }
    float GetSpeed() const { return m_speed; }
    
private:
    std::string m_navMeshName;
    glm::vec3 m_target;
    std::vector<glm::vec3> m_path;
    int m_currentWaypoint;
    float m_speed;
    float m_arrivalDistance;
    bool m_isMoving;
    
    // Callbacks
    std::function<void()> m_onPathComplete;
    std::function<void()> m_onPathFailed;
    
public:
    void SetOnPathComplete(std::function<void()> callback) { m_onPathComplete = callback; }
    void SetOnPathFailed(std::function<void()> callback) { m_onPathFailed = callback; }
};

} // namespace FastEngine
