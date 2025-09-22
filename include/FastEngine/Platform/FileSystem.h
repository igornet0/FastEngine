#pragma once

#include <string>
#include <vector>
#include <memory>

namespace FastEngine {
    class FileSystem {
    public:
        FileSystem();
        ~FileSystem();
        
        // Инициализация
        bool Initialize();
        void Shutdown();
        
        // Работа с файлами
        bool FileExists(const std::string& filePath) const;
        std::string ReadFile(const std::string& filePath) const;
        bool WriteFile(const std::string& filePath, const std::string& content) const;
        
        // Работа с директориями
        bool DirectoryExists(const std::string& dirPath) const;
        std::vector<std::string> ListFiles(const std::string& dirPath) const;
        std::vector<std::string> ListDirectories(const std::string& dirPath) const;
        
        // Пути
        std::string GetResourcePath(const std::string& resource) const;
        std::string GetDocumentsPath() const;
        
        // Создание путей
        std::string JoinPath(const std::string& path1, const std::string& path2) const;
        std::string GetFileName(const std::string& filePath) const;
        std::string GetFileExtension(const std::string& filePath) const;
        
    private:
        bool m_initialized;
    };
}
