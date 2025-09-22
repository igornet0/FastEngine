#include "FastEngine/Components/Animator.h"
#include <algorithm>

namespace FastEngine {
    Animator::Animator() 
        : m_currentFrame(0)
        , m_frameTimer(0.0f)
        , m_isPlaying(false)
        , m_isPaused(false)
        , m_speed(1.0f) {
    }
    
    void Animator::AddAnimation(const Animation& animation) {
        m_animations[animation.name] = animation;
    }
    
    void Animator::RemoveAnimation(const std::string& name) {
        auto it = m_animations.find(name);
        if (it != m_animations.end()) {
            m_animations.erase(it);
            
            // Если удаляемая анимация была текущей, останавливаем воспроизведение
            if (m_currentAnimation == name) {
                Stop();
            }
        }
    }
    
    void Animator::ClearAnimations() {
        m_animations.clear();
        Stop();
    }
    
    void Animator::Play(const std::string& animationName) {
        auto it = m_animations.find(animationName);
        if (it == m_animations.end()) {
            return;
        }
        
        m_currentAnimation = animationName;
        m_currentFrame = 0;
        m_frameTimer = 0.0f;
        m_isPlaying = true;
        m_isPaused = false;
        
        // Устанавливаем скорость из анимации
        m_speed = it->second.speed;
    }
    
    void Animator::Stop() {
        m_isPlaying = false;
        m_isPaused = false;
        m_currentFrame = 0;
        m_frameTimer = 0.0f;
    }
    
    void Animator::Pause() {
        if (m_isPlaying) {
            m_isPaused = true;
        }
    }
    
    void Animator::Resume() {
        if (m_isPaused) {
            m_isPaused = false;
        }
    }
    
    AnimationFrame Animator::GetCurrentFrameData() const {
        auto it = m_animations.find(m_currentAnimation);
        if (it == m_animations.end() || it->second.frames.empty()) {
            return AnimationFrame();
        }
        
        if (m_currentFrame >= 0 && m_currentFrame < static_cast<int>(it->second.frames.size())) {
            return it->second.frames[m_currentFrame];
        }
        
        return it->second.frames[0];
    }
    
    void Animator::Update(float deltaTime) {
        if (!m_isPlaying || m_isPaused) {
            return;
        }
        
        auto it = m_animations.find(m_currentAnimation);
        if (it == m_animations.end() || it->second.frames.empty()) {
            return;
        }
        
        const Animation& animation = it->second;
        m_frameTimer += deltaTime * m_speed;
        
        float frameDuration = animation.frames[m_currentFrame].duration;
        
        if (m_frameTimer >= frameDuration) {
            m_frameTimer -= frameDuration;
            NextFrame();
        }
    }
    
    void Animator::SetOnAnimationComplete(std::function<void(const std::string&)> callback) {
        m_onAnimationComplete = callback;
    }
    
    void Animator::SetOnFrameChange(std::function<void(int)> callback) {
        m_onFrameChange = callback;
    }
    
    const std::vector<std::string>& Animator::GetAnimationNames() const {
        static std::vector<std::string> names;
        names.clear();
        for (const auto& pair : m_animations) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    bool Animator::HasAnimation(const std::string& name) const {
        return m_animations.find(name) != m_animations.end();
    }
    
    const Animation* Animator::GetAnimation(const std::string& name) const {
        auto it = m_animations.find(name);
        return it != m_animations.end() ? &it->second : nullptr;
    }
    
    void Animator::NextFrame() {
        auto it = m_animations.find(m_currentAnimation);
        if (it == m_animations.end()) {
            return;
        }
        
        const Animation& animation = it->second;
        int frameCount = static_cast<int>(animation.frames.size());
        
        // Вызываем callback смены кадра
        if (m_onFrameChange) {
            m_onFrameChange(m_currentFrame);
        }
        
        switch (animation.type) {
            case AnimationType::Loop:
                m_currentFrame = (m_currentFrame + 1) % frameCount;
                break;
                
            case AnimationType::Once:
                if (m_currentFrame < frameCount - 1) {
                    m_currentFrame++;
                } else {
                    // Анимация завершена
                    Stop();
                    if (m_onAnimationComplete) {
                        m_onAnimationComplete(m_currentAnimation);
                    }
                }
                break;
                
            case AnimationType::PingPong:
                static bool forward = true;
                if (forward) {
                    if (m_currentFrame < frameCount - 1) {
                        m_currentFrame++;
                    } else {
                        forward = false;
                        m_currentFrame--;
                    }
                } else {
                    if (m_currentFrame > 0) {
                        m_currentFrame--;
                    } else {
                        forward = true;
                        m_currentFrame++;
                    }
                }
                break;
        }
    }
    
    void Animator::ResetAnimation() {
        m_currentFrame = 0;
        m_frameTimer = 0.0f;
    }
}

