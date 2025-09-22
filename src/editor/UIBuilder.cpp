#include "FastEngine/Editor/UIBuilder.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <sstream>

namespace FastEngine {

UIBuilder::UIBuilder() 
    : m_mode(UIBuilderMode::Design)
    , m_initialized(false)
    , m_visible(true) {
}

UIBuilder::~UIBuilder() {
    if (m_initialized) {
        Clear();
    }
}

bool UIBuilder::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    m_initialized = true;
    
    std::cout << "UIBuilder initialized successfully" << std::endl;
    return true;
}

void UIBuilder::Update(float deltaTime) {
    if (!m_initialized || !m_visible) {
        return;
    }
    
    // Обновляем UI элементы
    for (auto& element : m_elements) {
        if (element) {
            element->Update(deltaTime);
        }
    }
    
    // Обновляем layout
    UpdateLayout();
    
    // Вызываем callback для обновления UI
    if (m_updateCallback) {
        m_updateCallback();
    }
}

void UIBuilder::Render(void* renderer) {
    if (!m_initialized || !m_visible) {
        return;
    }
    
    // В реальной реализации здесь бы рендерился UI редактора
    std::cout << "=== UI Builder ===" << std::endl;
    std::cout << "Mode: " << (m_mode == UIBuilderMode::Design ? "Design" : 
                              m_mode == UIBuilderMode::Preview ? "Preview" : "Code") << std::endl;
    std::cout << "Elements: " << m_elements.size() << std::endl;
    
    // Рендерим элементы
    RenderElements(renderer);
    
    // Рендерим UI
    RenderUI(renderer);
    
    std::cout << "==================" << std::endl;
}

bool UIBuilder::HandleInput(float mouseX, float mouseY, int mouseButton, int key) {
    if (!m_initialized || !m_visible) {
        return false;
    }
    
    // Обработка клавиатуры
    switch (key) {
        case 'D':
            SetMode(UIBuilderMode::Design);
            return true;
        case 'P':
            SetMode(UIBuilderMode::Preview);
            return true;
        case 'C':
            SetMode(UIBuilderMode::Code);
            return true;
        case 'N':
            // Создаем новый элемент
            {
                auto panel = CreateElement(UIElementType::Panel, glm::vec2(mouseX, mouseY));
                if (panel) {
                    std::cout << "Created new panel" << std::endl;
                }
            }
            return true;
    }
    
    // Обрабатываем ввод элементов
    for (auto& element : m_elements) {
        if (element && HandleElementInput(element, mouseX, mouseY, mouseButton)) {
            return true;
        }
    }
    
    return false;
}

std::shared_ptr<UIElement> UIBuilder::CreateElement(UIElementType type, const glm::vec2& position) {
    std::shared_ptr<UIElement> element;
    
    switch (type) {
        case UIElementType::Panel:
            element = std::make_shared<UIPanel>();
            break;
        case UIElementType::Button:
            element = std::make_shared<UIButton>();
            break;
        case UIElementType::Label:
            element = std::make_shared<UILabel>();
            break;
        case UIElementType::TextBox:
            element = std::make_shared<UITextBox>();
            break;
        case UIElementType::Image:
            element = std::make_shared<UIImage>();
            break;
        default:
            element = std::make_shared<UIElement>();
            break;
    }
    
    if (element) {
        element->SetPosition(position);
        element->SetId(GenerateElementId());
        
        m_elements.push_back(element);
        m_elementMap[element->GetId()] = element;
        
        std::cout << "Created UI element: " << element->GetName() << " (ID: " << element->GetId() << ")" << std::endl;
    }
    
    return element;
}

bool UIBuilder::RemoveElement(const std::string& elementId) {
    auto it = m_elementMap.find(elementId);
    if (it == m_elementMap.end()) {
        return false;
    }
    
    // Удаляем из списка
    m_elements.erase(
        std::remove_if(m_elements.begin(), m_elements.end(),
            [&elementId](const std::shared_ptr<UIElement>& element) {
                return element && element->GetId() == elementId;
            }),
        m_elements.end()
    );
    
    // Удаляем из карты
    m_elementMap.erase(it);
    
    std::cout << "Removed UI element: " << elementId << std::endl;
    return true;
}

