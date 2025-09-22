#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Project.h"

namespace ProjectSimulator {

/**
 * Загрузчик проектов
 * Управляет загрузкой и сохранением проектов
 */
class ProjectLoader {
public:
    /**
     * Загрузка проекта из файла
     * @param projectPath путь к файлу проекта
     * @param project ссылка на загруженный проект
     * @return true если загрузка успешна
     */
    static bool LoadProject(const std::string& projectPath, std::shared_ptr<Project>& project);

    /**
     * Сохранение проекта в файл
     * @param projectPath путь к файлу проекта
     * @param project проект для сохранения
     * @return true если сохранение успешно
     */
    static bool SaveProject(const std::string& projectPath, std::shared_ptr<Project> project);

    /**
     * Поиск проектов в директории
     * @param directory директория для поиска
     * @return список путей к файлам проектов
     */
    static std::vector<std::string> FindProjects(const std::string& directory);
};

} // namespace ProjectSimulator

