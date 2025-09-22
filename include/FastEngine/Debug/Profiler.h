#pragma once

#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <memory>
#include <glm/glm.hpp>

namespace FastEngine {
    class Renderer;
    class Text;
    
    struct ProfileData {
        std::string name;
        double totalTime;
        double averageTime;
        double minTime;
        double maxTime;
        int callCount;
        double lastTime;
        
        ProfileData() : totalTime(0.0), averageTime(0.0), minTime(0.0), maxTime(0.0), callCount(0), lastTime(0.0) {}
    };
    
    class Profiler {
    public:
        static Profiler& GetInstance();
        
        // Инициализация и завершение
        bool Initialize();
        void Shutdown();
        
        // Управление профилированием
        void StartProfile(const std::string& name);
        void EndProfile(const std::string& name);
        
        // Автоматическое профилирование с RAII
        class ScopedTimer {
        public:
            ScopedTimer(const std::string& name) : m_name(name) {
                Profiler::GetInstance().StartProfile(name);
            }
            ~ScopedTimer() {
                Profiler::GetInstance().EndProfile(m_name);
            }
        private:
            std::string m_name;
        };
        
        // Получение данных
        const ProfileData* GetProfileData(const std::string& name) const;
        const std::map<std::string, ProfileData>& GetAllProfileData() const { return m_profiles; }
        
        // Статистика
        double GetTotalTime() const;
        double GetAverageFPS() const;
        int GetFrameCount() const;
        
        // Обновление и рендеринг
        void Update(float deltaTime);
        void Render(Renderer* renderer);
        
        // Настройки
        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }
        
        void SetMaxHistory(int max) { m_maxHistory = max; }
        int GetMaxHistory() const { return m_maxHistory; }
        
        void SetDisplayMode(int mode) { m_displayMode = mode; }
        int GetDisplayMode() const { return m_displayMode; }
        
        // Очистка
        void Clear();
        void ResetProfile(const std::string& name);
        
        // Экспорт данных
        void ExportToCSV(const std::string& filename) const;
        void ExportToJSON(const std::string& filename) const;
        
        // Фильтрация
        void SetMinTime(double minTime) { m_minTime = minTime; }
        double GetMinTime() const { return m_minTime; }
        
        // События
        void OnFrameStart();
        void OnFrameEnd();
        
    private:
        Profiler() = default;
        ~Profiler() = default;
        
        // Запрещаем копирование
        Profiler(const Profiler&) = delete;
        Profiler& operator=(const Profiler&) = delete;
        
        // Состояние
        bool m_enabled;
        bool m_initialized;
        int m_displayMode; // 0 = Simple, 1 = Detailed, 2 = Graph
        int m_maxHistory;
        double m_minTime;
        
        // Профили
        std::map<std::string, ProfileData> m_profiles;
        std::map<std::string, std::chrono::high_resolution_clock::time_point> m_activeProfiles;
        
        // Статистика кадров
        std::vector<double> m_frameTimes;
        double m_totalFrameTime;
        int m_frameCount;
        
        // Вспомогательные методы
        void UpdateProfileData(const std::string& name, double time);
        void RenderSimple(Renderer* renderer);
        void RenderDetailed(Renderer* renderer);
        void RenderGraph(Renderer* renderer);
        std::string FormatTime(double time) const;
        glm::vec4 GetColorForTime(double time) const;
    };
    
    // Макрос для удобного профилирования
    #define PROFILE_SCOPE(name) Profiler::ScopedTimer _profiler_timer(name)
    #define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
}

