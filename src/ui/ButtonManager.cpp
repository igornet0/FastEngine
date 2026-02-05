#include "FastEngine/UI/ButtonManager.h"
#include "FastEngine/Input/InputManager.h"
#include "FastEngine/Render/Renderer.h"

namespace FastEngine {
    void ButtonManager::AddButton(int id, float x, float y, float width, float height,
                                   std::function<void()> onClick,
                                   const glm::vec4& color) {
        m_buttons[id] = Button{ x, y, width, height, color, std::move(onClick) };
    }

    void ButtonManager::RemoveButton(int id) {
        m_buttons.erase(id);
    }

    void ButtonManager::Clear() {
        m_buttons.clear();
    }

    void ButtonManager::Update(InputManager* input, Renderer* renderer) {
        if (!input || !renderer) return;
        bool nowPressed = input->IsTouchPressed(0);
        if (m_wasTouchPressed && !nowPressed) {
            glm::vec2 pos = input->GetTouchPosition(0);
            glm::vec2 world = renderer->ScreenToWorld(static_cast<int>(pos.x), static_cast<int>(pos.y));
            for (auto& [id, btn] : m_buttons) {
                if (world.x >= btn.x && world.x <= btn.x + btn.width &&
                    world.y >= btn.y && world.y <= btn.y + btn.height) {
                    if (btn.onClick) btn.onClick();
                    break;
                }
            }
        }
        m_wasTouchPressed = nowPressed;
    }

    void ButtonManager::Draw(Renderer* renderer) {
        if (!renderer) return;
        renderer->SetBlendMode(true);
        for (const auto& [id, btn] : m_buttons) {
            renderer->DrawFilledRect(btn.x, btn.y, btn.width, btn.height, btn.color);
        }
    }
}
