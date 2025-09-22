#include "FastEngine/Editor/BlueprintNode.h"
#include <random>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace FastEngine {

BlueprintNode::BlueprintNode() 
    : m_type(NodeType::Custom)
    , m_position(0.0f, 0.0f)
    , m_size(200.0f, 100.0f)
    , m_selected(false)
    , m_visible(true) {
    m_id = GenerateId();
}

void BlueprintNode::AddInputPort(const NodePort& port) {
    m_inputPorts.push_back(port);
}

void BlueprintNode::AddOutputPort(const NodePort& port) {
    m_outputPorts.push_back(port);
}

void BlueprintNode::RemoveInputPort(const std::string& portId) {
    auto it = std::find_if(m_inputPorts.begin(), m_inputPorts.end(),
        [&portId](const NodePort& port) { return port.id == portId; });
    
    if (it != m_inputPorts.end()) {
        m_inputPorts.erase(it);
    }
}

void BlueprintNode::RemoveOutputPort(const std::string& portId) {
    auto it = std::find_if(m_outputPorts.begin(), m_outputPorts.end(),
        [&portId](const NodePort& port) { return port.id == portId; });
    
    if (it != m_outputPorts.end()) {
        m_outputPorts.erase(it);
    }
}

NodePort* BlueprintNode::GetInputPort(const std::string& portId) {
    auto it = std::find_if(m_inputPorts.begin(), m_inputPorts.end(),
        [&portId](const NodePort& port) { return port.id == portId; });
    
    return it != m_inputPorts.end() ? &(*it) : nullptr;
}

NodePort* BlueprintNode::GetOutputPort(const std::string& portId) {
    auto it = std::find_if(m_outputPorts.begin(), m_outputPorts.end(),
        [&portId](const NodePort& port) { return port.id == portId; });
    
    return it != m_outputPorts.end() ? &(*it) : nullptr;
}

std::string BlueprintNode::Serialize() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"id\": \"" << m_id << "\",\n";
    ss << "  \"name\": \"" << m_name << "\",\n";
    ss << "  \"title\": \"" << m_title << "\",\n";
    ss << "  \"type\": " << static_cast<int>(m_type) << ",\n";
    ss << "  \"position\": {\"x\": " << m_position.x << ", \"y\": " << m_position.y << "},\n";
    ss << "  \"size\": {\"x\": " << m_size.x << ", \"y\": " << m_size.y << "},\n";
    ss << "  \"selected\": " << (m_selected ? "true" : "false") << ",\n";
    ss << "  \"visible\": " << (m_visible ? "true" : "false") << ",\n";
    
    // Сериализуем входные порты
    ss << "  \"inputPorts\": [\n";
    for (size_t i = 0; i < m_inputPorts.size(); ++i) {
        const auto& port = m_inputPorts[i];
        ss << "    {\n";
        ss << "      \"name\": \"" << port.name << "\",\n";
        ss << "      \"id\": \"" << port.id << "\",\n";
        ss << "      \"type\": " << static_cast<int>(port.type) << ",\n";
        ss << "      \"dataType\": " << static_cast<int>(port.dataType) << ",\n";
        ss << "      \"isConnected\": " << (port.isConnected ? "true" : "false") << ",\n";
        ss << "      \"defaultValue\": \"" << port.defaultValue << "\"\n";
        ss << "    }";
        if (i < m_inputPorts.size() - 1) ss << ",";
        ss << "\n";
    }
    ss << "  ],\n";
    
    // Сериализуем выходные порты
    ss << "  \"outputPorts\": [\n";
    for (size_t i = 0; i < m_outputPorts.size(); ++i) {
        const auto& port = m_outputPorts[i];
        ss << "    {\n";
        ss << "      \"name\": \"" << port.name << "\",\n";
        ss << "      \"id\": \"" << port.id << "\",\n";
        ss << "      \"type\": " << static_cast<int>(port.type) << ",\n";
        ss << "      \"dataType\": " << static_cast<int>(port.dataType) << ",\n";
        ss << "      \"isConnected\": " << (port.isConnected ? "true" : "false") << ",\n";
        ss << "      \"defaultValue\": \"" << port.defaultValue << "\"\n";
        ss << "    }";
        if (i < m_outputPorts.size() - 1) ss << ",";
        ss << "\n";
    }
    ss << "  ]\n";
    ss << "}";
    
    return ss.str();
}

