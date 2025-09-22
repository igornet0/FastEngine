#include "ProjectSimulator/CLI.h"
#include "ProjectSimulator/ProjectSimulator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace ProjectSimulator {

CLI::CLI()
    : m_projectsPath("./projects")
    , m_outputPath("./test_results")
    , m_initialized(false) {
}

CLI::~CLI() {
    // Очистка ресурсов
}

bool CLI::Initialize() {
    if (m_initialized) {
        return true;
    }

    try {
        // Создаем директории если они не существуют
        std::filesystem::create_directories(m_projectsPath);
        std::filesystem::create_directories(m_outputPath);

        m_initialized = true;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize CLI: " << e.what() << std::endl;
        return false;
    }
}

CLIArgs CLI::ParseArgs(int argc, char* argv[]) {
    CLIArgs args;
    
    // Значения по умолчанию
    args.command = "help";
    args.projectPath = "";
    args.outputPath = m_outputPath;
    args.testTypes = {"unit", "integration"};
    args.platforms = {"desktop"};
    args.iterations = 1;
    args.timeout = 30.0f;
    args.verbose = false;
    args.generateReport = true;
    args.help = false;
    args.listProjects = false;
    args.listTests = false;
    args.configFile = "";

    // Парсинг аргументов
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--list-projects" || arg == "-l") {
            args.listProjects = true;
        } else if (arg == "--list-tests") {
            args.listTests = true;
        } else if (arg == "--project" || arg == "-p") {
            if (i + 1 < argc) {
                args.projectPath = argv[++i];
            }
        } else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                args.outputPath = argv[++i];
            }
        } else if (arg == "--test-types" || arg == "-t") {
            if (i + 1 < argc) {
                std::string types = argv[++i];
                std::istringstream ss(types);
                std::string type;
                args.testTypes.clear();
                while (std::getline(ss, type, ',')) {
                    args.testTypes.push_back(type);
                }
            }
        } else if (arg == "--platforms") {
            if (i + 1 < argc) {
                std::string platforms = argv[++i];
                std::istringstream ss(platforms);
                std::string platform;
                args.platforms.clear();
                while (std::getline(ss, platform, ',')) {
                    args.platforms.push_back(platform);
                }
            }
        } else if (arg == "--iterations" || arg == "-i") {
            if (i + 1 < argc) {
                args.iterations = std::stoi(argv[++i]);
            }
        } else if (arg == "--timeout") {
            if (i + 1 < argc) {
                args.timeout = std::stof(argv[++i]);
            }
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--no-report") {
            args.generateReport = false;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg == "test") {
            args.command = "test";
        } else if (arg == "simulate") {
            args.command = "simulate";
        } else if (arg == "validate") {
            args.command = "validate";
        } else if (arg == "create") {
            args.command = "create";
        } else if (arg == "batch-test") {
            args.command = "batch-test";
        } else if (arg == "help") {
            args.command = "help";
        } else if (arg[0] != '-') {
            // Если это не флаг, то это может быть команда
            args.command = arg;
        }
    }

    // Загружаем конфигурацию из файла если указан
    if (!args.configFile.empty()) {
        CLIArgs configArgs = LoadConfig(args.configFile);
        // Применяем конфигурацию, но аргументы командной строки имеют приоритет
        if (args.projectPath.empty()) args.projectPath = configArgs.projectPath;
        if (args.outputPath == m_outputPath) args.outputPath = configArgs.outputPath;
        if (args.testTypes.size() == 2 && args.testTypes[0] == "unit") {
            args.testTypes = configArgs.testTypes;
        }
        if (args.platforms.size() == 1 && args.platforms[0] == "desktop") {
            args.platforms = configArgs.platforms;
        }
        if (args.iterations == 1) args.iterations = configArgs.iterations;
        if (args.timeout == 30.0f) args.timeout = configArgs.timeout;
        if (!args.verbose) args.verbose = configArgs.verbose;
        if (args.generateReport) args.generateReport = configArgs.generateReport;
    }

    return args;
}

