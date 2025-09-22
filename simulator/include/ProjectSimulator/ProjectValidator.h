#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Project.h"

namespace ProjectSimulator {

/**
 * Валидатор проектов
 * Проверяет корректность проектов
 */
class ProjectValidator {
public:
    /**
     * Валидация проекта
     * @param project проект для валидации
     * @return true если проект валиден
     */
    static bool ValidateProject(std::shared_ptr<Project> project);

    /**
     * Валидация структуры проекта
     * @param projectPath путь к проекту
     * @return true если структура корректна
     */
    static bool ValidateProjectStructure(const std::string& projectPath);

    /**
     * Получение списка ошибок валидации
     * @param project проект для проверки
     * @return список ошибок
     */
    static std::vector<std::string> GetValidationErrors(std::shared_ptr<Project> project);
};

} // namespace ProjectSimulator