std::shared_ptr<UIElement> UIBuilder::GetElement(const std::string& elementId) {
    auto it = m_elementMap.find(elementId);
    return it != m_elementMap.end() ? it->second : nullptr;
}

void UIBuilder::SelectElement(const std::string& elementId) {
    // Отменяем предыдущий выбор
    DeselectAll();
    
    auto element = GetElement(elementId);
    if (element) {
        m_selectedElementId = elementId;
        std::cout << "Selected UI element: " << elementId << std::endl;
    }
}

void UIBuilder::DeselectAll() {
    if (!m_selectedElementId.empty()) {
        m_selectedElementId.clear();
        std::cout << "Deselected all UI elements" << std::endl;
    }
}

std::shared_ptr<UIElement> UIBuilder::GetSelectedElement() const {
    auto it = m_elementMap.find(m_selectedElementId);
    return it != m_elementMap.end() ? it->second : nullptr;
}

void UIBuilder::MoveElement(const std::string& elementId, const glm::vec2& newPosition) {
    auto element = GetElement(elementId);
    if (element) {
        element->SetPosition(newPosition);
        std::cout << "Moved UI element " << elementId << " to (" << newPosition.x << ", " << newPosition.y << ")" << std::endl;
    }
}

void UIBuilder::ResizeElement(const std::string& elementId, const glm::vec2& newSize) {
    auto element = GetElement(elementId);
    if (element) {
        element->SetSize(newSize);
        std::cout << "Resized UI element " << elementId << " to (" << newSize.x << ", " << newSize.y << ")" << std::endl;
    }
}

void UIBuilder::ToggleMode() {
    switch (m_mode) {
        case UIBuilderMode::Design:
            SetMode(UIBuilderMode::Preview);
            break;
        case UIBuilderMode::Preview:
            SetMode(UIBuilderMode::Code);
            break;
        case UIBuilderMode::Code:
            SetMode(UIBuilderMode::Design);
            break;
    }
}

void UIBuilder::Clear() {
    m_elements.clear();
    m_elementMap.clear();
    m_selectedElementId.clear();
    
    std::cout << "Cleared UI Builder" << std::endl;
}

bool UIBuilder::Save(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << "{\n";
    file << "  \"elements\": [\n";
    
    for (size_t i = 0; i < m_elements.size(); ++i) {
        const auto& element = m_elements[i];
        if (element) {
            file << "    " << element->Serialize();
            if (i < m_elements.size() - 1) {
                file << ",";
            }
            file << "\n";
        }
    }
    
    file << "  ],\n";
    file << "  \"mode\": " << static_cast<int>(m_mode) << "\n";
    file << "}\n";
    
    file.close();
    
    std::cout << "Saved UI to: " << filename << std::endl;
    return true;
}

bool UIBuilder::Load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << filename << std::endl;
        return false;
    }
    
    // В реальной реализации здесь бы парсился JSON
    std::cout << "Loaded UI from: " << filename << std::endl;
    return true;
}

std::string UIBuilder::GenerateCode() const {
    std::stringstream ss;
    ss << "// Generated UI Code\n";
    ss << "#include \"FastEngine/Editor/UIElement.h\"\n\n";
    ss << "void CreateUI() {\n";
    
    for (const auto& element : m_elements) {
        if (element) {
            ss << GenerateElementCode(element, 1);
        }
    }
    
    ss << "}\n";
    return ss.str();
}

bool UIBuilder::Validate() {
    ClearValidationErrors();
    
    // Проверяем элементы
    for (const auto& element : m_elements) {
        if (element && !element->IsValid()) {
            AddValidationError("UI element " + element->GetId() + ": " + element->GetValidationError());
        }
    }
    
    bool isValid = m_validationErrors.empty();
    std::cout << "UI validation: " << (isValid ? "Valid" : "Invalid") << std::endl;
    
    if (!isValid) {
        for (const auto& error : m_validationErrors) {
            std::cout << "  Error: " << error << std::endl;
        }
    }
    
    return isValid;
}

void UIBuilder::RenderElements(void* renderer) {
    for (const auto& element : m_elements) {
        if (element) {
            element->Render(renderer);
        }
    }
}

