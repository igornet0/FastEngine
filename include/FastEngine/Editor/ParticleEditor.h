#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

namespace FastEngine {

/**
 * Параметры частиц
 */
struct ParticleParameters {
    float lifetime;
    float speed;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec4 startColor;
    glm::vec4 endColor;
    float startSize;
    float endSize;
    float rotation;
    float rotationSpeed;
    
    ParticleParameters() 
        : lifetime(1.0f)
        , speed(1.0f)
        , velocity(0.0f)
        , acceleration(0.0f, -9.81f, 0.0f)
        , startColor(1.0f)
        , endColor(1.0f)
        , startSize(1.0f)
        , endSize(0.0f)
        , rotation(0.0f)
        , rotationSpeed(0.0f) {}
};

/**
 * Эмиттер частиц
 */
class ParticleEmitter {
public:
    ParticleEmitter();
    ~ParticleEmitter() = default;
    
    std::string GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    glm::vec3 GetPosition() const { return m_position; }
    void SetPosition(const glm::vec3& position) { m_position = position; }
    
    float GetEmissionRate() const { return m_emissionRate; }
    void SetEmissionRate(float rate) { m_emissionRate = rate; }
    
    int GetMaxParticles() const { return m_maxParticles; }
    void SetMaxParticles(int max) { m_maxParticles = max; }
    
    const ParticleParameters& GetParameters() const { return m_parameters; }
    void SetParameters(const ParticleParameters& params) { m_parameters = params; }
    
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    void Update(float deltaTime);
    void Render(void* renderer);
    
    std::string Serialize() const;
    void Deserialize(const std::string& data);
    
private:
    std::string m_name;
    glm::vec3 m_position;
    float m_emissionRate;
    int m_maxParticles;
    ParticleParameters m_parameters;
    bool m_enabled;
    
    float m_emissionTimer;
    std::vector<struct Particle> m_particles;
    
    void EmitParticle();
    void UpdateParticle(struct Particle& particle, float deltaTime);
};

/**
 * Редактор частиц
 */
class ParticleEditor {
public:
    ParticleEditor();
    ~ParticleEditor();

    bool Initialize();
    void Update(float deltaTime);
    void Render(void* renderer);
    bool HandleInput(float mouseX, float mouseY, int mouseButton, int key);
    
    std::shared_ptr<ParticleEmitter> CreateEmitter(const std::string& name, const glm::vec3& position);
    bool RemoveEmitter(const std::string& name);
    std::shared_ptr<ParticleEmitter> GetEmitter(const std::string& name);
    
    void PlayAll();
    void PauseAll();
    void StopAll();
    
    void SetUpdateCallback(std::function<void()> callback) { m_updateCallback = callback; }
    
private:
    std::map<std::string, std::shared_ptr<ParticleEmitter>> m_emitters;
    bool m_initialized;
    bool m_playing;
    
    std::function<void()> m_updateCallback;
    
    void RenderEmitters(void* renderer);
    void RenderPreview(void* renderer);
    void RenderControls(void* renderer);
};

} // namespace FastEngine

