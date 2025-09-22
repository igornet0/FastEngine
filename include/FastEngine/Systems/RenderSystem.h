#pragma once

#include "System.h"
#include <memory>

namespace FastEngine {
    class Renderer;
    class Camera;
    
    class RenderSystem : public System {
    public:
        RenderSystem(World* world, Renderer* renderer);
        ~RenderSystem();
        
        // Инициализация и завершение работы
        void Initialize() override;
        void Update(float deltaTime) override;
        void Cleanup() override;
        
        // Управление камерой
        void SetCamera(Camera* camera) { m_camera = camera; }
        Camera* GetCamera() const { return m_camera; }
        
    private:
        Renderer* m_renderer;
        Camera* m_camera;
    };
}
