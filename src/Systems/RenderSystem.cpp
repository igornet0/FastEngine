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
    
    void RenderSystem::SetCamera(Camera* camera) {
        m_camera = camera;
        if (m_renderer) {
            m_renderer->SetCamera(camera);
        }
    }
    
    void RenderSystem::Update(float deltaTime) {
        if (!m_renderer) {
            return;
        }
        
        int w = m_renderer->GetWidth();
        int h = m_renderer->GetHeight();
        // Сохраняем размер экрана в рендерере (viewport уже задан в RunOneFrame)
        m_renderer->SetViewport(0, 0, w, h);
        
        // Очистка на весь экран
        m_renderer->Clear(0.1f, 0.1f, 0.2f, 1.0f);
        
        // Letterbox: вписываем игровое соотношение 800x600 (4:3) в экран без растяжения
        const float gameAspect = 800.0f / 600.0f;
        float screenAspect = (h > 0) ? (float)w / (float)h : gameAspect;
        int vpX = 0, vpY = 0, vpW = w, vpH = h;
        if (screenAspect > gameAspect) {
            vpH = h;
            vpW = (int)((float)h * gameAspect);
            vpX = (w - vpW) / 2;
        } else {
            vpW = w;
            vpH = (int)((float)w / gameAspect);
            vpY = (h - vpH) / 2;
        }
#if defined(__APPLE__) && TARGET_OS_IPHONE
        m_renderer->SetViewportRect(0, 0, w, h);
#else
        m_renderer->SetViewportRect(vpX, vpY, vpW, vpH);
#endif
        
        // Получаем все сущности с компонентами Sprite и Transform
        auto entities = m_world->GetEntitiesWithComponents<Sprite, Transform>();
        
#if 0
        // Debug: размер экрана, letterbox, камера и элементы (отключено — включить при отладке)
        {
            static float debugAccum = 0.0f;
            static bool firstRun = true;
            debugAccum += deltaTime;
            if (debugAccum >= 1.0f || firstRun) {
                debugAccum = 0.0f;
                firstRun = false;
                int drawCount = 0;
                for (auto* entity : entities) {
                    auto* sprite = entity->template GetComponent<Sprite>();
                    auto* transform = entity->template GetComponent<Transform>();
                    if (sprite && transform && sprite->IsVisible()) drawCount++;
                }
                std::cout << "[RenderSystem] screen: " << w << " x " << h
                          << " | letterbox: " << vpX << "," << vpY << " " << vpW << " x " << vpH
                          << " | draws: " << drawCount << std::endl;
                if (m_camera) {
                    glm::vec2 camPos = m_camera->GetPosition();
                    std::cout << "[RenderSystem] camera pos: (" << camPos.x << ", " << camPos.y << ")" << std::endl;
                }
                int idx = 0;
                for (auto* entity : entities) {
                    auto* sprite = entity->template GetComponent<Sprite>();
                    auto* transform = entity->template GetComponent<Transform>();
                    if (sprite && transform && sprite->IsVisible()) {
                        glm::vec2 pos = transform->GetPosition();
                        glm::vec2 size = sprite->GetSize();
                        std::cout << "  element " << idx << ": pos=(" << pos.x << ", " << pos.y
                                  << ") size=(" << size.x << " x " << size.y << ")" << std::endl;
                        idx++;
                    }
                }
            }
        }
#endif
        
        // Отрисовываем каждую сущность
        for (auto* entity : entities) {
            auto* sprite = entity->template GetComponent<Sprite>();
            auto* transform = entity->template GetComponent<Transform>();
            
            if (sprite && transform && sprite->IsVisible()) {
                // Модель: T * R * S — позиция не должна масштабироваться (quad -0.5..0.5)
                glm::mat4 transformMatrix = glm::mat4(1.0f);
                transformMatrix = glm::translate(transformMatrix, glm::vec3(transform->GetPosition(), 0.0f));
                transformMatrix = glm::rotate(transformMatrix, glm::radians(transform->GetRotation()), glm::vec3(0.0f, 0.0f, 1.0f));
                transformMatrix = glm::scale(transformMatrix, glm::vec3(sprite->GetSize(), 1.0f));
                
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
