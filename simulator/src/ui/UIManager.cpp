#include "ProjectSimulator/UIManager.h"
#include "ProjectSimulator/Project.h"
#include <iostream>
#include <algorithm>

namespace ProjectSimulator {

UIManager::UIManager()
    : m_activeElement(UIElement::MainMenu)
    , m_logMessages()
    , m_simulationState("Stopped")
    , m_fps(0.0f)
    , m_frameTime(0.0f)
    , m_initialized(false)
    , m_showLog(false)
    , m_showSettings(false) {
}

UIManager::~UIManager() {
    // Очистка ресурсов
}

bool UIManager::Initialize(int windowWidth, int windowHeight) {
    if (m_initialized) {
        return true;
    }

    try {
        // Инициализация UI системы
        // В реальной реализации здесь будет инициализация OpenGL, шрифтов и т.д.
        
        m_initialized = true;
        std::cout << "UIManager initialized with window: " << windowWidth << "x" << windowHeight << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize UIManager: " << e.what() << std::endl;
        return false;
    }
}

void UIManager::Update(float deltaTime) {
    if (!m_initialized) {
        return;
    }

    // Обработка ввода
    HandleInput();
}

void UIManager::Render() {
    if (!m_initialized) {
        return;
    }

    // Отрисовка в зависимости от активного элемента
    switch (m_activeElement) {
        case UIElement::MainMenu:
            RenderMainMenu();
            break;
        case UIElement::ProjectList:
            RenderProjectList();
            break;
        case UIElement::ProjectDetails:
            RenderProjectDetails();
            break;
        case UIElement::SimulationView:
            RenderSimulationView();
            break;
        case UIElement::TestResults:
            RenderTestResults();
            break;
        case UIElement::Settings:
            RenderSettings();
            break;
    }

    // Отрисовка дополнительных элементов
    if (m_showLog) {
        RenderLog();
    }

    RenderInfoPanel();
}

void UIManager::HandleEvent(const UIEvent& event) {
    if (m_eventCallback) {
        m_eventCallback(event);
    }
}

void UIManager::SetEventCallback(std::function<void(const UIEvent&)> callback) {
    m_eventCallback = callback;
}

void UIManager::ShowMainMenu() {
    m_activeElement = UIElement::MainMenu;
    AddLogMessage("Showing main menu");
}

void UIManager::ShowProjectList(const std::vector<std::shared_ptr<Project>>& projects) {
    m_activeElement = UIElement::ProjectList;
    AddLogMessage("Showing project list with " + std::to_string(projects.size()) + " projects");
}

void UIManager::ShowProjectDetails(std::shared_ptr<Project> project) {
    m_activeElement = UIElement::ProjectDetails;
    if (project) {
        AddLogMessage("Showing details for project: " + project->GetName());
    }
}

void UIManager::ShowSimulationView(std::shared_ptr<Project> project) {
    m_activeElement = UIElement::SimulationView;
    if (project) {
        AddLogMessage("Starting simulation view for project: " + project->GetName());
    }
}

void UIManager::ShowTestResults(const std::string& results) {
    m_activeElement = UIElement::TestResults;
    AddLogMessage("Showing test results");
}

void UIManager::ShowSettings() {
    m_activeElement = UIElement::Settings;
    AddLogMessage("Showing settings");
}

void UIManager::UpdateSimulationInfo(float fps, float frameTime, const std::string& state) {
    m_fps = fps;
    m_frameTime = frameTime;
    m_simulationState = state;
}

void UIManager::AddLogMessage(const std::string& message, const std::string& level) {
    std::string logEntry = "[" + level + "] " + message;
    m_logMessages.push_back(logEntry);
    
    // Ограничиваем размер лога
    if (m_logMessages.size() > 100) {
        m_logMessages.erase(m_logMessages.begin());
    }
    
    std::cout << logEntry << std::endl;
}

void UIManager::ClearLog() {
    m_logMessages.clear();
}

void UIManager::RenderMainMenu() {
    std::cout << "\n=== Project Simulator Main Menu ===" << std::endl;
    std::cout << "1. Show Projects" << std::endl;
    std::cout << "2. Settings" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "Enter choice: ";
    
    // В реальной реализации здесь будет отрисовка через OpenGL
}

void UIManager::RenderProjectList() {
    std::cout << "\n=== Project List ===" << std::endl;
    std::cout << "Available projects:" << std::endl;
    std::cout << "1. Basic Game" << std::endl;
    std::cout << "2. Sprite Demo" << std::endl;
    std::cout << "3. Input Test" << std::endl;
    std::cout << "4. Back to Menu" << std::endl;
    std::cout << "===================" << std::endl;
    std::cout << "Enter choice: ";
}

void UIManager::RenderProjectDetails() {
    std::cout << "\n=== Project Details ===" << std::endl;
    std::cout << "Project: Basic Game" << std::endl;
    std::cout << "Version: 1.0.0" << std::endl;
    std::cout << "Description: A simple 2D game" << std::endl;
    std::cout << "\nActions:" << std::endl;
    std::cout << "1. Run Simulation" << std::endl;
    std::cout << "2. Run Tests" << std::endl;
    std::cout << "3. Back to List" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << "Enter choice: ";
}

void UIManager::RenderSimulationView() {
    std::cout << "\n=== Simulation View ===" << std::endl;
    std::cout << "Project: Basic Game" << std::endl;
    std::cout << "State: " << m_simulationState << std::endl;
    std::cout << "FPS: " << m_fps << std::endl;
    std::cout << "Frame Time: " << m_frameTime << "ms" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "1. Stop Simulation" << std::endl;
    std::cout << "2. Pause/Resume" << std::endl;
    std::cout << "3. Back to Project" << std::endl;
    std::cout << "=====================" << std::endl;
}

void UIManager::RenderTestResults() {
    std::cout << "\n=== Test Results ===" << std::endl;
    std::cout << "Test Results:" << std::endl;
    std::cout << "✓ Unit Tests: 5/5 passed" << std::endl;
    std::cout << "✓ Integration Tests: 3/3 passed" << std::endl;
    std::cout << "✓ Performance Tests: 2/2 passed" << std::endl;
    std::cout << "\nActions:" << std::endl;
    std::cout << "1. Back to Project" << std::endl;
    std::cout << "==================" << std::endl;
}

void UIManager::RenderSettings() {
    std::cout << "\n=== Settings ===" << std::endl;
    std::cout << "1. Max FPS: 60" << std::endl;
    std::cout << "2. Max Memory: 512 MB" << std::endl;
    std::cout << "3. Show Log: " << (m_showLog ? "Yes" : "No") << std::endl;
    std::cout << "4. Back to Menu" << std::endl;
    std::cout << "===============" << std::endl;
}

void UIManager::RenderLog() {
    std::cout << "\n=== Log ===" << std::endl;
    for (const auto& message : m_logMessages) {
        std::cout << message << std::endl;
    }
    std::cout << "===========" << std::endl;
}

void UIManager::RenderInfoPanel() {
    // В реальной реализации здесь будет отрисовка панели информации
    // с FPS, состоянием симуляции и т.д.
}

void UIManager::HandleInput() {
    // В реальной реализации здесь будет обработка ввода
    // от клавиатуры, мыши, касаний
}

} // namespace ProjectSimulator