int CLI::ExecuteCommand(const CLIArgs& args) {
    if (!m_initialized) {
        std::cerr << "CLI not initialized" << std::endl;
        return 1;
    }

    if (args.help) {
        ShowHelp();
        return 0;
    }

    if (args.listProjects) {
        ListProjects();
        return 0;
    }

    if (args.listTests) {
        ListTests();
        return 0;
    }

    if (args.command == "test") {
        return RunTests(args);
    } else if (args.command == "simulate") {
        return RunSimulation(args);
    } else if (args.command == "validate") {
        return ValidateProject(args);
    } else if (args.command == "create") {
        return CreateProject(args);
    } else if (args.command == "batch-test") {
        return BatchTest(args);
    } else {
        std::cerr << "Unknown command: " << args.command << std::endl;
        ShowHelp();
        return 1;
    }
}

void CLI::ShowHelp() {
    std::cout << "Project Simulator CLI - Command Line Interface" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE:" << std::endl;
    std::cout << "  simulator-cli [COMMAND] [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "COMMANDS:" << std::endl;
    std::cout << "  test                    Run tests on a project" << std::endl;
    std::cout << "  simulate                Run simulation of a project" << std::endl;
    std::cout << "  validate                Validate a project" << std::endl;
    std::cout << "  create                  Create a new project" << std::endl;
    std::cout << "  batch-test              Run tests on all projects" << std::endl;
    std::cout << "  help                    Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  -h, --help              Show help message" << std::endl;
    std::cout << "  -l, --list-projects     List available projects" << std::endl;
    std::cout << "  --list-tests            List available test types" << std::endl;
    std::cout << "  -p, --project PATH      Path to project directory" << std::endl;
    std::cout << "  -o, --output PATH       Output directory for results" << std::endl;
    std::cout << "  -t, --test-types TYPES  Comma-separated test types" << std::endl;
    std::cout << "  --platforms PLATFORMS   Comma-separated target platforms" << std::endl;
    std::cout << "  -i, --iterations N      Number of test iterations" << std::endl;
    std::cout << "  --timeout SECONDS       Test timeout in seconds" << std::endl;
    std::cout << "  -v, --verbose           Verbose output" << std::endl;
    std::cout << "  --no-report             Don't generate HTML report" << std::endl;
    std::cout << "  -c, --config FILE       Load configuration from file" << std::endl;
    std::cout << std::endl;
    std::cout << "TEST TYPES:" << std::endl;
    std::cout << "  unit                    Unit tests" << std::endl;
    std::cout << "  integration             Integration tests" << std::endl;
    std::cout << "  performance             Performance tests" << std::endl;
    std::cout << "  compatibility           Compatibility tests" << std::endl;
    std::cout << "  stress                  Stress tests" << std::endl;
    std::cout << std::endl;
    std::cout << "PLATFORMS:" << std::endl;
    std::cout << "  desktop                 Desktop platforms" << std::endl;
    std::cout << "  ios                     iOS platform" << std::endl;
    std::cout << "  android                 Android platform" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  simulator-cli test -p ./projects/basic_game" << std::endl;
    std::cout << "  simulator-cli test -p ./projects/basic_game -t unit,performance" << std::endl;
    std::cout << "  simulator-cli simulate -p ./projects/sprite_demo" << std::endl;
    std::cout << "  simulator-cli batch-test --platforms desktop,ios" << std::endl;
    std::cout << "  simulator-cli create -p ./projects/my_game" << std::endl;
    std::cout << "  simulator-cli validate -p ./projects/basic_game" << std::endl;
    std::cout << std::endl;
}

