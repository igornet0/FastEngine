#include "FastEngine/AI/BehaviorTree.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace FastEngine {

// BehaviorContext implementation
BehaviorContext::BehaviorContext() {}

void BehaviorContext::SetData(const std::string& key, const std::any& value) {
    m_data[key] = value;
}

std::any BehaviorContext::GetData(const std::string& key) const {
    auto it = m_data.find(key);
    return (it != m_data.end()) ? it->second : std::any();
}

bool BehaviorContext::HasData(const std::string& key) const {
    return m_data.find(key) != m_data.end();
}

void BehaviorContext::RemoveData(const std::string& key) {
    m_data.erase(key);
}

void BehaviorContext::SetState(const std::string& key, const std::string& value) {
    m_state[key] = value;
}

std::string BehaviorContext::GetState(const std::string& key) const {
    auto it = m_state.find(key);
    return (it != m_state.end()) ? it->second : "";
}

bool BehaviorContext::HasState(const std::string& key) const {
    return m_state.find(key) != m_state.end();
}

void BehaviorContext::Clear() {
    m_data.clear();
    m_state.clear();
}

// BehaviorNode implementation
BehaviorNode::BehaviorNode() : m_name("BehaviorNode") {}

void BehaviorNode::AddChild(std::shared_ptr<BehaviorNode> child) {
    if (child) {
        m_children.push_back(child);
    }
}

void BehaviorNode::RemoveChild(std::shared_ptr<BehaviorNode> child) {
    if (child) {
        m_children.erase(
            std::remove(m_children.begin(), m_children.end(), child),
            m_children.end()
        );
    }
}

