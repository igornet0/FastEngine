#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <any>

namespace FastEngine {

/**
 * Статус выполнения узла поведения
 */
enum class BehaviorStatus {
    Success,
    Failure,
    Running
};

/**
 * Контекст для узлов поведения
 */
class BehaviorContext {
public:
    BehaviorContext();
    ~BehaviorContext() = default;
    
    // Управление данными
    void SetData(const std::string& key, const std::any& value);
    std::any GetData(const std::string& key) const;
    bool HasData(const std::string& key) const;
    void RemoveData(const std::string& key);
    
    // Управление состоянием
    void SetState(const std::string& key, const std::string& value);
    std::string GetState(const std::string& key) const;
    bool HasState(const std::string& key) const;
    
    // Очистка
    void Clear();
    
private:
    std::unordered_map<std::string, std::any> m_data;
    std::unordered_map<std::string, std::string> m_state;
};

/**
 * Базовый узел поведения
 */
class BehaviorNode {
public:
    BehaviorNode();
    virtual ~BehaviorNode() = default;
    
    // Выполнение узла
    virtual BehaviorStatus Execute(BehaviorContext& context) = 0;
    virtual void Reset() {}
    
    // Управление дочерними узлами
    void AddChild(std::shared_ptr<BehaviorNode> child);
    void RemoveChild(std::shared_ptr<BehaviorNode> child);
    const std::vector<std::shared_ptr<BehaviorNode>>& GetChildren() const { return m_children; }
    
    // Свойства узла
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    // Сериализация
    virtual std::string Serialize() const;
    virtual void Deserialize(const std::string& data);
    
protected:
    std::string m_name;
    std::vector<std::shared_ptr<BehaviorNode>> m_children;
};

/**
 * Узел последовательности (выполняет дочерние узлы по порядку)
 */
class SequenceNode : public BehaviorNode {
public:
    SequenceNode();
    ~SequenceNode() = default;
    
    BehaviorStatus Execute(BehaviorContext& context) override;
    void Reset() override;
    
private:
    int m_currentChild;
};

/**
 * Узел выбора (выполняет дочерние узлы до первого успешного)
 */
class SelectorNode : public BehaviorNode {
public:
    SelectorNode();
    ~SelectorNode() = default;
    
    BehaviorStatus Execute(BehaviorContext& context) override;
    void Reset() override;
    
private:
    int m_currentChild;
};

/**
 * Узел параллельного выполнения
 */
class ParallelNode : public BehaviorNode {
public:
    ParallelNode();
    ~ParallelNode() = default;
    
    BehaviorStatus Execute(BehaviorContext& context) override;
    void Reset() override;
    
    // Настройки параллельного выполнения
    void SetSuccessPolicy(int required) { m_successRequired = required; }
    void SetFailurePolicy(int required) { m_failureRequired = required; }
    
private:
    int m_successRequired;
    int m_failureRequired;
    std::vector<BehaviorStatus> m_childStatuses;
};

/**
 * Узел условия
 */
class ConditionNode : public BehaviorNode {
public:
    ConditionNode();
    ~ConditionNode() = default;
    
    BehaviorStatus Execute(BehaviorContext& context) override;
    
    // Настройка условия
    void SetCondition(std::function<bool(BehaviorContext&)> condition) { m_condition = condition; }
    
private:
    std::function<bool(BehaviorContext&)> m_condition;
};

/**
 * Узел действия
 */
class ActionNode : public BehaviorNode {
public:
    ActionNode();
    ~ActionNode() = default;
    
    BehaviorStatus Execute(BehaviorContext& context) override;
    
    // Настройка действия
    void SetAction(std::function<BehaviorStatus(BehaviorContext&)> action) { m_action = action; }
    
private:
    std::function<BehaviorStatus(BehaviorContext&)> m_action;
};

/**
 * Узел повтора
 */
class RepeatNode : public BehaviorNode {
public:
    RepeatNode();
    ~RepeatNode() = default;
    
    BehaviorStatus Execute(BehaviorContext& context) override;
    void Reset() override;
    
    // Настройки повтора
    void SetRepeatCount(int count) { m_repeatCount = count; }
    void SetInfinite(bool infinite) { m_infinite = infinite; }
    
private:
    int m_repeatCount;
    int m_currentCount;
    bool m_infinite;
};

/**
 * Узел инвертора
 */
class InverterNode : public BehaviorNode {
public:
    InverterNode();
    ~InverterNode() = default;
    
    BehaviorStatus Execute(BehaviorContext& context) override;
};

/**
 * Узел задержки
 */
class DelayNode : public BehaviorNode {
public:
    DelayNode();
    ~DelayNode() = default;
    
    BehaviorStatus Execute(BehaviorContext& context) override;
    void Reset() override;
    
    // Настройки задержки
    void SetDelay(float delay) { m_delay = delay; }
    
private:
    float m_delay;
    float m_elapsedTime;
};

/**
 * Дерево поведения
 */
class BehaviorTree {
public:
    BehaviorTree();
    ~BehaviorTree() = default;
    
    // Управление деревом
    void SetRoot(std::shared_ptr<BehaviorNode> root) { m_root = root; }
    std::shared_ptr<BehaviorNode> GetRoot() const { return m_root; }
    
    // Выполнение
    BehaviorStatus Execute(BehaviorContext& context);
    void Reset();
    
    // Сериализация
    std::string Serialize() const;
    void Deserialize(const std::string& data);
    
    // Создание узлов
    std::shared_ptr<SequenceNode> CreateSequence();
    std::shared_ptr<SelectorNode> CreateSelector();
    std::shared_ptr<ParallelNode> CreateParallel();
    std::shared_ptr<ConditionNode> CreateCondition(std::function<bool(BehaviorContext&)> condition);
    std::shared_ptr<ActionNode> CreateAction(std::function<BehaviorStatus(BehaviorContext&)> action);
    std::shared_ptr<RepeatNode> CreateRepeat(int count = -1);
    std::shared_ptr<InverterNode> CreateInverter();
    std::shared_ptr<DelayNode> CreateDelay(float delay);
    
private:
    std::shared_ptr<BehaviorNode> m_root;
    BehaviorContext m_context;
};

/**
 * Менеджер деревьев поведения
 */
class BehaviorTreeManager {
public:
    BehaviorTreeManager();
    ~BehaviorTreeManager() = default;
    
    // Инициализация
    bool Initialize();
    void Shutdown();
    
    // Управление деревьями
    void AddTree(const std::string& name, std::shared_ptr<BehaviorTree> tree);
    void RemoveTree(const std::string& name);
    std::shared_ptr<BehaviorTree> GetTree(const std::string& name) const;
    
    // Выполнение
    void Update(float deltaTime);
    
    // Статистика
    int GetActiveTrees() const { return m_activeTrees.size(); }
    
private:
    std::unordered_map<std::string, std::shared_ptr<BehaviorTree>> m_trees;
    std::vector<std::string> m_activeTrees;
};

} // namespace FastEngine

