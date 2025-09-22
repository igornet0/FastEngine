#include "ProjectSimulator/ProjectSimulator.h"
#include <iostream>
#include <stdexcept>

namespace ProjectSimulator {

ProjectSimulator::ProjectSimulator()
    : m_projectManager(nullptr)
    , m_simulationEngine(nullptr)
    , m_uiManager(nullptr)
    , m_testRunner(nullptr)
    , m_initialized(false)
    , m_running(false) {
}

ProjectSimulator::~ProjectSimulator() {
    Shutdown();
}

bool ProjectSimulator::Initialize(int windowWidth, int windowHeight) {
    if (m_initialized) {
        return true;
    }

    try {
        // Инициализируем менеджер проектов
        m_projectManager = new ProjectManager();
        if (!m_projectManager->Initialize("./projects")) {
            std::cerr << "Failed to initialize ProjectManager" << std::endl;
            return false;
        }

        // Инициализируем движок симуляции
        m_simulationEngine = new SimulationEngine();
        if (!m_simulationEngine->Initialize()) {
            std::cerr << "Failed to initialize SimulationEngine" << std::endl;
            return false;
        }

        // Инициализируем UI менеджер
        m_uiManager = new UIManager();
        if (!m_uiManager->Initialize(windowWidth, windowHeight)) {
            std::cerr << "Failed to initialize UIManager" << std::endl;
            return false;
        }

        // Инициализируем тест раннер
        m_testRunner = new TestRunner();
        if (!m_testRunner->Initialize()) {
            std::cerr << "Failed to initialize TestRunner" << std::endl;
            return false;
        }

        // Настраиваем callback'и
        m_uiManager->SetEventCallback([this](const UIEvent& event) {
            HandleUIEvent(event);
        });

        m_simulationEngine->SetUpdateCallback([this](float deltaTime) {
            if (m_uiManager) {
                m_uiManager->Update(deltaTime);
            }
        });

        m_simulationEngine->SetErrorCallback([this](const std::string& error) {
            if (m_uiManager) {
                m_uiManager->AddLogMessage("Simulation Error: " + error, "error");
            }
        });

        // Загружаем проекты
        int projectCount = m_projectManager->LoadAllProjects();
        std::cout << "Loaded " << projectCount << " projects" << std::endl;

        // Показываем главное меню
        m_uiManager->ShowMainMenu();

        m_initialized = true;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Exception during initialization: " << e.what() << std::endl;
        return false;
    }
}

void ProjectSimulator::Run() {
    if (!m_initialized) {
        std::cerr << "Simulator not initialized" << std::endl;
        return;
    }

    m_running = true;
    std::cout << "Starting Project Simulator..." << std::endl;

    // Главный цикл
    while (m_running) {
        // Обновляем UI
        if (m_uiManager) {
            m_uiManager->Update(0.016f); // ~60 FPS
            m_uiManager->Render();
        }

        // Обновляем симуляцию
        if (m_simulationEngine && m_simulationEngine->GetState() == SimulationState::Running) {
            // Симуляция обновляется через callback
        }

        // Небольшая задержка для предотвращения 100% загрузки CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void ProjectSimulator::Shutdown() {
    if (!m_initialized) {
        return;
    }

    m_running = false;

    // Останавливаем симуляцию
    if (m_simulationEngine) {
        m_simulationEngine->StopSimulation();
    }

    // Очищаем ресурсы
    if (m_testRunner) {
        delete m_testRunner;
        m_testRunner = nullptr;
    }

    if (m_uiManager) {
        delete m_uiManager;
        m_uiManager = nullptr;
    }

    if (m_simulationEngine) {
        delete m_simulationEngine;
        m_simulationEngine = nullptr;
    }

    if (m_projectManager) {
        delete m_projectManager;
        m_projectManager = nullptr;
    }

    m_initialized = false;
    std::cout << "Project Simulator shutdown complete" << std::endl;
}

void ProjectSimulator::HandleUIEvent(const UIEvent& event) {
    switch (event.element) {
        case UIElement::MainMenu:
            HandleMainMenuEvent(event);
            break;
        case UIElement::ProjectList:
            HandleProjectListEvent(event);
            break;
        case UIElement::ProjectDetails:
            HandleProjectDetailsEvent(event);
            break;
        case UIElement::SimulationView:
            HandleSimulationViewEvent(event);
            break;
        case UIElement::TestResults:
            HandleTestResultsEvent(event);
            break;
        case UIElement::Settings:
            HandleSettingsEvent(event);
            break;
    }
}

void ProjectSimulator::HandleMainMenuEvent(const UIEvent& event) {
    if (event.action == "show_projects") {
        auto projects = m_projectManager->GetProjects();
        m_uiManager->ShowProjectList(projects);
    } else if (event.action == "show_settings") {
        m_uiManager->ShowSettings();
    } else if (event.action == "exit") {
        m_running = false;
    }
}

void ProjectSimulator::HandleProjectListEvent(const UIEvent& event) {
    if (event.action == "select_project") {
        auto project = m_projectManager->GetProject(event.data);
        if (project) {
            m_projectManager->SetActiveProject(project);
            m_uiManager->ShowProjectDetails(project);
        }
    } else if (event.action == "back_to_menu") {
        m_uiManager->ShowMainMenu();
    }
}

void ProjectSimulator::HandleProjectDetailsEvent(const UIEvent& event) {
    if (event.action == "run_simulation") {
        auto project = m_projectManager->GetActiveProject();
        if (project) {
            m_uiManager->ShowSimulationView(project);
            auto result = m_simulationEngine->RunProject(project);
            if (!result.success) {
                m_uiManager->AddLogMessage("Simulation failed: " + result.errorMessage, "error");
            }
        }
    } else if (event.action == "run_tests") {
        auto project = m_projectManager->GetActiveProject();
        if (project) {
            TestConfig config;
            config.enabledTests = {TestType::Unit, TestType::Integration, TestType::Performance};
            config.targetPlatforms = {"iOS", "Android"};
            config.iterations = 1;
            config.timeout = 30.0f;
            config.generateReport = true;
            config.reportPath = "./test_reports/";

            auto results = m_testRunner->RunTests(project, config);
            std::string report = m_testRunner->GetTestStatistics(results);
            m_uiManager->ShowTestResults(report);
        }
    } else if (event.action == "back_to_list") {
        auto projects = m_projectManager->GetProjects();
        m_uiManager->ShowProjectList(projects);
    }
}

void ProjectSimulator::HandleSimulationViewEvent(const UIEvent& event) {
    if (event.action == "stop_simulation") {
        m_simulationEngine->StopSimulation();
        auto project = m_projectManager->GetActiveProject();
        m_uiManager->ShowProjectDetails(project);
    } else if (event.action == "pause_simulation") {
        m_simulationEngine->PauseSimulation();
    } else if (event.action == "resume_simulation") {
        m_simulationEngine->ResumeSimulation();
    }
}

void ProjectSimulator::HandleTestResultsEvent(const UIEvent& event) {
    if (event.action == "back_to_project") {
        auto project = m_projectManager->GetActiveProject();
        m_uiManager->ShowProjectDetails(project);
    }
}

void ProjectSimulator::HandleSettingsEvent(const UIEvent& event) {
    if (event.action == "back_to_menu") {
        m_uiManager->ShowMainMenu();
    }
}

} // namespace ProjectSimulator

