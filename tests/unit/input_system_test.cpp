#include <gtest/gtest.h>
#include "FastEngine/Systems/InputSystem.h"
#include "FastEngine/World.h"
#include "FastEngine/Entity.h"
#include <memory>

using namespace FastEngine;

class InputSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        world = std::make_unique<World>();
        world->Initialize();
        inputSystem = std::make_unique<InputSystem>();
        inputSystem->Initialize();
    }
    
    void TearDown() override {
        inputSystem->Shutdown();
        inputSystem.reset();
        world.reset();
    }
    
    std::unique_ptr<World> world;
    std::unique_ptr<InputSystem> inputSystem;
};

TEST_F(InputSystemTest, InputSystemInitialization) {
    EXPECT_TRUE(inputSystem != nullptr);
}

TEST_F(InputSystemTest, KeyboardInput) {
    // Тест ввода с клавиатуры
    inputSystem->SetKeyPressed(KeyCode::W, true);
    EXPECT_TRUE(inputSystem->IsKeyPressed(KeyCode::W));
    
    inputSystem->SetKeyPressed(KeyCode::W, false);
    EXPECT_FALSE(inputSystem->IsKeyPressed(KeyCode::W));
}

TEST_F(InputSystemTest, KeyDown) {
    // Тест нажатия клавиши
    inputSystem->SetKeyDown(KeyCode::Space, true);
    EXPECT_TRUE(inputSystem->IsKeyDown(KeyCode::Space));
    
    inputSystem->SetKeyDown(KeyCode::Space, false);
    EXPECT_FALSE(inputSystem->IsKeyDown(KeyCode::Space));
}

TEST_F(InputSystemTest, KeyUp) {
    // Тест отпускания клавиши
    inputSystem->SetKeyUp(KeyCode::Escape, true);
    EXPECT_TRUE(inputSystem->IsKeyUp(KeyCode::Escape));
    
    inputSystem->SetKeyUp(KeyCode::Escape, false);
    EXPECT_FALSE(inputSystem->IsKeyUp(KeyCode::Escape));
}

TEST_F(InputSystemTest, MouseInput) {
    // Тест ввода с мыши
    inputSystem->SetMousePosition(glm::vec2(100.0f, 200.0f));
    auto mousePos = inputSystem->GetMousePosition();
    
    EXPECT_FLOAT_EQ(mousePos.x, 100.0f);
    EXPECT_FLOAT_EQ(mousePos.y, 200.0f);
}

TEST_F(InputSystemTest, MouseButtons) {
    // Тест кнопок мыши
    inputSystem->SetMouseButtonPressed(MouseButton::Left, true);
    EXPECT_TRUE(inputSystem->IsMouseButtonPressed(MouseButton::Left));
    
    inputSystem->SetMouseButtonPressed(MouseButton::Right, true);
    EXPECT_TRUE(inputSystem->IsMouseButtonPressed(MouseButton::Right));
    
    inputSystem->SetMouseButtonPressed(MouseButton::Middle, true);
    EXPECT_TRUE(inputSystem->IsMouseButtonPressed(MouseButton::Middle));
}

TEST_F(InputSystemTest, MouseWheel) {
    // Тест колеса мыши
    inputSystem->SetMouseWheelDelta(glm::vec2(0.0f, 1.0f));
    auto wheelDelta = inputSystem->GetMouseWheelDelta();
    
    EXPECT_FLOAT_EQ(wheelDelta.x, 0.0f);
    EXPECT_FLOAT_EQ(wheelDelta.y, 1.0f);
}

TEST_F(InputSystemTest, TouchInput) {
    // Тест касаний
    TouchInfo touch;
    touch.id = 0;
    touch.position = glm::vec2(150.0f, 250.0f);
    touch.pressure = 1.0f;
    
    inputSystem->SetTouch(touch);
    
    auto retrievedTouch = inputSystem->GetTouch(0);
    EXPECT_TRUE(retrievedTouch != nullptr);
    EXPECT_EQ(retrievedTouch->id, 0);
    EXPECT_FLOAT_EQ(retrievedTouch->position.x, 150.0f);
    EXPECT_FLOAT_EQ(retrievedTouch->position.y, 250.0f);
    EXPECT_FLOAT_EQ(retrievedTouch->pressure, 1.0f);
}

TEST_F(InputSystemTest, GamepadInput) {
    // Тест геймпада
    inputSystem->SetGamepadConnected(0, true);
    EXPECT_TRUE(inputSystem->IsGamepadConnected(0));
    
    inputSystem->SetGamepadButtonPressed(0, GamepadButton::A, true);
    EXPECT_TRUE(inputSystem->IsGamepadButtonPressed(0, GamepadButton::A));
    
    inputSystem->SetGamepadAxisValue(0, GamepadAxis::LeftStickX, 0.5f);
    EXPECT_FLOAT_EQ(inputSystem->GetGamepadAxisValue(0, GamepadAxis::LeftStickX), 0.5f);
}