void BlueprintNode::Deserialize(const std::string& data) {
    // В реальной реализации здесь бы парсился JSON
    // Для простоты пропускаем
    std::cout << "Deserializing node: " << m_name << std::endl;
}

std::shared_ptr<BlueprintNode> BlueprintNode::Clone() const {
    auto clone = std::make_shared<BlueprintNode>();
    clone->m_name = m_name;
    clone->m_title = m_title;
    clone->m_type = m_type;
    clone->m_position = m_position;
    clone->m_size = m_size;
    clone->m_selected = false;
    clone->m_visible = m_visible;
    clone->m_inputPorts = m_inputPorts;
    clone->m_outputPorts = m_outputPorts;
    return clone;
}

bool BlueprintNode::IsValid() const {
    // Проверяем, что у узла есть все необходимые порты
    return !m_name.empty() && !m_title.empty();
}

std::string BlueprintNode::GetValidationError() const {
    if (m_name.empty()) {
        return "Node name is empty";
    }
    if (m_title.empty()) {
        return "Node title is empty";
    }
    return "";
}

void BlueprintNode::UpdatePortConnections() {
    // Обновляем состояние подключений портов
    for (auto& port : m_inputPorts) {
        // В реальной реализации здесь бы проверялись подключения
        port.isConnected = false;
    }
    
    for (auto& port : m_outputPorts) {
        // В реальной реализации здесь бы проверялись подключения
        port.isConnected = false;
    }
}

std::string BlueprintNode::GenerateId() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    
    return "node_" + std::to_string(dis(gen));
}

// EventNode implementation
EventNode::EventNode() {
    m_type = NodeType::Event;
    m_name = "Event";
    m_title = "Event";
    
    // Добавляем порт выполнения
    NodePort execPort;
    execPort.name = "Execute";
    execPort.id = "exec";
    execPort.type = PortType::Output;
    execPort.dataType = DataType::Void;
    AddOutputPort(execPort);
}

void EventNode::Execute() {
    std::cout << "Event node executed: " << m_name << std::endl;
}

void EventNode::OnInputChanged(const std::string& portId, const std::string& value) {
    std::cout << "Event node input changed: " << portId << " = " << value << std::endl;
}

std::shared_ptr<BlueprintNode> EventNode::Clone() const {
    auto clone = std::make_shared<EventNode>();
    clone->m_name = m_name;
    clone->m_title = m_title;
    clone->m_position = m_position;
    clone->m_size = m_size;
    clone->m_selected = false;
    clone->m_visible = m_visible;
    clone->m_inputPorts = m_inputPorts;
    clone->m_outputPorts = m_outputPorts;
    return clone;
}

// ActionNode implementation
ActionNode::ActionNode() {
    m_type = NodeType::Action;
    m_name = "Action";
    m_title = "Action";
    
    // Добавляем порты выполнения
    NodePort execInPort;
    execInPort.name = "Execute";
    execInPort.id = "exec_in";
    execInPort.type = PortType::Input;
    execInPort.dataType = DataType::Void;
    AddInputPort(execInPort);
    
    NodePort execOutPort;
    execOutPort.name = "Execute";
    execOutPort.id = "exec_out";
    execOutPort.type = PortType::Output;
    execOutPort.dataType = DataType::Void;
    AddOutputPort(execOutPort);
}

void ActionNode::Execute() {
    std::cout << "Action node executed: " << m_name << std::endl;
}

void ActionNode::OnInputChanged(const std::string& portId, const std::string& value) {
    std::cout << "Action node input changed: " << portId << " = " << value << std::endl;
}

std::shared_ptr<BlueprintNode> ActionNode::Clone() const {
    auto clone = std::make_shared<ActionNode>();
    clone->m_name = m_name;
    clone->m_title = m_title;
    clone->m_position = m_position;
    clone->m_size = m_size;
    clone->m_selected = false;
    clone->m_visible = m_visible;
    clone->m_inputPorts = m_inputPorts;
    clone->m_outputPorts = m_outputPorts;
    return clone;
}

