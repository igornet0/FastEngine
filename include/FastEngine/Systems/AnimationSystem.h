#pragma once

#include "FastEngine/System.h"
#include "FastEngine/Components/Animator.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Entity.h"
#include <vector>

namespace FastEngine {
    class AnimationSystem : public System {
    public:
        AnimationSystem();
        ~AnimationSystem() override = default;
        
        // Обновление системы
        void Update(float deltaTime) override;
        
        // Управление анимациями
        void PlayAnimation(Entity* entity, const std::string& animationName);
        void StopAnimation(Entity* entity);
        void PauseAnimation(Entity* entity);
        void ResumeAnimation(Entity* entity);
        
        // Получение информации
        bool IsAnimationPlaying(Entity* entity) const;
        std::string GetCurrentAnimation(Entity* entity) const;
        int GetCurrentFrame(Entity* entity) const;
        
        // Глобальные настройки
        void SetGlobalSpeed(float speed) { m_globalSpeed = speed; }
        float GetGlobalSpeed() const { return m_globalSpeed; }
        
        void SetPaused(bool paused) { m_paused = paused; }
        bool IsPaused() const { return m_paused; }
        
    private:
        float m_globalSpeed;
        bool m_paused;
        
        // Вспомогательные методы
        Animator* GetAnimator(Entity* entity) const;
        Sprite* GetSprite(Entity* entity) const;
        void UpdateSpriteTexture(Entity* entity, const AnimationFrame& frame);
    };
}