TEST_F(InputSystemTest, InputEvents) {
    // Тест событий ввода
    bool keyEventCalled = false;
    bool mouseEventCalled = false;
    bool touchEventCalled = false;
    
    inputSystem->SetOnKeyEvent([&keyEventCalled](KeyCode key, bool pressed) {
        keyEventCalled = true;
    });
    
    inputSystem->SetOnMouseEvent([&mouseEventCalled](MouseButton button, bool pressed, glm::vec2 position) {
        mouseEventCalled = true;
    });
    
    inputSystem->SetOnTouchEvent([&touchEventCalled](const TouchInfo& touch) {
        touchEventCalled = true;
    });
    
    // Симулируем события
    inputSystem->SetKeyPressed(KeyCode::A, true);
    inputSystem->SetMouseButtonPressed(MouseButton::Left, true);
    
    TouchInfo touch;
    touch.id = 0;
    touch.position = glm::vec2(100.0f, 100.0f);
    inputSystem->SetTouch(touch);
    
    // Проверяем, что события сработали
    EXPECT_TRUE(keyEventCalled);
    EXPECT_TRUE(mouseEventCalled);
    EXPECT_TRUE(touchEventCalled);
}

TEST_F(InputSystemTest, InputMapping) {
    // Тест маппинга ввода
    inputSystem->MapKeyToAction(KeyCode::W, "MoveForward");
    inputSystem->MapKeyToAction(KeyCode::S, "MoveBackward");
    inputSystem->MapKeyToAction(KeyCode::A, "MoveLeft");
    inputSystem->MapKeyToAction(KeyCode::D, "MoveRight");
    
    inputSystem->SetKeyPressed(KeyCode::W, true);
    EXPECT_TRUE(inputSystem->IsActionPressed("MoveForward"));
    
    inputSystem->SetKeyPressed(KeyCode::S, true);
    EXPECT_TRUE(inputSystem->IsActionPressed("MoveBackward"));
}

TEST_F(InputSystemTest, InputAxis) {
    // Тест осей ввода
    inputSystem->MapKeyToAxis(KeyCode::W, "Vertical", 1.0f);
    inputSystem->MapKeyToAxis(KeyCode::S, "Vertical", -1.0f);
    inputSystem->MapKeyToAxis(KeyCode::A, "Horizontal", -1.0f);
    inputSystem->MapKeyToAxis(KeyCode::D, "Horizontal", 1.0f);
    
    inputSystem->SetKeyPressed(KeyCode::W, true);
    inputSystem->SetKeyPressed(KeyCode::D, true);
    
    EXPECT_FLOAT_EQ(inputSystem->GetAxisValue("Vertical"), 1.0f);
    EXPECT_FLOAT_EQ(inputSystem->GetAxisValue("Horizontal"), 1.0f);
}

TEST_F(InputSystemTest, InputCombinations) {
    // Тест комбинаций клавиш
    inputSystem->SetKeyPressed(KeyCode::LeftControl, true);
    inputSystem->SetKeyPressed(KeyCode::S, true);
    
    EXPECT_TRUE(inputSystem->IsKeyCombinationPressed({KeyCode::LeftControl, KeyCode::S}));
    
    inputSystem->SetKeyPressed(KeyCode::LeftControl, false);
    EXPECT_FALSE(inputSystem->IsKeyCombinationPressed({KeyCode::LeftControl, KeyCode::S}));
}

TEST_F(InputSystemTest, InputSequence) {
    // Тест последовательности ввода
    std::vector<KeyCode> sequence = {KeyCode::Up, KeyCode::Up, KeyCode::Down, KeyCode::Down, KeyCode::Left, KeyCode::Right, KeyCode::Left, KeyCode::Right, KeyCode::B, KeyCode::A};
    
    inputSystem->SetInputSequence(sequence);
    
    // Симулируем последовательность
    for (KeyCode key : sequence) {
        inputSystem->SetKeyPressed(key, true);
        inputSystem->SetKeyPressed(key, false);
    }
    
    EXPECT_TRUE(inputSystem->IsSequenceCompleted());
}

TEST_F(InputSystemTest, InputRepeat) {
    // Тест повторения ввода
    inputSystem->SetKeyRepeat(KeyCode::Space, true);
    inputSystem->SetKeyRepeatDelay(0.5f);
    inputSystem->SetKeyRepeatRate(0.1f);
    
    inputSystem->SetKeyPressed(KeyCode::Space, true);
    
    // Проверяем, что повторение настроено
    EXPECT_TRUE(inputSystem->IsKeyRepeatEnabled(KeyCode::Space));
    EXPECT_FLOAT_EQ(inputSystem->GetKeyRepeatDelay(), 0.5f);
    EXPECT_FLOAT_EQ(inputSystem->GetKeyRepeatRate(), 0.1f);
}