void CLI::ListProjects() {
    std::cout << "Available Projects:" << std::endl;
    std::cout << "==================" << std::endl;
    
    auto projects = FindProjects(m_projectsPath);
    
    if (projects.empty()) {
        std::cout << "No projects found in " << m_projectsPath << std::endl;
        return;
    }
    
    for (const auto& projectPath : projects) {
        std::filesystem::path path(projectPath);
        std::string projectName = path.parent_path().filename().string();
        
        // Загружаем информацию о проекте
        auto project = std::make_shared<Project>();
        if (project->LoadFromFile(projectPath)) {
            std::cout << "  " << projectName << " - " << project->GetDescription() << std::endl;
            std::cout << "    Path: " << projectPath << std::endl;
            std::cout << "    Version: " << project->GetVersion() << std::endl;
            std::cout << "    Author: " << project->GetAuthor() << std::endl;
            std::cout << std::endl;
        }
    }
}

void CLI::ListTests() {
    std::cout << "Available Test Types:" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << "  unit         - Unit tests (component testing)" << std::endl;
    std::cout << "  integration  - Integration tests (system testing)" << std::endl;
    std::cout << "  performance  - Performance tests (FPS, memory)" << std::endl;
    std::cout << "  compatibility - Compatibility tests (platform testing)" << std::endl;
    std::cout << "  stress       - Stress tests (load testing)" << std::endl;
    std::cout << std::endl;
    std::cout << "Available Platforms:" << std::endl;
    std::cout << "===================" << std::endl;
    std::cout << "  desktop      - Desktop platforms (macOS, Linux, Windows)" << std::endl;
    std::cout << "  ios          - iOS platform" << std::endl;
    std::cout << "  android      - Android platform" << std::endl;
    std::cout << std::endl;
}

int CLI::RunTests(const CLIArgs& args) {
    if (args.projectPath.empty()) {
        std::cerr << "Error: Project path not specified" << std::endl;
        std::cerr << "Use: simulator-cli test -p <project_path>" << std::endl;
        return 1;
    }

    if (!std::filesystem::exists(args.projectPath)) {
        std::cerr << "Error: Project not found: " << args.projectPath << std::endl;
        return 1;
    }

    std::cout << "Running tests for project: " << args.projectPath << std::endl;
    std::cout << "================================" << std::endl;

    // Создаем симулятор
    ProjectSimulator::ProjectSimulator simulator;
    if (!simulator.Initialize(800, 600)) {
        std::cerr << "Failed to initialize simulator" << std::endl;
        return 1;
    }

    // Загружаем проект
    auto project = simulator.GetProjectManager()->LoadProject(args.projectPath);
    if (!project) {
        std::cerr << "Failed to load project" << std::endl;
        return 1;
    }

    // Настраиваем тестирование
    TestConfig config;
    
    // Преобразуем типы тестов
    for (const auto& type : args.testTypes) {
        if (type == "unit") {
            config.enabledTests.push_back(TestType::Unit);
        } else if (type == "integration") {
            config.enabledTests.push_back(TestType::Integration);
        } else if (type == "performance") {
            config.enabledTests.push_back(TestType::Performance);
        } else if (type == "compatibility") {
            config.enabledTests.push_back(TestType::Compatibility);
        } else if (type == "stress") {
            config.enabledTests.push_back(TestType::Stress);
        }
    }

    config.targetPlatforms = args.platforms;
    config.iterations = args.iterations;
    config.timeout = args.timeout;
    config.generateReport = args.generateReport;
    config.reportPath = args.outputPath + "/";

    // Запускаем тесты
    auto testRunner = simulator.GetTestRunner();
    auto results = testRunner->RunTests(project, config);

    // Выводим результаты
    std::cout << std::endl;
    std::cout << "Test Results:" << std::endl;
    std::cout << "============" << std::endl;

    for (const auto& result : results) {
        PrintTestResult(result, args.verbose);
    }

    // Выводим статистику
    PrintTestStatistics(results);

    // Генерируем отчет
    if (args.generateReport) {
        std::string reportPath = args.outputPath + "/test_report_" + 
                                std::to_string(std::time(nullptr)) + ".html";
        if (GenerateReport(results, reportPath) == 0) {
            std::cout << "Report generated: " << reportPath << std::endl;
        }
    }

    // Определяем код возврата
    int failedTests = 0;
    for (const auto& result : results) {
        if (!result.passed) {
            failedTests++;
        }
    }

    return failedTests > 0 ? 1 : 0;
}

