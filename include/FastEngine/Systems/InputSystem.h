#pragma once

#include "System.h"
#include <memory>

namespace FastEngine {
    class InputManager;
    
    class InputSystem : public System {
    public:
        InputSystem(World* world, InputManager* inputManager);
        ~InputSystem();
        
        // Инициализация и завершение работы
        void Initialize() override;
        void Update(float deltaTime) override;
        void Cleanup() override;
        
    private:
        InputManager* m_inputManager;
    };
}
