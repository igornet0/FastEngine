#pragma once

#include <functional>
#include <unordered_map>
#include <glm/glm.hpp>

namespace FastEngine {
    class InputManager;
    class Renderer;
}

namespace FastEngine {
    class ButtonManager {
    public:
        ButtonManager() = default;

        /// Добавить кнопку: (x, y) — левый нижний угол в мировых координатах, onClick вызывается при клике
        void AddButton(int id, float x, float y, float width, float height,
                       std::function<void()> onClick,
                       const glm::vec4& color = glm::vec4(0.4f, 0.5f, 0.6f, 0.9f));
        void RemoveButton(int id);
        void Clear();

        /// Обработка кликов: вызывать каждый кадр после обновления ввода
        void Update(InputManager* input, Renderer* renderer);
        /// Отрисовка кнопок: вызывать в Render (после сцены)
        void Draw(Renderer* renderer);

    private:
        struct Button {
            float x, y, width, height;
            glm::vec4 color;
            std::function<void()> onClick;
        };
        std::unordered_map<int, Button> m_buttons;
        bool m_wasTouchPressed = false;
    };
}
