#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Project.h"
#include "TestRunner.h"

namespace ProjectSimulator {

/**
 * Аргументы командной строки
 */
struct CLIArgs {
    std::string command;
    std::string projectPath;
    std::string outputPath;
    std::vector<std::string> testTypes;
    std::vector<std::string> platforms;
    int iterations;
    float timeout;
    bool verbose;
    bool generateReport;
    bool help;
    bool listProjects;
    bool listTests;
    std::string configFile;
};

/**
 * CLI интерфейс для Project Simulator
 * Позволяет запускать тестирование и симуляцию через терминал
 */
class CLI {
public:
    CLI();
    ~CLI();

    /**
     * Инициализация CLI
     * @return true если инициализация успешна
     */
    bool Initialize();

    /**
     * Парсинг аргументов командной строки
     * @param argc количество аргументов
     * @param argv массив аргументов
     * @return структура с распарсенными аргументами
     */
    CLIArgs ParseArgs(int argc, char* argv[]);

    /**
     * Выполнение команды
     * @param args аргументы команды
     * @return код возврата (0 - успех, >0 - ошибка)
     */
    int ExecuteCommand(const CLIArgs& args);

    /**
     * Показать справку
     */
    void ShowHelp();

    /**
     * Показать список доступных проектов
     */
    void ListProjects();

    /**
     * Показать список доступных тестов
     */
    void ListTests();

    /**
     * Запуск тестирования проекта
     * @param args аргументы команды
     * @return код возврата
     */
    int RunTests(const CLIArgs& args);

    /**
     * Запуск симуляции проекта
     * @param args аргументы команды
     * @return код возврата
     */
    int RunSimulation(const CLIArgs& args);

    /**
     * Валидация проекта
     * @param args аргументы команды
     * @return код возврата
     */
    int ValidateProject(const CLIArgs& args);

    /**
     * Создание проекта
     * @param args аргументы команды
     * @return код возврата
     */
    int CreateProject(const CLIArgs& args);

    /**
     * Пакетное тестирование всех проектов
     * @param args аргументы команды
     * @return код возврата
     */
    int BatchTest(const CLIArgs& args);

    /**
     * Генерация отчета
     * @param results результаты тестирования
     * @param outputPath путь для сохранения отчета
     * @return код возврата
     */
    int GenerateReport(const std::vector<TestResult>& results, const std::string& outputPath);

private:
    std::string m_projectsPath;
    std::string m_outputPath;
    bool m_initialized;

    /**
     * Загрузка конфигурации из файла
     * @param configPath путь к файлу конфигурации
     * @return структура с конфигурацией
     */
    CLIArgs LoadConfig(const std::string& configPath);

    /**
     * Сохранение конфигурации в файл
     * @param args конфигурация для сохранения
     * @param configPath путь к файлу конфигурации
     * @return true если сохранение успешно
     */
    bool SaveConfig(const CLIArgs& args, const std::string& configPath);

    /**
     * Поиск проектов в директории
     * @param directory директория для поиска
     * @return список путей к проектам
     */
    std::vector<std::string> FindProjects(const std::string& directory);

    /**
     * Вывод результата тестирования
     * @param result результат теста
     * @param verbose подробный вывод
     */
    void PrintTestResult(const TestResult& result, bool verbose = false);

    /**
     * Вывод статистики тестирования
     * @param results результаты тестирования
     */
    void PrintTestStatistics(const std::vector<TestResult>& results);
};

} // namespace ProjectSimulator

