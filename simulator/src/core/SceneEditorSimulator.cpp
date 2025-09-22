#include "ProjectSimulator/SceneEditorSimulator.h"
#include "FastEngine/Engine.h"
#include "FastEngine/World.h"
#include "FastEngine/Resources/ResourceManager.h"
#include <iostream>

namespace ProjectSimulator {

SceneEditorSimulator::SceneEditorSimulator() 
    : m_sceneEditor(nullptr)
    , m_inspector(nullptr)
    , m_hierarchy(nullptr)
    , m_assetBrowser(nullptr)
    , m_editorInitialized(false)
    , m_editMode(true)
    , m_simulationMode(false) {
}

SceneEditorSimulator::~SceneEditorSimulator() {
    if (m_editorInitialized) {
        Shutdown();
    }
}

bool SceneEditorSimulator::Initialize(int windowWidth, int windowHeight) {
    // Инициализируем базовый симулятор
    if (!ProjectSimulator::Initialize(windowWidth, windowHeight)) {
        return false;
    }
    
    // Инициализируем редактор
    if (!InitializeEditor()) {
        std::cerr << "Failed to initialize scene editor" << std::endl;
        return false;
    }
    
    m_editorInitialized = true;
    
    std::cout << "SceneEditorSimulator initialized successfully" << std::endl;
    return true;
}

void SceneEditorSimulator::Run() {
    if (!m_editorInitialized) {
        std::cerr << "SceneEditorSimulator not initialized" << std::endl;
        return;
    }
    
    std::cout << "Starting SceneEditorSimulator main loop..." << std::endl;
    
    // Главный цикл редактора
    while (m_running) {
        float deltaTime = 0.016f; // 60 FPS
        
        // Обновляем базовый симулятор
        if (m_simulationEngine) {
            m_simulationEngine->Update(deltaTime);
        }
        
        // Обновляем редактор
        UpdateEditor(deltaTime);
        
        // Рендерим
        RenderEditor();
        
        // Обрабатываем ввод
        HandleEditorInput();
        
        // Небольшая задержка для стабильности
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    std::cout << "SceneEditorSimulator main loop ended" << std::endl;
}

void SceneEditorSimulator::Shutdown() {
    if (!m_editorInitialized) {
        return;
    }
    
    // Очищаем редактор
    CleanupEditor();
    
    // Останавливаем базовый симулятор
    ProjectSimulator::Shutdown();
    
    m_editorInitialized = false;
    
    std::cout << "SceneEditorSimulator shutdown complete" << std::endl;
}

bool SceneEditorSimulator::CreateNewProject(const std::string& projectName, const std::string& projectPath) {
    if (!m_editorInitialized) {
        return false;
    }
    
    // Создаем новый проект
    auto project = std::make_shared<Project>();
    project->SetName(projectName);
    project->SetPath(projectPath);
    
    // Инициализируем FastEngine для проекта
    if (m_simulationEngine) {
        auto result = m_simulationEngine->RunProject(project);
        if (!result.success) {
            std::cerr << "Failed to initialize project: " << result.errorMessage << std::endl;
            return false;
        }
    }
    
    m_currentProjectPath = projectPath;
    m_currentSceneName = "MainScene";
    
    // Очищаем сцену
    if (m_sceneEditor) {
        m_sceneEditor->ClearScene();
    }
    
    std::cout << "Created new project: " << projectName << " at " << projectPath << std::endl;
    return true;
}

bool SceneEditorSimulator::OpenProject(const std::string& projectPath) {
    if (!m_editorInitialized) {
        return false;
    }
    
    // Загружаем проект
    if (m_projectManager) {
        auto project = m_projectManager->LoadProject(projectPath);
        if (!project) {
            std::cerr << "Failed to load project: " << projectPath << std::endl;
            return false;
        }
        
        // Инициализируем FastEngine для проекта
        if (m_simulationEngine) {
            auto result = m_simulationEngine->RunProject(project);
            if (!result.success) {
                std::cerr << "Failed to initialize project: " << result.errorMessage << std::endl;
                return false;
            }
        }
        
        m_currentProjectPath = projectPath;
        m_currentSceneName = "MainScene";
        
        // Загружаем сцену
        LoadScene(m_currentSceneName);
        
        std::cout << "Opened project: " << projectPath << std::endl;
        return true;
    }
    
    return false;
}

bool SceneEditorSimulator::SaveProject() {
    if (!m_editorInitialized || m_currentProjectPath.empty()) {
        return false;
    }
    
    // Сохраняем сцену
    if (m_sceneEditor) {
        std::string scenePath = m_currentProjectPath + "/scenes/" + m_currentSceneName + ".scene";
        if (!m_sceneEditor->SaveScene(scenePath)) {
            std::cerr << "Failed to save scene: " << scenePath << std::endl;
            return false;
        }
    }
    
    std::cout << "Project saved: " << m_currentProjectPath << std::endl;
    return true;
}

bool SceneEditorSimulator::SaveScene(const std::string& sceneName) {
    if (!m_editorInitialized || !m_sceneEditor) {
        return false;
    }
    
    std::string scenePath = m_currentProjectPath + "/scenes/" + sceneName + ".scene";
    bool success = m_sceneEditor->SaveScene(scenePath);
    
    if (success) {
        m_currentSceneName = sceneName;
        std::cout << "Scene saved: " << scenePath << std::endl;
    } else {
        std::cerr << "Failed to save scene: " << scenePath << std::endl;
    }
    
    return success;
}

bool SceneEditorSimulator::LoadScene(const std::string& sceneName) {
    if (!m_editorInitialized || !m_sceneEditor) {
        return false;
    }
    
    std::string scenePath = m_currentProjectPath + "/scenes/" + sceneName + ".scene";
    bool success = m_sceneEditor->LoadScene(scenePath);
    
    if (success) {
        m_currentSceneName = sceneName;
        std::cout << "Scene loaded: " << scenePath << std::endl;
    } else {
        std::cerr << "Failed to load scene: " << scenePath << std::endl;
    }
    
    return success;
}

FastEngine::Entity* SceneEditorSimulator::CreateSceneObject(const std::string& objectType, const glm::vec3& position) {
    if (!m_editorInitialized || !m_sceneEditor) {
        return nullptr;
    }
    
    FastEngine::Entity* entity = m_sceneEditor->CreateObject(objectType, position);
    
    if (entity && m_hierarchy) {
        m_hierarchy->AddObject(entity);
    }
    
    return entity;
}

void SceneEditorSimulator::DeleteSceneObject(FastEngine::Entity* entity) {
    if (!m_editorInitialized || !m_sceneEditor || !entity) {
        return;
    }
    
    if (m_hierarchy) {
        m_hierarchy->RemoveObject(entity);
    }
    
    m_sceneEditor->DeleteObject(entity);
}

void SceneEditorSimulator::SelectSceneObject(FastEngine::Entity* entity) {
    if (!m_editorInitialized) {
        return;
    }
    
    if (m_sceneEditor) {
        m_sceneEditor->SelectObject(entity);
    }
    
    if (m_hierarchy) {
        auto node = m_hierarchy->FindNode(entity);
        if (node) {
            m_hierarchy->SelectNode(node);
        }
    }
    
    if (m_inspector) {
        m_inspector->SetEntity(entity);
    }
}

FastEngine::Entity* SceneEditorSimulator::GetSelectedObject() const {
    if (!m_sceneEditor) {
        return nullptr;
    }
    
    return m_sceneEditor->GetSelectedObject().entity;
}

void SceneEditorSimulator::ToggleMode() {
    if (m_editMode) {
        SetEditMode(false);
        SetSimulationMode(true);
        std::cout << "Switched to simulation mode" << std::endl;
    } else {
        SetEditMode(true);
        SetSimulationMode(false);
        std::cout << "Switched to edit mode" << std::endl;
    }
}

bool SceneEditorSimulator::InitializeEditor() {
    // Создаем компоненты редактора
    m_sceneEditor = new SceneEditor();
    m_inspector = new Inspector();
    m_hierarchy = new Hierarchy();
    m_assetBrowser = new AssetBrowser();
    
    // Инициализируем компоненты
    if (!m_inspector->Initialize()) {
        std::cerr << "Failed to initialize Inspector" << std::endl;
        return false;
    }
    
    if (m_simulationEngine && m_simulationEngine->GetActiveProject()) {
        // Получаем мир из активного проекта
        // В реальной реализации здесь бы получался мир из FastEngine
        FastEngine::World* world = nullptr; // TODO: Get from simulation engine
        
        if (m_sceneEditor && world) {
            if (!m_sceneEditor->Initialize(nullptr, world)) { // TODO: Get engine from simulation
                std::cerr << "Failed to initialize SceneEditor" << std::endl;
                return false;
            }
        }
        
        if (m_hierarchy && world) {
            if (!m_hierarchy->Initialize(world)) {
                std::cerr << "Failed to initialize Hierarchy" << std::endl;
                return false;
            }
        }
    }
    
    if (m_assetBrowser) {
        FastEngine::ResourceManager* resourceManager = nullptr; // TODO: Get from simulation engine
        if (!m_assetBrowser->Initialize(resourceManager, "assets")) {
            std::cerr << "Failed to initialize AssetBrowser" << std::endl;
            return false;
        }
    }
    
    // Настраиваем callbacks
    SetupEditorCallbacks();
    
    std::cout << "Editor components initialized successfully" << std::endl;
    return true;
}

void SceneEditorSimulator::UpdateEditor(float deltaTime) {
    if (!m_editMode) {
        return;
    }
    
    // Обновляем компоненты редактора
    if (m_sceneEditor) {
        m_sceneEditor->Update(deltaTime);
    }
    
    if (m_inspector) {
        m_inspector->Update(deltaTime);
    }
    
    if (m_hierarchy) {
        m_hierarchy->Update(deltaTime);
    }
    
    if (m_assetBrowser) {
        m_assetBrowser->Update(deltaTime);
    }
}

void SceneEditorSimulator::RenderEditor() {
    if (!m_editMode) {
        return;
    }
    
    // Рендерим компоненты редактора
    if (m_sceneEditor) {
        m_sceneEditor->Render(nullptr); // TODO: Get renderer from simulation engine
    }
    
    if (m_inspector) {
        m_inspector->Render(nullptr);
    }
    
    if (m_hierarchy) {
        m_hierarchy->Render(nullptr);
    }
    
    if (m_assetBrowser) {
        m_assetBrowser->Render(nullptr);
    }
}

void SceneEditorSimulator::HandleEditorInput() {
    // В реальной реализации здесь бы обрабатывался ввод для редактора
    // Для простоты пропускаем
}

void SceneEditorSimulator::OnObjectSelected(FastEngine::Entity* entity) {
    if (m_inspector) {
        m_inspector->SetEntity(entity);
    }
}

void SceneEditorSimulator::OnAssetSelected(const AssetInfo& assetInfo) {
    // В реальной реализации здесь бы обрабатывался выбор ассета
    std::cout << "Asset selected: " << assetInfo.name << std::endl;
}

void SceneEditorSimulator::SetupEditorCallbacks() {
    if (m_hierarchy) {
        m_hierarchy->SetSelectionCallback([this](FastEngine::Entity* entity) {
            OnObjectSelected(entity);
        });
    }
    
    if (m_assetBrowser) {
        m_assetBrowser->SetSelectionCallback([this](const AssetInfo& assetInfo) {
            OnAssetSelected(assetInfo);
        });
    }
}

void SceneEditorSimulator::CleanupEditor() {
    if (m_sceneEditor) {
        delete m_sceneEditor;
        m_sceneEditor = nullptr;
    }
    
    if (m_inspector) {
        delete m_inspector;
        m_inspector = nullptr;
    }
    
    if (m_hierarchy) {
        delete m_hierarchy;
        m_hierarchy = nullptr;
    }
    
    if (m_assetBrowser) {
        delete m_assetBrowser;
        m_assetBrowser = nullptr;
    }
    
    std::cout << "Editor components cleaned up" << std::endl;
}

} // namespace ProjectSimulator

