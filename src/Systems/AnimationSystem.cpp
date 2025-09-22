#include "FastEngine/Systems/AnimationSystem.h"
#include "FastEngine/Entity.h"
#include "FastEngine/World.h"
#include "FastEngine/Render/Texture.h"
#include <iostream>

namespace FastEngine {
    AnimationSystem::AnimationSystem() 
        : System(nullptr)
        , m_globalSpeed(1.0f)
        , m_paused(false) {
    }
    
    void AnimationSystem::Update(float deltaTime) {
        if (m_paused) {
            return;
        }
        
        // Получаем все сущности с компонентом Animator
        auto entities = m_world->GetEntitiesWithComponents<Animator>();
        
        for (auto* entity : entities) {
            Animator* animator = GetAnimator(entity);
            if (!animator) {
                continue;
            }
            
            // Обновляем анимацию
            animator->Update(deltaTime * m_globalSpeed);
            
            // Обновляем спрайт, если есть
            Sprite* sprite = GetSprite(entity);
            if (sprite && animator->IsPlaying()) {
                AnimationFrame frame = animator->GetCurrentFrameData();
                UpdateSpriteTexture(entity, frame);
            }
        }
    }
    
    void AnimationSystem::PlayAnimation(Entity* entity, const std::string& animationName) {
        Animator* animator = GetAnimator(entity);
        if (animator) {
            animator->Play(animationName);
        }
    }
    
    void AnimationSystem::StopAnimation(Entity* entity) {
        Animator* animator = GetAnimator(entity);
        if (animator) {
            animator->Stop();
        }
    }
    
    void AnimationSystem::PauseAnimation(Entity* entity) {
        Animator* animator = GetAnimator(entity);
        if (animator) {
            animator->Pause();
        }
    }
    
    void AnimationSystem::ResumeAnimation(Entity* entity) {
        Animator* animator = GetAnimator(entity);
        if (animator) {
            animator->Resume();
        }
    }
    
    bool AnimationSystem::IsAnimationPlaying(Entity* entity) const {
        Animator* animator = GetAnimator(entity);
        return animator ? animator->IsPlaying() : false;
    }
    
    std::string AnimationSystem::GetCurrentAnimation(Entity* entity) const {
        Animator* animator = GetAnimator(entity);
        return animator ? animator->GetCurrentAnimation() : "";
    }
    
    int AnimationSystem::GetCurrentFrame(Entity* entity) const {
        Animator* animator = GetAnimator(entity);
        return animator ? animator->GetCurrentFrame() : 0;
    }
    
    Animator* AnimationSystem::GetAnimator(Entity* entity) const {
        return entity->GetComponent<Animator>();
    }
    
    Sprite* AnimationSystem::GetSprite(Entity* entity) const {
        return entity->GetComponent<Sprite>();
    }
    
    void AnimationSystem::UpdateSpriteTexture(Entity* entity, const AnimationFrame& frame) {
        Sprite* sprite = GetSprite(entity);
        if (!sprite) {
            return;
        }
        
        // Загружаем текстуру из пути кадра
        if (!frame.texturePath.empty()) {
            // В реальной реализации здесь бы загружалась текстура
            // sprite->SetTexture(LoadTexture(frame.texturePath));
        }
        
        // Обновляем смещение и размер спрайта
        // sprite->SetOffset(frame.offset);
        // sprite->SetSize(frame.size);
    }
}
