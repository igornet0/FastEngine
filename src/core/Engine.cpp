#include "FastEngine/Engine.h"
#include "FastEngine/World.h"
#include "FastEngine/Render/Renderer.h"
#include "FastEngine/Audio/AudioManager.h"
#include "FastEngine/Input/InputManager.h"
#include "FastEngine/Platform/Platform.h"

namespace FastEngine {
    Engine::Engine() 
        : m_running(false)
        , m_deltaTime(0.0f)
        , m_fps(0.0f)
        , m_lastFrameTime(0.0f) {
    }
    
    Engine::~Engine() {
        Shutdown();
    }
    
    bool Engine::Initialize(const std::string& title, int width, int height) {
        // Инициализация платформы
        if (!Platform::GetInstance().Initialize()) {
            return false;
        }
        
        // Создание основных систем
        m_world = std::make_unique<World>();
        m_renderer = std::make_unique<Renderer>();
        m_audioManager = std::make_unique<AudioManager>();
        m_inputManager = std::make_unique<InputManager>();
        
        // Инициализация рендерера
        if (!m_renderer->Initialize(width, height)) {
            return false;
        }
        
        // Инициализация аудио менеджера
        if (!m_audioManager->Initialize()) {
            return false;
        }
        
        // Инициализация менеджера ввода
        if (!m_inputManager->Initialize()) {
            return false;
        }
        
        m_running = true;
        return true;
    }
    
    void Engine::Shutdown() {
        if (m_audioManager) {
            m_audioManager->Shutdown();
        }
        
        if (m_renderer) {
            m_renderer->Shutdown();
        }
        
        if (m_inputManager) {
            m_inputManager->Shutdown();
        }
        
        Platform::GetInstance().Shutdown();
        
        m_world.reset();
        m_renderer.reset();
        m_audioManager.reset();
        m_inputManager.reset();
        
        m_running = false;
    }
    
    void Engine::Run() {
        while (m_running && !Platform::GetInstance().ShouldClose()) {
            float currentTime = Platform::GetInstance().GetTimer()->GetTime();
            m_deltaTime = currentTime - m_lastFrameTime;
            m_lastFrameTime = currentTime;
            
            // Обновление FPS
            static float fpsTimer = 0.0f;
            static int frameCount = 0;
            fpsTimer += m_deltaTime;
            frameCount++;
            
            if (fpsTimer >= 1.0f) {
                m_fps = frameCount / fpsTimer;
                frameCount = 0;
                fpsTimer = 0.0f;
            }
            
            // Обработка событий
            Platform::GetInstance().PollEvents();
            
            // Обновление систем
            Update(m_deltaTime);
            
            // Отрисовка
            Render();
        }
    }
    
    void Engine::Update(float deltaTime) {
        if (m_world) {
            m_world->Update(deltaTime);
        }
        
        if (m_inputManager) {
            m_inputManager->Update(deltaTime);
        }
    }
    
    void Engine::Render() {
        if (m_renderer) {
            m_renderer->Clear();
            
            // Здесь будет логика отрисовки мира
            // m_renderer->RenderWorld(m_world.get());
            
            m_renderer->Present();
        }
    }
}
