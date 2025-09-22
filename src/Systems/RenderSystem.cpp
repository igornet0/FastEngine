#include "FastEngine/Systems/RenderSystem.h"
#include "FastEngine/Render/Renderer.h"
#include "FastEngine/Render/Camera.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Entity.h"
#include "FastEngine/World.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace FastEngine {
    RenderSystem::RenderSystem(World* world, Renderer* renderer)
        : System(world)
        , m_renderer(renderer)
        , m_camera(nullptr) {
    }
    
    RenderSystem::~RenderSystem() = default;
    
    void RenderSystem::Initialize() {
        std::cout << "RenderSystem initialized" << std::endl;
    }
    
    void RenderSystem::Update(float deltaTime) {
        if (!m_renderer) {
            return;
        }
        
        // Очищаем экран
        m_renderer->Clear(0.1f, 0.1f, 0.2f, 1.0f);
        
        // Получаем все сущности с компонентами Sprite и Transform
        auto entities = m_world->GetEntitiesWithComponents<Sprite, Transform>();
        
        // Отрисовываем каждую сущность
        for (auto* entity : entities) {
            auto* sprite = entity->template GetComponent<Sprite>();
            auto* transform = entity->template GetComponent<Transform>();
            
            if (sprite && transform && sprite->IsVisible()) {
                // Получаем матрицу трансформации
                glm::mat4 transformMatrix = transform->GetTransformMatrix();
                
                // Применяем размер спрайта к матрице
                glm::vec2 size = sprite->GetSize();
                transformMatrix = glm::scale(transformMatrix, glm::vec3(size, 1.0f));
                
                // Отрисовываем спрайт
                m_renderer->DrawSprite(sprite, transformMatrix);
            }
        }
        
        // Показываем результат
        m_renderer->Present();
    }
    
    void RenderSystem::Cleanup() {
        std::cout << "RenderSystem cleaned up" << std::endl;
    }
}
