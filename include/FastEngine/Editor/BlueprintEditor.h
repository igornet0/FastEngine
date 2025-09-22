#pragma once

#include "BlueprintNode.h"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <glm/glm.hpp>

namespace FastEngine {

/**
 * Редактор Blueprint
 * Управляет визуальным программированием
 */
class BlueprintEditor {
public:
    BlueprintEditor();
    ~BlueprintEditor();

    /**
     * Инициализация редактора
     * @return true если инициализация успешна
     */
    bool Initialize();

    /**
     * Обновление редактора
     * @param deltaTime время между кадрами
     */
    void Update(float deltaTime);

    /**
     * Рендеринг редактора
     * @param renderer рендерер
     */
    void Render(void* renderer);

    /**
     * Обработка ввода
     * @param mouseX позиция мыши X
     * @param mouseY позиция мыши Y
     * @param mouseButton кнопка мыши
     * @param key клавиша
     * @return true если ввод обработан
     */
    bool HandleInput(float mouseX, float mouseY, int mouseButton, int key);

    /**
     * Добавление узла
     * @param node узел для добавления
     * @return true если узел добавлен
     */
    bool AddNode(std::shared_ptr<BlueprintNode> node);

    /**
     * Удаление узла
     * @param nodeId ID узла
     * @return true если узел удален
     */
    bool RemoveNode(const std::string& nodeId);

    /**
     * Получение узла по ID
     * @param nodeId ID узла
     * @return узел или nullptr
     */
    std::shared_ptr<BlueprintNode> GetNode(const std::string& nodeId);

    /**
     * Получение всех узлов
     */
    const std::vector<std::shared_ptr<BlueprintNode>>& GetNodes() const { return m_nodes; }

    /**
     * Создание соединения между узлами
     * @param connection соединение
     * @return true если соединение создано
     */
    bool CreateConnection(const NodeConnection& connection);

    /**
     * Удаление соединения
     * @param connection соединение
     * @return true если соединение удалено
     */
    bool RemoveConnection(const NodeConnection& connection);

    /**
     * Получение всех соединений
     */
    const std::vector<NodeConnection>& GetConnections() const { return m_connections; }

    /**
     * Выбор узла
     * @param nodeId ID узла
     */
    void SelectNode(const std::string& nodeId);

    /**
     * Отмена выбора
     */
    void DeselectAll();

    /**
     * Получение выбранного узла
     */
    std::shared_ptr<BlueprintNode> GetSelectedNode() const;

    /**
     * Перемещение узла
     * @param nodeId ID узла
     * @param newPosition новая позиция
     */
    void MoveNode(const std::string& nodeId, const glm::vec2& newPosition);

    /**
     * Выполнение Blueprint
     */
    void Execute();

    /**
     * Остановка выполнения
     */
    void Stop();

    /**
     * Пауза/возобновление выполнения
     */
    void Pause();
    void Resume();

    /**
     * Очистка редактора
     */
    void Clear();

    /**
     * Сохранение Blueprint
     * @param filename имя файла
     * @return true если сохранение успешно
     */
    bool Save(const std::string& filename);

    /**
     * Загрузка Blueprint
     * @param filename имя файла
     * @return true если загрузка успешна
     */
    bool Load(const std::string& filename);

    /**
     * Валидация Blueprint
     * @return true если Blueprint валиден
     */
    bool Validate();

    /**
     * Получение ошибок валидации
     */
    const std::vector<std::string>& GetValidationErrors() const { return m_validationErrors; }

    /**
     * Установка callback для обновления UI
     * @param callback функция обратного вызова
     */
    void SetUpdateCallback(std::function<void()> callback) { m_updateCallback = callback; }

    /**
     * Установка callback для выполнения
     * @param callback функция обратного вызова
     */
    void SetExecuteCallback(std::function<void()> callback) { m_executeCallback = callback; }

    /**
     * Получение состояния редактора
     */
    bool IsInitialized() const { return m_initialized; }
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }
    bool IsExecuting() const { return m_executing; }
    bool IsPaused() const { return m_paused; }

private:
    std::vector<std::shared_ptr<BlueprintNode>> m_nodes;
    std::vector<NodeConnection> m_connections;
    std::map<std::string, std::shared_ptr<BlueprintNode>> m_nodeMap;
    
    std::string m_selectedNodeId;
    bool m_initialized;
    bool m_visible;
    bool m_executing;
    bool m_paused;
    
    std::vector<std::string> m_validationErrors;
    
    std::function<void()> m_updateCallback;
    std::function<void()> m_executeCallback;
    
    // Вспомогательные методы
    void RenderNodes(void* renderer);
    void RenderConnections(void* renderer);
    void RenderGrid(void* renderer);
    void RenderUI(void* renderer);
    
    bool HandleNodeInput(std::shared_ptr<BlueprintNode> node, float mouseX, float mouseY, int mouseButton);
    bool HandleConnectionInput(float mouseX, float mouseY, int mouseButton);
    void HandleDragAndDrop(float mouseX, float mouseY);
    
    void UpdateConnections();
    void ValidateConnections();
    void ExecuteNode(std::shared_ptr<BlueprintNode> node);
    
    std::string GenerateNodeId() const;
    bool IsConnectionValid(const NodeConnection& connection) const;
    void AddValidationError(const std::string& error);
    void ClearValidationErrors();
};

} // namespace FastEngine

