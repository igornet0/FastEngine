#include "FastEngine/Editor/BlueprintEditor.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>

namespace FastEngine {

BlueprintEditor::BlueprintEditor() 
    : m_initialized(false)
    , m_visible(true)
    , m_executing(false)
    , m_paused(false) {
}

BlueprintEditor::~BlueprintEditor() {
    if (m_initialized) {
        Clear();
    }
}

bool BlueprintEditor::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    m_initialized = true;
    
    std::cout << "BlueprintEditor initialized successfully" << std::endl;
    return true;
}

void BlueprintEditor::Update(float deltaTime) {
    if (!m_initialized || !m_visible) {
        return;
    }
    
    // Обновляем узлы
    for (auto& node : m_nodes) {
        if (node) {
            node->Update(deltaTime);
        }
    }
    
    // Обновляем соединения
    UpdateConnections();
    
    // Вызываем callback для обновления UI
    if (m_updateCallback) {
        m_updateCallback();
    }
}

void BlueprintEditor::Render(void* renderer) {
    if (!m_initialized || !m_visible) {
        return;
    }
    
    // В реальной реализации здесь бы рендерился UI редактора
    // Для простоты выводим информацию в консоль
    std::cout << "=== Blueprint Editor ===" << std::endl;
    std::cout << "Nodes: " << m_nodes.size() << std::endl;
    std::cout << "Connections: " << m_connections.size() << std::endl;
    std::cout << "Executing: " << (m_executing ? "Yes" : "No") << std::endl;
    std::cout << "Paused: " << (m_paused ? "Yes" : "No") << std::endl;
    
    // Рендерим узлы
    RenderNodes(renderer);
    
    // Рендерим соединения
    RenderConnections(renderer);
    
    // Рендерим UI
    RenderUI(renderer);
    
    std::cout << "=======================" << std::endl;
}

bool BlueprintEditor::HandleInput(float mouseX, float mouseY, int mouseButton, int key) {
    if (!m_initialized || !m_visible) {
        return false;
    }
    
    // Обработка клика мыши
    if (mouseButton == 0) { // Левая кнопка мыши
        // Проверяем клик по узлам
        for (auto& node : m_nodes) {
            if (node && HandleNodeInput(node, mouseX, mouseY, mouseButton)) {
                return true;
            }
        }
        
        // Проверяем клик по соединениям
        if (HandleConnectionInput(mouseX, mouseY, mouseButton)) {
            return true;
        }
        
        // Если клик не по узлу, отменяем выбор
        DeselectAll();
    }
    
    // Обработка клавиатуры
    switch (key) {
        case 'E':
            Execute();
            return true;
        case 'S':
            Stop();
            return true;
        case 'P':
            if (m_executing) {
                if (m_paused) {
                    Resume();
                } else {
                    Pause();
                }
            }
            return true;
        case 'C':
            Clear();
            return true;
        case 'N':
            // Создаем новый узел
            {
                auto eventNode = std::make_shared<EventNode>();
                eventNode->SetPosition(glm::vec2(mouseX, mouseY));
                AddNode(eventNode);
            }
            return true;
    }
    
    return false;
}

bool BlueprintEditor::AddNode(std::shared_ptr<BlueprintNode> node) {
    if (!node) {
        return false;
    }
    
    // Генерируем уникальный ID
    std::string nodeId = GenerateNodeId();
    node->SetId(nodeId);
    
    m_nodes.push_back(node);
    m_nodeMap[nodeId] = node;
    
    std::cout << "Added node: " << node->GetName() << " (ID: " << nodeId << ")" << std::endl;
    return true;
}

