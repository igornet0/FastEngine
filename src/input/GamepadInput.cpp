#include "FastEngine/Input/GamepadInput.h"

#ifdef _WIN32
#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if !TARGET_OS_IPHONE
#include <IOKit/IOKitLib.h>
#include <ForceFeedback/ForceFeedback.h>
#endif
#else
#include <linux/joystick.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <iostream>
#include <algorithm>

namespace FastEngine {
    // GamepadInput реализация
    GamepadInput::GamepadInput() : m_maxGamepads(4) {
        m_gamepads.resize(m_maxGamepads);
        m_previousGamepads.resize(m_maxGamepads);
    }
    
    GamepadInput::~GamepadInput() {
        Shutdown();
    }
    
    bool GamepadInput::Initialize() {
        // Инициализация зависит от платформы
        // Для простоты пока что возвращаем true
        return true;
    }
    
    void GamepadInput::Shutdown() {
        // Очистка ресурсов
    }
    
    void GamepadInput::Update() {
        // Сохраняем предыдущее состояние
        m_previousGamepads = m_gamepads;
        
        // Обновляем состояние каждого геймпада
        for (int i = 0; i < m_maxGamepads; i++) {
            UpdateGamepad(i);
        }
    }
    
    bool GamepadInput::IsButtonPressed(int gamepad, GamepadButton button) const {
        if (gamepad < 0 || gamepad >= m_maxGamepads || !m_gamepads[gamepad].connected) {
            return false;
        }
        
        auto it = m_gamepads[gamepad].buttons.find(button);
        return it != m_gamepads[gamepad].buttons.end() ? it->second : false;
    }
    
    bool GamepadInput::IsButtonJustPressed(int gamepad, GamepadButton button) const {
        if (gamepad < 0 || gamepad >= m_maxGamepads || !m_gamepads[gamepad].connected) {
            return false;
        }
        
        bool current = IsButtonPressed(gamepad, button);
        bool previous = false;
        
        if (gamepad < m_previousGamepads.size()) {
            auto it = m_previousGamepads[gamepad].buttons.find(button);
            previous = it != m_previousGamepads[gamepad].buttons.end() ? it->second : false;
        }
        
        return current && !previous;
    }
    
    bool GamepadInput::IsButtonJustReleased(int gamepad, GamepadButton button) const {
        if (gamepad < 0 || gamepad >= m_maxGamepads || !m_gamepads[gamepad].connected) {
            return false;
        }
        
        bool current = IsButtonPressed(gamepad, button);
        bool previous = false;
        
        if (gamepad < m_previousGamepads.size()) {
            auto it = m_previousGamepads[gamepad].buttons.find(button);
            previous = it != m_previousGamepads[gamepad].buttons.end() ? it->second : false;
        }
        
        return !current && previous;
    }
    
    float GamepadInput::GetAxisValue(int gamepad, GamepadAxis axis) const {
        if (gamepad < 0 || gamepad >= m_maxGamepads || !m_gamepads[gamepad].connected) {
            return 0.0f;
        }
        
        auto it = m_gamepads[gamepad].axes.find(axis);
        return it != m_gamepads[gamepad].axes.end() ? it->second : 0.0f;
    }
    
    glm::vec2 GamepadInput::GetLeftStick(int gamepad) const {
        return glm::vec2(
            GetAxisValue(gamepad, GamepadAxis::LeftX),
            GetAxisValue(gamepad, GamepadAxis::LeftY)
        );
    }
    
    glm::vec2 GamepadInput::GetRightStick(int gamepad) const {
        return glm::vec2(
            GetAxisValue(gamepad, GamepadAxis::RightX),
            GetAxisValue(gamepad, GamepadAxis::RightY)
        );
    }
    
    float GamepadInput::GetLeftTrigger(int gamepad) const {
        return GetAxisValue(gamepad, GamepadAxis::LeftTrigger);
    }
    
    float GamepadInput::GetRightTrigger(int gamepad) const {
        return GetAxisValue(gamepad, GamepadAxis::RightTrigger);
    }
    
    int GamepadInput::GetGamepadCount() const {
        int count = 0;
        for (const auto& gamepad : m_gamepads) {
            if (gamepad.connected) count++;
        }
        return count;
    }
    
    bool GamepadInput::IsGamepadConnected(int gamepad) const {
        if (gamepad < 0 || gamepad >= m_maxGamepads) {
            return false;
        }
        return m_gamepads[gamepad].connected;
    }
    
    std::string GamepadInput::GetGamepadName(int gamepad) const {
        if (gamepad < 0 || gamepad >= m_maxGamepads || !m_gamepads[gamepad].connected) {
            return "";
        }
        return m_gamepads[gamepad].name;
    }
    