int CLI::RunSimulation(const CLIArgs& args) {
    if (args.projectPath.empty()) {
        std::cerr << "Error: Project path not specified" << std::endl;
        std::cerr << "Use: simulator-cli simulate -p <project_path>" << std::endl;
        return 1;
    }

    if (!std::filesystem::exists(args.projectPath)) {
        std::cerr << "Error: Project not found: " << args.projectPath << std::endl;
        return 1;
    }

    std::cout << "Running simulation for project: " << args.projectPath << std::endl;
    std::cout << "=====================================" << std::endl;

    // Создаем симулятор
    ProjectSimulator::ProjectSimulator simulator;
    if (!simulator.Initialize(800, 600)) {
        std::cerr << "Failed to initialize simulator" << std::endl;
        return 1;
    }

    // Загружаем проект
    auto project = simulator.GetProjectManager()->LoadProject(args.projectPath);
    if (!project) {
        std::cerr << "Failed to load project" << std::endl;
        return 1;
    }

    // Запускаем симуляцию
    auto simulationEngine = simulator.GetSimulationEngine();
    auto result = simulationEngine->RunProject(project);

    std::cout << std::endl;
    std::cout << "Simulation Results:" << std::endl;
    std::cout << "==================" << std::endl;
    std::cout << "Success: " << (result.success ? "Yes" : "No") << std::endl;
    std::cout << "Duration: " << result.duration << " seconds" << std::endl;
    std::cout << "Exit Code: " << result.exitCode << std::endl;

    if (!result.success) {
        std::cout << "Error: " << result.errorMessage << std::endl;
    }

    if (args.verbose && !result.logOutput.empty()) {
        std::cout << std::endl;
        std::cout << "Log Output:" << std::endl;
        std::cout << "===========" << std::endl;
        std::cout << result.logOutput << std::endl;
    }

    return result.success ? 0 : 1;
}

int CLI::ValidateProject(const CLIArgs& args) {
    if (args.projectPath.empty()) {
        std::cerr << "Error: Project path not specified" << std::endl;
        std::cerr << "Use: simulator-cli validate -p <project_path>" << std::endl;
        return 1;
    }

    if (!std::filesystem::exists(args.projectPath)) {
        std::cerr << "Error: Project not found: " << args.projectPath << std::endl;
        return 1;
    }

    std::cout << "Validating project: " << args.projectPath << std::endl;
    std::cout << "=========================" << std::endl;

    // Создаем симулятор
    ProjectSimulator::ProjectSimulator simulator;
    if (!simulator.Initialize(800, 600)) {
        std::cerr << "Failed to initialize simulator" << std::endl;
        return 1;
    }

    // Загружаем проект
    auto project = simulator.GetProjectManager()->LoadProject(args.projectPath);
    if (!project) {
        std::cerr << "Failed to load project" << std::endl;
        return 1;
    }

    // Валидируем проект
    bool isValid = simulator.GetProjectManager()->ValidateProject(project);
    
    std::cout << "Project: " << project->GetName() << std::endl;
    std::cout << "Version: " << project->GetVersion() << std::endl;
    std::cout << "Author: " << project->GetAuthor() << std::endl;
    std::cout << "Valid: " << (isValid ? "Yes" : "No") << std::endl;

    if (!isValid) {
        std::cout << std::endl;
        std::cout << "Validation Errors:" << std::endl;
        std::cout << "==================" << std::endl;
        // Здесь можно добавить детальную информацию об ошибках
        std::cout << "Project validation failed. Check project structure and files." << std::endl;
    }

    return isValid ? 0 : 1;
}

