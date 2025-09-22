#include "ProjectSimulator/Project.h"
#include <filesystem>
#include <iostream>

namespace ProjectSimulator {

bool ProjectValidator::ValidateProject(std::shared_ptr<Project> project) {
    if (!project) {
        return false;
    }

    // Проверяем основные поля
    if (project->GetName().empty()) {
        std::cerr << "Project name is empty" << std::endl;
        return false;
    }

    if (project->GetMainFile().empty()) {
        std::cerr << "Main file is not specified" << std::endl;
        return false;
    }

    // Проверяем существование главного файла
    std::string mainFilePath = project->GetMainFilePath();
    if (!std::filesystem::exists(mainFilePath)) {
        std::cerr << "Main file not found: " << mainFilePath << std::endl;
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

bool ProjectValidator::ValidateProjectStructure(const std::string& projectPath) {
    if (!std::filesystem::exists(projectPath)) {
        return false;
    }

    // Проверяем наличие основных файлов
    std::filesystem::path projectDir = std::filesystem::path(projectPath).parent_path();
    
    std::string projectFile = projectDir / "project.conf";
    if (!std::filesystem::exists(projectFile)) {
        std::cerr << "project.conf not found" << std::endl;
        return false;
    }

    return true;
}

std::vector<std::string> ProjectValidator::GetValidationErrors(std::shared_ptr<Project> project) {
    std::vector<std::string> errors;

    if (!project) {
        errors.push_back("Project is null");
        return errors;
    }

    if (project->GetName().empty()) {
        errors.push_back("Project name is empty");
    }

    if (project->GetMainFile().empty()) {
        errors.push_back("Main file is not specified");
    }

    std::string mainFilePath = project->GetMainFilePath();
    if (!std::filesystem::exists(mainFilePath)) {
        errors.push_back("Main file not found: " + mainFilePath);
    }

    return errors;
}

} // namespace ProjectSimulator

