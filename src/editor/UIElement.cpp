#include "FastEngine/Editor/UIElement.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <sstream>

namespace FastEngine {

UIElement::UIElement() 
    : m_type(UIElementType::Custom)
    , m_position(0.0f, 0.0f)
    , m_size(100.0f, 50.0f)
    , m_visible(true)
    , m_enabled(true)
    , m_parent(nullptr)
    , m_layoutAlignment(UIAlignment::None) {
    m_id = GenerateId();
}

void UIElement::AddChild(std::shared_ptr<UIElement> child) {
    if (child) {
        child->SetParent(this);
        m_children.push_back(child);
    }
}

void UIElement::RemoveChild(std::shared_ptr<UIElement> child) {
    if (child) {
        child->SetParent(nullptr);
        m_children.erase(
            std::remove(m_children.begin(), m_children.end(), child),
            m_children.end()
        );
    }
}

void UIElement::RemoveChild(const std::string& childId) {
    auto it = std::find_if(m_children.begin(), m_children.end(),
        [&childId](const std::shared_ptr<UIElement>& child) {
            return child && child->GetId() == childId;
        });
    
    if (it != m_children.end()) {
        (*it)->SetParent(nullptr);
        m_children.erase(it);
    }
}

void UIElement::Update(float deltaTime) {
    if (!m_visible || !m_enabled) {
        return;
    }
    
    // Обновляем дочерние элементы
    for (auto& child : m_children) {
        if (child) {
            child->Update(deltaTime);
        }
    }
}

void UIElement::Render(void* renderer) {
    if (!m_visible) {
        return;
    }
    
    // В реальной реализации здесь бы рендерился UI элемент
    std::cout << "Rendering UI element: " << m_name << " at (" 
              << m_position.x << ", " << m_position.y << ")" << std::endl;
    
    // Рендерим дочерние элементы
    for (auto& child : m_children) {
        if (child) {
            child->Render(renderer);
        }
    }
}

bool UIElement::HandleInput(float mouseX, float mouseY, int mouseButton, int key) {
    if (!m_visible || !m_enabled) {
        return false;
    }
    
    // Проверяем клик по элементу
    if (IsPointInside(mouseX, mouseY)) {
        if (mouseButton == 0) { // Левая кнопка мыши
            TriggerEvent(UIEventType::Click);
            return true;
        }
    }
    
    // Обрабатываем ввод дочерних элементов
    for (auto& child : m_children) {
        if (child && child->HandleInput(mouseX, mouseY, mouseButton, key)) {
            return true;
        }
    }
    
    return false;
}

void UIElement::UpdateLayout() {
    // Обновляем layout дочерних элементов
    UpdateChildrenLayout();
}

void UIElement::SetEventHandler(UIEventType eventType, std::function<void()> handler) {
    m_eventHandlers[eventType] = handler;
}

void UIElement::RemoveEventHandler(UIEventType eventType) {
    m_eventHandlers.erase(eventType);
}

void UIElement::TriggerEvent(UIEventType eventType) {
    auto it = m_eventHandlers.find(eventType);
    if (it != m_eventHandlers.end()) {
        it->second();
    }
}

std::string UIElement::Serialize() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"id\": \"" << m_id << "\",\n";
    ss << "  \"name\": \"" << m_name << "\",\n";
    ss << "  \"type\": " << static_cast<int>(m_type) << ",\n";
    ss << "  \"position\": {\"x\": " << m_position.x << ", \"y\": " << m_position.y << "},\n";
    ss << "  \"size\": {\"x\": " << m_size.x << ", \"y\": " << m_size.y << "},\n";
    ss << "  \"visible\": " << (m_visible ? "true" : "false") << ",\n";
    ss << "  \"enabled\": " << (m_enabled ? "true" : "false") << ",\n";
    ss << "  \"layoutAlignment\": " << static_cast<int>(m_layoutAlignment) << "\n";
    ss << "}";
    return ss.str();
}

void UIElement::Deserialize(const std::string& data) {
    // В реальной реализации здесь бы парсился JSON
    std::cout << "Deserializing UI element: " << m_name << std::endl;
}

std::shared_ptr<UIElement> UIElement::Clone() const {
    auto clone = std::make_shared<UIElement>();
    clone->m_name = m_name;
    clone->m_type = m_type;
    clone->m_position = m_position;
    clone->m_size = m_size;
    clone->m_visible = m_visible;
    clone->m_enabled = m_enabled;
    clone->m_style = m_style;
    clone->m_layoutAlignment = m_layoutAlignment;
    clone->m_eventHandlers = m_eventHandlers;
    return clone;
}

bool UIElement::IsValid() const {
    return !m_name.empty() && m_size.x > 0 && m_size.y > 0;
}

std::string UIElement::GetValidationError() const {
    if (m_name.empty()) {
        return "UI element name is empty";
    }
    if (m_size.x <= 0 || m_size.y <= 0) {
        return "UI element size is invalid";
    }
    return "";
}

std::string UIElement::GenerateId() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    
    return "ui_element_" + std::to_string(dis(gen));
}