int CLI::CreateProject(const CLIArgs& args) {
    if (args.projectPath.empty()) {
        std::cerr << "Error: Project path not specified" << std::endl;
        std::cerr << "Use: simulator-cli create -p <project_path>" << std::endl;
        return 1;
    }

    std::cout << "Creating project: " << args.projectPath << std::endl;
    std::cout << "=========================" << std::endl;

    // Создаем симулятор
    ProjectSimulator::ProjectSimulator simulator;
    if (!simulator.Initialize(800, 600)) {
        std::cerr << "Failed to initialize simulator" << std::endl;
        return 1;
    }

    // Извлекаем имя проекта из пути
    std::filesystem::path path(args.projectPath);
    std::string projectName = path.filename().string();
    
    // Создаем проект
    auto project = simulator.GetProjectManager()->CreateProject(projectName, "empty");
    if (!project) {
        std::cerr << "Failed to create project" << std::endl;
        return 1;
    }

    std::cout << "Project created successfully!" << std::endl;
    std::cout << "Name: " << project->GetName() << std::endl;
    std::cout << "Path: " << project->GetPath() << std::endl;
    std::cout << "Version: " << project->GetVersion() << std::endl;

    return 0;
}

int CLI::BatchTest(const CLIArgs& args) {
    std::cout << "Running batch tests on all projects" << std::endl;
    std::cout << "===================================" << std::endl;

    // Создаем симулятор
    ProjectSimulator::ProjectSimulator simulator;
    if (!simulator.Initialize(800, 600)) {
        std::cerr << "Failed to initialize simulator" << std::endl;
        return 1;
    }

    // Находим все проекты
    auto projects = FindProjects(m_projectsPath);
    
    if (projects.empty()) {
        std::cout << "No projects found in " << m_projectsPath << std::endl;
        return 0;
    }

    std::cout << "Found " << projects.size() << " projects" << std::endl;
    std::cout << std::endl;

    int totalProjects = 0;
    int passedProjects = 0;
    int failedProjects = 0;

    // Тестируем каждый проект
    for (const auto& projectPath : projects) {
        totalProjects++;
        
        std::filesystem::path path(projectPath);
        std::string projectName = path.parent_path().filename().string();
        
        std::cout << "Testing project: " << projectName << std::endl;
        std::cout << "Path: " << projectPath << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        // Загружаем проект
        auto project = simulator.GetProjectManager()->LoadProject(projectPath);
        if (!project) {
            std::cout << "Failed to load project" << std::endl;
            failedProjects++;
            continue;
        }

        // Настраиваем тестирование
        TestConfig config;
        config.enabledTests = {TestType::Unit, TestType::Integration};
        config.targetPlatforms = args.platforms;
        config.iterations = 1;
        config.timeout = args.timeout;
        config.generateReport = false; // Не генерируем отчет для каждого проекта

        // Запускаем тесты
        auto testRunner = simulator.GetTestRunner();
        auto results = testRunner->RunTests(project, config);

        // Подсчитываем результаты
        int passedTests = 0;
        int failedTests = 0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passedTests++;
            } else {
                failedTests++;
            }
        }

        std::cout << "Results: " << passedTests << " passed, " << failedTests << " failed" << std::endl;
        
        if (failedTests == 0) {
            passedProjects++;
            std::cout << "Status: PASSED" << std::endl;
        } else {
            failedProjects++;
            std::cout << "Status: FAILED" << std::endl;
        }

        std::cout << std::endl;
    }

    // Выводим общую статистику
    std::cout << "Batch Test Summary:" << std::endl;
    std::cout << "==================" << std::endl;
    std::cout << "Total Projects: " << totalProjects << std::endl;
    std::cout << "Passed: " << passedProjects << std::endl;
    std::cout << "Failed: " << failedProjects << std::endl;
    std::cout << "Success Rate: " << (totalProjects > 0 ? (passedProjects * 100 / totalProjects) : 0) << "%" << std::endl;

    return failedProjects > 0 ? 1 : 0;
}

int CLI::GenerateReport(const std::vector<TestResult>& results, const std::string& outputPath) {
    // Создаем симулятор для доступа к TestRunner
    ProjectSimulator::ProjectSimulator simulator;
    if (!simulator.Initialize(800, 600)) {
        return 1;
    }

    auto testRunner = simulator.GetTestRunner();
    return testRunner->GenerateReport(results, outputPath) ? 0 : 1;
}

