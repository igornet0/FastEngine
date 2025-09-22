#include "FastEngine/Export/ProjectExporter.h"
#include "FastEngine/Profiling/PerformanceProfiler.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>

using namespace FastEngine;

class ExportTestApp {
public:
    ExportTestApp() : m_exporter(nullptr) {}
    
    bool Initialize() {
        std::cout << "=== FastEngine Export Test ===" << std::endl;
        
        // Инициализируем экспортер
        m_exporter = std::make_unique<ProjectExporter>();
        if (!m_exporter->Initialize()) {
            std::cout << "Failed to initialize ProjectExporter" << std::endl;
            return false;
        }
        
        // Инициализируем монитор производительности
        if (!m_performanceMonitor.Initialize()) {
            std::cout << "Failed to initialize PerformanceMonitor" << std::endl;
            return false;
        }
        
        // Настраиваем callback для прогресса
        m_exporter->SetProgressCallback([this](float progress, const std::string& message) {
            std::cout << "[" << (int)(progress * 100) << "%] " << message << std::endl;
        });
        
        // Настраиваем callback для алертов производительности
        m_performanceMonitor.SetOnPerformanceAlert([this](const std::string& metric, double value) {
            std::cout << "⚠️  Performance Alert: " << metric << " = " << value << std::endl;
        });
        
        std::cout << "Export Test initialized successfully!" << std::endl;
        return true;
    }
    
    void Run() {
        std::cout << "\n=== Testing Project Export ===" << std::endl;
        
        // Создаем тестовый проект
        std::string projectPath = "./test_project";
        CreateTestProject(projectPath);
        
        // Получаем поддерживаемые платформы
        auto platforms = m_exporter->GetSupportedPlatforms();
        std::cout << "Supported platforms: ";
        for (const auto& platform : platforms) {
            std::cout << platform << " ";
        }
        std::cout << std::endl;
        
        // Тестируем экспорт для каждой платформы
        for (const auto& platform : platforms) {
            std::cout << "\n--- Exporting for " << platform << " ---" << std::endl;
            
            std::string outputPath = "./exports/" + platform;
            
            // Начинаем профилирование
            m_performanceMonitor.StartMonitoring();
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Экспортируем проект
            ExportResult result = m_exporter->ExportProject(projectPath, platform, outputPath);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            // Останавливаем профилирование
            m_performanceMonitor.StopMonitoring();
            
            // Выводим результаты
            if (result.success) {
                std::cout << "✅ Export successful!" << std::endl;
                std::cout << "   Output: " << result.outputPath << std::endl;
                std::cout << "   Build time: " << result.buildTime.count() << "ms" << std::endl;
                std::cout << "   Total size: " << result.totalSize << " bytes" << std::endl;
                
                if (!result.generatedFiles.empty()) {
                    std::cout << "   Generated files:" << std::endl;
                    for (const auto& file : result.generatedFiles) {
                        std::cout << "     - " << file.first << " -> " << file.second << std::endl;
                    }
                }
            } else {
                std::cout << "❌ Export failed!" << std::endl;
                for (const auto& error : result.errors) {
                    std::cout << "   Error: " << error << std::endl;
                }
            }
            
            if (!result.warnings.empty()) {
                std::cout << "   Warnings:" << std::endl;
                for (const auto& warning : result.warnings) {
                    std::cout << "     - " << warning << std::endl;
                }
            }
            
            // Экспортируем данные профилирования
            std::string perfReport = outputPath + "/performance_report";
            m_performanceMonitor.ExportToCSV(perfReport + ".csv");
            m_performanceMonitor.ExportToJSON(perfReport + ".json");
            m_performanceMonitor.ExportToHTML(perfReport + ".html");
            
            std::cout << "   Performance report exported to " << perfReport << ".*" << std::endl;
        }
        
        // Тестируем валидацию проекта
        std::cout << "\n--- Testing Project Validation ---" << std::endl;
        if (m_exporter->ValidateProject(projectPath)) {
            std::cout << "✅ Project validation passed" << std::endl;
        } else {
            std::cout << "❌ Project validation failed" << std::endl;
        }
        
        // Тестируем настройки качества
        std::cout << "\n--- Testing Quality Settings ---" << std::endl;
        TestQualitySettings();
        
        // Тестируем мониторинг производительности
        std::cout << "\n--- Testing Performance Monitoring ---" << std::endl;
        TestPerformanceMonitoring();
        
        std::cout << "\n=== Export Test Completed ===" << std::endl;
    }
    
