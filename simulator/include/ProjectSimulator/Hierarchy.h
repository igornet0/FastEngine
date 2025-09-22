#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <glm/glm.hpp>

// Forward declarations
namespace FastEngine {
    class Entity;
    class World;
}

namespace ProjectSimulator {

/**
 * Узел иерархии
 */
struct HierarchyNode {
    FastEngine::Entity* entity;
    std::string name;
    bool isExpanded;
    bool isSelected;
    int depth;
    std::vector<std::shared_ptr<HierarchyNode>> children;
    std::shared_ptr<HierarchyNode> parent;
    
    HierarchyNode() : entity(nullptr), isExpanded(true), isSelected(false), depth(0) {}
};

/**
 * Иерархия объектов
 * Управляет древовидной структурой объектов сцены
 */
class Hierarchy {
public:
    Hierarchy();
    ~Hierarchy();

    /**
     * Инициализация иерархии
     * @param world мир для управления
     * @return true если инициализация успешна
     */
    bool Initialize(FastEngine::World* world);

    /**
     * Обновление иерархии
     * @param deltaTime время между кадрами
     */
    void Update(float deltaTime);

    /**
     * Рендеринг иерархии
     * @param renderer рендерер
     */
    void Render(void* renderer);

    /**
     * Добавление объекта в иерархию
     * @param entity сущность
     * @param parent родительский узел (nullptr для корня)
     * @return узел иерархии
     */
    std::shared_ptr<HierarchyNode> AddObject(FastEngine::Entity* entity, std::shared_ptr<HierarchyNode> parent = nullptr);

    /**
     * Удаление объекта из иерархии
     * @param entity сущность
     * @return true если объект удален
     */
    bool RemoveObject(FastEngine::Entity* entity);

    /**
     * Поиск узла по сущности
     * @param entity сущность
     * @return узел иерархии
     */
    std::shared_ptr<HierarchyNode> FindNode(FastEngine::Entity* entity);

    /**
     * Выбор узла
     * @param node узел для выбора
     */
    void SelectNode(std::shared_ptr<HierarchyNode> node);

    /**
     * Отмена выбора
     */
    void DeselectAll();

    /**
     * Получение выбранного узла
     */
    std::shared_ptr<HierarchyNode> GetSelectedNode() const { return m_selectedNode; }

    /**
     * Перемещение узла
     * @param node узел для перемещения
     * @param newParent новый родитель
     * @param index индекс в списке детей
     * @return true если перемещение успешно
     */
    bool MoveNode(std::shared_ptr<HierarchyNode> node, std::shared_ptr<HierarchyNode> newParent, int index = -1);

    /**
     * Переименование узла
     * @param node узел для переименования
     * @param newName новое имя
     */
    void RenameNode(std::shared_ptr<HierarchyNode> node, const std::string& newName);

    /**
     * Разворачивание/сворачивание узла
     * @param node узел
     * @param expanded развернут ли узел
     */
    void SetNodeExpanded(std::shared_ptr<HierarchyNode> node, bool expanded);

    /**
     * Получение корневых узлов
     */
    const std::vector<std::shared_ptr<HierarchyNode>>& GetRootNodes() const { return m_rootNodes; }

    /**
     * Очистка иерархии
     */
    void Clear();

    /**
     * Обновление иерархии из мира
     */
    void RefreshFromWorld();

    /**
     * Установка callback для обновления UI
     * @param callback функция обратного вызова
     */
    void SetUpdateCallback(std::function<void()> callback) { m_updateCallback = callback; }

    /**
     * Установка callback для выбора объекта
     * @param callback функция обратного вызова
     */
    void SetSelectionCallback(std::function<void(FastEngine::Entity*)> callback) { m_selectionCallback = callback; }

    /**
     * Получение состояния иерархии
     */
    bool IsInitialized() const { return m_initialized; }
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }

private:
    FastEngine::World* m_world;
    std::vector<std::shared_ptr<HierarchyNode>> m_rootNodes;
    std::shared_ptr<HierarchyNode> m_selectedNode;
    
    bool m_initialized;
    bool m_visible;
    
    std::function<void()> m_updateCallback;
    std::function<void(FastEngine::Entity*)> m_selectionCallback;
    
    // Вспомогательные методы
    void RenderNode(std::shared_ptr<HierarchyNode> node);
    void RenderNodeContent(std::shared_ptr<HierarchyNode> node);
    
    std::string GetEntityName(FastEngine::Entity* entity);
    void SetEntityName(FastEngine::Entity* entity, const std::string& name);
    
    void RemoveNodeFromParent(std::shared_ptr<HierarchyNode> node);
    void AddNodeToParent(std::shared_ptr<HierarchyNode> node, std::shared_ptr<HierarchyNode> parent, int index = -1);
    
    void UpdateNodeDepth(std::shared_ptr<HierarchyNode> node, int depth);
    void SortNodes(std::vector<std::shared_ptr<HierarchyNode>>& nodes);
    
    bool HandleNodeInput(std::shared_ptr<HierarchyNode> node, float mouseX, float mouseY, int mouseButton);
    void HandleDragAndDrop(std::shared_ptr<HierarchyNode> draggedNode, std::shared_ptr<HierarchyNode> targetNode);
};

} // namespace ProjectSimulator

