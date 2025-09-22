#include "FastEngine/Editor/AnimationEditor.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace FastEngine {

Animation::Animation() : m_duration(1.0f), m_looping(false) {}

void Animation::AddKeyframe(const Keyframe& keyframe) {
    m_keyframes.push_back(keyframe);
    std::sort(m_keyframes.begin(), m_keyframes.end(), 
        [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });
}

void Animation::RemoveKeyframe(float time) {
    m_keyframes.erase(
        std::remove_if(m_keyframes.begin(), m_keyframes.end(),
            [time](const Keyframe& kf) { return std::abs(kf.time - time) < 0.001f; }),
        m_keyframes.end()
    );
}

void Animation::UpdateKeyframe(float time, const Keyframe& keyframe) {
    auto it = std::find_if(m_keyframes.begin(), m_keyframes.end(),
        [time](const Keyframe& kf) { return std::abs(kf.time - time) < 0.001f; });
    
    if (it != m_keyframes.end()) {
        *it = keyframe;
    } else {
        AddKeyframe(keyframe);
    }
}

glm::vec3 Animation::EvaluatePosition(float time) const {
    if (m_keyframes.empty()) return glm::vec3(0.0f);
    
    // Нормализуем время
    if (m_looping && m_duration > 0.0f) {
        time = fmod(time, m_duration);
    }
    
    if (time <= m_keyframes.front().time) {
        return m_keyframes.front().position;
    }
    if (time >= m_keyframes.back().time) {
        return m_keyframes.back().position;
    }
    
    // Находим соседние ключевые кадры
    for (size_t i = 0; i < m_keyframes.size() - 1; ++i) {
        if (time >= m_keyframes[i].time && time <= m_keyframes[i + 1].time) {
            float t = (time - m_keyframes[i].time) / (m_keyframes[i + 1].time - m_keyframes[i].time);
            return InterpolateVec3(m_keyframes[i].position, m_keyframes[i + 1].position, t, m_keyframes[i].interpolation);
        }
    }
    
    return glm::vec3(0.0f);
}

glm::vec3 Animation::EvaluateRotation(float time) const {
    if (m_keyframes.empty()) return glm::vec3(0.0f);
    
    if (m_looping && m_duration > 0.0f) {
        time = fmod(time, m_duration);
    }
    
    if (time <= m_keyframes.front().time) {
        return m_keyframes.front().rotation;
    }
    if (time >= m_keyframes.back().time) {
        return m_keyframes.back().rotation;
    }
    
    for (size_t i = 0; i < m_keyframes.size() - 1; ++i) {
        if (time >= m_keyframes[i].time && time <= m_keyframes[i + 1].time) {
            float t = (time - m_keyframes[i].time) / (m_keyframes[i + 1].time - m_keyframes[i].time);
            return InterpolateVec3(m_keyframes[i].rotation, m_keyframes[i + 1].rotation, t, m_keyframes[i].interpolation);
        }
    }
    
    return glm::vec3(0.0f);
}

glm::vec3 Animation::EvaluateScale(float time) const {
    if (m_keyframes.empty()) return glm::vec3(1.0f);
    
    if (m_looping && m_duration > 0.0f) {
        time = fmod(time, m_duration);
    }
    
    if (time <= m_keyframes.front().time) {
        return m_keyframes.front().scale;
    }
    if (time >= m_keyframes.back().time) {
        return m_keyframes.back().scale;
    }
    
    for (size_t i = 0; i < m_keyframes.size() - 1; ++i) {
        if (time >= m_keyframes[i].time && time <= m_keyframes[i + 1].time) {
            float t = (time - m_keyframes[i].time) / (m_keyframes[i + 1].time - m_keyframes[i].time);
            return InterpolateVec3(m_keyframes[i].scale, m_keyframes[i + 1].scale, t, m_keyframes[i].interpolation);
        }
    }
    
    return glm::vec3(1.0f);
}

glm::vec4 Animation::EvaluateColor(float time) const {
    if (m_keyframes.empty()) return glm::vec4(1.0f);
    
    if (m_looping && m_duration > 0.0f) {
        time = fmod(time, m_duration);
    }
    
    if (time <= m_keyframes.front().time) {
        return m_keyframes.front().color;
    }
    if (time >= m_keyframes.back().time) {
        return m_keyframes.back().color;
    }
    
    for (size_t i = 0; i < m_keyframes.size() - 1; ++i) {
        if (time >= m_keyframes[i].time && time <= m_keyframes[i + 1].time) {
            float t = (time - m_keyframes[i].time) / (m_keyframes[i + 1].time - m_keyframes[i].time);
            return InterpolateVec4(m_keyframes[i].color, m_keyframes[i + 1].color, t, m_keyframes[i].interpolation);
        }
    }
    
    return glm::vec4(1.0f);
}