CLIArgs CLI::LoadConfig(const std::string& configPath) {
    CLIArgs args;
    
    std::ifstream file(configPath);
    if (!file.is_open()) {
        return args;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Убираем пробелы
        key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
        value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());

        if (key == "project_path") {
            args.projectPath = value;
        } else if (key == "output_path") {
            args.outputPath = value;
        } else if (key == "test_types") {
            std::istringstream ss(value);
            std::string type;
            args.testTypes.clear();
            while (std::getline(ss, type, ',')) {
                args.testTypes.push_back(type);
            }
        } else if (key == "platforms") {
            std::istringstream ss(value);
            std::string platform;
            args.platforms.clear();
            while (std::getline(ss, platform, ',')) {
                args.platforms.push_back(platform);
            }
        } else if (key == "iterations") {
            args.iterations = std::stoi(value);
        } else if (key == "timeout") {
            args.timeout = std::stof(value);
        } else if (key == "verbose") {
            args.verbose = (value == "true" || value == "1");
        } else if (key == "generate_report") {
            args.generateReport = (value == "true" || value == "1");
        }
    }

    file.close();
    return args;
}

bool CLI::SaveConfig(const CLIArgs& args, const std::string& configPath) {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        return false;
    }

    file << "# Project Simulator CLI Configuration" << std::endl;
    file << "# Generated on " << std::time(nullptr) << std::endl;
    file << std::endl;

    file << "project_path=" << args.projectPath << std::endl;
    file << "output_path=" << args.outputPath << std::endl;
    
    file << "test_types=";
    for (size_t i = 0; i < args.testTypes.size(); ++i) {
        if (i > 0) file << ",";
        file << args.testTypes[i];
    }
    file << std::endl;
    
    file << "platforms=";
    for (size_t i = 0; i < args.platforms.size(); ++i) {
        if (i > 0) file << ",";
        file << args.platforms[i];
    }
    file << std::endl;
    
    file << "iterations=" << args.iterations << std::endl;
    file << "timeout=" << args.timeout << std::endl;
    file << "verbose=" << (args.verbose ? "true" : "false") << std::endl;
    file << "generate_report=" << (args.generateReport ? "true" : "false") << std::endl;

    file.close();
    return true;
}

std::vector<std::string> CLI::FindProjects(const std::string& directory) {
    std::vector<std::string> projects;
    
    if (!std::filesystem::exists(directory)) {
        return projects;
    }

    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().filename() == "project.conf") {
                projects.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error scanning directory: " << e.what() << std::endl;
    }

    return projects;
}

void CLI::PrintTestResult(const TestResult& result, bool verbose) {
    std::cout << "  " << (result.passed ? "✓" : "✗") << " " << result.testName;
    
    if (verbose) {
        std::cout << " (" << result.duration << "s)";
        if (!result.message.empty()) {
            std::cout << " - " << result.message;
        }
    }
    
    std::cout << std::endl;
}

void CLI::PrintTestStatistics(const std::vector<TestResult>& results) {
    int total = results.size();
    int passed = 0;
    int failed = 0;
    float totalDuration = 0.0f;

    for (const auto& result : results) {
        if (result.passed) {
            passed++;
        } else {
            failed++;
        }
        totalDuration += result.duration;
    }

    std::cout << std::endl;
    std::cout << "Test Statistics:" << std::endl;
    std::cout << "===============" << std::endl;
    std::cout << "Total Tests: " << total << std::endl;
    std::cout << "Passed: " << passed << " (" << (total > 0 ? (passed * 100 / total) : 0) << "%)" << std::endl;
    std::cout << "Failed: " << failed << " (" << (total > 0 ? (failed * 100 / total) : 0) << "%)" << std::endl;
    std::cout << "Total Duration: " << totalDuration << "s" << std::endl;
    std::cout << "Average Duration: " << (total > 0 ? totalDuration / total : 0) << "s" << std::endl;
}

} // namespace ProjectSimulator

