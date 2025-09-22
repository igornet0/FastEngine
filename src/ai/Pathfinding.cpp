#include "FastEngine/AI/Pathfinding.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>

namespace FastEngine {

AStarPathfinding::AStarPathfinding() 
    : m_maxIterations(1000)
    , m_smoothPath(true)
    , m_lastNodesExplored(0)
    , m_lastPathCost(0.0f) {
    
    // Устанавливаем эвристику по умолчанию (евклидово расстояние)
    m_heuristic = [](const glm::vec3& start, const glm::vec3& end) {
        return glm::distance(start, end);
    };
}

void AStarPathfinding::SetHeuristic(std::function<float(const glm::vec3&, const glm::vec3&)> heuristic) {
    m_heuristic = heuristic;
}

PathfindingResult AStarPathfinding::FindPath(const NavMesh& navMesh, 
                                            const glm::vec3& start, 
                                            const glm::vec3& end) {
    PathfindingResult result;
    
    if (!navMesh.IsValid()) {
        std::cerr << "AStarPathfinding: Invalid NavMesh" << std::endl;
        return result;
    }
    
    const auto& nodes = navMesh.GetNodes();
    if (nodes.empty()) {
        std::cerr << "AStarPathfinding: No nodes in NavMesh" << std::endl;
        return result;
    }
    
    // Находим ближайшие узлы
    int startNode = -1;
    int endNode = -1;
    float minStartDist = std::numeric_limits<float>::max();
    float minEndDist = std::numeric_limits<float>::max();
    
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (!nodes[i].walkable) continue;
        
        float startDist = glm::distance(start, nodes[i].position);
        float endDist = glm::distance(end, nodes[i].position);
        
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
        std::cerr << "AStarPathfinding: Could not find valid start or end node" << std::endl;
        return result;
    }
    
    if (startNode == endNode) {
        result.success = true;
        result.path = {start, end};
        result.totalCost = 0.0f;
        result.nodesExplored = 1;
        return result;
    }
    
    // Инициализируем A* алгоритм
    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> openSet;
    std::unordered_set<int> closedSet;
    std::unordered_map<int, int> cameFrom;
    std::unordered_map<int, float> gScore;
    std::unordered_map<int, float> fScore;
    
    // Начальный узел
    float hStart = CalculateHeuristic(nodes[startNode].position, nodes[endNode].position);
    gScore[startNode] = 0.0f;
    fScore[startNode] = hStart;
    openSet.emplace(startNode, 0.0f, hStart, -1);
    
    int iterations = 0;
    
    while (!openSet.empty() && iterations < m_maxIterations) {
        ++iterations;
        
        PathNode current = openSet.top();
        openSet.pop();
        
        if (current.nodeIndex == endNode) {
            // Найден путь
            std::vector<int> nodePath = ReconstructPath(cameFrom, startNode, endNode);
            
            // Преобразуем в позиции
            for (int nodeIndex : nodePath) {
                result.path.push_back(nodes[nodeIndex].position);
            }
            
            if (m_smoothPath) {
                result.path = SmoothPath(result.path);
            }
            
            result.success = true;
            result.totalCost = gScore[endNode];
            result.nodesExplored = static_cast<int>(closedSet.size());
            m_lastNodesExplored = result.nodesExplored;
            m_lastPathCost = result.totalCost;
            
            return result;
        }
        
        closedSet.insert(current.nodeIndex);
        
        // Проверяем соседние узлы
        for (int neighbor : nodes[current.nodeIndex].connections) {
            if (closedSet.find(neighbor) != closedSet.end()) {
                continue;
            }
            
            if (!nodes[neighbor].walkable) {
                continue;
            }
            
            if (!IsValidMove(navMesh, current.nodeIndex, neighbor)) {
                continue;
            }
            
            float tentativeGScore = gScore[current.nodeIndex] + 
                                  glm::distance(nodes[current.nodeIndex].position, nodes[neighbor].position) * 
                                  nodes[neighbor].cost;
            
            if (gScore.find(neighbor) == gScore.end() || tentativeGScore < gScore[neighbor]) {
                cameFrom[neighbor] = current.nodeIndex;
                gScore[neighbor] = tentativeGScore;
                float hNeighbor = CalculateHeuristic(nodes[neighbor].position, nodes[endNode].position);
                fScore[neighbor] = tentativeGScore + hNeighbor;
                
                openSet.emplace(neighbor, tentativeGScore, hNeighbor, current.nodeIndex);
            }
        }
    }
    
    // Путь не найден
    result.success = false;
    result.nodesExplored = static_cast<int>(closedSet.size());
    m_lastNodesExplored = result.nodesExplored;
    m_lastPathCost = 0.0f;
    
    std::cerr << "AStarPathfinding: Path not found after " << iterations << " iterations" << std::endl;
    return result;
}

float AStarPathfinding::CalculateHeuristic(const glm::vec3& start, const glm::vec3& end) const {
    if (m_heuristic) {
        return m_heuristic(start, end);
    }
    return glm::distance(start, end);
}

