#include "FastEngine/Platform/Timer.h"
#include <chrono>

namespace FastEngine {
    Timer::Timer() 
        : m_startTime(0.0f)
        , m_lastTime(0.0f)
        , m_deltaTime(0.0f)
        , m_fps(0.0f)
        , m_paused(false)
        , m_initialized(false) {
    }
    
    Timer::~Timer() {
        Shutdown();
    }
    
    bool Timer::Initialize() {
        if (m_initialized) {
            return true;
        }
        
        m_startTime = GetCurrentTime();
        m_lastTime = m_startTime;
        m_initialized = true;
        return true;
    }
    
    void Timer::Shutdown() {
        m_initialized = false;
    }
    
    float Timer::GetTime() const {
        if (!m_initialized) return 0.0f;
        return GetCurrentTime() - m_startTime;
    }
    
    float Timer::GetDeltaTime() const {
        return m_deltaTime;
    }
    
    float Timer::GetFPS() const {
        return m_fps;
    }
    
    void Timer::Reset() {
        m_startTime = GetCurrentTime();
        m_lastTime = m_startTime;
    }
    
    void Timer::Pause() {
        m_paused = true;
    }
    
    void Timer::Resume() {
        m_paused = false;
    }
    
    void Timer::Update() {
        if (!m_initialized || m_paused) return;
        
        float currentTime = GetCurrentTime();
        m_deltaTime = currentTime - m_lastTime;
        m_lastTime = currentTime;
        
        // Обновляем FPS
        static float fpsTimer = 0.0f;
        static int frameCount = 0;
        fpsTimer += m_deltaTime;
        frameCount++;
        
        if (fpsTimer >= 1.0f) {
            m_fps = frameCount / fpsTimer;
            frameCount = 0;
            fpsTimer = 0.0f;
        }
    }
    
    float Timer::GetCurrentTime() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000000.0f;
    }
}