TEST_F(InputSystemTest, InputFocus) {
    // Тест фокуса ввода
    inputSystem->SetInputFocus(true);
    EXPECT_TRUE(inputSystem->HasInputFocus());
    
    inputSystem->SetInputFocus(false);
    EXPECT_FALSE(inputSystem->HasInputFocus());
}

TEST_F(InputSystemTest, InputCapture) {
    // Тест захвата ввода
    inputSystem->SetInputCapture(true);
    EXPECT_TRUE(inputSystem->IsInputCaptured());
    
    inputSystem->SetInputCapture(false);
    EXPECT_FALSE(inputSystem->IsInputCaptured());
}

TEST_F(InputSystemTest, InputSensitivity) {
    // Тест чувствительности ввода
    inputSystem->SetMouseSensitivity(2.0f);
    inputSystem->SetTouchSensitivity(1.5f);
    inputSystem->SetGamepadSensitivity(1.2f);
    
    EXPECT_FLOAT_EQ(inputSystem->GetMouseSensitivity(), 2.0f);
    EXPECT_FLOAT_EQ(inputSystem->GetTouchSensitivity(), 1.5f);
    EXPECT_FLOAT_EQ(inputSystem->GetGamepadSensitivity(), 1.2f);
}

TEST_F(InputSystemTest, InputDeadzone) {
    // Тест мертвой зоны
    inputSystem->SetGamepadDeadzone(0.1f);
    inputSystem->SetTouchDeadzone(5.0f);
    
    EXPECT_FLOAT_EQ(inputSystem->GetGamepadDeadzone(), 0.1f);
    EXPECT_FLOAT_EQ(inputSystem->GetTouchDeadzone(), 5.0f);
}

TEST_F(InputSystemTest, InputUpdate) {
    // Тест обновления системы ввода
    inputSystem->SetKeyPressed(KeyCode::W, true);
    inputSystem->SetMousePosition(glm::vec2(100.0f, 200.0f));
    
    inputSystem->Update(0.016f);
    
    // Проверяем, что система обновилась
    EXPECT_TRUE(inputSystem->IsKeyPressed(KeyCode::W));
    auto mousePos = inputSystem->GetMousePosition();
    EXPECT_FLOAT_EQ(mousePos.x, 100.0f);
    EXPECT_FLOAT_EQ(mousePos.y, 200.0f);
}

TEST_F(InputSystemTest, InputState) {
    // Тест состояния ввода
    inputSystem->SetKeyPressed(KeyCode::A, true);
    inputSystem->SetKeyPressed(KeyCode::B, true);
    inputSystem->SetMouseButtonPressed(MouseButton::Left, true);
    
    auto pressedKeys = inputSystem->GetPressedKeys();
    auto pressedMouseButtons = inputSystem->GetPressedMouseButtons();
    
    EXPECT_EQ(pressedKeys.size(), 2);
    EXPECT_EQ(pressedMouseButtons.size(), 1);
    
    EXPECT_TRUE(std::find(pressedKeys.begin(), pressedKeys.end(), KeyCode::A) != pressedKeys.end());
    EXPECT_TRUE(std::find(pressedKeys.begin(), pressedKeys.end(), KeyCode::B) != pressedKeys.end());
    EXPECT_TRUE(std::find(pressedMouseButtons.begin(), pressedMouseButtons.end(), MouseButton::Left) != pressedMouseButtons.end());
}

TEST_F(InputSystemTest, InputHistory) {
    // Тест истории ввода
    inputSystem->SetKeyPressed(KeyCode::A, true);
    inputSystem->SetKeyPressed(KeyCode::B, true);
    inputSystem->SetKeyPressed(KeyCode::C, true);
    
    auto inputHistory = inputSystem->GetInputHistory(3);
    EXPECT_EQ(inputHistory.size(), 3);
}

TEST_F(InputSystemTest, InputValidation) {
    // Тест валидации ввода
    // Некорректные значения должны обрабатываться корректно
    inputSystem->SetMousePosition(glm::vec2(-1000.0f, -1000.0f));
    inputSystem->SetMousePosition(glm::vec2(10000.0f, 10000.0f));
    
    auto mousePos = inputSystem->GetMousePosition();
    // Позиция должна быть ограничена разумными пределами
    EXPECT_GE(mousePos.x, -1000.0f);
    EXPECT_LE(mousePos.x, 10000.0f);
    EXPECT_GE(mousePos.y, -1000.0f);
    EXPECT_LE(mousePos.y, 10000.0f);
}

TEST_F(InputSystemTest, InputPerformance) {
    // Тест производительности системы ввода
    const int iterations = 1000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        inputSystem->SetKeyPressed(static_cast<KeyCode>(i % 100), true);
        inputSystem->SetMousePosition(glm::vec2(i, i));
        inputSystem->Update(0.016f);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Обработка 1000 итераций ввода должна занять менее 100мс
    EXPECT_LT(duration.count(), 100);
}
