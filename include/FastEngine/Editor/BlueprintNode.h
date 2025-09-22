#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

namespace FastEngine {

/**
 * Типы узлов Blueprint
 */
enum class NodeType {
    Event,
    Action,
    Condition,
    Variable,
    Function,
    Comment,
    Custom
};

/**
 * Типы портов
 */
enum class PortType {
    Input,
    Output,
    Execution
};

/**
 * Типы данных портов
 */
enum class DataType {
    Void,
    Boolean,
    Integer,
    Float,
    String,
    Vector2,
    Vector3,
    Color,
    Object,
    Custom
};

/**
 * Порт узла
 */
struct NodePort {
    std::string name;
    std::string id;
    PortType type;
    DataType dataType;
    bool isConnected;
    std::string defaultValue;
    
    NodePort() : type(PortType::Input), dataType(DataType::Void), isConnected(false) {}
};

/**
 * Соединение между узлами
 */
struct NodeConnection {
    std::string fromNodeId;
    std::string fromPortId;
    std::string toNodeId;
    std::string toPortId;
    
    NodeConnection() = default;
    NodeConnection(const std::string& fromNode, const std::string& fromPort, 
                   const std::string& toNode, const std::string& toPort)
        : fromNodeId(fromNode), fromPortId(fromPort), toNodeId(toNode), toPortId(toPort) {}
    
    bool operator==(const NodeConnection& other) const {
        return fromNodeId == other.fromNodeId && 
               fromPortId == other.fromPortId &&
               toNodeId == other.toNodeId && 
               toPortId == other.toPortId;
    }
};

/**
 * Узел Blueprint
 */
class BlueprintNode {
public:
    BlueprintNode();
    virtual ~BlueprintNode() = default;
    
    // Основные свойства
    std::string GetId() const { return m_id; }
    void SetId(const std::string& id) { m_id = id; }
    
    std::string GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    std::string GetTitle() const { return m_title; }
    void SetTitle(const std::string& title) { m_title = title; }
    
    NodeType GetType() const { return m_type; }
    void SetType(NodeType type) { m_type = type; }
    
    glm::vec2 GetPosition() const { return m_position; }
    void SetPosition(const glm::vec2& position) { m_position = position; }
    
    glm::vec2 GetSize() const { return m_size; }
    void SetSize(const glm::vec2& size) { m_size = size; }
    
    bool IsSelected() const { return m_selected; }
    void SetSelected(bool selected) { m_selected = selected; }
    
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }
    
    // Порты
    const std::vector<NodePort>& GetInputPorts() const { return m_inputPorts; }
    const std::vector<NodePort>& GetOutputPorts() const { return m_outputPorts; }
    
    void AddInputPort(const NodePort& port);
    void AddOutputPort(const NodePort& port);
    void RemoveInputPort(const std::string& portId);
    void RemoveOutputPort(const std::string& portId);
    
    NodePort* GetInputPort(const std::string& portId);
    NodePort* GetOutputPort(const std::string& portId);
    
    // Выполнение
    virtual void Execute() {}
    virtual void Update(float deltaTime) {}
    virtual void OnInputChanged(const std::string& portId, const std::string& value) {}
    
    // Сериализация
    virtual std::string Serialize() const;
    virtual void Deserialize(const std::string& data);
    
    // Клонирование
    virtual std::shared_ptr<BlueprintNode> Clone() const;
    
    // Валидация
    virtual bool IsValid() const;
    virtual std::string GetValidationError() const;
    
    // UI
    virtual void Render(void* renderer) {}
    virtual void RenderPorts(void* renderer) {}
    virtual void RenderConnections(void* renderer) {}
    
    // События
    virtual void OnNodeCreated() {}
    virtual void OnNodeDestroyed() {}
    virtual void OnNodeSelected() {}
    virtual void OnNodeDeselected() {}
    virtual void OnNodeMoved(const glm::vec2& newPosition) {}
    
protected:
    std::string m_id;
    std::string m_name;
    std::string m_title;
    NodeType m_type;
    glm::vec2 m_position;
    glm::vec2 m_size;
    bool m_selected;
    bool m_visible;
    
    std::vector<NodePort> m_inputPorts;
    std::vector<NodePort> m_outputPorts;
    
    // Вспомогательные методы
    std::string GenerateId() const;
    void UpdatePortConnections();
};

/**
 * Специализированные узлы
 */
class EventNode : public BlueprintNode {
public:
    EventNode();
    virtual ~EventNode() = default;
    
    void Execute() override;
    void OnInputChanged(const std::string& portId, const std::string& value) override;
    
    std::shared_ptr<BlueprintNode> Clone() const override;
};

class ActionNode : public BlueprintNode {
public:
    ActionNode();
    virtual ~ActionNode() = default;
    
    void Execute() override;
    void OnInputChanged(const std::string& portId, const std::string& value) override;
    
    std::shared_ptr<BlueprintNode> Clone() const override;
};

class ConditionNode : public BlueprintNode {
public:
    ConditionNode();
    virtual ~ConditionNode() = default;
    
    void Execute() override;
    void OnInputChanged(const std::string& portId, const std::string& value) override;
    
    std::shared_ptr<BlueprintNode> Clone() const override;
};

class VariableNode : public BlueprintNode {
public:
    VariableNode();
    virtual ~VariableNode() = default;
    
    void Execute() override;
    void OnInputChanged(const std::string& portId, const std::string& value) override;
    
    std::shared_ptr<BlueprintNode> Clone() const override;
};

class FunctionNode : public BlueprintNode {
public:
    FunctionNode();
    virtual ~FunctionNode() = default;
    
    void Execute() override;
    void OnInputChanged(const std::string& portId, const std::string& value) override;
    
    std::shared_ptr<BlueprintNode> Clone() const override;
};

} // namespace FastEngine