std::string BehaviorNode::Serialize() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"name\": \"" << m_name << "\",\n";
    ss << "  \"type\": \"BehaviorNode\",\n";
    ss << "  \"children\": [\n";
    
    for (size_t i = 0; i < m_children.size(); ++i) {
        ss << "    " << m_children[i]->Serialize();
        if (i < m_children.size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "  ]\n";
    ss << "}";
    return ss.str();
}

void BehaviorNode::Deserialize(const std::string& data) {
    std::cout << "Deserializing behavior node: " << m_name << std::endl;
}

// SequenceNode implementation
SequenceNode::SequenceNode() : m_currentChild(0) {
    m_name = "Sequence";
}

BehaviorStatus SequenceNode::Execute(BehaviorContext& context) {
    for (size_t i = m_currentChild; i < m_children.size(); ++i) {
        BehaviorStatus status = m_children[i]->Execute(context);
        
        if (status == BehaviorStatus::Failure) {
            m_currentChild = 0;
            return BehaviorStatus::Failure;
        }
        
        if (status == BehaviorStatus::Running) {
            m_currentChild = static_cast<int>(i);
            return BehaviorStatus::Running;
        }
    }
    
    m_currentChild = 0;
    return BehaviorStatus::Success;
}

void SequenceNode::Reset() {
    m_currentChild = 0;
    for (auto& child : m_children) {
        child->Reset();
    }
}

// SelectorNode implementation
SelectorNode::SelectorNode() : m_currentChild(0) {
    m_name = "Selector";
}

BehaviorStatus SelectorNode::Execute(BehaviorContext& context) {
    for (size_t i = m_currentChild; i < m_children.size(); ++i) {
        BehaviorStatus status = m_children[i]->Execute(context);
        
        if (status == BehaviorStatus::Success) {
            m_currentChild = 0;
            return BehaviorStatus::Success;
        }
        
        if (status == BehaviorStatus::Running) {
            m_currentChild = static_cast<int>(i);
            return BehaviorStatus::Running;
        }
    }
    
    m_currentChild = 0;
    return BehaviorStatus::Failure;
}

void SelectorNode::Reset() {
    m_currentChild = 0;
    for (auto& child : m_children) {
        child->Reset();
    }
}

// ParallelNode implementation
ParallelNode::ParallelNode() : m_successRequired(1), m_failureRequired(1) {
    m_name = "Parallel";
}

BehaviorStatus ParallelNode::Execute(BehaviorContext& context) {
    if (m_childStatuses.size() != m_children.size()) {
        m_childStatuses.resize(m_children.size(), BehaviorStatus::Running);
    }
    
    int successCount = 0;
    int failureCount = 0;
    
    for (size_t i = 0; i < m_children.size(); ++i) {
        if (m_childStatuses[i] == BehaviorStatus::Running) {
            m_childStatuses[i] = m_children[i]->Execute(context);
        }
        
        if (m_childStatuses[i] == BehaviorStatus::Success) {
            ++successCount;
        } else if (m_childStatuses[i] == BehaviorStatus::Failure) {
            ++failureCount;
        }
    }
    
    if (successCount >= m_successRequired) {
        return BehaviorStatus::Success;
    }
    
    if (failureCount >= m_failureRequired) {
        return BehaviorStatus::Failure;
    }
    
    return BehaviorStatus::Running;
}

void ParallelNode::Reset() {
    m_childStatuses.clear();
    for (auto& child : m_children) {
        child->Reset();
    }
}

// ConditionNode implementation
ConditionNode::ConditionNode() {
    m_name = "Condition";
}

BehaviorStatus ConditionNode::Execute(BehaviorContext& context) {
    if (m_condition) {
        return m_condition(context) ? BehaviorStatus::Success : BehaviorStatus::Failure;
    }
    return BehaviorStatus::Failure;
}

// ActionNode implementation
ActionNode::ActionNode() {
    m_name = "Action";
}

BehaviorStatus ActionNode::Execute(BehaviorContext& context) {
    if (m_action) {
        return m_action(context);
    }
    return BehaviorStatus::Failure;
}

// RepeatNode implementation
RepeatNode::RepeatNode() : m_repeatCount(-1), m_currentCount(0), m_infinite(false) {
    m_name = "Repeat";
}

BehaviorStatus RepeatNode::Execute(BehaviorContext& context) {
    if (m_children.empty()) {
        return BehaviorStatus::Failure;
    }
    
    if (m_infinite || m_currentCount < m_repeatCount) {
        BehaviorStatus status = m_children[0]->Execute(context);
        
        if (status == BehaviorStatus::Success || status == BehaviorStatus::Failure) {
            ++m_currentCount;
            m_children[0]->Reset();
            
            if (m_infinite || m_currentCount < m_repeatCount) {
                return BehaviorStatus::Running;
            }
        }
        
        return status;
    }
    
    return BehaviorStatus::Success;
}

void RepeatNode::Reset() {
    m_currentCount = 0;
    for (auto& child : m_children) {
        child->Reset();
    }
}

// InverterNode implementation
InverterNode::InverterNode() {
    m_name = "Inverter";
}

BehaviorStatus InverterNode::Execute(BehaviorContext& context) {
    if (m_children.empty()) {
        return BehaviorStatus::Failure;
    }
    
    BehaviorStatus status = m_children[0]->Execute(context);
    
    switch (status) {
        case BehaviorStatus::Success:
            return BehaviorStatus::Failure;
        case BehaviorStatus::Failure:
            return BehaviorStatus::Success;
        case BehaviorStatus::Running:
            return BehaviorStatus::Running;
    }
    
    return BehaviorStatus::Failure;
}

// DelayNode implementation
DelayNode::DelayNode() : m_delay(1.0f), m_elapsedTime(0.0f) {
    m_name = "Delay";
}

BehaviorStatus DelayNode::Execute(BehaviorContext& context) {
    m_elapsedTime += 0.016f; // Предполагаем 60 FPS
    
    if (m_elapsedTime >= m_delay) {
        return BehaviorStatus::Success;
    }
    
    return BehaviorStatus::Running;
}

void DelayNode::Reset() {
    m_elapsedTime = 0.0f;
}

// BehaviorTree implementation
BehaviorTree::BehaviorTree() {}

BehaviorStatus BehaviorTree::Execute(BehaviorContext& context) {
    if (!m_root) {
        return BehaviorStatus::Failure;
    }
    
    return m_root->Execute(context);
}

void BehaviorTree::Reset() {
    if (m_root) {
        m_root->Reset();
    }
}

std::string BehaviorTree::Serialize() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"root\": ";
    if (m_root) {
        ss << m_root->Serialize();
    } else {
        ss << "null";
    }
    ss << "\n";
    ss << "}";
    return ss.str();
}

