#include "FastEngine/Editor/MaterialEditor.h"
#include <iostream>
#include <random>
#include <sstream>

namespace FastEngine {

MaterialNode::MaterialNode() 
    : m_type(MaterialNodeType::Custom)
    , m_position(0.0f, 0.0f) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    
    m_id = "node_" + std::to_string(dis(gen));
}

std::string MaterialNode::Serialize() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"id\": \"" << m_id << "\",\n";
    ss << "  \"name\": \"" << m_name << "\",\n";
    ss << "  \"type\": " << static_cast<int>(m_type) << ",\n";
    ss << "  \"position\": {\"x\": " << m_position.x << ", \"y\": " << m_position.y << "},\n";
    ss << "  \"shaderCode\": \"" << m_shaderCode << "\"\n";
    ss << "}";
    return ss.str();
}

void MaterialNode::Deserialize(const std::string& data) {
    std::cout << "Deserializing material node: " << m_name << std::endl;
}

// MaterialEditor implementation
MaterialEditor::MaterialEditor() : m_initialized(false) {}

MaterialEditor::~MaterialEditor() {
    if (m_initialized) {
        // Очистка ресурсов
    }
}

bool MaterialEditor::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    m_initialized = true;
    
    std::cout << "MaterialEditor initialized successfully" << std::endl;
    return true;
}

void MaterialEditor::Update(float deltaTime) {
    if (!m_initialized) {
        return;
    }
    
    if (m_updateCallback) {
        m_updateCallback();
    }
}

void MaterialEditor::Render(void* renderer) {
    if (!m_initialized) {
        return;
    }
    
    std::cout << "=== Material Editor ===" << std::endl;
    std::cout << "Nodes: " << m_nodes.size() << std::endl;
    
    RenderNodes(renderer);
    RenderPreview(renderer);
    
    std::cout << "=====================" << std::endl;
}

bool MaterialEditor::HandleInput(float mouseX, float mouseY, int mouseButton, int key) {
    if (!m_initialized) {
        return false;
    }
    
    switch (key) {
        case 'N':
            // Создаем новую ноду
            {
                auto node = CreateNode(MaterialNodeType::Texture, glm::vec2(mouseX, mouseY));
                if (node) {
                    std::cout << "Created new material node" << std::endl;
                }
            }
            return true;
        case 'C':
            // Компилируем шейдер
            {
                std::string shaderCode = GenerateShader();
                CompileShader(shaderCode);
            }
            return true;
    }
    
    return false;
}

std::shared_ptr<MaterialNode> MaterialEditor::CreateNode(MaterialNodeType type, const glm::vec2& position) {
    auto node = std::make_shared<MaterialNode>();
    node->SetType(type);
    node->SetPosition(position);
    
    // Генерируем код шейдера в зависимости от типа ноды
    switch (type) {
        case MaterialNodeType::Texture:
            node->SetName("Texture");
            node->SetShaderCode("texture2D(u_texture, v_texCoord)");
            break;
        case MaterialNodeType::Color:
            node->SetName("Color");
            node->SetShaderCode("vec4(1.0, 0.0, 0.0, 1.0)");
            break;
        case MaterialNodeType::Math:
            node->SetName("Math");
            node->SetShaderCode("a + b");
            break;
        case MaterialNodeType::Mix:
            node->SetName("Mix");
            node->SetShaderCode("mix(a, b, t)");
            break;
        default:
            node->SetName("Custom");
            node->SetShaderCode("vec4(1.0)");
            break;
    }
    
    m_nodes[node->GetId()] = node;
    
    std::cout << "Created material node: " << node->GetName() << " (ID: " << node->GetId() << ")" << std::endl;
    return node;
}

bool MaterialEditor::RemoveNode(const std::string& nodeId) {
    auto it = m_nodes.find(nodeId);
    if (it == m_nodes.end()) {
        return false;
    }
    
    m_nodes.erase(it);
    
    std::cout << "Removed material node: " << nodeId << std::endl;
    return true;
}

std::shared_ptr<MaterialNode> MaterialEditor::GetNode(const std::string& nodeId) {
    auto it = m_nodes.find(nodeId);
    return it != m_nodes.end() ? it->second : nullptr;
}

std::string MaterialEditor::GenerateShader() const {
    std::stringstream ss;
    ss << "#version 330 core\n";
    ss << "in vec2 v_texCoord;\n";
    ss << "out vec4 FragColor;\n";
    ss << "uniform sampler2D u_texture;\n\n";
    
    ss << "void main() {\n";
    ss << "    // Generated material shader\n";
    
    // Генерируем код на основе нод
    for (const auto& pair : m_nodes) {
        const auto& node = pair.second;
        if (node && node->GetType() == MaterialNodeType::Output) {
            ss << "    FragColor = " << node->GetShaderCode() << ";\n";
            break;
        }
    }
    
    ss << "}\n";
    
    std::string shaderCode = ss.str();
    std::cout << "Generated shader code:\n" << shaderCode << std::endl;
    return shaderCode;
}

bool MaterialEditor::CompileShader(const std::string& shaderCode) {
    // В реальной реализации здесь бы компилировался шейдер
    std::cout << "Compiling shader..." << std::endl;
    std::cout << "Shader compilation: " << (shaderCode.empty() ? "Failed" : "Success") << std::endl;
    return !shaderCode.empty();
}

void MaterialEditor::RenderNodes(void* renderer) {
    for (const auto& pair : m_nodes) {
        const auto& node = pair.second;
        if (node) {
            std::cout << "  Node: " << node->GetName() << " at (" 
                      << node->GetPosition().x << ", " << node->GetPosition().y << ")" << std::endl;
        }
    }
}

void MaterialEditor::RenderPreview(void* renderer) {
    std::cout << "Rendering material preview" << std::endl;
}

} // namespace FastEngine
