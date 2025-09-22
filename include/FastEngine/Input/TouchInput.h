#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace FastEngine {
    class TouchInput {
    public:
        struct TouchState {
            bool pressed;
            glm::vec2 position;
            glm::vec2 lastPosition;
            float timestamp;
        };
        
        TouchInput();
        ~TouchInput();
        
        // Инициализация
        bool Initialize();
        void Shutdown();
        
        // Обработка событий
        void OnTouchDown(int id, float x, float y);
        void OnTouchUp(int id, float x, float y);
        void OnTouchMove(int id, float x, float y);
        
        // Получение состояния
        bool IsPressed(int id) const;
        bool IsJustPressed(int id) const;
        bool IsJustReleased(int id) const;
        glm::vec2 GetPosition(int id) const;
        glm::vec2 GetDelta(int id) const;
        
        // Получение всех активных касаний
        const std::vector<TouchState>& GetActiveTouches() const { return m_touches; }
        
    private:
        std::vector<TouchState> m_touches;
        bool m_initialized;
    };
}
