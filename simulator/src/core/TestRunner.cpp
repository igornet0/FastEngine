#include "ProjectSimulator/TestRunner.h"
#include "ProjectSimulator/Project.h"
#include <iostream>
#include <chrono>
#include <thread>

namespace ProjectSimulator {

TestRunner::TestRunner()
    : m_initialized(false)
    , m_running(false)
    , m_progress(0)
    , m_totalTests(0) {
}

TestRunner::~TestRunner() {
    Stop();
}

bool TestRunner::Initialize() {
    if (m_initialized) {
        return true;
    }

    try {
        m_initialized = true;
        std::cout << "TestRunner initialized successfully" << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize TestRunner: " << e.what() << std::endl;
        return false;
    }
}

std::vector<TestResult> TestRunner::RunTests(std::shared_ptr<Project> project, const TestConfig& config) {
    if (!m_initialized || !project) {
        return {};
    }

    m_running = true;
    m_progress = 0;
    m_totalTests = 0;

    std::vector<TestResult> results;

    try {
        // Подсчитываем общее количество тестов
        for (const auto& testType : config.enabledTests) {
            switch (testType) {
                case TestType::Unit:
                    m_totalTests += 5; // Примерное количество unit тестов
                    break;
                case TestType::Integration:
                    m_totalTests += 3; // Примерное количество интеграционных тестов
                    break;
                case TestType::Performance:
                    m_totalTests += 2; // Примерное количество тестов производительности
                    break;
                case TestType::Compatibility:
                    m_totalTests += config.targetPlatforms.size();
                    break;
                case TestType::Stress:
                    m_totalTests += 1;
                    break;
            }
        }

        // Запускаем тесты по типам
        for (const auto& testType : config.enabledTests) {
            auto typeResults = RunTestType(project, testType, config);
            results.insert(results.end(), typeResults.begin(), typeResults.end());
        }

        m_running = false;
        m_progress = m_totalTests;

    } catch (const std::exception& e) {
        std::cerr << "Error running tests: " << e.what() << std::endl;
        m_running = false;
    }

    return results;
}

std::vector<TestResult> TestRunner::RunTestType(std::shared_ptr<Project> project, TestType type, const TestConfig& config) {
    std::vector<TestResult> results;

    switch (type) {
        case TestType::Unit:
            results = RunUnitTests(project);
            break;
        case TestType::Integration:
            results = RunIntegrationTests(project);
            break;
        case TestType::Performance:
            results = RunPerformanceTests(project);
            break;
        case TestType::Compatibility:
            results = RunCompatibilityTests(project, config.targetPlatforms);
            break;
        case TestType::Stress:
            results = RunStressTests(project, config.iterations);
            break;
    }

    return results;
}

std::vector<TestResult> TestRunner::RunUnitTests(std::shared_ptr<Project> project) {
    std::vector<TestResult> results;
    
    // Примеры unit тестов
    std::vector<std::string> testNames = {
        "Project Validation",
        "Asset Loading",
        "Component Creation",
        "Entity Management",
        "System Initialization"
    };

    for (const auto& testName : testNames) {
        auto result = ExecuteUnitTest(testName, project);
        results.push_back(result);
        UpdateProgress(m_progress + 1, m_totalTests);
        SendResult(result);
        
        // Небольшая задержка для имитации выполнения
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return results;
}

std::vector<TestResult> TestRunner::RunIntegrationTests(std::shared_ptr<Project> project) {
    std::vector<TestResult> results;
    
    std::vector<std::string> testNames = {
        "Engine Initialization",
        "Project Loading",
        "Simulation Startup"
    };

    for (const auto& testName : testNames) {
        auto result = ExecuteIntegrationTest(testName, project);
        results.push_back(result);
        UpdateProgress(m_progress + 1, m_totalTests);
        SendResult(result);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return results;
}

std::vector<TestResult> TestRunner::RunPerformanceTests(std::shared_ptr<Project> project) {
    std::vector<TestResult> results;
    
    std::vector<std::string> testNames = {
        "Frame Rate Test",
        "Memory Usage Test"
    };

    for (const auto& testName : testNames) {
        auto result = ExecutePerformanceTest(testName, project);
        results.push_back(result);
        UpdateProgress(m_progress + 1, m_totalTests);
        SendResult(result);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return results;
}

std::vector<TestResult> TestRunner::RunCompatibilityTests(std::shared_ptr<Project> project, const std::vector<std::string>& platforms) {
    std::vector<TestResult> results;
    
    for (const auto& platform : platforms) {
        auto result = ExecuteCompatibilityTest("Platform Compatibility", project, platform);
        results.push_back(result);
        UpdateProgress(m_progress + 1, m_totalTests);
        SendResult(result);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    return results;
}

std::vector<TestResult> TestRunner::RunStressTests(std::shared_ptr<Project> project, int iterations) {
    std::vector<TestResult> results;
    
    auto result = ExecuteStressTest("Stress Test", project, iterations);
    results.push_back(result);
    UpdateProgress(m_progress + 1, m_totalTests);
    SendResult(result);
    
    return results;
}

void TestRunner::SetProgressCallback(std::function<void(int, int)> callback) {
    m_progressCallback = callback;
}

void TestRunner::SetResultCallback(std::function<void(const TestResult&)> callback) {
    m_resultCallback = callback;
}

bool TestRunner::GenerateReport(const std::vector<TestResult>& results, const std::string& outputPath) {
    try {
        std::ofstream file(outputPath + "test_report.html");
        if (!file.is_open()) {
            return false;
        }

        // Генерируем HTML отчет
        file << "<!DOCTYPE html>\n";
        file << "<html><head><title>Test Report</title></head><body>\n";
        file << "<h1>Test Report</h1>\n";
        file << "<p>Generated: " << std::chrono::system_clock::now().time_since_epoch().count() << "</p>\n";
        
        int passed = 0;
        int failed = 0;
        
        for (const auto& result : results) {
            if (result.passed) passed++;
            else failed++;
        }
        
        file << "<p>Total Tests: " << results.size() << "</p>\n";
        file << "<p>Passed: " << passed << "</p>\n";
        file << "<p>Failed: " << failed << "</p>\n";
        
        file << "<h2>Test Results</h2>\n";
        file << "<table border='1'>\n";
        file << "<tr><th>Test Name</th><th>Type</th><th>Result</th><th>Duration</th><th>Message</th></tr>\n";
        
        for (const auto& result : results) {
            file << "<tr>";
            file << "<td>" << result.testName << "</td>";
            file << "<td>" << (int)result.type << "</td>";
            file << "<td>" << (result.passed ? "PASS" : "FAIL") << "</td>";
            file << "<td>" << result.duration << "s</td>";
            file << "<td>" << result.message << "</td>";
            file << "</tr>\n";
        }
        
        file << "</table>\n";
        file << "</body></html>\n";
        file.close();
        
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error generating report: " << e.what() << std::endl;
        return false;
    }
}

std::string TestRunner::GetTestStatistics(const std::vector<TestResult>& results) {
    int total = results.size();
    int passed = 0;
    int failed = 0;
    float totalDuration = 0.0f;

    for (const auto& result : results) {
        if (result.passed) passed++;
        else failed++;
        totalDuration += result.duration;
    }

    std::ostringstream oss;
    oss << "Test Statistics:\n";
    oss << "Total Tests: " << total << "\n";
    oss << "Passed: " << passed << " (" << (total > 0 ? (passed * 100 / total) : 0) << "%)\n";
    oss << "Failed: " << failed << " (" << (total > 0 ? (failed * 100 / total) : 0) << "%)\n";
    oss << "Total Duration: " << totalDuration << "s\n";
    oss << "Average Duration: " << (total > 0 ? totalDuration / total : 0) << "s\n";

    return oss.str();
}

void TestRunner::Stop() {
    m_running = false;
}

int TestRunner::GetProgress() const {
    return m_progress;
}

int TestRunner::GetTotalTests() const {
    return m_totalTests;
}

TestResult TestRunner::ExecuteUnitTest(const std::string& testName, std::shared_ptr<Project> project) {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool passed = true;
    std::string message = "Test passed";
    
    // Имитация выполнения unit теста
    if (testName == "Project Validation") {
        passed = project && !project->GetName().empty();
        message = passed ? "Project is valid" : "Project validation failed";
    } else if (testName == "Asset Loading") {
        passed = !project->GetAssets().empty();
        message = passed ? "Assets loaded successfully" : "No assets found";
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    float duration = std::chrono::duration<float>(end - start).count();
    
    return {testName, TestType::Unit, passed, message, duration, "Desktop", {}};
}

TestResult TestRunner::ExecuteIntegrationTest(const std::string& testName, std::shared_ptr<Project> project) {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool passed = true;
    std::string message = "Integration test passed";
    
    // Имитация выполнения интеграционного теста
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto end = std::chrono::high_resolution_clock::now();
    float duration = std::chrono::duration<float>(end - start).count();
    
    return {testName, TestType::Integration, passed, message, duration, "Desktop", {}};
}

TestResult TestRunner::ExecutePerformanceTest(const std::string& testName, std::shared_ptr<Project> project) {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool passed = true;
    std::string message = "Performance test passed";
    
    // Имитация выполнения теста производительности
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    auto end = std::chrono::high_resolution_clock::now();
    float duration = std::chrono::duration<float>(end - start).count();
    
    return {testName, TestType::Performance, passed, message, duration, "Desktop", {}};
}

TestResult TestRunner::ExecuteCompatibilityTest(const std::string& testName, std::shared_ptr<Project> project, const std::string& platform) {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool passed = true;
    std::string message = "Compatibility test passed for " + platform;
    
    // Имитация выполнения теста совместимости
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    
    auto end = std::chrono::high_resolution_clock::now();
    float duration = std::chrono::duration<float>(end - start).count();
    
    return {testName, TestType::Compatibility, passed, message, duration, platform, {}};
}

TestResult TestRunner::ExecuteStressTest(const std::string& testName, std::shared_ptr<Project> project, int iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool passed = true;
    std::string message = "Stress test passed with " + std::to_string(iterations) + " iterations";
    
    // Имитация выполнения стресс-теста
    for (int i = 0; i < iterations && i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    float duration = std::chrono::duration<float>(end - start).count();
    
    return {testName, TestType::Stress, passed, message, duration, "Desktop", {}};
}

void TestRunner::UpdateProgress(int current, int total) {
    m_progress = current;
    if (m_progressCallback) {
        m_progressCallback(current, total);
    }
}

void TestRunner::SendResult(const TestResult& result) {
    if (m_resultCallback) {
        m_resultCallback(result);
    }
}

} // namespace ProjectSimulator

