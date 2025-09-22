#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

namespace FastEngine {

/**
 * Типы UI элементов
 */
enum class UIElementType {
    Panel,
    Button,
    Label,
    TextBox,
    Image,
    Slider,
    CheckBox,
    RadioButton,
    ListBox,
    ComboBox,
    ProgressBar,
    Custom
};

/**
 * Выравнивание UI элементов
 */
enum class UIAlignment {
    None,
    Left,
    Right,
    Center,
    Top,
    Bottom,
    Middle,
    Stretch
};

/**
 * Стили UI элементов
 */
struct UIStyle {
    glm::vec4 backgroundColor;
    glm::vec4 borderColor;
    glm::vec4 textColor;
    float borderWidth;
    float borderRadius;
    float padding;
    float margin;
    std::string fontFamily;
    float fontSize;
    UIAlignment textAlignment;
    UIAlignment contentAlignment;
    
    UIStyle() 
        : backgroundColor(0.2f, 0.2f, 0.2f, 1.0f)
        , borderColor(0.5f, 0.5f, 0.5f, 1.0f)
        , textColor(1.0f, 1.0f, 1.0f, 1.0f)
        , borderWidth(1.0f)
        , borderRadius(4.0f)
        , padding(8.0f)
        , margin(4.0f)
        , fontFamily("Arial")
        , fontSize(14.0f)
        , textAlignment(UIAlignment::Left)
        , contentAlignment(UIAlignment::Center) {}
};

/**
 * События UI элементов
 */
enum class UIEventType {
    Click,
    DoubleClick,
    MouseEnter,
    MouseLeave,
    MouseDown,
    MouseUp,
    KeyDown,
    KeyUp,
    TextChanged,
    ValueChanged,
    FocusGained,
    FocusLost
};

/**
 * UI элемент
 */
class UIElement {
public:
    UIElement();
    virtual ~UIElement() = default;
    
    // Основные свойства
    std::string GetId() const { return m_id; }
    void SetId(const std::string& id) { m_id = id; }
    
    std::string GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    UIElementType GetType() const { return m_type; }
    void SetType(UIElementType type) { m_type = type; }
    
    glm::vec2 GetPosition() const { return m_position; }
    void SetPosition(const glm::vec2& position) { m_position = position; }
    
    glm::vec2 GetSize() const { return m_size; }
    void SetSize(const glm::vec2& size) { m_size = size; }
    
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }
    
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    // Родитель и дети
    UIElement* GetParent() const { return m_parent; }
    void SetParent(UIElement* parent) { m_parent = parent; }
    
    const std::vector<std::shared_ptr<UIElement>>& GetChildren() const { return m_children; }
    void AddChild(std::shared_ptr<UIElement> child);
    void RemoveChild(std::shared_ptr<UIElement> child);
    void RemoveChild(const std::string& childId);
    
    // Стили
    const UIStyle& GetStyle() const { return m_style; }
    void SetStyle(const UIStyle& style) { m_style = style; }
    
    // События
    void SetEventHandler(UIEventType eventType, std::function<void()> handler);
    void RemoveEventHandler(UIEventType eventType);
    void TriggerEvent(UIEventType eventType);
    
    // Обновление и рендеринг
    virtual void Update(float deltaTime);
    virtual void Render(void* renderer);
    
    // Обработка ввода
    virtual bool HandleInput(float mouseX, float mouseY, int mouseButton, int key);
    
    // Layout
    virtual void UpdateLayout();
    virtual void SetLayout(UIAlignment alignment) { m_layoutAlignment = alignment; }
    UIAlignment GetLayout() const { return m_layoutAlignment; }
    
    // Сериализация
    virtual std::string Serialize() const;
    virtual void Deserialize(const std::string& data);
    
    // Клонирование
    virtual std::shared_ptr<UIElement> Clone() const;
    
    // Валидация
    virtual bool IsValid() const;
    virtual std::string GetValidationError() const;
    
protected:
    std::string m_id;
    std::string m_name;
    UIElementType m_type;
    glm::vec2 m_position;
    glm::vec2 m_size;
    bool m_visible;
    bool m_enabled;
    
    UIElement* m_parent;
    std::vector<std::shared_ptr<UIElement>> m_children;
    
    UIStyle m_style;
    UIAlignment m_layoutAlignment;
    
    std::map<UIEventType, std::function<void()>> m_eventHandlers;
    
    // Вспомогательные методы
    std::string GenerateId() const;
    void UpdateChildrenLayout();
    bool IsPointInside(float x, float y) const;
};

/**
 * Специализированные UI элементы
 */
class UIPanel : public UIElement {
public:
    UIPanel();
    virtual ~UIPanel() = default;
    
    void Render(void* renderer) override;
    std::shared_ptr<UIElement> Clone() const override;
};

class UIButton : public UIElement {
public:
    UIButton();
    virtual ~UIButton() = default;
    
    std::string GetText() const { return m_text; }
    void SetText(const std::string& text) { m_text = text; }
    
    void Render(void* renderer) override;
    bool HandleInput(float mouseX, float mouseY, int mouseButton, int key) override;
    std::shared_ptr<UIElement> Clone() const override;
    
private:
    std::string m_text;
};

class UILabel : public UIElement {
public:
    UILabel();
    virtual ~UILabel() = default;
    
    std::string GetText() const { return m_text; }
    void SetText(const std::string& text) { m_text = text; }
    
    void Render(void* renderer) override;
    std::shared_ptr<UIElement> Clone() const override;
    
private:
    std::string m_text;
};

class UITextBox : public UIElement {
public:
    UITextBox();
    virtual ~UITextBox() = default;
    
    std::string GetText() const { return m_text; }
    void SetText(const std::string& text) { m_text = text; }
    
    void Render(void* renderer) override;
    bool HandleInput(float mouseX, float mouseY, int mouseButton, int key) override;
    std::shared_ptr<UIElement> Clone() const override;
    
private:
    std::string m_text;
    bool m_focused;
};

class UIImage : public UIElement {
public:
    UIImage();
    virtual ~UIImage() = default;
    
    std::string GetImagePath() const { return m_imagePath; }
    void SetImagePath(const std::string& path) { m_imagePath = path; }
    
    void Render(void* renderer) override;
    std::shared_ptr<UIElement> Clone() const override;
    
private:
    std::string m_imagePath;
};

} // namespace FastEngine

