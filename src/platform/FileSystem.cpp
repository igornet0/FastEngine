#include "FastEngine/Platform/FileSystem.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#include <sys/stat.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#endif

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

    bool FileSystem::DirectoryExists(const std::string& dirPath) const {
        if (dirPath.empty()) return false;
#if defined(_WIN32) || defined(_WIN64)
        struct _stat info;
        if (_stat(dirPath.c_str(), &info) != 0) return false;
        return (info.st_mode & _S_IFDIR) != 0;
#else
        struct stat info;
        if (stat(dirPath.c_str(), &info) != 0) return false;
        return S_ISDIR(info.st_mode);
#endif
    }

    std::vector<std::string> FileSystem::ListFiles(const std::string& dirPath) const {
        std::vector<std::string> result;
#if defined(_WIN32) || defined(_WIN64)
        (void)dirPath;
        return result;
#else
        DIR* dir = opendir(dirPath.c_str());
        if (!dir) return result;
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_name[0] == '.') continue;
            std::string fullPath = JoinPath(dirPath, entry->d_name);
            if (!DirectoryExists(fullPath)) {
                result.push_back(entry->d_name);
            }
        }
        closedir(dir);
        return result;
#endif
    }

    std::vector<std::string> FileSystem::ListDirectories(const std::string& dirPath) const {
        std::vector<std::string> result;
#if defined(_WIN32) || defined(_WIN64)
        (void)dirPath;
        return result;
#else
        DIR* dir = opendir(dirPath.c_str());
        if (!dir) return result;
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_name[0] == '.' && (entry->d_name[1] == '\0' || (entry->d_name[1] == '.' && entry->d_name[2] == '\0')))
                continue;
            std::string fullPath = JoinPath(dirPath, entry->d_name);
            if (DirectoryExists(fullPath)) {
                result.push_back(entry->d_name);
            }
        }
        closedir(dir);
        return result;
#endif
    }

    std::string FileSystem::JoinPath(const std::string& path1, const std::string& path2) const {
        if (path1.empty()) return path2;
        if (path2.empty()) return path1;
        char sep = '/';
        if (path1.back() == sep || path2.front() == sep) {
            if (path1.back() == sep && path2.front() == sep)
                return path1 + path2.substr(1);
            return path1 + path2;
        }
        return path1 + sep + path2;
    }

    std::string FileSystem::GetFileName(const std::string& filePath) const {
        if (filePath.empty()) return "";
        size_t pos = filePath.find_last_of("/\\");
        if (pos == std::string::npos) return filePath;
        return filePath.substr(pos + 1);
    }

    std::string FileSystem::GetFileExtension(const std::string& filePath) const {
        std::string name = GetFileName(filePath);
        if (name.empty()) return "";
        size_t pos = name.find_last_of('.');
        if (pos == std::string::npos || pos == name.length() - 1) return "";
        return name.substr(pos + 1);
    }
}