bool BlueprintEditor::RemoveNode(const std::string& nodeId) {
    auto it = m_nodeMap.find(nodeId);
    if (it == m_nodeMap.end()) {
        return false;
    }
    
    // Удаляем все соединения с этим узлом
    m_connections.erase(
        std::remove_if(m_connections.begin(), m_connections.end(),
            [&nodeId](const NodeConnection& conn) {
                return conn.fromNodeId == nodeId || conn.toNodeId == nodeId;
            }),
        m_connections.end()
    );
    
    // Удаляем узел из списка
    m_nodes.erase(
        std::remove_if(m_nodes.begin(), m_nodes.end(),
            [&nodeId](const std::shared_ptr<BlueprintNode>& node) {
                return node && node->GetId() == nodeId;
            }),
        m_nodes.end()
    );
    
    // Удаляем из карты
    m_nodeMap.erase(it);
    
    // Если это выбранный узел, отменяем выбор
    if (m_selectedNodeId == nodeId) {
        DeselectAll();
    }
    
    std::cout << "Removed node: " << nodeId << std::endl;
    return true;
}

std::shared_ptr<BlueprintNode> BlueprintEditor::GetNode(const std::string& nodeId) {
    auto it = m_nodeMap.find(nodeId);
    return it != m_nodeMap.end() ? it->second : nullptr;
}

bool BlueprintEditor::CreateConnection(const NodeConnection& connection) {
    if (!IsConnectionValid(connection)) {
        std::cerr << "Invalid connection" << std::endl;
        return false;
    }
    
    // Проверяем, что соединение не существует
    auto it = std::find(m_connections.begin(), m_connections.end(), connection);
    if (it != m_connections.end()) {
        std::cerr << "Connection already exists" << std::endl;
        return false;
    }
    
    m_connections.push_back(connection);
    
    // Обновляем состояние портов
    auto fromNode = GetNode(connection.fromNodeId);
    auto toNode = GetNode(connection.toNodeId);
    
    if (fromNode) {
        auto fromPort = fromNode->GetOutputPort(connection.fromPortId);
        if (fromPort) {
            fromPort->isConnected = true;
        }
    }
    
    if (toNode) {
        auto toPort = toNode->GetInputPort(connection.toPortId);
        if (toPort) {
            toPort->isConnected = true;
        }
    }
    
    std::cout << "Created connection: " << connection.fromNodeId << ":" << connection.fromPortId 
              << " -> " << connection.toNodeId << ":" << connection.toPortId << std::endl;
    return true;
}

bool BlueprintEditor::RemoveConnection(const NodeConnection& connection) {
    auto it = std::find(m_connections.begin(), m_connections.end(), connection);
    if (it == m_connections.end()) {
        return false;
    }
    
    m_connections.erase(it);
    
    // Обновляем состояние портов
    auto fromNode = GetNode(connection.fromNodeId);
    auto toNode = GetNode(connection.toNodeId);
    
    if (fromNode) {
        auto fromPort = fromNode->GetOutputPort(connection.fromPortId);
        if (fromPort) {
            fromPort->isConnected = false;
        }
    }
    
    if (toNode) {
        auto toPort = toNode->GetInputPort(connection.toPortId);
        if (toPort) {
            toPort->isConnected = false;
        }
    }
    
    std::cout << "Removed connection: " << connection.fromNodeId << ":" << connection.fromPortId 
              << " -> " << connection.toNodeId << ":" << connection.toPortId << std::endl;
    return true;
}

void BlueprintEditor::SelectNode(const std::string& nodeId) {
    // Отменяем предыдущий выбор
    DeselectAll();
    
    auto node = GetNode(nodeId);
    if (node) {
        node->SetSelected(true);
        m_selectedNodeId = nodeId;
        std::cout << "Selected node: " << nodeId << std::endl;
    }
}

void BlueprintEditor::DeselectAll() {
    if (!m_selectedNodeId.empty()) {
        auto node = GetNode(m_selectedNodeId);
        if (node) {
            node->SetSelected(false);
        }
        m_selectedNodeId.clear();
        std::cout << "Deselected all nodes" << std::endl;
    }
}

std::shared_ptr<BlueprintNode> BlueprintEditor::GetSelectedNode() const {
    auto it = m_nodeMap.find(m_selectedNodeId);
    return it != m_nodeMap.end() ? it->second : nullptr;
}

void BlueprintEditor::MoveNode(const std::string& nodeId, const glm::vec2& newPosition) {
    auto node = GetNode(nodeId);
    if (node) {
        node->SetPosition(newPosition);
        std::cout << "Moved node " << nodeId << " to (" << newPosition.x << ", " << newPosition.y << ")" << std::endl;
    }
}