    void Shutdown() {
        m_performanceMonitor.Shutdown();
        if (m_exporter) {
            m_exporter->Shutdown();
        }
        std::cout << "Export Test shutdown" << std::endl;
    }
    
private:
    std::unique_ptr<ProjectExporter> m_exporter;
    PerformanceMonitor m_performanceMonitor;
    
    void CreateTestProject(const std::string& projectPath) {
        std::cout << "Creating test project at: " << projectPath << std::endl;
        
        // Создаем структуру проекта
        std::filesystem::create_directories(projectPath + "/src");
        std::filesystem::create_directories(projectPath + "/assets/textures");
        std::filesystem::create_directories(projectPath + "/assets/audio");
        std::filesystem::create_directories(projectPath + "/assets/models");
        
        // Создаем project.json
        std::ofstream projectFile(projectPath + "/project.json");
        if (projectFile.is_open()) {
            projectFile << "{\n";
            projectFile << "  \"name\": \"TestProject\",\n";
            projectFile << "  \"version\": \"1.0.0\",\n";
            projectFile << "  \"description\": \"Test project for FastEngine export\",\n";
            projectFile << "  \"main\": \"src/main.cpp\",\n";
            projectFile << "  \"dependencies\": {\n";
            projectFile << "    \"FastEngine\": \"latest\"\n";
            projectFile << "  }\n";
            projectFile << "}\n";
            projectFile.close();
        }
        
        // Создаем main.cpp
        std::ofstream mainFile(projectPath + "/src/main.cpp");
        if (mainFile.is_open()) {
            mainFile << "#include \"FastEngine/FastEngine.h\"\n";
            mainFile << "#include <iostream>\n\n";
            mainFile << "int main() {\n";
            mainFile << "    std::cout << \"Hello from FastEngine!\" << std::endl;\n";
            mainFile << "    return 0;\n";
            mainFile << "}\n";
            mainFile.close();
        }
        
        // Создаем тестовые ресурсы
        std::ofstream textureFile(projectPath + "/assets/textures/test.txt");
        if (textureFile.is_open()) {
            textureFile << "Test texture data\n";
            textureFile.close();
        }
        
        std::ofstream audioFile(projectPath + "/assets/audio/test.txt");
        if (audioFile.is_open()) {
            audioFile << "Test audio data\n";
            audioFile.close();
        }
        
        std::ofstream modelFile(projectPath + "/assets/models/test.txt");
        if (modelFile.is_open()) {
            modelFile << "Test model data\n";
            modelFile.close();
        }
    }
    
    void TestQualitySettings() {
        QualitySettings settings;
        settings.maxLODLevels = 3;
        settings.generateMipmaps = true;
        settings.enableShadows = true;
        settings.shadowMapSize = 2048;
        settings.maxTextureSize = 4096;
        settings.compressTextures = true;
        settings.textureFormat = "DXT5";
        settings.audioSampleRate = 48000;
        settings.audioBitRate = 256;
        settings.audioFormat = "OGG";
        settings.stripDebugInfo = true;
        settings.optimizeForSize = true;
        
        m_exporter->SetQualitySettings(settings);
        
        const auto& currentSettings = m_exporter->GetQualitySettings();
        std::cout << "Quality settings applied:" << std::endl;
        std::cout << "  LOD levels: " << currentSettings.maxLODLevels << std::endl;
        std::cout << "  Mipmaps: " << (currentSettings.generateMipmaps ? "enabled" : "disabled") << std::endl;
        std::cout << "  Shadows: " << (currentSettings.enableShadows ? "enabled" : "disabled") << std::endl;
        std::cout << "  Shadow map size: " << currentSettings.shadowMapSize << std::endl;
        std::cout << "  Max texture size: " << currentSettings.maxTextureSize << std::endl;
        std::cout << "  Texture compression: " << (currentSettings.compressTextures ? "enabled" : "disabled") << std::endl;
        std::cout << "  Texture format: " << currentSettings.textureFormat << std::endl;
        std::cout << "  Audio sample rate: " << currentSettings.audioSampleRate << std::endl;
        std::cout << "  Audio bit rate: " << currentSettings.audioBitRate << std::endl;
        std::cout << "  Audio format: " << currentSettings.audioFormat << std::endl;
        std::cout << "  Strip debug info: " << (currentSettings.stripDebugInfo ? "enabled" : "disabled") << std::endl;
        std::cout << "  Optimize for size: " << (currentSettings.optimizeForSize ? "enabled" : "disabled") << std::endl;
    }
    
