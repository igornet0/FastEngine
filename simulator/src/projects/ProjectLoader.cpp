#include "ProjectSimulator/Project.h"
#include <fstream>
#include <iostream>
#include <filesystem>

namespace ProjectSimulator {

bool ProjectLoader::LoadProject(const std::string& projectPath, std::shared_ptr<Project>& project) {
    if (!std::filesystem::exists(projectPath)) {
        std::cerr << "Project file not found: " << projectPath << std::endl;
        return false;
    }

    project = std::make_shared<Project>();
    return project->LoadFromFile(projectPath);
}

bool ProjectLoader::SaveProject(const std::string& projectPath, std::shared_ptr<Project> project) {
    if (!project) {
        return false;
    }

    return project->SaveToFile(projectPath);
}

std::vector<std::string> ProjectLoader::FindProjects(const std::string& directory) {
    std::vector<std::string> projects;
    
    if (!std::filesystem::exists(directory)) {
        return projects;
    }

    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().filename() == "project.conf") {
                projects.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error scanning directory: " << e.what() << std::endl;
    }

    return projects;
}

} // namespace ProjectSimulator

