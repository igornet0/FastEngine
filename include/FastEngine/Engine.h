#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace FastEngine {
    class Application;
    class World;
    class Renderer;
    class AudioManager;
    class InputManager;
    class RenderSystem;
    
    class Engine {
    public:
        Engine();
        ~Engine();
        
        // Инициализация и завершение работы
        bool Initialize(const std::string& title, int width, int height);
        void Shutdown();
        
        // Основной игровой цикл
        void Run();
        
        // Один кадр (для iOS/внешнего цикла)
        void RunOneFrame();
        
        // Получение основных систем
        World* GetWorld() const { return m_world.get(); }
        Renderer* GetRenderer() const { return m_renderer.get(); }
        AudioManager* GetAudioManager() const { return m_audioManager.get(); }
        InputManager* GetInputManager() const { return m_inputManager.get(); }
        RenderSystem* GetRenderSystem() const { return m_renderSystem.get(); }
        
        // Управление состоянием
        bool IsRunning() const { return m_running; }
        void Stop() { m_running = false; }
        
        // Получение времени и счётчика кадров
        float GetDeltaTime() const { return m_deltaTime; }
        float GetFPS() const { return m_fps; }
        uint64_t GetFrameCount() const { return m_frameCount; }
        
        // Получение информации о платформе
        std::string GetPlatformName() const;
        
        /// Колбэк, вызываемый в конце каждого кадра после отрисовки мира (для UI, FPS и т.д.)
        void SetRenderCallback(std::function<void()> cb) { m_renderCallback = std::move(cb); }
        
    private:
        void Update(float deltaTime);
        void Render();
        
        std::unique_ptr<World> m_world;
        std::unique_ptr<Renderer> m_renderer;
        std::unique_ptr<AudioManager> m_audioManager;
        std::unique_ptr<InputManager> m_inputManager;
        std::unique_ptr<RenderSystem> m_renderSystem;
        
        bool m_running;
        float m_deltaTime;
        float m_fps;
        float m_lastFrameTime;
        uint64_t m_frameCount;
        std::function<void()> m_renderCallback;
    };
}
