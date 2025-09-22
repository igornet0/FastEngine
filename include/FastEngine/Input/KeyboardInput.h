#pragma once

#include <unordered_set>

namespace FastEngine {
    class KeyboardInput {
    public:
        KeyboardInput();
        ~KeyboardInput();
        
        // Инициализация
        bool Initialize();
        void Shutdown();
        
        // Обработка событий
        void OnKeyDown(int key);
        void OnKeyUp(int key);
        
        // Получение состояния
        bool IsPressed(int key) const;
        bool IsJustPressed(int key) const;
        bool IsJustReleased(int key) const;
        
        // Получение всех нажатых клавиш
        const std::unordered_set<int>& GetPressedKeys() const { return m_pressedKeys; }
        
    private:
        std::unordered_set<int> m_pressedKeys;
        std::unordered_set<int> m_justPressedKeys;
        std::unordered_set<int> m_justReleasedKeys;
        bool m_initialized;
    };
}
