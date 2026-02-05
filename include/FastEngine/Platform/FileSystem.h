#pragma once

#include <string>
#include <vector>
#include <memory>

namespace FastEngine {
    class FileSystem {
    public:
        FileSystem();
        virtual ~FileSystem();
        
        // Инициализация
        virtual bool Initialize();
        virtual void Shutdown();
        
        // Работа с файлами
        virtual bool FileExists(const std::string& filePath) const;
        virtual std::string ReadFile(const std::string& filePath) const;
        virtual bool WriteFile(const std::string& filePath, const std::string& content) const;
        
        // Работа с директориями
        virtual bool DirectoryExists(const std::string& dirPath) const;
        virtual std::vector<std::string> ListFiles(const std::string& dirPath) const;
        virtual std::vector<std::string> ListDirectories(const std::string& dirPath) const;
        
        // Пути
        virtual std::string GetResourcePath(const std::string& resource) const;
        virtual std::string GetDocumentsPath() const;
        
        // Создание путей
        virtual std::string JoinPath(const std::string& path1, const std::string& path2) const;
        virtual std::string GetFileName(const std::string& filePath) const;
        virtual std::string GetFileExtension(const std::string& filePath) const;
        
    private:
        bool m_initialized;
    };
}
