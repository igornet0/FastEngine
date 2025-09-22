#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

namespace FastEngine {

/**
 * Типы анимации
 */
enum class AnimationType {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    Bezier,
    Custom
};

/**
 * Ключевой кадр
 */
struct Keyframe {
    float time;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec4 color;
    float alpha;
    AnimationType interpolation;
    
    Keyframe() : time(0.0f), position(0.0f), rotation(0.0f), scale(1.0f), 
                 color(1.0f), alpha(1.0f), interpolation(AnimationType::Linear) {}
};

/**
 * Анимация
 */
class Animation {
public:
    Animation();
    ~Animation() = default;
    
    std::string GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    float GetDuration() const { return m_duration; }
    void SetDuration(float duration) { m_duration = duration; }
    
    bool IsLooping() const { return m_looping; }
    void SetLooping(bool looping) { m_looping = looping; }
    
    const std::vector<Keyframe>& GetKeyframes() const { return m_keyframes; }
    void AddKeyframe(const Keyframe& keyframe);
    void RemoveKeyframe(float time);
    void UpdateKeyframe(float time, const Keyframe& keyframe);
    
    glm::vec3 EvaluatePosition(float time) const;
    glm::vec3 EvaluateRotation(float time) const;
    glm::vec3 EvaluateScale(float time) const;
    glm::vec4 EvaluateColor(float time) const;
    float EvaluateAlpha(float time) const;
    
    std::string Serialize() const;
    void Deserialize(const std::string& data);
    
private:
    std::string m_name;
    float m_duration;
    bool m_looping;
    std::vector<Keyframe> m_keyframes;
    
    Keyframe InterpolateKeyframes(const Keyframe& a, const Keyframe& b, float t) const;
    float InterpolateFloat(float a, float b, float t, AnimationType type) const;
    glm::vec3 InterpolateVec3(const glm::vec3& a, const glm::vec3& b, float t, AnimationType type) const;
    glm::vec4 InterpolateVec4(const glm::vec4& a, const glm::vec4& b, float t, AnimationType type) const;
};

/**
 * Редактор анимаций
 */
class AnimationEditor {
public:
    AnimationEditor();
    ~AnimationEditor();

    bool Initialize();
    void Update(float deltaTime);
    void Render(void* renderer);
    bool HandleInput(float mouseX, float mouseY, int mouseButton, int key);
    
    std::shared_ptr<Animation> CreateAnimation(const std::string& name);
    bool RemoveAnimation(const std::string& name);
    std::shared_ptr<Animation> GetAnimation(const std::string& name);
    
    void PlayAnimation(const std::string& name);
    void PauseAnimation();
    void StopAnimation();
    void SetCurrentTime(float time);
    
    bool IsPlaying() const { return m_playing; }
    float GetCurrentTime() const { return m_currentTime; }
    
    void SetUpdateCallback(std::function<void()> callback) { m_updateCallback = callback; }
    
private:
    std::map<std::string, std::shared_ptr<Animation>> m_animations;
    std::string m_currentAnimation;
    float m_currentTime;
    bool m_playing;
    bool m_initialized;
    
    std::function<void()> m_updateCallback;
    
    void RenderTimeline(void* renderer);
    void RenderKeyframes(void* renderer);
    void RenderControls(void* renderer);
};

} // namespace FastEngine

