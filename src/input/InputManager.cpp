#include "FastEngine/Input/InputManager.h"
#include "FastEngine/Input/TouchInput.h"
#include "FastEngine/Input/KeyboardInput.h"

namespace FastEngine {
    InputManager::InputManager() 
        : m_initialized(false) {
    }
    
    InputManager::~InputManager() {
        Shutdown();
    }
    
    bool InputManager::Initialize() {
        if (m_initialized) {
            return true;
        }
        
        // Инициализация систем ввода
        m_touchInput = std::make_unique<TouchInput>();
        m_keyboardInput = std::make_unique<KeyboardInput>();
        
        if (!m_touchInput->Initialize()) {
            return false;
        }
        
        if (!m_keyboardInput->Initialize()) {
            return false;
        }
        
        m_initialized = true;
        return true;
    }
    
    void InputManager::Shutdown() {
        if (!m_initialized) {
            return;
        }
        
        if (m_touchInput) {
            m_touchInput->Shutdown();
        }
        
        if (m_keyboardInput) {
            m_keyboardInput->Shutdown();
        }
        
        m_touchInput.reset();
        m_keyboardInput.reset();
        
        m_activeTouches.clear();
        m_keyEvents.clear();
        
        m_initialized = false;
    }
    
    void InputManager::Update(float deltaTime) {
        if (!m_initialized) {
            return;
        }
        
        // Обновляем состояние касаний
        if (m_touchInput) {
            const auto& touches = m_touchInput->GetActiveTouches();
            m_activeTouches.clear();
            
            for (const auto& touch : touches) {
                TouchPoint point;
                point.id = &touch - &touches[0]; // Получаем индекс
                point.position = touch.position;
                point.pressed = touch.pressed;
                point.released = !touch.pressed && touch.timestamp > 0.0f;
                m_activeTouches.push_back(point);
            }
        }
        
        // Очищаем события клавиш (они обрабатываются в реальном времени)
        m_keyEvents.clear();
    }
    
    void InputManager::OnTouchDown(int id, float x, float y) {
        if (m_touchInput) {
            m_touchInput->OnTouchDown(id, x, y);
        }
    }
    
    void InputManager::OnTouchUp(int id, float x, float y) {
        if (m_touchInput) {
            m_touchInput->OnTouchUp(id, x, y);
        }
    }
    
    void InputManager::OnTouchMove(int id, float x, float y) {
        if (m_touchInput) {
            m_touchInput->OnTouchMove(id, x, y);
        }
    }
    
    void InputManager::OnKeyDown(int key) {
        if (m_keyboardInput) {
            m_keyboardInput->OnKeyDown(key);
        }
    }
    
    void InputManager::OnKeyUp(int key) {
        if (m_keyboardInput) {
            m_keyboardInput->OnKeyUp(key);
        }
    }
    
    bool InputManager::IsKeyPressed(int key) const {
        if (m_keyboardInput) {
            return m_keyboardInput->IsPressed(key);
        }
        return false;
    }
    
    bool InputManager::IsKeyJustPressed(int key) const {
        if (m_keyboardInput) {
            return m_keyboardInput->IsJustPressed(key);
        }
        return false;
    }
    
    bool InputManager::IsKeyJustReleased(int key) const {
        if (m_keyboardInput) {
            return m_keyboardInput->IsJustReleased(key);
        }
        return false;
    }
    
    bool InputManager::IsTouchPressed(int id) const {
        if (m_touchInput) {
            return m_touchInput->IsPressed(id);
        }
        return false;
    }
    
    bool InputManager::IsTouchJustPressed(int id) const {
        if (m_touchInput) {
            return m_touchInput->IsJustPressed(id);
        }
        return false;
    }
    
    bool InputManager::IsTouchJustReleased(int id) const {
        if (m_touchInput) {
            return m_touchInput->IsJustReleased(id);
        }
        return false;
    }
    
    glm::vec2 InputManager::GetTouchPosition(int id) const {
        if (m_touchInput) {
            return m_touchInput->GetPosition(id);
        }
        return glm::vec2(0.0f);
    }
}