    void TestPerformanceMonitoring() {
        std::cout << "Testing performance monitoring..." << std::endl;
        
        // Начинаем мониторинг
        m_performanceMonitor.StartMonitoring();
        
        // Симулируем работу
        for (int i = 0; i < 100; ++i) {
            // CPU профилирование
            m_performanceMonitor.GetCPUProfiler().BeginSample("TestOperation");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            m_performanceMonitor.GetCPUProfiler().EndSample("TestOperation");
            
            // GPU профилирование
            m_performanceMonitor.GetGPUProfiler().BeginQuery("TestRender");
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            m_performanceMonitor.GetGPUProfiler().EndQuery("TestRender");
            
            // Записываем метрики GPU
            m_performanceMonitor.GetGPUProfiler().RecordDrawCalls(100);
            m_performanceMonitor.GetGPUProfiler().RecordTriangles(1000);
            m_performanceMonitor.GetGPUProfiler().RecordVertices(3000);
            m_performanceMonitor.GetGPUProfiler().RecordTextureMemory(1024 * 1024);
            m_performanceMonitor.GetGPUProfiler().RecordBufferMemory(512 * 1024);
            
            // Память
            m_performanceMonitor.GetMemoryProfiler().RecordAllocation("TestCategory", 1024);
            m_performanceMonitor.GetMemoryProfiler().RecordMemoryUsage("TestCategory", 1024 * i, 1024 * 100);
            
            // Обновляем монитор
            m_performanceMonitor.Update(0.016f); // 60 FPS
        }
        
        // Останавливаем мониторинг
        m_performanceMonitor.StopMonitoring();
        
        // Получаем статистику
        auto cpuStats = m_performanceMonitor.GetCPUProfiler().GetStats("TestOperation");
        auto gpuStats = m_performanceMonitor.GetGPUProfiler().GetStats("TestRender");
        auto memoryStats = m_performanceMonitor.GetMemoryProfiler().GetStats("TestCategory");
        
        std::cout << "Performance statistics:" << std::endl;
        std::cout << "  CPU TestOperation - Avg: " << cpuStats.average << "ms, Min: " << cpuStats.min 
                  << "ms, Max: " << cpuStats.max << "ms" << std::endl;
        std::cout << "  GPU TestRender - Avg: " << gpuStats.average << "ms, Min: " << gpuStats.min 
                  << "ms, Max: " << gpuStats.max << "ms" << std::endl;
        std::cout << "  Memory TestCategory - Current: " << memoryStats.average 
                  << " bytes, Peak: " << memoryStats.max << " bytes" << std::endl;
        
        // Проверяем утечки памяти
        m_performanceMonitor.GetMemoryProfiler().DetectMemoryLeaks();
        auto leaks = m_performanceMonitor.GetMemoryProfiler().GetMemoryLeaks();
        if (!leaks.empty()) {
            std::cout << "  Memory leaks detected:" << std::endl;
            for (const auto& leak : leaks) {
                std::cout << "    - " << leak << std::endl;
            }
        } else {
            std::cout << "  No memory leaks detected" << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    std::cout << "FastEngine Export Test Application" << std::endl;
    std::cout << "===================================" << std::endl;
    
    ExportTestApp app;
    
    if (!app.Initialize()) {
        std::cout << "Failed to initialize application" << std::endl;
        return 1;
    }
    
    app.Run();
    app.Shutdown();
    
    std::cout << "\nExport test completed successfully!" << std::endl;
    return 0;
}
