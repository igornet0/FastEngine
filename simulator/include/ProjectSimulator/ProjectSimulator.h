#pragma once

#include "ProjectManager.h"
#include "SimulationEngine.h"
#include "UIManager.h"
#include "TestRunner.h"

namespace ProjectSimulator {

/**
 * Главный класс симулятора проектов
 * Управляет всеми компонентами симулятора
 */
class ProjectSimulator {
public:
    ProjectSimulator();
    ~ProjectSimulator();

    /**
     * Инициализация симулятора
     * @param windowWidth ширина окна
     * @param windowHeight высота окна
     * @return true если инициализация успешна
     */
    bool Initialize(int windowWidth = 1024, int windowHeight = 768);

    /**
     * Запуск главного цикла симулятора
     */
    void Run();

    /**
     * Остановка симулятора
     */
    void Shutdown();

    /**
     * Получение менеджера проектов
     */
    ProjectManager* GetProjectManager() const { return m_projectManager; }

    /**
     * Получение движка симуляции
     */
    SimulationEngine* GetSimulationEngine() const { return m_simulationEngine; }

    /**
     * Получение UI менеджера
     */
    UIManager* GetUIManager() const { return m_uiManager; }

    /**
     * Получение тест раннера
     */
    TestRunner* GetTestRunner() const { return m_testRunner; }

private:
    ProjectManager* m_projectManager;
    SimulationEngine* m_simulationEngine;
    UIManager* m_uiManager;
    TestRunner* m_testRunner;
    
    bool m_initialized;
    bool m_running;
};

} // namespace ProjectSimulator

