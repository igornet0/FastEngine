#include "ProjectSimulator/ProjectManager.h"
#include "ProjectSimulator/Project.h"
#include <filesystem>
#include <iostream>
#include <algorithm>

namespace ProjectSimulator {

ProjectManager::ProjectManager()
    : m_initialized(false) {
}

ProjectManager::~ProjectManager() {
    m_projects.clear();
    m_activeProject.reset();
}

bool ProjectManager::Initialize(const std::string& projectsPath) {
    m_projectsPath = projectsPath;
    
    // Создаем директорию проектов если она не существует
    if (!std::filesystem::exists(m_projectsPath)) {
        std::filesystem::create_directories(m_projectsPath);
    }

    // Загружаем шаблоны
    LoadTemplates();

    m_initialized = true;
    return true;
}

int ProjectManager::LoadAllProjects() {
    if (!m_initialized) {
        return 0;
    }

    m_projects.clear();
    int loadedCount = 0;

    try {
        // Сканируем директорию проектов
        for (const auto& entry : std::filesystem::directory_iterator(m_projectsPath)) {
            if (entry.is_directory()) {
                std::string projectDir = entry.path().string();
                std::string projectFile = projectDir + "/project.conf";
                
                if (std::filesystem::exists(projectFile)) {
                    auto project = LoadProject(projectFile);
                    if (project) {
                        m_projects.push_back(project);
                        loadedCount++;
                    }
                }
            }
        }

        std::cout << "Loaded " << loadedCount << " projects from " << m_projectsPath << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error loading projects: " << e.what() << std::endl;
    }

    return loadedCount;
}

std::shared_ptr<Project> ProjectManager::LoadProject(const std::string& projectPath) {
    auto project = std::make_shared<Project>();
    
    if (project->LoadFromFile(projectPath)) {
        return project;
    }
    
    return nullptr;
}

bool ProjectManager::SaveProject(std::shared_ptr<Project> project) {
    if (!project) {
        return false;
    }

    if (project->GetPath().empty()) {
        // Создаем новую директорию для проекта
        std::string projectDir = m_projectsPath + "/" + project->GetName();
        std::filesystem::create_directories(projectDir);
        
        std::string projectFile = projectDir + "/project.conf";
        project->SetPath(projectFile);
    }

    return project->SaveToFile(project->GetPath());
}

std::shared_ptr<Project> ProjectManager::CreateProject(const std::string& name, const std::string& templateName) {
    auto project = std::make_shared<Project>();
    project->SetName(name);
    project->SetVersion("1.0.0");
    project->SetDescription("A new FastEngine project");
    project->SetAuthor("Project Simulator");

    // Загружаем шаблон если указан
    if (!templateName.empty() && templateName != "empty") {
        auto templateIt = m_templatePaths.find(templateName);
        if (templateIt != m_templatePaths.end()) {
            project->LoadFromTemplate(templateIt->second);
        }
    }

    // Создаем директорию проекта
    std::string projectDir = m_projectsPath + "/" + name;
    std::filesystem::create_directories(projectDir);
    std::filesystem::create_directories(projectDir + "/assets");
    std::filesystem::create_directories(projectDir + "/src");

    // Создаем файл проекта
    std::string projectFile = projectDir + "/project.conf";
    project->SetPath(projectFile);

    // Создаем базовые файлы
    if (templateName == "empty" || templateName.empty()) {
        CreateEmptyProjectFiles(projectDir, name);
    }

    // Сохраняем проект
    if (SaveProject(project)) {
        m_projects.push_back(project);
        return project;
    }

    return nullptr;
}

std::shared_ptr<Project> ProjectManager::GetProject(const std::string& name) const {
    auto it = std::find_if(m_projects.begin(), m_projects.end(),
        [&name](const std::shared_ptr<Project>& project) {
            return project->GetName() == name;
        });

    if (it != m_projects.end()) {
        return *it;
    }

    return nullptr;
}

void ProjectManager::SetActiveProject(std::shared_ptr<Project> project) {
    m_activeProject = project;
}

bool ProjectManager::ValidateProject(std::shared_ptr<Project> project) const {
    if (!project) {
        return false;
    }

    // Проверяем основные поля
    if (project->GetName().empty()) {
        return false;
    }

    if (project->GetMainFile().empty()) {
        return false;
    }

    // Проверяем существование главного файла
    std::string mainFilePath = project->GetMainFilePath();
    if (!std::filesystem::exists(mainFilePath)) {
        return false;
    }

    // Проверяем существование ассетов
    for (const auto& asset : project->GetAssets()) {
        std::string assetPath = project->GetAssetPath(asset);
        if (!std::filesystem::exists(assetPath)) {
            std::cout << "Warning: Asset not found: " << assetPath << std::endl;
        }
    }

    return true;
}

void ProjectManager::LoadTemplates() {
    m_templates.clear();
    m_templatePaths.clear();

    // Добавляем встроенные шаблоны
    m_templates.push_back("empty");
    m_templates.push_back("basic_game");
    m_templates.push_back("sprite_demo");
    m_templates.push_back("input_test");

    // Сканируем директорию шаблонов
    std::string templatesPath = m_projectsPath + "/templates";
    if (std::filesystem::exists(templatesPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(templatesPath)) {
            if (entry.is_directory()) {
                std::string templateName = entry.path().filename().string();
                std::string templateFile = entry.path().string() + "/project.conf";
                
                if (std::filesystem::exists(templateFile)) {
                    m_templates.push_back(templateName);
                    m_templatePaths[templateName] = templateFile;
                }
            }
        }
    }
}

void ProjectManager::CreateEmptyProjectFiles(const std::string& projectDir, const std::string& projectName) {
    // Создаем основной файл
    std::string mainFile = projectDir + "/src/main.cpp";
    std::ofstream main(mainFile);
    if (main.is_open()) {
        main << "#include \"FastEngine/Engine.h\"\n";
        main << "#include \"FastEngine/Components/Transform.h\"\n";
        main << "#include \"FastEngine/Components/Sprite.h\"\n";
        main << "#include <iostream>\n\n";
        main << "int main() {\n";
        main << "    // Создаем движок\n";
        main << "    FastEngine::Engine engine;\n\n";
        main << "    // Инициализируем\n";
        main << "    if (!engine.Initialize(\"" << projectName << "\", 800, 600)) {\n";
        main << "        std::cerr << \"Failed to initialize engine\" << std::endl;\n";
        main << "        return -1;\n";
        main << "    }\n\n";
        main << "    // Получаем мир\n";
        main << "    auto* world = engine.GetWorld();\n\n";
        main << "    // Создаем простой спрайт\n";
        main << "    auto* entity = world->CreateEntity();\n";
        main << "    entity->AddComponent<FastEngine::Transform>(400.0f, 300.0f);\n";
        main << "    entity->AddComponent<FastEngine::Sprite>(\"test.png\");\n\n";
        main << "    // Запускаем игру\n";
        main << "    engine.Run();\n\n";
        main << "    return 0;\n";
        main << "}\n";
        main.close();
    }

    // Создаем CMakeLists.txt
    std::string cmakeFile = projectDir + "/CMakeLists.txt";
    std::ofstream cmake(cmakeFile);
    if (cmake.is_open()) {
        cmake << "cmake_minimum_required(VERSION 3.16)\n";
        cmake << "project(" << projectName << ")\n\n";
        cmake << "set(CMAKE_CXX_STANDARD 17)\n";
        cmake << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n\n";
        cmake << "find_package(FastEngine REQUIRED)\n\n";
        cmake << "add_executable(" << projectName << " src/main.cpp)\n";
        cmake << "target_link_libraries(" << projectName << " FastEngine)\n";
        cmake.close();
    }

    // Создаем README
    std::string readmeFile = projectDir + "/README.md";
    std::ofstream readme(readmeFile);
    if (readme.is_open()) {
        readme << "# " << projectName << "\n\n";
        readme << "A FastEngine project created with Project Simulator.\n\n";
        readme << "## Building\n\n";
        readme << "```bash\n";
        readme << "mkdir build && cd build\n";
        readme << "cmake ..\n";
        readme << "make\n";
        readme << "```\n\n";
        readme << "## Running\n\n";
        readme << "```bash\n";
        readme << "./" << projectName << "\n";
        readme << "```\n";
        readme.close();
    }
}

} // namespace ProjectSimulator

