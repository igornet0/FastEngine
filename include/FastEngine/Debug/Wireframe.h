#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace FastEngine {
    class Renderer;
    class Entity;
    
    struct WireframeLine {
        glm::vec3 start;
        glm::vec3 end;
        glm::vec4 color;
        
        WireframeLine(const glm::vec3& s, const glm::vec3& e, const glm::vec4& c)
            : start(s), end(e), color(c) {}
    };
    
    class Wireframe {
    public:
        static Wireframe& GetInstance();
        
        // Инициализация и завершение
        bool Initialize();
        void Shutdown();
        
        // Управление режимом
        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }
        
        void Toggle() { m_enabled = !m_enabled; }
        
        // Рендеринг
        void Render(Renderer* renderer);
        void Update(float deltaTime);
        
        // Добавление линий
        void AddLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = glm::vec4(1.0f));
        void AddBox(const glm::vec3& center, const glm::vec3& size, const glm::vec4& color = glm::vec4(1.0f));
        void AddCircle(const glm::vec3& center, float radius, const glm::vec4& color = glm::vec4(1.0f));
        void AddSphere(const glm::vec3& center, float radius, const glm::vec4& color = glm::vec4(1.0f));
        void AddCylinder(const glm::vec3& center, float radius, float height, const glm::vec4& color = glm::vec4(1.0f));
        
        // Очистка
        void Clear();
        void ClearLines();
        
        // Настройки
        void SetLineWidth(float width) { m_lineWidth = width; }
        float GetLineWidth() const { return m_lineWidth; }
        
        void SetDepthTest(bool enabled) { m_depthTest = enabled; }
        bool IsDepthTestEnabled() const { return m_depthTest; }
        
        void SetCulling(bool enabled) { m_culling = enabled; }
        bool IsCullingEnabled() const { return m_culling; }
        
        // Автоматическое добавление линий для сущностей
        void AddEntityWireframe(Entity* entity, const glm::vec4& color = glm::vec4(1.0f));
        void AddAllEntitiesWireframes();
        
        // Получение информации
        int GetLineCount() const { return static_cast<int>(m_lines.size()); }
        const std::vector<WireframeLine>& GetLines() const { return m_lines; }
        
    private:
        Wireframe() = default;
        ~Wireframe() = default;
        
        // Запрещаем копирование
        Wireframe(const Wireframe&) = delete;
        Wireframe& operator=(const Wireframe&) = delete;
        
        // Состояние
        bool m_enabled;
        bool m_initialized;
        std::vector<WireframeLine> m_lines;
        
        // Настройки
        float m_lineWidth;
        bool m_depthTest;
        bool m_culling;
        
        // Вспомогательные методы
        void AddBoxLines(const glm::vec3& center, const glm::vec3& size, const glm::vec4& color);
        void AddCircleLines(const glm::vec3& center, float radius, const glm::vec4& color);
        void AddSphereLines(const glm::vec3& center, float radius, const glm::vec4& color);
        void AddCylinderLines(const glm::vec3& center, float radius, float height, const glm::vec4& color);
    };
}