float Animation::EvaluateAlpha(float time) const {
    if (m_keyframes.empty()) return 1.0f;
    
    if (m_looping && m_duration > 0.0f) {
        time = fmod(time, m_duration);
    }
    
    if (time <= m_keyframes.front().time) {
        return m_keyframes.front().alpha;
    }
    if (time >= m_keyframes.back().time) {
        return m_keyframes.back().alpha;
    }
    
    for (size_t i = 0; i < m_keyframes.size() - 1; ++i) {
        if (time >= m_keyframes[i].time && time <= m_keyframes[i + 1].time) {
            float t = (time - m_keyframes[i].time) / (m_keyframes[i + 1].time - m_keyframes[i].time);
            return InterpolateFloat(m_keyframes[i].alpha, m_keyframes[i + 1].alpha, t, m_keyframes[i].interpolation);
        }
    }
    
    return 1.0f;
}

std::string Animation::Serialize() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"name\": \"" << m_name << "\",\n";
    ss << "  \"duration\": " << m_duration << ",\n";
    ss << "  \"looping\": " << (m_looping ? "true" : "false") << ",\n";
    ss << "  \"keyframes\": [\n";
    
    for (size_t i = 0; i < m_keyframes.size(); ++i) {
        const auto& kf = m_keyframes[i];
        ss << "    {\n";
        ss << "      \"time\": " << kf.time << ",\n";
        ss << "      \"position\": {\"x\": " << kf.position.x << ", \"y\": " << kf.position.y << ", \"z\": " << kf.position.z << "},\n";
        ss << "      \"rotation\": {\"x\": " << kf.rotation.x << ", \"y\": " << kf.rotation.y << ", \"z\": " << kf.rotation.z << "},\n";
        ss << "      \"scale\": {\"x\": " << kf.scale.x << ", \"y\": " << kf.scale.y << ", \"z\": " << kf.scale.z << "},\n";
        ss << "      \"color\": {\"r\": " << kf.color.r << ", \"g\": " << kf.color.g << ", \"b\": " << kf.color.b << ", \"a\": " << kf.color.a << "},\n";
        ss << "      \"alpha\": " << kf.alpha << ",\n";
        ss << "      \"interpolation\": " << static_cast<int>(kf.interpolation) << "\n";
        ss << "    }";
        if (i < m_keyframes.size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "  ]\n";
    ss << "}";
    return ss.str();
}

void Animation::Deserialize(const std::string& data) {
    // В реальной реализации здесь бы парсился JSON
    std::cout << "Deserializing animation: " << m_name << std::endl;
}

Keyframe Animation::InterpolateKeyframes(const Keyframe& a, const Keyframe& b, float t) const {
    Keyframe result;
    result.time = a.time + t * (b.time - a.time);
    result.position = InterpolateVec3(a.position, b.position, t, a.interpolation);
    result.rotation = InterpolateVec3(a.rotation, b.rotation, t, a.interpolation);
    result.scale = InterpolateVec3(a.scale, b.scale, t, a.interpolation);
    result.color = InterpolateVec4(a.color, b.color, t, a.interpolation);
    result.alpha = InterpolateFloat(a.alpha, b.alpha, t, a.interpolation);
    result.interpolation = a.interpolation;
    return result;
}

float Animation::InterpolateFloat(float a, float b, float t, AnimationType type) const {
    switch (type) {
        case AnimationType::Linear:
            return a + t * (b - a);
        case AnimationType::EaseIn:
            return a + t * t * (b - a);
        case AnimationType::EaseOut:
            return a + (2 * t - t * t) * (b - a);
        case AnimationType::EaseInOut:
            return a + (t < 0.5f ? 2 * t * t : 1 - 2 * (1 - t) * (1 - t)) * (b - a);
        default:
            return a + t * (b - a);
    }
}

glm::vec3 Animation::InterpolateVec3(const glm::vec3& a, const glm::vec3& b, float t, AnimationType type) const {
    return glm::vec3(
        InterpolateFloat(a.x, b.x, t, type),
        InterpolateFloat(a.y, b.y, t, type),
        InterpolateFloat(a.z, b.z, t, type)
    );
}

glm::vec4 Animation::InterpolateVec4(const glm::vec4& a, const glm::vec4& b, float t, AnimationType type) const {
    return glm::vec4(
        InterpolateFloat(a.x, b.x, t, type),
        InterpolateFloat(a.y, b.y, t, type),
        InterpolateFloat(a.z, b.z, t, type),
        InterpolateFloat(a.w, b.w, t, type)
    );
}

// AnimationEditor implementation
AnimationEditor::AnimationEditor() 
    : m_currentTime(0.0f)
    , m_playing(false)
    , m_initialized(false) {
}

AnimationEditor::~AnimationEditor() {
    if (m_initialized) {
        // Очистка ресурсов
    }
}

bool AnimationEditor::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    m_initialized = true;
    
    std::cout << "AnimationEditor initialized successfully" << std::endl;
    return true;
}