// ConditionNode implementation
ConditionNode::ConditionNode() {
    m_type = NodeType::Condition;
    m_name = "Condition";
    m_title = "Condition";
    
    // Добавляем порты
    NodePort execInPort;
    execInPort.name = "Execute";
    execInPort.id = "exec_in";
    execInPort.type = PortType::Input;
    execInPort.dataType = DataType::Void;
    AddInputPort(execInPort);
    
    NodePort truePort;
    truePort.name = "True";
    truePort.id = "true";
    truePort.type = PortType::Output;
    truePort.dataType = DataType::Void;
    AddOutputPort(truePort);
    
    NodePort falsePort;
    falsePort.name = "False";
    falsePort.id = "false";
    falsePort.type = PortType::Output;
    falsePort.dataType = DataType::Void;
    AddOutputPort(falsePort);
}

void ConditionNode::Execute() {
    std::cout << "Condition node executed: " << m_name << std::endl;
}

void ConditionNode::OnInputChanged(const std::string& portId, const std::string& value) {
    std::cout << "Condition node input changed: " << portId << " = " << value << std::endl;
}

std::shared_ptr<BlueprintNode> ConditionNode::Clone() const {
    auto clone = std::make_shared<ConditionNode>();
    clone->m_name = m_name;
    clone->m_title = m_title;
    clone->m_position = m_position;
    clone->m_size = m_size;
    clone->m_selected = false;
    clone->m_visible = m_visible;
    clone->m_inputPorts = m_inputPorts;
    clone->m_outputPorts = m_outputPorts;
    return clone;
}

// VariableNode implementation
VariableNode::VariableNode() {
    m_type = NodeType::Variable;
    m_name = "Variable";
    m_title = "Variable";
    
    // Добавляем порты
    NodePort getPort;
    getPort.name = "Get";
    getPort.id = "get";
    getPort.type = PortType::Output;
    getPort.dataType = DataType::Void;
    AddOutputPort(getPort);
    
    NodePort setPort;
    setPort.name = "Set";
    setPort.id = "set";
    setPort.type = PortType::Input;
    setPort.dataType = DataType::Void;
    AddInputPort(setPort);
}

void VariableNode::Execute() {
    std::cout << "Variable node executed: " << m_name << std::endl;
}

void VariableNode::OnInputChanged(const std::string& portId, const std::string& value) {
    std::cout << "Variable node input changed: " << portId << " = " << value << std::endl;
}

std::shared_ptr<BlueprintNode> VariableNode::Clone() const {
    auto clone = std::make_shared<VariableNode>();
    clone->m_name = m_name;
    clone->m_title = m_title;
    clone->m_position = m_position;
    clone->m_size = m_size;
    clone->m_selected = false;
    clone->m_visible = m_visible;
    clone->m_inputPorts = m_inputPorts;
    clone->m_outputPorts = m_outputPorts;
    return clone;
}

// FunctionNode implementation
FunctionNode::FunctionNode() {
    m_type = NodeType::Function;
    m_name = "Function";
    m_title = "Function";
    
    // Добавляем порты
    NodePort execInPort;
    execInPort.name = "Execute";
    execInPort.id = "exec_in";
    execInPort.type = PortType::Input;
    execInPort.dataType = DataType::Void;
    AddInputPort(execInPort);
    
    NodePort execOutPort;
    execOutPort.name = "Execute";
    execOutPort.id = "exec_out";
    execOutPort.type = PortType::Output;
    execOutPort.dataType = DataType::Void;
    AddOutputPort(execOutPort);
}

void FunctionNode::Execute() {
    std::cout << "Function node executed: " << m_name << std::endl;
}

void FunctionNode::OnInputChanged(const std::string& portId, const std::string& value) {
    std::cout << "Function node input changed: " << portId << " = " << value << std::endl;
}

std::shared_ptr<BlueprintNode> FunctionNode::Clone() const {
    auto clone = std::make_shared<FunctionNode>();
    clone->m_name = m_name;
    clone->m_title = m_title;
    clone->m_position = m_position;
    clone->m_size = m_size;
    clone->m_selected = false;
    clone->m_visible = m_visible;
    clone->m_inputPorts = m_inputPorts;
    clone->m_outputPorts = m_outputPorts;
    return clone;
}

} // namespace FastEngine
