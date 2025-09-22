#pragma once

namespace FastEngine {
    class Timer {
    public:
        Timer();
        ~Timer();
        
        // Инициализация
        bool Initialize();
        void Shutdown();
        
        // Получение времени
        float GetTime() const;           // Время в секундах с момента инициализации
        float GetDeltaTime() const;      // Время между кадрами
        float GetFPS() const;            // FPS
        
        // Управление временем
        void Reset();
        void Pause();
        void Resume();
        bool IsPaused() const { return m_paused; }
        
        // Обновление (вызывается каждый кадр)
        void Update();
        
    private:
        float GetCurrentTime() const;
        
        float m_startTime;
        float m_lastTime;
        float m_deltaTime;
        float m_fps;
        bool m_paused;
        bool m_initialized;
    };
}
