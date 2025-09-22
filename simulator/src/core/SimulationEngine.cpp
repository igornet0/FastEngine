#include "ProjectSimulator/SimulationEngine.h"
#include "FastEngine/Engine.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>

namespace ProjectSimulator {

SimulationEngine::SimulationEngine()
    : m_activeProject(nullptr)
    , m_engine(nullptr)
    , m_state(SimulationState::Stopped)
    , m_logs("")
    , m_fps(0.0f)
    , m_frameTime(0.0f)
    , m_maxFPS(60.0f)
    , m_maxMemory(512)
    , m_initialized(false)
    , m_running(false) {
}

SimulationEngine::~SimulationEngine() {
    StopSimulation();
    if (m_engine) {
        delete m_engine;
        m_engine = nullptr;
    }
}

bool SimulationEngine::Initialize() {
    if (m_initialized) {
        return true;
    }

    try {
        // Создаем новый экземпляр FastEngine
        m_engine = new FastEngine::Engine();
        
        m_initialized = true;
        m_state = SimulationState::Stopped;
        
        AddLog("SimulationEngine initialized successfully");
        return true;

    } catch (const std::exception& e) {
        AddLog("Failed to initialize SimulationEngine: " + std::string(e.what()));
        return false;
    }
}

SimulationResult SimulationEngine::RunProject(std::shared_ptr<Project> project) {
    if (!m_initialized) {
        return {false, "SimulationEngine not initialized", 0.0f, -1, ""};
    }

    if (!project) {
        return {false, "Invalid project", 0.0f, -1, ""};
    }

    if (m_state == SimulationState::Running) {
        StopSimulation();
    }

    m_activeProject = project;
    m_state = SimulationState::Running;
    m_running = true;
    
    AddLog("Starting simulation for project: " + project->GetName());
    
    auto startTime = std::chrono::high_resolution_clock::now();
    SimulationResult result;
    
    try {
        // Инициализируем FastEngine для проекта
        if (!InitializeFastEngine(project)) {
            result = {false, "Failed to initialize FastEngine", 0.0f, -1, m_logs};
            m_state = SimulationState::Error;
            return result;
        }

        // Загружаем ассеты проекта
        if (!LoadProjectAssets(project)) {
            AddLog("Warning: Some assets failed to load");
        }

        // Запускаем главный цикл симуляции
        RunSimulationLoop();

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<float>(endTime - startTime).count();
        
        result = {true, "Simulation completed successfully", duration, 0, m_logs};
        
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<float>(endTime - startTime).count();
        
        result = {false, "Simulation failed: " + std::string(e.what()), duration, -1, m_logs};
        m_state = SimulationState::Error;
        HandleError(e.what());
    }

    m_state = SimulationState::Stopped;
    m_running = false;
    
    AddLog("Simulation ended with result: " + (result.success ? "SUCCESS" : "FAILURE"));
    
    return result;
}

void SimulationEngine::StopSimulation() {
    if (m_state == SimulationState::Running || m_state == SimulationState::Paused) {
        m_running = false;
        m_state = SimulationState::Stopped;
        
        if (m_engine) {
            m_engine->Stop();
        }
        
        AddLog("Simulation stopped");
    }
}

void SimulationEngine::PauseSimulation() {
    if (m_state == SimulationState::Running) {
        m_state = SimulationState::Paused;
        AddLog("Simulation paused");
    }
}

void SimulationEngine::ResumeSimulation() {
    if (m_state == SimulationState::Paused) {
        m_state = SimulationState::Running;
        AddLog("Simulation resumed");
    }
}

void SimulationEngine::SetUpdateCallback(std::function<void(float)> callback) {
    m_updateCallback = callback;
}

void SimulationEngine::SetErrorCallback(std::function<void(const std::string&)> callback) {
    m_errorCallback = callback;
}

bool SimulationEngine::CheckCompatibility(std::shared_ptr<Project> project, const std::string& platform) const {
    if (!project) {
        return false;
    }

    // Проверяем основные зависимости
    const auto& dependencies = project->GetDependencies();
    
    // FastEngine всегда требуется
    bool hasFastEngine = std::find(dependencies.begin(), dependencies.end(), "FastEngine") != dependencies.end();
    if (!hasFastEngine) {
        return false;
    }

    // Проверяем платформо-специфичные зависимости
    if (platform == "iOS") {
        // iOS специфичные проверки
        return true; // Пока всегда true
    } else if (platform == "Android") {
        // Android специфичные проверки
        return true; // Пока всегда true
    }

    return true;
}

bool SimulationEngine::InitializeFastEngine(std::shared_ptr<Project> project) {
    if (!m_engine) {
        return false;
    }

    // Получаем настройки проекта
    std::string windowTitle = project->GetSetting("window_title");
    if (windowTitle.empty()) {
        windowTitle = project->GetName();
    }

    int windowWidth = 800;
    int windowHeight = 600;
    
    try {
        windowWidth = std::stoi(project->GetSetting("window_width"));
    } catch (...) {
        // Используем значение по умолчанию
    }
    
    try {
        windowHeight = std::stoi(project->GetSetting("window_height"));
    } catch (...) {
        // Используем значение по умолчанию
    }

    // Инициализируем движок
    if (!m_engine->Initialize(windowTitle, windowWidth, windowHeight)) {
        AddLog("Failed to initialize FastEngine");
        return false;
    }

    AddLog("FastEngine initialized with window: " + std::to_string(windowWidth) + "x" + std::to_string(windowHeight));
    return true;
}

bool SimulationEngine::LoadProjectAssets(std::shared_ptr<Project> project) {
    if (!project) {
        return false;
    }

    int loadedCount = 0;
    int totalCount = project->GetAssets().size();

    for (const auto& asset : project->GetAssets()) {
        std::string assetPath = project->GetAssetPath(asset);
        
        if (std::filesystem::exists(assetPath)) {
            AddLog("Loaded asset: " + asset);
            loadedCount++;
        } else {
            AddLog("Warning: Asset not found: " + assetPath);
        }
    }

    AddLog("Loaded " + std::to_string(loadedCount) + "/" + std::to_string(totalCount) + " assets");
    return loadedCount > 0;
}

void SimulationEngine::RunSimulationLoop() {
    if (!m_engine || !m_running) {
        return;
    }

    auto lastTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    float fpsTimer = 0.0f;

    while (m_running && m_engine->IsRunning()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Ограничиваем FPS
        if (deltaTime < 1.0f / m_maxFPS) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        // Обновляем FPS
        frameCount++;
        fpsTimer += deltaTime;
        if (fpsTimer >= 1.0f) {
            m_fps = frameCount / fpsTimer;
            frameCount = 0;
            fpsTimer = 0.0f;
        }

        m_frameTime = deltaTime;

        // Вызываем callback обновления
        if (m_updateCallback) {
            m_updateCallback(deltaTime);
        }

        // Обновляем движок
        m_engine->Update();

        // Проверяем состояние
        if (m_state == SimulationState::Paused) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }
    }
}

void SimulationEngine::HandleError(const std::string& error) {
    AddLog("ERROR: " + error);
    
    if (m_errorCallback) {
        m_errorCallback(error);
    }
}

void SimulationEngine::AddLog(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    char timeStr[64];
    std::strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &tm);
    
    m_logs += "[" + std::string(timeStr) + "] " + message + "\n";
    
    // Ограничиваем размер лога
    if (m_logs.length() > 10000) {
        m_logs = m_logs.substr(m_logs.length() - 5000);
    }
}

} // namespace ProjectSimulator

