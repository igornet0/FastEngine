#pragma once

namespace FastEngine {
    class Timer {
    public:
        Timer();
        virtual ~Timer();
        
        // Инициализация
        virtual bool Initialize();
        virtual void Shutdown();
        
        // Получение времени
        virtual float GetTime() const;           // Время в секундах с момента инициализации
        virtual float GetDeltaTime() const;      // Время между кадрами
        virtual float GetFPS() const;            // FPS
        
        // Управление временем
        virtual void Reset();
        virtual void Pause();
        virtual void Resume();
        bool IsPaused() const { return m_paused; }
        
        // Обновление (вызывается каждый кадр)
        virtual void Update();
        
    protected:
        float m_startTime;
        float m_lastTime;
        float m_deltaTime;
        float m_fps;
        bool m_paused;
        bool m_initialized;
    private:
        float GetCurrentTime() const;
    };
}
