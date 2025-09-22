#include "FastEngine/Platform/FileSystem.h"
#include <fstream>
#include <iostream>
#include <vector>

namespace FastEngine {
    FileSystem::FileSystem() = default;
    FileSystem::~FileSystem() = default;
    
    bool FileSystem::Initialize() {
        return true;
    }
    
    void FileSystem::Shutdown() {
    }
    
    bool FileSystem::FileExists(const std::string& path) const {
        std::ifstream file(path);
        return file.good();
    }
    
    std::string FileSystem::ReadFile(const std::string& path) const {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return "";
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        return content;
    }
    
    bool FileSystem::WriteFile(const std::string& path, const std::string& content) const {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        file.write(content.c_str(), content.length());
        return file.good();
    }
    
    std::string FileSystem::GetResourcePath(const std::string& resource) const {
        // Ищем файлы в нескольких возможных местах
        std::vector<std::string> possiblePaths = {
            "assets/" + resource,
            "../assets/" + resource,
            "../../assets/" + resource,
            "../../../assets/" + resource
        };
        
        for (const auto& path : possiblePaths) {
            if (FileExists(path)) {
                return path;
            }
        }
        
        // Если не найден, возвращаем первый путь
        return "assets/" + resource;
    }
    
    std::string FileSystem::GetDocumentsPath() const {
        return "Documents";
    }
}