void UIBuilder::RenderGrid(void* renderer) {
    // В реальной реализации здесь бы рендерилась сетка
    std::cout << "Rendering grid" << std::endl;
}

void UIBuilder::RenderUI(void* renderer) {
    // В реальной реализации здесь бы рендерился UI редактора
    std::cout << "Rendering UI Builder interface" << std::endl;
}

void UIBuilder::RenderToolbox(void* renderer) {
    // В реальной реализации здесь бы рендерился toolbox
    std::cout << "Rendering toolbox" << std::endl;
}

void UIBuilder::RenderProperties(void* renderer) {
    // В реальной реализации здесь бы рендерились свойства
    std::cout << "Rendering properties" << std::endl;
}

bool UIBuilder::HandleElementInput(std::shared_ptr<UIElement> element, float mouseX, float mouseY, int mouseButton) {
    if (!element) {
        return false;
    }
    
    // В реальной реализации здесь бы проверялся клик по элементу
    if (mouseButton == 0) { // Левая кнопка мыши
        SelectElement(element->GetId());
        return true;
    }
    
    return false;
}

void UIBuilder::HandleDragAndDrop(float mouseX, float mouseY) {
    // В реальной реализации здесь бы обрабатывался drag and drop
}

void UIBuilder::UpdateLayout() {
    // Обновляем layout всех элементов
    for (auto& element : m_elements) {
        if (element) {
            element->UpdateLayout();
        }
    }
}

void UIBuilder::ValidateElements() {
    // Валидируем элементы
    Validate();
}

std::string UIBuilder::GenerateElementId() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    
    return "ui_element_" + std::to_string(dis(gen));
}

void UIBuilder::AddValidationError(const std::string& error) {
    m_validationErrors.push_back(error);
}

void UIBuilder::ClearValidationErrors() {
    m_validationErrors.clear();
}

std::string UIBuilder::GenerateElementCode(std::shared_ptr<UIElement> element, int indent) const {
    std::string indentStr(indent * 2, ' ');
    std::stringstream ss;
    
    ss << indentStr << "auto " << element->GetName() << " = std::make_shared<UI" << 
          (element->GetType() == UIElementType::Panel ? "Panel" :
           element->GetType() == UIElementType::Button ? "Button" :
           element->GetType() == UIElementType::Label ? "Label" :
           element->GetType() == UIElementType::TextBox ? "TextBox" :
           element->GetType() == UIElementType::Image ? "Image" : "Element") << ">();\n";
    
    ss << indentStr << element->GetName() << "->SetPosition(glm::vec2(" << element->GetPosition().x << ", " << element->GetPosition().y << "));\n";
    ss << indentStr << element->GetName() << "->SetSize(glm::vec2(" << element->GetSize().x << ", " << element->GetSize().y << "));\n";
    
    return ss.str();
}

std::string UIBuilder::GenerateStyleCode(const UIStyle& style) const {
    std::stringstream ss;
    ss << "UIStyle style;\n";
    ss << "style.backgroundColor = glm::vec4(" << style.backgroundColor.r << ", " << style.backgroundColor.g << ", " << style.backgroundColor.b << ", " << style.backgroundColor.a << ");\n";
    ss << "style.borderColor = glm::vec4(" << style.borderColor.r << ", " << style.borderColor.g << ", " << style.borderColor.b << ", " << style.borderColor.a << ");\n";
    ss << "style.textColor = glm::vec4(" << style.textColor.r << ", " << style.textColor.g << ", " << style.textColor.b << ", " << style.textColor.a << ");\n";
    return ss.str();
}

std::string UIBuilder::GenerateLayoutCode(UIAlignment alignment) const {
    std::stringstream ss;
    ss << "UIAlignment alignment = UIAlignment::";
    switch (alignment) {
        case UIAlignment::Left: ss << "Left"; break;
        case UIAlignment::Right: ss << "Right"; break;
        case UIAlignment::Center: ss << "Center"; break;
        case UIAlignment::Top: ss << "Top"; break;
        case UIAlignment::Bottom: ss << "Bottom"; break;
        case UIAlignment::Middle: ss << "Middle"; break;
        case UIAlignment::Stretch: ss << "Stretch"; break;
        default: ss << "None"; break;
    }
    ss << ";\n";
    return ss.str();
}

} // namespace FastEngine
