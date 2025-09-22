#include "ProjectSimulator/Hierarchy.h"
#include "FastEngine/World.h"
#include "FastEngine/Entity.h"
#include "FastEngine/Components/Transform.h"
#include <iostream>
#include <algorithm>

namespace ProjectSimulator {

Hierarchy::Hierarchy() 
    : m_world(nullptr)
    , m_initialized(false)
    , m_visible(true) {
}

Hierarchy::~Hierarchy() {
    if (m_initialized) {
        Clear();
    }
}

bool Hierarchy::Initialize(FastEngine::World* world) {
    if (m_initialized) {
        return true;
    }
    
    if (!world) {
        std::cerr << "Hierarchy: Invalid world" << std::endl;
        return false;
    }
    
    m_world = world;
    m_initialized = true;
    
    // Загружаем существующие объекты
    RefreshFromWorld();
    
    std::cout << "Hierarchy initialized successfully" << std::endl;
    return true;
}

void Hierarchy::Update(float deltaTime) {
    if (!m_initialized || !m_visible) {
        return;
    }
    
    // Обновляем иерархию из мира
    RefreshFromWorld();
    
    // Вызываем callback для обновления UI
    if (m_updateCallback) {
        m_updateCallback();
    }
}

void Hierarchy::Render(void* renderer) {
    if (!m_initialized || !m_visible) {
        return;
    }
    
    // В реальной реализации здесь бы рендерился UI иерархии
    // Для простоты выводим информацию в консоль
    std::cout << "=== Hierarchy ===" << std::endl;
    
    for (const auto& rootNode : m_rootNodes) {
        RenderNode(rootNode);
    }
    
    std::cout << "================" << std::endl;
}

std::shared_ptr<HierarchyNode> Hierarchy::AddObject(FastEngine::Entity* entity, std::shared_ptr<HierarchyNode> parent) {
    if (!entity) {
        return nullptr;
    }
    
    auto node = std::make_shared<HierarchyNode>();
    node->entity = entity;
    node->name = GetEntityName(entity);
    node->isExpanded = true;
    node->isSelected = false;
    node->parent = parent;
    
    if (parent) {
        node->depth = parent->depth + 1;
        parent->children.push_back(node);
    } else {
        node->depth = 0;
        m_rootNodes.push_back(node);
    }
    
    std::cout << "Added object to hierarchy: " << node->name << std::endl;
    return node;
}

bool Hierarchy::RemoveObject(FastEngine::Entity* entity) {
    if (!entity) {
        return false;
    }
    
    auto node = FindNode(entity);
    if (!node) {
        return false;
    }
    
    // Удаляем из родителя
    RemoveNodeFromParent(node);
    
    // Если это выбранный узел, отменяем выбор
    if (m_selectedNode == node) {
        DeselectAll();
    }
    
    std::cout << "Removed object from hierarchy: " << node->name << std::endl;
    return true;
}

std::shared_ptr<HierarchyNode> Hierarchy::FindNode(FastEngine::Entity* entity) {
    if (!entity) {
        return nullptr;
    }
    
    // Рекурсивный поиск по всем узлам
    std::function<std::shared_ptr<HierarchyNode>(const std::vector<std::shared_ptr<HierarchyNode>>&)> search;
    search = [&](const std::vector<std::shared_ptr<HierarchyNode>>& nodes) -> std::shared_ptr<HierarchyNode> {
        for (const auto& node : nodes) {
            if (node->entity == entity) {
                return node;
            }
            
            auto found = search(node->children);
            if (found) {
                return found;
            }
        }
        return nullptr;
    };
    
    return search(m_rootNodes);
}

void Hierarchy::SelectNode(std::shared_ptr<HierarchyNode> node) {
    // Отменяем предыдущий выбор
    DeselectAll();
    
    if (node) {
        node->isSelected = true;
        m_selectedNode = node;
        
        // Вызываем callback для выбора объекта
        if (m_selectionCallback) {
            m_selectionCallback(node->entity);
        }
        
        std::cout << "Selected node: " << node->name << std::endl;
    }
}

void Hierarchy::DeselectAll() {
    if (m_selectedNode) {
        m_selectedNode->isSelected = false;
        m_selectedNode = nullptr;
        std::cout << "Deselected all nodes" << std::endl;
    }
}

bool Hierarchy::MoveNode(std::shared_ptr<HierarchyNode> node, std::shared_ptr<HierarchyNode> newParent, int index) {
    if (!node) {
        return false;
    }
    
    // Удаляем из текущего родителя
    RemoveNodeFromParent(node);
    
    // Добавляем к новому родителю
    if (newParent) {
        node->parent = newParent;
        node->depth = newParent->depth + 1;
        UpdateNodeDepth(node, node->depth);
        
        if (index >= 0 && index < static_cast<int>(newParent->children.size())) {
            newParent->children.insert(newParent->children.begin() + index, node);
        } else {
            newParent->children.push_back(node);
        }
    } else {
        node->parent = nullptr;
        node->depth = 0;
        UpdateNodeDepth(node, 0);
        
        if (index >= 0 && index < static_cast<int>(m_rootNodes.size())) {
            m_rootNodes.insert(m_rootNodes.begin() + index, node);
        } else {
            m_rootNodes.push_back(node);
        }
    }
    
    std::cout << "Moved node: " << node->name << std::endl;
    return true;
}

void Hierarchy::RenameNode(std::shared_ptr<HierarchyNode> node, const std::string& newName) {
    if (!node || newName.empty()) {
        return;
    }
    
    node->name = newName;
    SetEntityName(node->entity, newName);
    
    std::cout << "Renamed node to: " << newName << std::endl;
}

void Hierarchy::SetNodeExpanded(std::shared_ptr<HierarchyNode> node, bool expanded) {
    if (!node) {
        return;
    }
    
    node->isExpanded = expanded;
    std::cout << "Set node expanded: " << node->name << " = " << expanded << std::endl;
}

void Hierarchy::Clear() {
    m_rootNodes.clear();
    m_selectedNode = nullptr;
    std::cout << "Hierarchy cleared" << std::endl;
}

void Hierarchy::RefreshFromWorld() {
    if (!m_world) {
        return;
    }
    
    // В реальной реализации здесь бы синхронизировались объекты с миром
    // Для простоты пропускаем
}

void Hierarchy::RenderNode(std::shared_ptr<HierarchyNode> node) {
    if (!node) {
        return;
    }
    
    // Отступы для глубины
    std::string indent(node->depth * 2, ' ');
    
    // Иконка развернутости
    std::string expandIcon = node->children.empty() ? "  " : (node->isExpanded ? "▼ " : "▶ ");
    
    // Иконка выбора
    std::string selectIcon = node->isSelected ? "● " : "○ ";
    
    std::cout << indent << expandIcon << selectIcon << node->name << std::endl;
    
    // Рендерим дочерние узлы
    if (node->isExpanded) {
        for (const auto& child : node->children) {
            RenderNode(child);
        }
    }
}

void Hierarchy::RenderNodeContent(std::shared_ptr<HierarchyNode> node) {
    // В реальной реализации здесь бы рендерилось содержимое узла
    std::cout << "Rendering node content: " << node->name << std::endl;
}

std::string Hierarchy::GetEntityName(FastEngine::Entity* entity) {
    if (!entity) {
        return "Unknown";
    }
    
    // В реальной реализации здесь бы получалось имя сущности
    // Для простоты используем адрес
    std::stringstream ss;
    ss << "Entity_" << std::hex << reinterpret_cast<uintptr_t>(entity);
    return ss.str();
}

void Hierarchy::SetEntityName(FastEngine::Entity* entity, const std::string& name) {
    if (!entity) {
        return;
    }
    
    // В реальной реализации здесь бы устанавливалось имя сущности
    std::cout << "Set entity name: " << name << std::endl;
}

void Hierarchy::RemoveNodeFromParent(std::shared_ptr<HierarchyNode> node) {
    if (!node) {
        return;
    }
    
    if (node->parent) {
        auto& children = node->parent->children;
        auto it = std::find(children.begin(), children.end(), node);
        if (it != children.end()) {
            children.erase(it);
        }
    } else {
        auto it = std::find(m_rootNodes.begin(), m_rootNodes.end(), node);
        if (it != m_rootNodes.end()) {
            m_rootNodes.erase(it);
        }
    }
    
    node->parent = nullptr;
}

void Hierarchy::AddNodeToParent(std::shared_ptr<HierarchyNode> node, std::shared_ptr<HierarchyNode> parent, int index) {
    if (!node) {
        return;
    }
    
    if (parent) {
        node->parent = parent;
        node->depth = parent->depth + 1;
        
        if (index >= 0 && index < static_cast<int>(parent->children.size())) {
            parent->children.insert(parent->children.begin() + index, node);
        } else {
            parent->children.push_back(node);
        }
    } else {
        node->parent = nullptr;
        node->depth = 0;
        
        if (index >= 0 && index < static_cast<int>(m_rootNodes.size())) {
            m_rootNodes.insert(m_rootNodes.begin() + index, node);
        } else {
            m_rootNodes.push_back(node);
        }
    }
}

void Hierarchy::UpdateNodeDepth(std::shared_ptr<HierarchyNode> node, int depth) {
    if (!node) {
        return;
    }
    
    node->depth = depth;
    
    for (auto& child : node->children) {
        UpdateNodeDepth(child, depth + 1);
    }
}

void Hierarchy::SortNodes(std::vector<std::shared_ptr<HierarchyNode>>& nodes) {
    std::sort(nodes.begin(), nodes.end(), [](const std::shared_ptr<HierarchyNode>& a, const std::shared_ptr<HierarchyNode>& b) {
        return a->name < b->name;
    });
}

bool Hierarchy::HandleNodeInput(std::shared_ptr<HierarchyNode> node, float mouseX, float mouseY, int mouseButton) {
    if (!node) {
        return false;
    }
    
    // В реальной реализации здесь бы обрабатывался ввод для узла
    return false;
}

void Hierarchy::HandleDragAndDrop(std::shared_ptr<HierarchyNode> draggedNode, std::shared_ptr<HierarchyNode> targetNode) {
    if (!draggedNode) {
        return;
    }
    
    // Перемещаем узел
    MoveNode(draggedNode, targetNode);
    
    std::cout << "Drag and drop: " << draggedNode->name << " to " 
              << (targetNode ? targetNode->name : "root") << std::endl;
}

} // namespace ProjectSimulator