void BlueprintEditor::Execute() {
    if (m_executing) {
        return;
    }
    
    m_executing = true;
    m_paused = false;
    
    std::cout << "Executing Blueprint..." << std::endl;
    
    // Находим все узлы событий и выполняем их
    for (auto& node : m_nodes) {
        if (node && node->GetType() == NodeType::Event) {
            ExecuteNode(node);
        }
    }
    
    // Вызываем callback выполнения
    if (m_executeCallback) {
        m_executeCallback();
    }
}

void BlueprintEditor::Stop() {
    m_executing = false;
    m_paused = false;
    std::cout << "Stopped Blueprint execution" << std::endl;
}

void BlueprintEditor::Pause() {
    if (m_executing) {
        m_paused = true;
        std::cout << "Paused Blueprint execution" << std::endl;
    }
}

void BlueprintEditor::Resume() {
    if (m_executing && m_paused) {
        m_paused = false;
        std::cout << "Resumed Blueprint execution" << std::endl;
    }
}

void BlueprintEditor::Clear() {
    m_nodes.clear();
    m_connections.clear();
    m_nodeMap.clear();
    m_selectedNodeId.clear();
    m_executing = false;
    m_paused = false;
    
    std::cout << "Cleared Blueprint editor" << std::endl;
}

bool BlueprintEditor::Save(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << "{\n";
    file << "  \"nodes\": [\n";
    
    for (size_t i = 0; i < m_nodes.size(); ++i) {
        const auto& node = m_nodes[i];
        if (node) {
            file << "    " << node->Serialize();
            if (i < m_nodes.size() - 1) {
                file << ",";
            }
            file << "\n";
        }
    }
    
    file << "  ],\n";
    file << "  \"connections\": [\n";
    
    for (size_t i = 0; i < m_connections.size(); ++i) {
        const auto& conn = m_connections[i];
        file << "    {\n";
        file << "      \"fromNodeId\": \"" << conn.fromNodeId << "\",\n";
        file << "      \"fromPortId\": \"" << conn.fromPortId << "\",\n";
        file << "      \"toNodeId\": \"" << conn.toNodeId << "\",\n";
        file << "      \"toPortId\": \"" << conn.toPortId << "\"\n";
        file << "    }";
        if (i < m_connections.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    
    std::cout << "Saved Blueprint to: " << filename << std::endl;
    return true;
}

bool BlueprintEditor::Load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << filename << std::endl;
        return false;
    }
    
    // В реальной реализации здесь бы парсился JSON
    // Для простоты пропускаем
    
    std::cout << "Loaded Blueprint from: " << filename << std::endl;
    return true;
}

bool BlueprintEditor::Validate() {
    ClearValidationErrors();
    
    // Проверяем узлы
    for (const auto& node : m_nodes) {
        if (node && !node->IsValid()) {
            AddValidationError("Node " + node->GetId() + ": " + node->GetValidationError());
        }
    }
    
    // Проверяем соединения
    ValidateConnections();
    
    bool isValid = m_validationErrors.empty();
    std::cout << "Blueprint validation: " << (isValid ? "Valid" : "Invalid") << std::endl;
    
    if (!isValid) {
        for (const auto& error : m_validationErrors) {
            std::cout << "  Error: " << error << std::endl;
        }
    }
    
    return isValid;
}

void BlueprintEditor::RenderNodes(void* renderer) {
    // В реальной реализации здесь бы рендерились узлы
    for (const auto& node : m_nodes) {
        if (node) {
            std::cout << "  Node: " << node->GetName() << " at (" 
                      << node->GetPosition().x << ", " << node->GetPosition().y << ")" << std::endl;
        }
    }
}

void BlueprintEditor::RenderConnections(void* renderer) {
    // В реальной реализации здесь бы рендерились соединения
    for (const auto& conn : m_connections) {
        std::cout << "  Connection: " << conn.fromNodeId << ":" << conn.fromPortId 
                  << " -> " << conn.toNodeId << ":" << conn.toPortId << std::endl;
    }
}

