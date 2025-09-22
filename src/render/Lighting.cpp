#include "FastEngine/Render/Lighting.h"

namespace FastEngine {
    LightingSystem::LightingSystem() 
        : m_ambientColor(0.1f, 0.1f, 0.1f)
        , m_ambientIntensity(0.3f) {
    }
    
    LightingSystem::~LightingSystem() = default;
    
    void LightingSystem::AddLight(const Light& light) {
        m_lights.push_back(light);
    }
    
    void LightingSystem::RemoveLight(size_t index) {
        if (index < m_lights.size()) {
            m_lights.erase(m_lights.begin() + index);
        }
    }
    
    void LightingSystem::ClearLights() {
        m_lights.clear();
    }
    
    void LightingSystem::SetAmbientLight(const glm::vec3& color, float intensity) {
        m_ambientColor = color;
        m_ambientIntensity = intensity;
    }
    
    void LightingSystem::Update() {
        // Здесь можно добавить логику обновления освещения
        // Например, обновление позиций динамических источников света
    }
}

