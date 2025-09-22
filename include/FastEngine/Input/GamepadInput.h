#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <map>

namespace FastEngine {
    enum class GamepadButton {
        A, B, X, Y,
        LeftBumper, RightBumper,
        Back, Start, Guide,
        LeftStick, RightStick,
        DPadUp, DPadRight, DPadDown, DPadLeft
    };
    
    enum class GamepadAxis {
        LeftX, LeftY,
        RightX, RightY,
        LeftTrigger, RightTrigger
    };
    
    struct GamepadState {
        bool connected;
        std::map<GamepadButton, bool> buttons;
        std::map<GamepadAxis, float> axes;
        std::string name;
        
        GamepadState() : connected(false) {
            // Инициализируем все кнопки как не нажатые
            for (int i = 0; i < 15; i++) {
                buttons[static_cast<GamepadButton>(i)] = false;
            }
            
            // Инициализируем все оси как нейтральные
            for (int i = 0; i < 6; i++) {
                axes[static_cast<GamepadAxis>(i)] = 0.0f;
            }
        }
    };
    
    struct TouchPoint {
        int id;
        glm::vec2 position;
        glm::vec2 previousPosition;
        bool active;
        float pressure;
        
        TouchPoint() : id(-1), position(0.0f), previousPosition(0.0f), active(false), pressure(1.0f) {}
    };
    
    enum class GestureType {
        Tap,
        Pinch,
        Swipe,
        Pan,
        Rotate
    };
    
    struct Gesture {
        GestureType type;
        glm::vec2 position;
        glm::vec2 delta;
        float scale;
        float rotation;
        int touchCount;
        
        Gesture() : type(GestureType::Tap), position(0.0f), delta(0.0f), scale(1.0f), rotation(0.0f), touchCount(0) {}
    };
    
    class GamepadInput {
    public:
        GamepadInput();
        ~GamepadInput();
        
        // Инициализация
        bool Initialize();
        void Shutdown();
        
        // Обновление состояния
        void Update();
        
        // Проверка кнопок
        bool IsButtonPressed(int gamepad, GamepadButton button) const;
        bool IsButtonJustPressed(int gamepad, GamepadButton button) const;
        bool IsButtonJustReleased(int gamepad, GamepadButton button) const;
        
        // Получение значений осей
        float GetAxisValue(int gamepad, GamepadAxis axis) const;
        glm::vec2 GetLeftStick(int gamepad) const;
        glm::vec2 GetRightStick(int gamepad) const;
        float GetLeftTrigger(int gamepad) const;
        float GetRightTrigger(int gamepad) const;
        
        // Информация о геймпадах
        int GetGamepadCount() const;
        bool IsGamepadConnected(int gamepad) const;
        std::string GetGamepadName(int gamepad) const;
        
        // Получение состояния
        const GamepadState& GetGamepadState(int gamepad) const;
        
    private:
        std::vector<GamepadState> m_gamepads;
        std::vector<GamepadState> m_previousGamepads;
        int m_maxGamepads;
        
        void UpdateGamepad(int index);
    };
    
    class AdvancedTouchInput {
    public:
        AdvancedTouchInput();
        ~AdvancedTouchInput();
        
        // Инициализация
        bool Initialize();
        void Shutdown();
        
        // Обновление состояния
        void Update();
        
        // Обработка событий касания
        void OnTouchDown(int id, const glm::vec2& position, float pressure = 1.0f);
        void OnTouchMove(int id, const glm::vec2& position, float pressure = 1.0f);
        void OnTouchUp(int id, const glm::vec2& position);
        
        // Получение информации о касаниях
        int GetTouchCount() const;
        const TouchPoint& GetTouchPoint(int index) const;
        const std::vector<TouchPoint>& GetAllTouchPoints() const;
        
        // Проверка касаний
        bool IsTouching(int id) const;
        glm::vec2 GetTouchPosition(int id) const;
        glm::vec2 GetTouchDelta(int id) const;
        
        // Жесты
        const std::vector<Gesture>& GetGestures() const;
        void ClearGestures();
        
    private:
        std::vector<TouchPoint> m_touchPoints;
        std::vector<TouchPoint> m_previousTouchPoints;
        std::vector<Gesture> m_gestures;
        
        // Параметры для распознавания жестов
        float m_tapThreshold;
        float m_swipeThreshold;
        float m_pinchThreshold;
        
        void DetectGestures();
        void DetectTap();
        void DetectSwipe();
        void DetectPinch();
        void DetectPan();
        void DetectRotate();
    };
}
