#pragma once

#include "FastEngine/Component.h"
#include <string>
#include <vector>
#include <functional>
#include <glm/glm.hpp>

namespace FastEngine {
    enum class TextAlignment {
        Left,
        Center,
        Right,
        Justify
    };
    
    enum class TextVerticalAlignment {
        Top,
        Middle,
        Bottom
    };
    
    class Text : public Component {
    public:
        Text();
        ~Text() override = default;
        
        // Текст
        void SetText(const std::string& text) { m_text = text; }
        const std::string& GetText() const { return m_text; }
        
        // Шрифт
        void SetFont(const std::string& fontPath);
        const std::string& GetFont() const { return m_fontPath; }
        
        void SetFontSize(int size) { m_fontSize = size; }
        int GetFontSize() const { return m_fontSize; }
        
        // Цвет
        void SetColor(const glm::vec4& color) { m_color = color; }
        glm::vec4 GetColor() const { return m_color; }
        
        void SetColor(const glm::vec3& color) { m_color = glm::vec4(color, 1.0f); }
        
        // Выравнивание
        void SetAlignment(TextAlignment alignment) { m_alignment = alignment; }
        TextAlignment GetAlignment() const { return m_alignment; }
        
        void SetVerticalAlignment(TextVerticalAlignment alignment) { m_verticalAlignment = alignment; }
        TextVerticalAlignment GetVerticalAlignment() const { return m_verticalAlignment; }
        
        // Размеры и границы
        void SetMaxWidth(float width) { m_maxWidth = width; }
        float GetMaxWidth() const { return m_maxWidth; }
        
        void SetMaxHeight(float height) { m_maxHeight = height; }
        float GetMaxHeight() const { return m_maxHeight; }
        
        void SetWrapText(bool wrap) { m_wrapText = wrap; }
        bool IsWrapText() const { return m_wrapText; }
        
        // Эффекты
        void SetBold(bool bold) { m_bold = bold; }
        bool IsBold() const { return m_bold; }
        
        void SetItalic(bool italic) { m_italic = italic; }
        bool IsItalic() const { return m_italic; }
        
        void SetUnderline(bool underline) { m_underline = underline; }
        bool IsUnderline() const { return m_underline; }
        
        void SetStrikethrough(bool strikethrough) { m_strikethrough = strikethrough; }
        bool IsStrikethrough() const { return m_strikethrough; }
        
        // Тень
        void SetShadowEnabled(bool enabled) { m_shadowEnabled = enabled; }
        bool IsShadowEnabled() const { return m_shadowEnabled; }
        
        void SetShadowColor(const glm::vec4& color) { m_shadowColor = color; }
        glm::vec4 GetShadowColor() const { return m_shadowColor; }
        
        void SetShadowOffset(const glm::vec2& offset) { m_shadowOffset = offset; }
        glm::vec2 GetShadowOffset() const { return m_shadowOffset; }
        
        // Контур
        void SetOutlineEnabled(bool enabled) { m_outlineEnabled = enabled; }
        bool IsOutlineEnabled() const { return m_outlineEnabled; }
        
        void SetOutlineColor(const glm::vec4& color) { m_outlineColor = color; }
        glm::vec4 GetOutlineColor() const { return m_outlineColor; }
        
        void SetOutlineWidth(float width) { m_outlineWidth = width; }
        float GetOutlineWidth() const { return m_outlineWidth; }
        
        // Анимация текста
        void SetAnimated(bool animated) { m_animated = animated; }
        bool IsAnimated() const { return m_animated; }
        
        void SetAnimationSpeed(float speed) { m_animationSpeed = speed; }
        float GetAnimationSpeed() const { return m_animationSpeed; }
        
        // Обновление
        void Update(float deltaTime) override;
        
        // Получение размеров текста
        glm::vec2 GetTextSize() const;
        glm::vec2 GetTextBounds() const;
        
        // Форматирование
        void SetLineSpacing(float spacing) { m_lineSpacing = spacing; }
        float GetLineSpacing() const { return m_lineSpacing; }
        
        void SetCharacterSpacing(float spacing) { m_characterSpacing = spacing; }
        float GetCharacterSpacing() const { return m_characterSpacing; }
        
        // События
        void SetOnTextChanged(std::function<void(const std::string&)> callback);
        
        // Утилиты
        std::vector<std::string> GetWrappedLines() const;
        int GetLineCount() const;
        int GetCharacterCount() const;
        
    private:
        std::string m_text;
        std::string m_fontPath;
        int m_fontSize;
        glm::vec4 m_color;
        
        // Выравнивание
        TextAlignment m_alignment;
        TextVerticalAlignment m_verticalAlignment;
        
        // Размеры
        float m_maxWidth;
        float m_maxHeight;
        bool m_wrapText;
        
        // Стиль
        bool m_bold;
        bool m_italic;
        bool m_underline;
        bool m_strikethrough;
        
        // Эффекты
        bool m_shadowEnabled;
        glm::vec4 m_shadowColor;
        glm::vec2 m_shadowOffset;
        
        bool m_outlineEnabled;
        glm::vec4 m_outlineColor;
        float m_outlineWidth;
        
        // Анимация
        bool m_animated;
        float m_animationSpeed;
        float m_animationTimer;
        int m_visibleCharacters;
        
        // Форматирование
        float m_lineSpacing;
        float m_characterSpacing;
        
        // События
        std::function<void(const std::string&)> m_onTextChanged;
        
        // Вспомогательные методы
        void UpdateAnimation(float deltaTime);
        std::vector<std::string> WrapText(const std::string& text, float maxWidth) const;
        glm::vec2 CalculateTextSize(const std::string& text) const;
    };
}