std::vector<int> AStarPathfinding::ReconstructPath(const std::unordered_map<int, int>& cameFrom, 
                                                  int start, int end) const {
    std::vector<int> path;
    int current = end;
    
    while (current != -1) {
        path.push_back(current);
        auto it = cameFrom.find(current);
        current = (it != cameFrom.end()) ? it->second : -1;
    }
    
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<glm::vec3> AStarPathfinding::SmoothPath(const std::vector<glm::vec3>& path) const {
    if (path.size() <= 2) {
        return path;
    }
    
    std::vector<glm::vec3> smoothedPath;
    smoothedPath.push_back(path[0]);
    
    for (size_t i = 1; i < path.size() - 1; ++i) {
        // Простое сглаживание - усредняем соседние точки
        glm::vec3 smoothed = (path[i - 1] + path[i] + path[i + 1]) / 3.0f;
        smoothedPath.push_back(smoothed);
    }
    
    smoothedPath.push_back(path.back());
    return smoothedPath;
}

bool AStarPathfinding::IsValidMove(const NavMesh& navMesh, int from, int to) const {
    // В реальной реализации здесь бы проверялись препятствия
    return true;
}

// PathfindingManager implementation
PathfindingManager::PathfindingManager() 
    : m_maxIterations(1000)
    , m_totalPathsFound(0)
    , m_averagePathCost(0.0f) {
}

bool PathfindingManager::Initialize() {
    m_pathfinder = std::make_unique<AStarPathfinding>();
    
    // Устанавливаем эвристику по умолчанию
    m_pathfinder->SetHeuristic([](const glm::vec3& start, const glm::vec3& end) {
        return glm::distance(start, end);
    });
    
    std::cout << "PathfindingManager initialized successfully" << std::endl;
    return true;
}

void PathfindingManager::Shutdown() {
    m_navMeshes.clear();
    m_pathfinder.reset();
    std::cout << "PathfindingManager shutdown" << std::endl;
}

void PathfindingManager::AddNavMesh(const std::string& name, std::shared_ptr<NavMesh> navMesh) {
    m_navMeshes[name] = navMesh;
    std::cout << "Added NavMesh: " << name << std::endl;
}

void PathfindingManager::RemoveNavMesh(const std::string& name) {
    auto it = m_navMeshes.find(name);
    if (it != m_navMeshes.end()) {
        m_navMeshes.erase(it);
        std::cout << "Removed NavMesh: " << name << std::endl;
    }
}

std::shared_ptr<NavMesh> PathfindingManager::GetNavMesh(const std::string& name) const {
    auto it = m_navMeshes.find(name);
    return (it != m_navMeshes.end()) ? it->second : nullptr;
}

PathfindingResult PathfindingManager::FindPath(const std::string& navMeshName,
                                              const glm::vec3& start,
                                              const glm::vec3& end) {
    auto navMesh = GetNavMesh(navMeshName);
    if (!navMesh) {
        std::cerr << "PathfindingManager: NavMesh not found: " << navMeshName << std::endl;
        return PathfindingResult();
    }
    
    if (!m_pathfinder) {
        std::cerr << "PathfindingManager: Pathfinder not initialized" << std::endl;
        return PathfindingResult();
    }
    
    PathfindingResult result = m_pathfinder->FindPath(*navMesh, start, end);
    UpdateStatistics(result);
    
    return result;
}

void PathfindingManager::SetDefaultHeuristic(std::function<float(const glm::vec3&, const glm::vec3&)> heuristic) {
    if (m_pathfinder) {
        m_pathfinder->SetHeuristic(heuristic);
    }
}

void PathfindingManager::Update(float deltaTime) {
    // В реальной реализации здесь бы обновлялись активные поиски путей
    // и кэшированные результаты
    // Пока что просто заглушка
}

void PathfindingManager::UpdateStatistics(const PathfindingResult& result) {
    if (result.success) {
        ++m_totalPathsFound;
        m_averagePathCost = (m_averagePathCost * (m_totalPathsFound - 1) + result.totalCost) / m_totalPathsFound;
    }
}

// PathfindingComponent implementation
PathfindingComponent::PathfindingComponent() 
    : m_currentWaypoint(0)
    , m_speed(5.0f)
    , m_arrivalDistance(0.5f)
    , m_isMoving(false) {
}

// SetTargetPosition уже определен в заголовочном файле как inline

void PathfindingComponent::StartPathfinding() {
    if (m_navMeshName.empty()) {
        std::cerr << "PathfindingComponent: No NavMesh name set" << std::endl;
        return;
    }
    
    // В реальной реализации здесь бы вызывался PathfindingManager
    std::cout << "PathfindingComponent: Starting pathfinding to target" << std::endl;
    m_isMoving = true;
}

void PathfindingComponent::StopPathfinding() {
    m_isMoving = false;
    m_path.clear();
    m_currentWaypoint = 0;
    std::cout << "PathfindingComponent: Stopped pathfinding" << std::endl;
}

void PathfindingComponent::Update(float deltaTime) {
    if (!m_isMoving || m_path.empty() || m_currentWaypoint >= m_path.size()) {
        if (m_isMoving && m_onPathComplete) {
            m_onPathComplete();
        }
        m_isMoving = false;
        return;
    }
    
    // В реальной реализации здесь бы обновлялась позиция объекта
    std::cout << "PathfindingComponent: Moving to waypoint " << m_currentWaypoint << std::endl;
    
    // Проверяем, достигли ли текущего waypoint
    if (m_currentWaypoint < m_path.size()) {
        ++m_currentWaypoint;
    }
}

glm::vec3 PathfindingComponent::GetNextWaypoint() const {
    if (m_currentWaypoint < m_path.size()) {
        return m_path[m_currentWaypoint];
    }
    return m_target;
}

} // namespace FastEngine
