#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Project.h"

namespace ProjectSimulator {

/**
 * Менеджер проектов
 * Управляет загрузкой, сохранением и валидацией проектов
 */
class ProjectManager {
public:
    ProjectManager();
    ~ProjectManager();

    /**
     * Инициализация менеджера проектов
     * @param projectsPath путь к папке с проектами
     * @return true если инициализация успешна
     */
    bool Initialize(const std::string& projectsPath);

    /**
     * Загрузка всех доступных проектов
     * @return количество загруженных проектов
     */
    int LoadAllProjects();

    /**
     * Загрузка конкретного проекта
     * @param projectPath путь к проекту
     * @return указатель на загруженный проект или nullptr
     */
    std::shared_ptr<Project> LoadProject(const std::string& projectPath);

    /**
     * Сохранение проекта
     * @param project проект для сохранения
     * @return true если сохранение успешно
     */
    bool SaveProject(std::shared_ptr<Project> project);

    /**
     * Создание нового проекта
     * @param name имя проекта
     * @param templateName имя шаблона
     * @return указатель на созданный проект
     */
    std::shared_ptr<Project> CreateProject(const std::string& name, const std::string& templateName = "empty");

    /**
     * Получение списка всех проектов
     */
    const std::vector<std::shared_ptr<Project>>& GetProjects() const { return m_projects; }

    /**
     * Получение проекта по имени
     * @param name имя проекта
     * @return указатель на проект или nullptr
     */
    std::shared_ptr<Project> GetProject(const std::string& name) const;

    /**
     * Получение активного проекта
     */
    std::shared_ptr<Project> GetActiveProject() const { return m_activeProject; }

    /**
     * Установка активного проекта
     * @param project проект для активации
     */
    void SetActiveProject(std::shared_ptr<Project> project);

    /**
     * Валидация проекта
     * @param project проект для валидации
     * @return true если проект валиден
     */
    bool ValidateProject(std::shared_ptr<Project> project) const;

    /**
     * Получение списка доступных шаблонов
     */
    const std::vector<std::string>& GetTemplates() const { return m_templates; }

    /**
     * Загрузка шаблонов
     */
    void LoadTemplates();

private:
    std::string m_projectsPath;
    std::vector<std::shared_ptr<Project>> m_projects;
    std::shared_ptr<Project> m_activeProject;
    std::vector<std::string> m_templates;
    std::map<std::string, std::string> m_templatePaths;
    
    bool m_initialized;
};

} // namespace ProjectSimulator