void BehaviorTree::Deserialize(const std::string& data) {
    std::cout << "Deserializing behavior tree..." << std::endl;
}

std::shared_ptr<SequenceNode> BehaviorTree::CreateSequence() {
    return std::make_shared<SequenceNode>();
}

std::shared_ptr<SelectorNode> BehaviorTree::CreateSelector() {
    return std::make_shared<SelectorNode>();
}

std::shared_ptr<ParallelNode> BehaviorTree::CreateParallel() {
    return std::make_shared<ParallelNode>();
}

std::shared_ptr<ConditionNode> BehaviorTree::CreateCondition(std::function<bool(BehaviorContext&)> condition) {
    auto node = std::make_shared<ConditionNode>();
    node->SetCondition(condition);
    return node;
}

std::shared_ptr<ActionNode> BehaviorTree::CreateAction(std::function<BehaviorStatus(BehaviorContext&)> action) {
    auto node = std::make_shared<ActionNode>();
    node->SetAction(action);
    return node;
}

std::shared_ptr<RepeatNode> BehaviorTree::CreateRepeat(int count) {
    auto node = std::make_shared<RepeatNode>();
    node->SetRepeatCount(count);
    return node;
}

std::shared_ptr<InverterNode> BehaviorTree::CreateInverter() {
    return std::make_shared<InverterNode>();
}

std::shared_ptr<DelayNode> BehaviorTree::CreateDelay(float delay) {
    auto node = std::make_shared<DelayNode>();
    node->SetDelay(delay);
    return node;
}

// BehaviorTreeManager implementation
BehaviorTreeManager::BehaviorTreeManager() {}

bool BehaviorTreeManager::Initialize() {
    std::cout << "BehaviorTreeManager initialized successfully" << std::endl;
    return true;
}

void BehaviorTreeManager::Shutdown() {
    m_trees.clear();
    m_activeTrees.clear();
    std::cout << "BehaviorTreeManager shutdown" << std::endl;
}

void BehaviorTreeManager::AddTree(const std::string& name, std::shared_ptr<BehaviorTree> tree) {
    m_trees[name] = tree;
    m_activeTrees.push_back(name);
    std::cout << "Added behavior tree: " << name << std::endl;
}

void BehaviorTreeManager::RemoveTree(const std::string& name) {
    auto it = m_trees.find(name);
    if (it != m_trees.end()) {
        m_trees.erase(it);
        m_activeTrees.erase(
            std::remove(m_activeTrees.begin(), m_activeTrees.end(), name),
            m_activeTrees.end()
        );
        std::cout << "Removed behavior tree: " << name << std::endl;
    }
}

std::shared_ptr<BehaviorTree> BehaviorTreeManager::GetTree(const std::string& name) const {
    auto it = m_trees.find(name);
    return (it != m_trees.end()) ? it->second : nullptr;
}

void BehaviorTreeManager::Update(float deltaTime) {
    // В реальной реализации здесь бы обновлялись все активные деревья
    for (const auto& treeName : m_activeTrees) {
        auto tree = GetTree(treeName);
        if (tree) {
            // Обновляем дерево
            std::cout << "Updating behavior tree: " << treeName << std::endl;
        }
    }
}

} // namespace FastEngine
