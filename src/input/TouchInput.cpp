#include "FastEngine/Input/TouchInput.h"
#include <algorithm>

namespace FastEngine {
    TouchInput::TouchInput() 
        : m_initialized(false) {
    }
    
    TouchInput::~TouchInput() {
        Shutdown();
    }
    
    bool TouchInput::Initialize() {
        if (m_initialized) {
            return true;
        }
        
        // Инициализация системы касаний
        m_touches.clear();
        m_initialized = true;
        return true;
    }
    
    void TouchInput::Shutdown() {
        if (!m_initialized) {
            return;
        }
        
        m_touches.clear();
        m_initialized = false;
    }
    
    void TouchInput::OnTouchDown(int id, float x, float y) {
        if (!m_initialized) {
            return;
        }
        
        // Ищем существующее касание или создаем новое
        auto it = std::find_if(m_touches.begin(), m_touches.end(),
            [this, id](const TouchState& touch) { return &touch - &m_touches[0] == id; });
        
        if (it != m_touches.end()) {
            // Обновляем существующее касание
            it->pressed = true;
            it->position = glm::vec2(x, y);
            it->lastPosition = it->position;
            it->timestamp = 0.0f; // В реальном приложении здесь должно быть текущее время
        } else {
            // Создаем новое касание
            TouchState touch;
            touch.pressed = true;
            touch.position = glm::vec2(x, y);
            touch.lastPosition = touch.position;
            touch.timestamp = 0.0f;
            m_touches.push_back(touch);
        }
    }
    
    void TouchInput::OnTouchUp(int id, float x, float y) {
        if (!m_initialized) {
            return;
        }
        
        // Ищем касание по ID
        auto it = std::find_if(m_touches.begin(), m_touches.end(),
            [this, id](const TouchState& touch) { return &touch - &m_touches[0] == id; });
        
        if (it != m_touches.end()) {
            it->pressed = false;
            it->position = glm::vec2(x, y);
        }
    }
    
    void TouchInput::OnTouchMove(int id, float x, float y) {
        if (!m_initialized) {
            return;
        }
        
        // Ищем касание по ID
        auto it = std::find_if(m_touches.begin(), m_touches.end(),
            [this, id](const TouchState& touch) { return &touch - &m_touches[0] == id; });
        
        if (it != m_touches.end()) {
            it->lastPosition = it->position;
            it->position = glm::vec2(x, y);
        }
    }
    
    bool TouchInput::IsPressed(int id) const {
        if (!m_initialized || id < 0 || id >= m_touches.size()) {
            return false;
        }
        
        return m_touches[id].pressed;
    }
    
    bool TouchInput::IsJustPressed(int id) const {
        if (!m_initialized || id < 0 || id >= m_touches.size()) {
            return false;
        }
        
        // В реальном приложении здесь должна быть проверка на то,
        // что касание было нажато в текущем кадре
        return m_touches[id].pressed;
    }
    
    bool TouchInput::IsJustReleased(int id) const {
        if (!m_initialized || id < 0 || id >= m_touches.size()) {
            return false;
        }
        
        // В реальном приложении здесь должна быть проверка на то,
        // что касание было отпущено в текущем кадре
        return !m_touches[id].pressed;
    }
    
    glm::vec2 TouchInput::GetPosition(int id) const {
        if (!m_initialized || id < 0 || id >= m_touches.size()) {
            return glm::vec2(0.0f);
        }
        
        return m_touches[id].position;
    }
    
    glm::vec2 TouchInput::GetDelta(int id) const {
        if (!m_initialized || id < 0 || id >= m_touches.size()) {
            return glm::vec2(0.0f);
        }
        
        return m_touches[id].position - m_touches[id].lastPosition;
    }
}
