#pragma once

#include <memory>
#include <string>

namespace FastEngine {
    class Application;
    class World;
    class Renderer;
    class AudioManager;
    class InputManager;
    
    class Engine {
    public:
        Engine();
        ~Engine();
        
        // Инициализация и завершение работы
        bool Initialize(const std::string& title, int width, int height);
        void Shutdown();
        
        // Основной игровой цикл
        void Run();
        
        // Получение основных систем
        World* GetWorld() const { return m_world.get(); }
        Renderer* GetRenderer() const { return m_renderer.get(); }
        AudioManager* GetAudioManager() const { return m_audioManager.get(); }
        InputManager* GetInputManager() const { return m_inputManager.get(); }
        
        // Управление состоянием
        bool IsRunning() const { return m_running; }
        void Stop() { m_running = false; }
        
        // Получение времени
        float GetDeltaTime() const { return m_deltaTime; }
        float GetFPS() const { return m_fps; }
        
    private:
        void Update(float deltaTime);
        void Render();
        
        std::unique_ptr<World> m_world;
        std::unique_ptr<Renderer> m_renderer;
        std::unique_ptr<AudioManager> m_audioManager;
        std::unique_ptr<InputManager> m_inputManager;
        
        bool m_running;
        float m_deltaTime;
        float m_fps;
        float m_lastFrameTime;
    };
}
