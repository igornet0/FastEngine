#pragma once

#include <memory>
#include <string>
#include <functional>
#include "Project.h"

// Forward declarations
namespace FastEngine {
    class Engine;
    class World;
}

namespace ProjectSimulator {

/**
 * Состояния симуляции
 */
enum class SimulationState {
    Stopped,
    Running,
    Paused,
    Error
};

/**
 * Результат симуляции
 */
struct SimulationResult {
    bool success;
    std::string errorMessage;
    float duration;
    int exitCode;
    std::string logOutput;
};

/**
 * Движок симуляции
 * Управляет запуском и выполнением проектов
 */
class SimulationEngine {
public:
    SimulationEngine();
    ~SimulationEngine();

    /**
     * Инициализация движка симуляции
     * @return true если инициализация успешна
     */
    bool Initialize();

    /**
     * Запуск симуляции проекта
     * @param project проект для симуляции
     * @return результат симуляции
     */
    SimulationResult RunProject(std::shared_ptr<Project> project);

    /**
     * Остановка текущей симуляции
     */
    void StopSimulation();

    /**
     * Пауза/возобновление симуляции
     */
    void PauseSimulation();
    void ResumeSimulation();

    /**
     * Получение текущего состояния
     */
    SimulationState GetState() const { return m_state; }

    /**
     * Получение активного проекта
     */
    std::shared_ptr<Project> GetActiveProject() const { return m_activeProject; }

    /**
     * Установка callback для обновления UI
     * @param callback функция обратного вызова
     */
    void SetUpdateCallback(std::function<void(float)> callback);

    /**
     * Установка callback для обработки ошибок
     * @param callback функция обратного вызова
     */
    void SetErrorCallback(std::function<void(const std::string&)> callback);

    /**
     * Получение логов симуляции
     */
    const std::string& GetLogs() const { return m_logs; }

    /**
     * Очистка логов
     */
    void ClearLogs() { m_logs.clear(); }

    /**
     * Получение производительности симуляции
     */
    float GetFPS() const { return m_fps; }
    float GetFrameTime() const { return m_frameTime; }

    /**
     * Установка ограничений симуляции
     * @param maxFPS максимальный FPS
     * @param maxMemory максимальная память в MB
     */
    void SetLimits(float maxFPS = 60.0f, int maxMemory = 512);

    /**
     * Проверка совместимости проекта с платформой
     * @param project проект для проверки
     * @param platform целевая платформа
     * @return true если проект совместим
     */
    bool CheckCompatibility(std::shared_ptr<Project> project, const std::string& platform) const;

private:
    std::shared_ptr<Project> m_activeProject;
    std::unique_ptr<FastEngine::Engine> m_engine;
    SimulationState m_state;
    std::string m_logs;
    float m_fps;
    float m_frameTime;
    float m_maxFPS;
    int m_maxMemory;
    
    std::function<void(float)> m_updateCallback;
    std::function<void(const std::string&)> m_errorCallback;
    
    bool m_initialized;
    bool m_running;

    /**
     * Инициализация FastEngine для проекта
     */
    bool InitializeFastEngine(std::shared_ptr<Project> project);

    /**
     * Загрузка ассетов проекта
     */
    bool LoadProjectAssets(std::shared_ptr<Project> project);

    /**
     * Выполнение главного цикла симуляции
     */
    void RunSimulationLoop();

    /**
     * Обработка ошибок
     */
    void HandleError(const std::string& error);
};

} // namespace ProjectSimulator

