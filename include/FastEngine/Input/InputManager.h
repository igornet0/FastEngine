#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <glm/glm.hpp>

namespace FastEngine {
    class TouchInput;
    class KeyboardInput;
    
    class InputManager {
    public:
        struct TouchPoint {
            int id;
            glm::vec2 position;
            bool pressed;
            bool released;
        };
        
        struct KeyEvent {
            int key;
            bool pressed;
            bool released;
            bool repeated;
        };
        
        InputManager();
        ~InputManager();
        
        // Инициализация и завершение работы
        bool Initialize();
        void Shutdown();
        
        // Обновление состояния ввода
        void Update(float deltaTime);
        
        // Обработка событий
        void OnTouchDown(int id, float x, float y);
        void OnTouchUp(int id, float x, float y);
        void OnTouchMove(int id, float x, float y);
        void OnKeyDown(int key);
        void OnKeyUp(int key);
        
        // Получение состояния ввода
        bool IsKeyPressed(int key) const;
        bool IsKeyJustPressed(int key) const;
        bool IsKeyJustReleased(int key) const;
        
        bool IsTouchPressed(int id) const;
        bool IsTouchJustPressed(int id) const;
        bool IsTouchJustReleased(int id) const;
        glm::vec2 GetTouchPosition(int id) const;
        
        // Получение всех активных касаний
        const std::vector<TouchPoint>& GetActiveTouches() const { return m_activeTouches; }
        
        // Обратные вызовы
        std::function<void(const TouchPoint&)> OnTouchDownCallback;
        std::function<void(const TouchPoint&)> OnTouchUpCallback;
        std::function<void(const TouchPoint&)> OnTouchMoveCallback;
        std::function<void(const KeyEvent&)> OnKeyDownCallback;
        std::function<void(const KeyEvent&)> OnKeyUpCallback;
        
    private:
        std::unique_ptr<TouchInput> m_touchInput;
        std::unique_ptr<KeyboardInput> m_keyboardInput;
        
        std::vector<TouchPoint> m_activeTouches;
        std::vector<KeyEvent> m_keyEvents;
        
        bool m_initialized;
    };
}