    const GamepadState& GamepadInput::GetGamepadState(int gamepad) const {
        static GamepadState emptyState;
        if (gamepad < 0 || gamepad >= m_maxGamepads) {
            return emptyState;
        }
        return m_gamepads[gamepad];
    }
    
    void GamepadInput::UpdateGamepad(int index) {
        // Заглушка - в реальной реализации здесь будет код для конкретной платформы
        // Например, для Windows - XInput, для Linux - /dev/input/js*
        m_gamepads[index].connected = false;
    }
    
    // AdvancedTouchInput реализация
    AdvancedTouchInput::AdvancedTouchInput() 
        : m_tapThreshold(10.0f)
        , m_swipeThreshold(50.0f)
        , m_pinchThreshold(0.1f) {
    }
    
    AdvancedTouchInput::~AdvancedTouchInput() {
        Shutdown();
    }
    
    bool AdvancedTouchInput::Initialize() {
        return true;
    }
    
    void AdvancedTouchInput::Shutdown() {
        m_touchPoints.clear();
        m_previousTouchPoints.clear();
        m_gestures.clear();
    }
    
    void AdvancedTouchInput::Update() {
        // Сохраняем предыдущее состояние
        m_previousTouchPoints = m_touchPoints;
        
        // Очищаем жесты предыдущего кадра
        m_gestures.clear();
        
        // Обновляем активные касания
        for (auto& touch : m_touchPoints) {
            if (touch.active) {
                touch.previousPosition = touch.position;
            }
        }
        
        // Удаляем неактивные касания
        m_touchPoints.erase(
            std::remove_if(m_touchPoints.begin(), m_touchPoints.end(),
                [](const TouchPoint& touch) { return !touch.active; }),
            m_touchPoints.end()
        );
        
        // Распознаем жесты
        DetectGestures();
    }
    
    void AdvancedTouchInput::OnTouchDown(int id, const glm::vec2& position, float pressure) {
        // Ищем существующее касание с таким ID
        auto it = std::find_if(m_touchPoints.begin(), m_touchPoints.end(),
            [id](const TouchPoint& touch) { return touch.id == id; });
        
        if (it != m_touchPoints.end()) {
            // Обновляем существующее касание
            it->position = position;
            it->previousPosition = position;
            it->active = true;
            it->pressure = pressure;
        } else {
            // Создаем новое касание
            TouchPoint touch;
            touch.id = id;
            touch.position = position;
            touch.previousPosition = position;
            touch.active = true;
            touch.pressure = pressure;
            m_touchPoints.push_back(touch);
        }
    }
    
    void AdvancedTouchInput::OnTouchMove(int id, const glm::vec2& position, float pressure) {
        auto it = std::find_if(m_touchPoints.begin(), m_touchPoints.end(),
            [id](const TouchPoint& touch) { return touch.id == id; });
        
        if (it != m_touchPoints.end()) {
            it->previousPosition = it->position;
            it->position = position;
            it->pressure = pressure;
        }
    }
    
    void AdvancedTouchInput::OnTouchUp(int id, const glm::vec2& position) {
        auto it = std::find_if(m_touchPoints.begin(), m_touchPoints.end(),
            [id](const TouchPoint& touch) { return touch.id == id; });
        
        if (it != m_touchPoints.end()) {
            it->position = position;
            it->active = false;
        }
    }
    
    int AdvancedTouchInput::GetTouchCount() const {
        return static_cast<int>(m_touchPoints.size());
    }
    
    const TouchPoint& AdvancedTouchInput::GetTouchPoint(int index) const {
        static TouchPoint emptyTouch;
        if (index < 0 || index >= m_touchPoints.size()) {
            return emptyTouch;
        }
        return m_touchPoints[index];
    }
    
    const std::vector<TouchPoint>& AdvancedTouchInput::GetAllTouchPoints() const {
        return m_touchPoints;
    }
    
    bool AdvancedTouchInput::IsTouching(int id) const {
        auto it = std::find_if(m_touchPoints.begin(), m_touchPoints.end(),
            [id](const TouchPoint& touch) { return touch.id == id && touch.active; });
        return it != m_touchPoints.end();
    }
    
    glm::vec2 AdvancedTouchInput::GetTouchPosition(int id) const {
        auto it = std::find_if(m_touchPoints.begin(), m_touchPoints.end(),
            [id](const TouchPoint& touch) { return touch.id == id; });
        
        if (it != m_touchPoints.end()) {
            return it->position;
        }
        return glm::vec2(0.0f);
    }
    