void AnimationEditor::Update(float deltaTime) {
    if (!m_initialized) {
        return;
    }
    
    if (m_playing && !m_currentAnimation.empty()) {
        auto anim = GetAnimation(m_currentAnimation);
        if (anim) {
            m_currentTime += deltaTime;
            if (m_currentTime > anim->GetDuration()) {
                if (anim->IsLooping()) {
                    m_currentTime = 0.0f;
                } else {
                    m_playing = false;
                }
            }
        }
    }
    
    if (m_updateCallback) {
        m_updateCallback();
    }
}

void AnimationEditor::Render(void* renderer) {
    if (!m_initialized) {
        return;
    }
    
    std::cout << "=== Animation Editor ===" << std::endl;
    std::cout << "Current Animation: " << m_currentAnimation << std::endl;
    std::cout << "Current Time: " << m_currentTime << std::endl;
    std::cout << "Playing: " << (m_playing ? "Yes" : "No") << std::endl;
    std::cout << "Animations: " << m_animations.size() << std::endl;
    
    RenderTimeline(renderer);
    RenderKeyframes(renderer);
    RenderControls(renderer);
    
    std::cout << "=======================" << std::endl;
}

bool AnimationEditor::HandleInput(float mouseX, float mouseY, int mouseButton, int key) {
    if (!m_initialized) {
        return false;
    }
    
    switch (key) {
        case 'P':
            if (m_playing) {
                PauseAnimation();
            } else {
                PlayAnimation(m_currentAnimation);
            }
            return true;
        case 'S':
            StopAnimation();
            return true;
        case 'N':
            // Создаем новую анимацию
            {
                std::string name = "Animation_" + std::to_string(m_animations.size() + 1);
                CreateAnimation(name);
                m_currentAnimation = name;
            }
            return true;
    }
    
    return false;
}

std::shared_ptr<Animation> AnimationEditor::CreateAnimation(const std::string& name) {
    auto animation = std::make_shared<Animation>();
    animation->SetName(name);
    m_animations[name] = animation;
    
    std::cout << "Created animation: " << name << std::endl;
    return animation;
}

bool AnimationEditor::RemoveAnimation(const std::string& name) {
    auto it = m_animations.find(name);
    if (it == m_animations.end()) {
        return false;
    }
    
    m_animations.erase(it);
    
    if (m_currentAnimation == name) {
        m_currentAnimation.clear();
        m_playing = false;
    }
    
    std::cout << "Removed animation: " << name << std::endl;
    return true;
}

std::shared_ptr<Animation> AnimationEditor::GetAnimation(const std::string& name) {
    auto it = m_animations.find(name);
    return it != m_animations.end() ? it->second : nullptr;
}

void AnimationEditor::PlayAnimation(const std::string& name) {
    if (m_animations.find(name) != m_animations.end()) {
        m_currentAnimation = name;
        m_playing = true;
        std::cout << "Playing animation: " << name << std::endl;
    }
}

void AnimationEditor::PauseAnimation() {
    m_playing = false;
    std::cout << "Paused animation" << std::endl;
}

void AnimationEditor::StopAnimation() {
    m_playing = false;
    m_currentTime = 0.0f;
    std::cout << "Stopped animation" << std::endl;
}

void AnimationEditor::SetCurrentTime(float time) {
    m_currentTime = time;
    std::cout << "Set current time to: " << time << std::endl;
}

void AnimationEditor::RenderTimeline(void* renderer) {
    std::cout << "Rendering timeline" << std::endl;
}

void AnimationEditor::RenderKeyframes(void* renderer) {
    std::cout << "Rendering keyframes" << std::endl;
}

void AnimationEditor::RenderControls(void* renderer) {
    std::cout << "Rendering controls" << std::endl;
}

} // namespace FastEngine
