#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "Project.h"

namespace ProjectSimulator {

/**
 * Типы тестов
 */
enum class TestType {
    Unit,           // Unit тесты
    Integration,    // Интеграционные тесты
    Performance,    // Тесты производительности
    Compatibility,  // Тесты совместимости
    Stress          // Стресс-тесты
};

/**
 * Результат теста
 */
struct TestResult {
    std::string testName;
    TestType type;
    bool passed;
    std::string message;
    float duration;
    std::string platform;
    std::map<std::string, std::string> metrics;
};

/**
 * Конфигурация тестирования
 */
struct TestConfig {
    std::vector<TestType> enabledTests;
    std::vector<std::string> targetPlatforms;
    int iterations;
    float timeout;
    bool generateReport;
    std::string reportPath;
};

/**
 * Тест раннер
 * Управляет выполнением тестов для проектов
 */
class TestRunner {
public:
    TestRunner();
    ~TestRunner();

    /**
     * Инициализация тест раннера
     * @return true если инициализация успешна
     */
    bool Initialize();

    /**
     * Запуск всех тестов для проекта
     * @param project проект для тестирования
     * @param config конфигурация тестирования
     * @return результаты тестирования
     */
    std::vector<TestResult> RunTests(std::shared_ptr<Project> project, const TestConfig& config);

    /**
     * Запуск конкретного типа тестов
     * @param project проект для тестирования
     * @param type тип тестов
     * @param config конфигурация тестирования
     * @return результаты тестирования
     */
    std::vector<TestResult> RunTestType(std::shared_ptr<Project> project, TestType type, const TestConfig& config);

    /**
     * Запуск unit тестов
     * @param project проект для тестирования
     * @return результаты тестирования
     */
    std::vector<TestResult> RunUnitTests(std::shared_ptr<Project> project);

    /**
     * Запуск интеграционных тестов
     * @param project проект для тестирования
     * @return результаты тестирования
     */
    std::vector<TestResult> RunIntegrationTests(std::shared_ptr<Project> project);

    /**
     * Запуск тестов производительности
     * @param project проект для тестирования
     * @return результаты тестирования
     */
    std::vector<TestResult> RunPerformanceTests(std::shared_ptr<Project> project);

    /**
     * Запуск тестов совместимости
     * @param project проект для тестирования
     * @param platforms целевые платформы
     * @return результаты тестирования
     */
    std::vector<TestResult> RunCompatibilityTests(std::shared_ptr<Project> project, const std::vector<std::string>& platforms);

    /**
     * Запуск стресс-тестов
     * @param project проект для тестирования
     * @param iterations количество итераций
     * @return результаты тестирования
     */
    std::vector<TestResult> RunStressTests(std::shared_ptr<Project> project, int iterations = 100);

    /**
     * Установка callback для прогресса тестирования
     * @param callback функция обратного вызова
     */
    void SetProgressCallback(std::function<void(int, int)> callback);

    /**
     * Установка callback для результатов тестов
     * @param callback функция обратного вызова
     */
    void SetResultCallback(std::function<void(const TestResult&)> callback);

    /**
     * Генерация отчета о тестировании
     * @param results результаты тестирования
     * @param outputPath путь к файлу отчета
     * @return true если отчет создан успешно
     */
    bool GenerateReport(const std::vector<TestResult>& results, const std::string& outputPath);

    /**
     * Получение статистики тестирования
     * @param results результаты тестирования
     * @return статистика в виде строки
     */
    std::string GetTestStatistics(const std::vector<TestResult>& results);

    /**
     * Проверка, запущены ли тесты
     */
    bool IsRunning() const { return m_running; }

    /**
     * Остановка тестирования
     */
    void Stop();

    /**
     * Получение прогресса тестирования
     */
    int GetProgress() const { return m_progress; }

    /**
     * Получение общего количества тестов
     */
    int GetTotalTests() const { return m_totalTests; }

private:
    std::function<void(int, int)> m_progressCallback;
    std::function<void(const TestResult&)> m_resultCallback;
    
    bool m_initialized;
    bool m_running;
    int m_progress;
    int m_totalTests;

    /**
     * Выполнение unit теста
     */
    TestResult ExecuteUnitTest(const std::string& testName, std::shared_ptr<Project> project);

    /**
     * Выполнение интеграционного теста
     */
    TestResult ExecuteIntegrationTest(const std::string& testName, std::shared_ptr<Project> project);

    /**
     * Выполнение теста производительности
     */
    TestResult ExecutePerformanceTest(const std::string& testName, std::shared_ptr<Project> project);

    /**
     * Выполнение теста совместимости
     */
    TestResult ExecuteCompatibilityTest(const std::string& testName, std::shared_ptr<Project> project, const std::string& platform);

    /**
     * Выполнение стресс-теста
     */
    TestResult ExecuteStressTest(const std::string& testName, std::shared_ptr<Project> project, int iterations);

    /**
     * Обновление прогресса
     */
    void UpdateProgress(int current, int total);

    /**
     * Отправка результата теста
     */
    void SendResult(const TestResult& result);
};

} // namespace ProjectSimulator