void UIElement::UpdateChildrenLayout() {
    // В реальной реализации здесь бы обновлялся layout дочерних элементов
    for (auto& child : m_children) {
        if (child) {
            child->UpdateLayout();
        }
    }
}

bool UIElement::IsPointInside(float x, float y) const {
    return x >= m_position.x && x <= m_position.x + m_size.x &&
           y >= m_position.y && y <= m_position.y + m_size.y;
}

// UIPanel implementation
UIPanel::UIPanel() {
    m_type = UIElementType::Panel;
    m_name = "Panel";
}

void UIPanel::Render(void* renderer) {
    std::cout << "Rendering Panel: " << m_name << std::endl;
    UIElement::Render(renderer);
}

std::shared_ptr<UIElement> UIPanel::Clone() const {
    auto clone = std::make_shared<UIPanel>();
    clone->m_name = m_name;
    clone->m_position = m_position;
    clone->m_size = m_size;
    clone->m_visible = m_visible;
    clone->m_enabled = m_enabled;
    clone->m_style = m_style;
    clone->m_layoutAlignment = m_layoutAlignment;
    clone->m_eventHandlers = m_eventHandlers;
    return clone;
}

// UIButton implementation
UIButton::UIButton() {
    m_type = UIElementType::Button;
    m_name = "Button";
    m_text = "Button";
}

void UIButton::Render(void* renderer) {
    std::cout << "Rendering Button: " << m_text << std::endl;
    UIElement::Render(renderer);
}

bool UIButton::HandleInput(float mouseX, float mouseY, int mouseButton, int key) {
    if (UIElement::HandleInput(mouseX, mouseY, mouseButton, key)) {
        std::cout << "Button clicked: " << m_text << std::endl;
        return true;
    }
    return false;
}

std::shared_ptr<UIElement> UIButton::Clone() const {
    auto clone = std::make_shared<UIButton>();
    clone->m_name = m_name;
    clone->m_position = m_position;
    clone->m_size = m_size;
    clone->m_visible = m_visible;
    clone->m_enabled = m_enabled;
    clone->m_style = m_style;
    clone->m_layoutAlignment = m_layoutAlignment;
    clone->m_eventHandlers = m_eventHandlers;
    clone->m_text = m_text;
    return clone;
}

// UILabel implementation
UILabel::UILabel() {
    m_type = UIElementType::Label;
    m_name = "Label";
    m_text = "Label";
}

void UILabel::Render(void* renderer) {
    std::cout << "Rendering Label: " << m_text << std::endl;
    UIElement::Render(renderer);
}

std::shared_ptr<UIElement> UILabel::Clone() const {
    auto clone = std::make_shared<UILabel>();
    clone->m_name = m_name;
    clone->m_position = m_position;
    clone->m_size = m_size;
    clone->m_visible = m_visible;
    clone->m_enabled = m_enabled;
    clone->m_style = m_style;
    clone->m_layoutAlignment = m_layoutAlignment;
    clone->m_eventHandlers = m_eventHandlers;
    clone->m_text = m_text;
    return clone;
}

// UITextBox implementation
UITextBox::UITextBox() {
    m_type = UIElementType::TextBox;
    m_name = "TextBox";
    m_text = "";
    m_focused = false;
}

void UITextBox::Render(void* renderer) {
    std::cout << "Rendering TextBox: " << m_text << (m_focused ? " [FOCUSED]" : "") << std::endl;
    UIElement::Render(renderer);
}

bool UITextBox::HandleInput(float mouseX, float mouseY, int mouseButton, int key) {
    if (UIElement::HandleInput(mouseX, mouseY, mouseButton, key)) {
        m_focused = true;
        std::cout << "TextBox focused: " << m_text << std::endl;
        return true;
    }
    return false;
}

std::shared_ptr<UIElement> UITextBox::Clone() const {
    auto clone = std::make_shared<UITextBox>();
    clone->m_name = m_name;
    clone->m_position = m_position;
    clone->m_size = m_size;
    clone->m_visible = m_visible;
    clone->m_enabled = m_enabled;
    clone->m_style = m_style;
    clone->m_layoutAlignment = m_layoutAlignment;
    clone->m_eventHandlers = m_eventHandlers;
    clone->m_text = m_text;
    clone->m_focused = m_focused;
    return clone;
}

// UIImage implementation
UIImage::UIImage() {
    m_type = UIElementType::Image;
    m_name = "Image";
    m_imagePath = "";
}

void UIImage::Render(void* renderer) {
    std::cout << "Rendering Image: " << m_imagePath << std::endl;
    UIElement::Render(renderer);
}

std::shared_ptr<UIElement> UIImage::Clone() const {
    auto clone = std::make_shared<UIImage>();
    clone->m_name = m_name;
    clone->m_position = m_position;
    clone->m_size = m_size;
    clone->m_visible = m_visible;
    clone->m_enabled = m_enabled;
    clone->m_style = m_style;
    clone->m_layoutAlignment = m_layoutAlignment;
    clone->m_eventHandlers = m_eventHandlers;
    clone->m_imagePath = m_imagePath;
    return clone;
}

} // namespace FastEngine
