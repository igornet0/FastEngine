#include "FastEngine/Components/Text.h"
#include <algorithm>
#include <sstream>

namespace FastEngine {
    Text::Text() 
        : m_text("")
        , m_fontSize(16)
        , m_color(1.0f, 1.0f, 1.0f, 1.0f)
        , m_alignment(TextAlignment::Left)
        , m_verticalAlignment(TextVerticalAlignment::Top)
        , m_maxWidth(0.0f)
        , m_maxHeight(0.0f)
        , m_wrapText(true)
        , m_bold(false)
        , m_italic(false)
        , m_underline(false)
        , m_strikethrough(false)
        , m_shadowEnabled(false)
        , m_shadowColor(0.0f, 0.0f, 0.0f, 0.5f)
        , m_shadowOffset(2.0f, 2.0f)
        , m_outlineEnabled(false)
        , m_outlineColor(0.0f, 0.0f, 0.0f, 1.0f)
        , m_outlineWidth(1.0f)
        , m_animated(false)
        , m_animationSpeed(1.0f)
        , m_animationTimer(0.0f)
        , m_visibleCharacters(0)
        , m_lineSpacing(1.0f)
        , m_characterSpacing(0.0f) {
    }
    
    void Text::SetFont(const std::string& fontPath) {
        m_fontPath = fontPath;
        // В реальной реализации здесь бы загружался шрифт
    }
    
    void Text::Update(float deltaTime) {
        if (m_animated) {
            UpdateAnimation(deltaTime);
        }
    }
    
    glm::vec2 Text::GetTextSize() const {
        return CalculateTextSize(m_text);
    }
    
    glm::vec2 Text::GetTextBounds() const {
        glm::vec2 size = GetTextSize();
        
        if (m_maxWidth > 0.0f) {
            size.x = std::min(size.x, m_maxWidth);
        }
        if (m_maxHeight > 0.0f) {
            size.y = std::min(size.y, m_maxHeight);
        }
        
        return size;
    }
    
    void Text::SetOnTextChanged(std::function<void(const std::string&)> callback) {
        m_onTextChanged = callback;
    }
    
    std::vector<std::string> Text::GetWrappedLines() const {
        if (!m_wrapText || m_maxWidth <= 0.0f) {
            return {m_text};
        }
        
        return WrapText(m_text, m_maxWidth);
    }
    
    int Text::GetLineCount() const {
        return static_cast<int>(GetWrappedLines().size());
    }
    
    int Text::GetCharacterCount() const {
        return static_cast<int>(m_text.length());
    }
    
    void Text::UpdateAnimation(float deltaTime) {
        if (!m_animated) {
            return;
        }
        
        m_animationTimer += deltaTime * m_animationSpeed;
        
        int totalCharacters = static_cast<int>(m_text.length());
        m_visibleCharacters = static_cast<int>(m_animationTimer * 10.0f); // 10 символов в секунду
        
        if (m_visibleCharacters > totalCharacters) {
            m_visibleCharacters = totalCharacters;
        }
    }
    
    std::vector<std::string> Text::WrapText(const std::string& text, float maxWidth) const {
        std::vector<std::string> lines;
        std::istringstream iss(text);
        std::string word;
        std::string currentLine;
        
        // Упрощенная реализация переноса слов
        // В реальной реализации здесь бы учитывалась ширина символов
        float currentWidth = 0.0f;
        float wordWidth = 0.0f;
        
        while (iss >> word) {
            wordWidth = word.length() * m_fontSize * 0.6f; // Примерная ширина символа
            
            if (currentWidth + wordWidth > maxWidth && !currentLine.empty()) {
                lines.push_back(currentLine);
                currentLine = word;
                currentWidth = wordWidth;
            } else {
                if (!currentLine.empty()) {
                    currentLine += " ";
                    currentWidth += m_fontSize * 0.3f; // Примерная ширина пробела
                }
                currentLine += word;
                currentWidth += wordWidth;
            }
        }
        
        if (!currentLine.empty()) {
            lines.push_back(currentLine);
        }
        
        return lines;
    }
    
    glm::vec2 Text::CalculateTextSize(const std::string& text) const {
        if (text.empty()) {
            return glm::vec2(0.0f);
        }
        
        // Упрощенный расчет размера текста
        // В реальной реализации здесь бы использовались метрики шрифта
        
        std::vector<std::string> lines = m_wrapText ? WrapText(text, m_maxWidth) : std::vector<std::string>{text};
        
        float maxWidth = 0.0f;
        for (const auto& line : lines) {
            float lineWidth = line.length() * m_fontSize * 0.6f; // Примерная ширина символа
            maxWidth = std::max(maxWidth, lineWidth);
        }
        
        float height = lines.size() * m_fontSize * m_lineSpacing;
        
        return glm::vec2(maxWidth, height);
    }
}