    glm::vec2 AdvancedTouchInput::GetTouchDelta(int id) const {
        auto it = std::find_if(m_touchPoints.begin(), m_touchPoints.end(),
            [id](const TouchPoint& touch) { return touch.id == id; });
        
        if (it != m_touchPoints.end()) {
            return it->position - it->previousPosition;
        }
        return glm::vec2(0.0f);
    }
    
    const std::vector<Gesture>& AdvancedTouchInput::GetGestures() const {
        return m_gestures;
    }
    
    void AdvancedTouchInput::ClearGestures() {
        m_gestures.clear();
    }
    
    void AdvancedTouchInput::DetectGestures() {
        DetectTap();
        DetectSwipe();
        DetectPinch();
        DetectPan();
        DetectRotate();
    }
    
    void AdvancedTouchInput::DetectTap() {
        for (const auto& touch : m_touchPoints) {
            if (!touch.active) continue;
            
            glm::vec2 delta = touch.position - touch.previousPosition;
            float distance = glm::length(delta);
            
            if (distance < m_tapThreshold) {
                Gesture gesture;
                gesture.type = GestureType::Tap;
                gesture.position = touch.position;
                gesture.touchCount = 1;
                m_gestures.push_back(gesture);
            }
        }
    }
    
    void AdvancedTouchInput::DetectSwipe() {
        for (const auto& touch : m_touchPoints) {
            if (!touch.active) continue;
            
            glm::vec2 delta = touch.position - touch.previousPosition;
            float distance = glm::length(delta);
            
            if (distance > m_swipeThreshold) {
                Gesture gesture;
                gesture.type = GestureType::Swipe;
                gesture.position = touch.position;
                gesture.delta = delta;
                gesture.touchCount = 1;
                m_gestures.push_back(gesture);
            }
        }
    }
    
    void AdvancedTouchInput::DetectPinch() {
        if (m_touchPoints.size() < 2) return;
        
        // Находим два активных касания
        std::vector<TouchPoint> activeTouches;
        for (const auto& touch : m_touchPoints) {
            if (touch.active) {
                activeTouches.push_back(touch);
            }
        }
        
        if (activeTouches.size() < 2) return;
        
        // Вычисляем расстояние между касаниями
        float currentDistance = glm::length(activeTouches[0].position - activeTouches[1].position);
        float previousDistance = glm::length(activeTouches[0].previousPosition - activeTouches[1].previousPosition);
        
        float scaleChange = currentDistance / previousDistance;
        
        if (std::abs(scaleChange - 1.0f) > m_pinchThreshold) {
            Gesture gesture;
            gesture.type = GestureType::Pinch;
            gesture.position = (activeTouches[0].position + activeTouches[1].position) * 0.5f;
            gesture.scale = scaleChange;
            gesture.touchCount = 2;
            m_gestures.push_back(gesture);
        }
    }
    
    void AdvancedTouchInput::DetectPan() {
        if (m_touchPoints.size() < 1) return;
        
        for (const auto& touch : m_touchPoints) {
            if (!touch.active) continue;
            
            glm::vec2 delta = touch.position - touch.previousPosition;
            float distance = glm::length(delta);
            
            if (distance > 1.0f && distance < m_swipeThreshold) {
                Gesture gesture;
                gesture.type = GestureType::Pan;
                gesture.position = touch.position;
                gesture.delta = delta;
                gesture.touchCount = 1;
                m_gestures.push_back(gesture);
            }
        }
    }
    
    void AdvancedTouchInput::DetectRotate() {
        if (m_touchPoints.size() < 2) return;
        
        // Находим два активных касания
        std::vector<TouchPoint> activeTouches;
        for (const auto& touch : m_touchPoints) {
            if (touch.active) {
                activeTouches.push_back(touch);
            }
        }
        
        if (activeTouches.size() < 2) return;
        
        // Вычисляем углы
        glm::vec2 currentVec = activeTouches[1].position - activeTouches[0].position;
        glm::vec2 previousVec = activeTouches[1].previousPosition - activeTouches[0].previousPosition;
        
        float currentAngle = std::atan2(currentVec.y, currentVec.x);
        float previousAngle = std::atan2(previousVec.y, previousVec.x);
        
        float rotation = currentAngle - previousAngle;
        
        if (std::abs(rotation) > 0.1f) {
            Gesture gesture;
            gesture.type = GestureType::Rotate;
            gesture.position = (activeTouches[0].position + activeTouches[1].position) * 0.5f;
            gesture.rotation = rotation;
            gesture.touchCount = 2;
            m_gestures.push_back(gesture);
        }
    }
}
