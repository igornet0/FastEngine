#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

namespace FastEngine {

/**
 * Типы нод материалов
 */
enum class MaterialNodeType {
    Input,
    Output,
    Texture,
    Color,
    Math,
    Mix,
    Custom
};

/**
 * Нода материала
 */
class MaterialNode {
public:
    MaterialNode();
    virtual ~MaterialNode() = default;
    
    std::string GetId() const { return m_id; }
    void SetId(const std::string& id) { m_id = id; }
    
    std::string GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    MaterialNodeType GetType() const { return m_type; }
    void SetType(MaterialNodeType type) { m_type = type; }
    
    glm::vec2 GetPosition() const { return m_position; }
    void SetPosition(const glm::vec2& position) { m_position = position; }
    
    std::string GetShaderCode() const { return m_shaderCode; }
    void SetShaderCode(const std::string& code) { m_shaderCode = code; }
    
    std::string Serialize() const;
    void Deserialize(const std::string& data);
    
protected:
    std::string m_id;
    std::string m_name;
    MaterialNodeType m_type;
    glm::vec2 m_position;
    std::string m_shaderCode;
};

/**
 * Редактор материалов
 */
class MaterialEditor {
public:
    MaterialEditor();
    ~MaterialEditor();

    bool Initialize();
    void Update(float deltaTime);
    void Render(void* renderer);
    bool HandleInput(float mouseX, float mouseY, int mouseButton, int key);
    
    std::shared_ptr<MaterialNode> CreateNode(MaterialNodeType type, const glm::vec2& position);
    bool RemoveNode(const std::string& nodeId);
    std::shared_ptr<MaterialNode> GetNode(const std::string& nodeId);
    
    std::string GenerateShader() const;
    bool CompileShader(const std::string& shaderCode);
    
    void SetUpdateCallback(std::function<void()> callback) { m_updateCallback = callback; }
    
private:
    std::map<std::string, std::shared_ptr<MaterialNode>> m_nodes;
    bool m_initialized;
    
    std::function<void()> m_updateCallback;
    
    void RenderNodes(void* renderer);
    void RenderPreview(void* renderer);
};

} // namespace FastEngine

