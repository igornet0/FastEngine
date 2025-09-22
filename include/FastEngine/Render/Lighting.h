#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace FastEngine {
    enum class LightType {
        Directional,
        Point,
        Spot
    };
    
    struct Light {
        LightType type;
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 color;
        float intensity;
        float range;
        float innerCone;
        float outerCone;
        bool enabled;
        
        Light() 
            : type(LightType::Point)
            , position(0.0f, 0.0f, 0.0f)
            , direction(0.0f, -1.0f, 0.0f)
            , color(1.0f, 1.0f, 1.0f)
            , intensity(1.0f)
            , range(10.0f)
            , innerCone(12.5f)
            , outerCone(17.5f)
            , enabled(true) {}
    };
    
    class LightingSystem {
    public:
        LightingSystem();
        ~LightingSystem();
        
        // Управление освещением
        void AddLight(const Light& light);
        void RemoveLight(size_t index);
        void ClearLights();
        
        // Получение освещения
        const std::vector<Light>& GetLights() const { return m_lights; }
        size_t GetLightCount() const { return m_lights.size(); }
        
        // Настройки освещения
        void SetAmbientLight(const glm::vec3& color, float intensity);
        glm::vec3 GetAmbientLight() const { return m_ambientColor; }
        float GetAmbientIntensity() const { return m_ambientIntensity; }
        
        // Обновление освещения
        void Update();
        
    private:
        std::vector<Light> m_lights;
        glm::vec3 m_ambientColor;
        float m_ambientIntensity;
    };
}

