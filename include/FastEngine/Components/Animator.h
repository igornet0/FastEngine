#pragma once

#include "FastEngine/Component.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace FastEngine {
    enum class AnimationType {
        Loop,       // Зацикленная анимация
        Once,       // Одноразовая анимация
        PingPong    // Анимация туда-обратно
    };
    
    struct AnimationFrame {
        std::string texturePath;
        float duration;     // Длительность кадра в секундах
        glm::vec2 offset;   // Смещение спрайта
        glm::vec2 size;     // Размер спрайта
        
        AnimationFrame() : duration(0.1f), offset(0.0f), size(1.0f) {}
        AnimationFrame(const std::string& path, float dur = 0.1f) 
            : texturePath(path), duration(dur), offset(0.0f), size(1.0f) {}
    };
    
    struct Animation {
        std::string name;
        std::vector<AnimationFrame> frames;
        AnimationType type;
        float speed;        // Скорость воспроизведения (множитель)
        bool autoPlay;      // Автоматический запуск
        
        Animation() : type(AnimationType::Loop), speed(1.0f), autoPlay(false) {}
        Animation(const std::string& animName) 
            : name(animName), type(AnimationType::Loop), speed(1.0f), autoPlay(false) {}
    };
    
    class Animator : public Component {
    public:
        Animator();
        ~Animator() override = default;
        
        // Управление анимациями
        void AddAnimation(const Animation& animation);
        void RemoveAnimation(const std::string& name);
        void ClearAnimations();
        
        // Воспроизведение анимаций
        void Play(const std::string& animationName);
        void Stop();
        void Pause();
        void Resume();
        
        // Управление состоянием
        bool IsPlaying() const { return m_isPlaying; }
        bool IsPaused() const { return m_isPaused; }
        std::string GetCurrentAnimation() const { return m_currentAnimation; }
        int GetCurrentFrame() const { return m_currentFrame; }
        
        // Получение текущего кадра
        AnimationFrame GetCurrentFrameData() const;
        
        // Обновление анимации
        void Update(float deltaTime) override;
        
        // События анимации
        void SetOnAnimationComplete(std::function<void(const std::string&)> callback);
        void SetOnFrameChange(std::function<void(int)> callback);
        
        // Настройки
        void SetSpeed(float speed) { m_speed = speed; }
        float GetSpeed() const { return m_speed; }
        
        // Получение информации об анимациях
        const std::vector<std::string>& GetAnimationNames() const;
        bool HasAnimation(const std::string& name) const;
        const Animation* GetAnimation(const std::string& name) const;
        
    private:
        std::map<std::string, Animation> m_animations;
        std::string m_currentAnimation;
        int m_currentFrame;
        float m_frameTimer;
        bool m_isPlaying;
        bool m_isPaused;
        float m_speed;
        
        // События
        std::function<void(const std::string&)> m_onAnimationComplete;
        std::function<void(int)> m_onFrameChange;
        
        void NextFrame();
        void ResetAnimation();
    };
}
