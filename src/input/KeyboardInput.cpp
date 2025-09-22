#include "FastEngine/Input/KeyboardInput.h"

namespace FastEngine {
    KeyboardInput::KeyboardInput() 
        : m_initialized(false) {
    }
    
    KeyboardInput::~KeyboardInput() {
        Shutdown();
    }
    
    bool KeyboardInput::Initialize() {
        if (m_initialized) {
            return true;
        }
        
        // Инициализация системы клавиатуры
        m_pressedKeys.clear();
        m_justPressedKeys.clear();
        m_justReleasedKeys.clear();
        
        m_initialized = true;
        return true;
    }
    
    void KeyboardInput::Shutdown() {
        if (!m_initialized) {
            return;
        }
        
        m_pressedKeys.clear();
        m_justPressedKeys.clear();
        m_justReleasedKeys.clear();
        
        m_initialized = false;
    }
    
    void KeyboardInput::OnKeyDown(int key) {
        if (!m_initialized) {
            return;
        }
        
        // Проверяем, не была ли клавиша уже нажата
        if (m_pressedKeys.find(key) == m_pressedKeys.end()) {
            m_justPressedKeys.insert(key);
        }
        
        m_pressedKeys.insert(key);
    }
    
    void KeyboardInput::OnKeyUp(int key) {
        if (!m_initialized) {
            return;
        }
        
        // Проверяем, была ли клавиша нажата
        if (m_pressedKeys.find(key) != m_pressedKeys.end()) {
            m_justReleasedKeys.insert(key);
        }
        
        m_pressedKeys.erase(key);
    }
    
    bool KeyboardInput::IsPressed(int key) const {
        if (!m_initialized) {
            return false;
        }
        
        return m_pressedKeys.find(key) != m_pressedKeys.end();
    }
    
    bool KeyboardInput::IsJustPressed(int key) const {
        if (!m_initialized) {
            return false;
        }
        
        return m_justPressedKeys.find(key) != m_justPressedKeys.end();
    }
    
    bool KeyboardInput::IsJustReleased(int key) const {
        if (!m_initialized) {
            return false;
        }
        
        return m_justReleasedKeys.find(key) != m_justReleasedKeys.end();
    }
}
