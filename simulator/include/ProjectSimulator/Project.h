#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

namespace ProjectSimulator {

/**
 * Структура проекта
 * Содержит метаданные и конфигурацию проекта
 */
class Project {
public:
    Project();
    ~Project() = default;

    /**
     * Загрузка проекта из файла
     * @param projectPath путь к файлу проекта
     * @return true если загрузка успешна
     */
    bool LoadFromFile(const std::string& projectPath);

    /**
     * Сохранение проекта в файл
     * @param projectPath путь к файлу проекта
     * @return true если сохранение успешно
     */
    bool SaveToFile(const std::string& projectPath) const;

    /**
     * Загрузка из шаблона
     * @param templatePath путь к шаблону
     * @return true если загрузка успешна
     */
    bool LoadFromTemplate(const std::string& templatePath);

    // Геттеры и сеттеры
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }

    const std::string& GetDescription() const { return m_description; }
    void SetDescription(const std::string& description) { m_description = description; }

    const std::string& GetVersion() const { return m_version; }
    void SetVersion(const std::string& version) { m_version = version; }

    const std::string& GetAuthor() const { return m_author; }
    void SetAuthor(const std::string& author) { m_author = author; }

    const std::string& GetPath() const { return m_path; }
    void SetPath(const std::string& path) { m_path = path; }

    const std::string& GetMainFile() const { return m_mainFile; }
    void SetMainFile(const std::string& mainFile) { m_mainFile = mainFile; }

    const std::vector<std::string>& GetAssets() const { return m_assets; }
    void AddAsset(const std::string& asset) { m_assets.push_back(asset); }
    void RemoveAsset(const std::string& asset);

    const std::vector<std::string>& GetDependencies() const { return m_dependencies; }
    void AddDependency(const std::string& dependency) { m_dependencies.push_back(dependency); }
    void RemoveDependency(const std::string& dependency);

    const std::map<std::string, std::string>& GetSettings() const { return m_settings; }
    void SetSetting(const std::string& key, const std::string& value) { m_settings[key] = value; }
    std::string GetSetting(const std::string& key) const;

    const std::chrono::system_clock::time_point& GetLastModified() const { return m_lastModified; }
    void UpdateLastModified() { m_lastModified = std::chrono::system_clock::now(); }

    bool IsValid() const { return m_valid; }
    void SetValid(bool valid) { m_valid = valid; }

    /**
     * Получение полного пути к главному файлу
     */
    std::string GetMainFilePath() const;

    /**
     * Получение полного пути к ассету
     */
    std::string GetAssetPath(const std::string& asset) const;

    /**
     * Проверка существования ассета
     */
    bool HasAsset(const std::string& asset) const;

    /**
     * Получение информации о проекте в виде строки
     */
    std::string GetInfo() const;

private:
    std::string m_name;
    std::string m_description;
    std::string m_version;
    std::string m_author;
    std::string m_path;
    std::string m_mainFile;
    std::vector<std::string> m_assets;
    std::vector<std::string> m_dependencies;
    std::map<std::string, std::string> m_settings;
    std::chrono::system_clock::time_point m_lastModified;
    bool m_valid;
};

} // namespace ProjectSimulator

