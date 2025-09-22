#include "FastEngine/Editor/ParticleEditor.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <sstream>

namespace FastEngine {

// Forward declaration
struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float size;
    float rotation;
    float lifetime;
    float maxLifetime;
    
    Particle() : position(0.0f), velocity(0.0f), color(1.0f), size(1.0f), 
                 rotation(0.0f), lifetime(0.0f), maxLifetime(1.0f) {}
};

// ParticleEmitter implementation
ParticleEmitter::ParticleEmitter() 
    : m_position(0.0f)
    , m_emissionRate(10.0f)
    , m_maxParticles(1000)
    , m_enabled(true)
    , m_emissionTimer(0.0f) {
}

void ParticleEmitter::Update(float deltaTime) {
    if (!m_enabled) {
        return;
    }
    
    // Обновляем таймер эмиссии
    m_emissionTimer += deltaTime;
    
    // Эмитируем новые частицы
    float emissionInterval = 1.0f / m_emissionRate;
    while (m_emissionTimer >= emissionInterval && m_particles.size() < m_maxParticles) {
        EmitParticle();
        m_emissionTimer -= emissionInterval;
    }
    
    // Обновляем существующие частицы
    for (auto it = m_particles.begin(); it != m_particles.end();) {
        UpdateParticle(*it, deltaTime);
        
        if (it->lifetime <= 0.0f) {
            it = m_particles.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleEmitter::Render(void* renderer) {
    // В реальной реализации здесь бы рендерились частицы
    std::cout << "Rendering " << m_particles.size() << " particles from emitter: " << m_name << std::endl;
}

std::string ParticleEmitter::Serialize() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"name\": \"" << m_name << "\",\n";
    ss << "  \"position\": {\"x\": " << m_position.x << ", \"y\": " << m_position.y << ", \"z\": " << m_position.z << "},\n";
    ss << "  \"emissionRate\": " << m_emissionRate << ",\n";
    ss << "  \"maxParticles\": " << m_maxParticles << ",\n";
    ss << "  \"enabled\": " << (m_enabled ? "true" : "false") << ",\n";
    ss << "  \"parameters\": {\n";
    ss << "    \"lifetime\": " << m_parameters.lifetime << ",\n";
    ss << "    \"speed\": " << m_parameters.speed << ",\n";
    ss << "    \"velocity\": {\"x\": " << m_parameters.velocity.x << ", \"y\": " << m_parameters.velocity.y << ", \"z\": " << m_parameters.velocity.z << "},\n";
    ss << "    \"acceleration\": {\"x\": " << m_parameters.acceleration.x << ", \"y\": " << m_parameters.acceleration.y << ", \"z\": " << m_parameters.acceleration.z << "},\n";
    ss << "    \"startColor\": {\"r\": " << m_parameters.startColor.r << ", \"g\": " << m_parameters.startColor.g << ", \"b\": " << m_parameters.startColor.b << ", \"a\": " << m_parameters.startColor.a << "},\n";
    ss << "    \"endColor\": {\"r\": " << m_parameters.endColor.r << ", \"g\": " << m_parameters.endColor.g << ", \"b\": " << m_parameters.endColor.b << ", \"a\": " << m_parameters.endColor.a << "},\n";
    ss << "    \"startSize\": " << m_parameters.startSize << ",\n";
    ss << "    \"endSize\": " << m_parameters.endSize << ",\n";
    ss << "    \"rotation\": " << m_parameters.rotation << ",\n";
    ss << "    \"rotationSpeed\": " << m_parameters.rotationSpeed << "\n";
    ss << "  }\n";
    ss << "}";
    return ss.str();
}

void ParticleEmitter::Deserialize(const std::string& data) {
    std::cout << "Deserializing particle emitter: " << m_name << std::endl;
}

void ParticleEmitter::EmitParticle() {
    Particle particle;
    particle.position = m_position;
    particle.velocity = m_parameters.velocity + glm::vec3(
        (rand() % 100 - 50) / 100.0f,
        (rand() % 100 - 50) / 100.0f,
        (rand() % 100 - 50) / 100.0f
    ) * m_parameters.speed;
    particle.color = m_parameters.startColor;
    particle.size = m_parameters.startSize;
    particle.rotation = m_parameters.rotation;
    particle.lifetime = m_parameters.lifetime;
    particle.maxLifetime = m_parameters.lifetime;
    
    m_particles.push_back(particle);
}

void ParticleEmitter::UpdateParticle(Particle& particle, float deltaTime) {
    // Обновляем позицию
    particle.position += particle.velocity * deltaTime;
    
    // Обновляем скорость
    particle.velocity += m_parameters.acceleration * deltaTime;
    
    // Обновляем размер
    float t = 1.0f - (particle.lifetime / particle.maxLifetime);
    particle.size = m_parameters.startSize + t * (m_parameters.endSize - m_parameters.startSize);
    
    // Обновляем цвет
    particle.color = m_parameters.startColor + t * (m_parameters.endColor - m_parameters.startColor);
    
    // Обновляем поворот
    particle.rotation += m_parameters.rotationSpeed * deltaTime;
    
    // Обновляем время жизни
    particle.lifetime -= deltaTime;
}

// ParticleEditor implementation
ParticleEditor::ParticleEditor() 
    : m_initialized(false)
    , m_playing(false) {
}

ParticleEditor::~ParticleEditor() {
    if (m_initialized) {
        // Очистка ресурсов
    }
}

bool ParticleEditor::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    m_initialized = true;
    
    std::cout << "ParticleEditor initialized successfully" << std::endl;
    return true;
}

void ParticleEditor::Update(float deltaTime) {
    if (!m_initialized) {
        return;
    }
    
    if (m_playing) {
        // Обновляем все эмиттеры
        for (auto& pair : m_emitters) {
            if (pair.second) {
                pair.second->Update(deltaTime);
            }
        }
    }
    
    if (m_updateCallback) {
        m_updateCallback();
    }
}

void ParticleEditor::Render(void* renderer) {
    if (!m_initialized) {
        return;
    }
    
    std::cout << "=== Particle Editor ===" << std::endl;
    std::cout << "Playing: " << (m_playing ? "Yes" : "No") << std::endl;
    std::cout << "Emitters: " << m_emitters.size() << std::endl;
    
    RenderEmitters(renderer);
    RenderPreview(renderer);
    RenderControls(renderer);
    
    std::cout << "======================" << std::endl;
}

bool ParticleEditor::HandleInput(float mouseX, float mouseY, int mouseButton, int key) {
    if (!m_initialized) {
        return false;
    }
    
    switch (key) {
        case 'P':
            if (m_playing) {
                PauseAll();
            } else {
                PlayAll();
            }
            return true;
        case 'S':
            StopAll();
            return true;
        case 'N':
            // Создаем новый эмиттер
            {
                std::string name = "Emitter_" + std::to_string(m_emitters.size() + 1);
                CreateEmitter(name, glm::vec3(mouseX, mouseY, 0.0f));
            }
            return true;
    }
    
    return false;
}

std::shared_ptr<ParticleEmitter> ParticleEditor::CreateEmitter(const std::string& name, const glm::vec3& position) {
    auto emitter = std::make_shared<ParticleEmitter>();
    emitter->SetName(name);
    emitter->SetPosition(position);
    
    m_emitters[name] = emitter;
    
    std::cout << "Created particle emitter: " << name << " at (" 
              << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    return emitter;
}

bool ParticleEditor::RemoveEmitter(const std::string& name) {
    auto it = m_emitters.find(name);
    if (it == m_emitters.end()) {
        return false;
    }
    
    m_emitters.erase(it);
    
    std::cout << "Removed particle emitter: " << name << std::endl;
    return true;
}

std::shared_ptr<ParticleEmitter> ParticleEditor::GetEmitter(const std::string& name) {
    auto it = m_emitters.find(name);
    return it != m_emitters.end() ? it->second : nullptr;
}

void ParticleEditor::PlayAll() {
    m_playing = true;
    std::cout << "Playing all particle emitters" << std::endl;
}

void ParticleEditor::PauseAll() {
    m_playing = false;
    std::cout << "Paused all particle emitters" << std::endl;
}

void ParticleEditor::StopAll() {
    m_playing = false;
    // Очищаем все частицы
    for (auto& pair : m_emitters) {
        if (pair.second) {
            // В реальной реализации здесь бы очищались частицы
        }
    }
    std::cout << "Stopped all particle emitters" << std::endl;
}

void ParticleEditor::RenderEmitters(void* renderer) {
    for (const auto& pair : m_emitters) {
        const auto& emitter = pair.second;
        if (emitter) {
            emitter->Render(renderer);
        }
    }
}

void ParticleEditor::RenderPreview(void* renderer) {
    std::cout << "Rendering particle preview" << std::endl;
}

void ParticleEditor::RenderControls(void* renderer) {
    std::cout << "Rendering particle controls" << std::endl;
}

} // namespace FastEngine