void BlueprintEditor::RenderGrid(void* renderer) {
    // В реальной реализации здесь бы рендерилась сетка
    std::cout << "Rendering grid" << std::endl;
}

void BlueprintEditor::RenderUI(void* renderer) {
    // В реальной реализации здесь бы рендерился UI
    std::cout << "Rendering UI" << std::endl;
}

bool BlueprintEditor::HandleNodeInput(std::shared_ptr<BlueprintNode> node, float mouseX, float mouseY, int mouseButton) {
    if (!node) {
        return false;
    }
    
    // В реальной реализации здесь бы проверялся клик по узлу
    // Для простоты всегда возвращаем true
    SelectNode(node->GetId());
    return true;
}

bool BlueprintEditor::HandleConnectionInput(float mouseX, float mouseY, int mouseButton) {
    // В реальной реализации здесь бы проверялся клик по соединению
    return false;
}

void BlueprintEditor::HandleDragAndDrop(float mouseX, float mouseY) {
    // В реальной реализации здесь бы обрабатывался drag and drop
}

void BlueprintEditor::UpdateConnections() {
    // Обновляем состояние соединений
    for (auto& node : m_nodes) {
        if (node) {
            // В реальной реализации здесь бы обновлялись соединения портов
            // node->UpdatePortConnections();
        }
    }
}

void BlueprintEditor::ValidateConnections() {
    for (const auto& conn : m_connections) {
        auto fromNodeIt = m_nodeMap.find(conn.fromNodeId);
        auto toNodeIt = m_nodeMap.find(conn.toNodeId);
        auto fromNode = fromNodeIt != m_nodeMap.end() ? fromNodeIt->second : nullptr;
        auto toNode = toNodeIt != m_nodeMap.end() ? toNodeIt->second : nullptr;
        
        if (!fromNode) {
            AddValidationError("Connection references non-existent from node: " + conn.fromNodeId);
        }
        
        if (!toNode) {
            AddValidationError("Connection references non-existent to node: " + conn.toNodeId);
        }
        
        if (fromNode && !fromNode->GetOutputPort(conn.fromPortId)) {
            AddValidationError("Connection references non-existent from port: " + conn.fromPortId);
        }
        
        if (toNode && !toNode->GetInputPort(conn.toPortId)) {
            AddValidationError("Connection references non-existent to port: " + conn.toPortId);
        }
    }
}

void BlueprintEditor::ExecuteNode(std::shared_ptr<BlueprintNode> node) {
    if (!node) {
        return;
    }
    
    node->Execute();
    
    // Выполняем связанные узлы
    for (const auto& conn : m_connections) {
        if (conn.fromNodeId == node->GetId()) {
            auto toNode = GetNode(conn.toNodeId);
            if (toNode) {
                ExecuteNode(toNode);
            }
        }
    }
}

std::string BlueprintEditor::GenerateNodeId() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    
    return "node_" + std::to_string(dis(gen));
}

bool BlueprintEditor::IsConnectionValid(const NodeConnection& connection) const {
    auto fromNodeIt = m_nodeMap.find(connection.fromNodeId);
    auto toNodeIt = m_nodeMap.find(connection.toNodeId);
    auto fromNode = fromNodeIt != m_nodeMap.end() ? fromNodeIt->second : nullptr;
    auto toNode = toNodeIt != m_nodeMap.end() ? toNodeIt->second : nullptr;
    
    if (!fromNode || !toNode) {
        return false;
    }
    
    auto fromPort = fromNode->GetOutputPort(connection.fromPortId);
    auto toPort = toNode->GetInputPort(connection.toPortId);
    
    if (!fromPort || !toPort) {
        return false;
    }
    
    // Проверяем совместимость типов данных
    return fromPort->dataType == toPort->dataType;
}

void BlueprintEditor::AddValidationError(const std::string& error) {
    m_validationErrors.push_back(error);
}

void BlueprintEditor::ClearValidationErrors() {
    m_validationErrors.clear();
}

} // namespace FastEngine
