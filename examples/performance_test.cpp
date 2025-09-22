#include "FastEngine/Profiling/PerformanceProfiler.h"
#include "FastEngine/Platform/Platform.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <random>

using namespace FastEngine;

class PerformanceTestApp {
public:
    PerformanceTestApp() : m_duration(60.0f), m_outputFile("performance_report.json") {}
    
    bool Initialize() {
        std::cout << "=== FastEngine Performance Test ===" << std::endl;
        
        // Инициализируем платформу
        if (!Platform::GetInstance().Initialize()) {
            std::cout << "Failed to initialize platform" << std::endl;
            return false;
        }
        
        // Инициализируем монитор производительности
        if (!m_performanceMonitor.Initialize()) {
            std::cout << "Failed to initialize PerformanceMonitor" << std::endl;
            return false;
        }
        
        // Настраиваем callback для алертов
        m_performanceMonitor.SetOnPerformanceAlert([this](const std::string& metric, double value) {
            std::cout << "⚠️  Performance Alert: " << metric << " = " << value << std::endl;
        });
        
        // Настраиваем экспорт
        m_performanceMonitor.SetExportEnabled(true);
        m_performanceMonitor.SetExportInterval(10.0f); // Экспорт каждые 10 секунд
        
        std::cout << "Performance Test initialized successfully!" << std::endl;
        return true;
    }
    
    void Run() {
        std::cout << "\n=== Starting Performance Test ===" << std::endl;
        std::cout << "Duration: " << m_duration << " seconds" << std::endl;
        std::cout << "Output file: " << m_outputFile << std::endl;
        
        // Начинаем мониторинг
        m_performanceMonitor.StartMonitoring();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto lastTime = startTime;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        
        int frameCount = 0;
        float fps = 0.0f;
        
        while (true) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            // Обновляем FPS
            fps = 1.0f / deltaTime;
            
            // Симулируем различные операции
            SimulateCPUWork(deltaTime);
            SimulateGPUWork(deltaTime);
            SimulateMemoryWork(deltaTime);
            SimulateNetworkWork(deltaTime);
            SimulateAudioWork(deltaTime);
            SimulateRenderingWork(deltaTime);
            SimulatePhysicsWork(deltaTime);
            SimulateAIWork(deltaTime);
            
            // Обновляем монитор производительности
            m_performanceMonitor.Update(deltaTime);
            
            // Рендерим отладочную информацию
            RenderDebugInfo(fps);
            
            frameCount++;
            
            // Проверяем, не истекло ли время
            auto elapsed = std::chrono::duration<float>(currentTime - startTime).count();
            if (elapsed >= m_duration) {
                break;
            }
            
            // Симулируем 60 FPS
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        
        // Останавливаем мониторинг
        m_performanceMonitor.StopMonitoring();
        
        // Выводим финальную статистику
        PrintFinalStats();
        
        // Экспортируем данные
        ExportResults();
        
        std::cout << "\n=== Performance Test Completed ===" << std::endl;
    }
    
    void Shutdown() {
        m_performanceMonitor.Shutdown();
        Platform::GetInstance().Shutdown();
        std::cout << "Performance Test shutdown" << std::endl;
    }
    
    void SetDuration(float duration) { m_duration = duration; }
    void SetOutputFile(const std::string& filename) { m_outputFile = filename; }
    
private:
    PerformanceMonitor m_performanceMonitor;
    float m_duration;
    std::string m_outputFile;
    
    void SimulateCPUWork(float deltaTime) {
        m_performanceMonitor.GetCPUProfiler().BeginSample("CPU_Update");
        
        // Симулируем CPU работу
        std::vector<float> data(1000);
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = std::sin(i * deltaTime) * std::cos(i * deltaTime);
        }
        
        // Сортировка
        std::sort(data.begin(), data.end());
        
        // Поиск
        auto it = std::find(data.begin(), data.end(), data[500]);
        
        m_performanceMonitor.GetCPUProfiler().EndSample("CPU_Update");
        
        // Дополнительные CPU операции
        m_performanceMonitor.GetCPUProfiler().BeginSample("CPU_Physics");
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        m_performanceMonitor.GetCPUProfiler().EndSample("CPU_Physics");
        
        m_performanceMonitor.GetCPUProfiler().BeginSample("CPU_AI");
        std::this_thread::sleep_for(std::chrono::microseconds(50));
        m_performanceMonitor.GetCPUProfiler().EndSample("CPU_AI");
    }
    
    void SimulateGPUWork(float deltaTime) {
        m_performanceMonitor.GetGPUProfiler().BeginQuery("GPU_Render");
        
        // Симулируем GPU работу
        int drawCalls = 100 + static_cast<int>(50 * std::sin(deltaTime * 10));
        int triangles = 1000 + static_cast<int>(500 * std::cos(deltaTime * 8));
        int vertices = triangles * 3;
        
        m_performanceMonitor.GetGPUProfiler().RecordDrawCalls(drawCalls);
        m_performanceMonitor.GetGPUProfiler().RecordTriangles(triangles);
        m_performanceMonitor.GetGPUProfiler().RecordVertices(vertices);
        
        // Симулируем время рендеринга
        std::this_thread::sleep_for(std::chrono::microseconds(500));
        
        m_performanceMonitor.GetGPUProfiler().EndQuery("GPU_Render");
        
        // Дополнительные GPU операции
        m_performanceMonitor.GetGPUProfiler().BeginQuery("GPU_PostProcess");
        std::this_thread::sleep_for(std::chrono::microseconds(200));
        m_performanceMonitor.GetGPUProfiler().EndQuery("GPU_PostProcess");
        
        // Обновляем память GPU
        size_t textureMemory = 50 * 1024 * 1024 + static_cast<size_t>(10 * 1024 * 1024 * std::sin(deltaTime * 5));
        size_t bufferMemory = 20 * 1024 * 1024 + static_cast<size_t>(5 * 1024 * 1024 * std::cos(deltaTime * 3));
        
        m_performanceMonitor.GetGPUProfiler().RecordTextureMemory(textureMemory);
        m_performanceMonitor.GetGPUProfiler().RecordBufferMemory(bufferMemory);
    }
    
    void SimulateMemoryWork(float deltaTime) {
        static size_t memoryCounter = 0;
        
        // Симулируем выделение памяти
        size_t allocationSize = 1024 + static_cast<size_t>(512 * std::sin(deltaTime * 2));
        m_performanceMonitor.GetMemoryProfiler().RecordAllocation("GameObjects", allocationSize);
        m_performanceMonitor.GetMemoryProfiler().RecordAllocation("Textures", allocationSize * 2);
        m_performanceMonitor.GetMemoryProfiler().RecordAllocation("Audio", allocationSize / 2);
        
        // Периодически освобождаем память
        if (memoryCounter % 60 == 0) {
            m_performanceMonitor.GetMemoryProfiler().RecordDeallocation("GameObjects", allocationSize);
        }
        
        // Обновляем использование памяти
        size_t currentMemory = 100 * 1024 * 1024 + static_cast<size_t>(20 * 1024 * 1024 * std::sin(deltaTime * 0.5));
        size_t peakMemory = 200 * 1024 * 1024;
        
        m_performanceMonitor.GetMemoryProfiler().RecordMemoryUsage("Total", currentMemory, peakMemory);
        
        memoryCounter++;
    }
    
    void SimulateNetworkWork(float deltaTime) {
        // Симулируем сетевую активность
        static int networkCounter = 0;
        
        if (networkCounter % 30 == 0) {
            m_performanceMonitor.GetCPUProfiler().BeginSample("Network_Update");
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
            m_performanceMonitor.GetCPUProfiler().EndSample("Network_Update");
        }
        
        networkCounter++;
    }
    
    void SimulateAudioWork(float deltaTime) {
        // Симулируем аудио обработку
        static int audioCounter = 0;
        
        if (audioCounter % 10 == 0) {
            m_performanceMonitor.GetCPUProfiler().BeginSample("Audio_Update");
            std::this_thread::sleep_for(std::chrono::microseconds(500));
            m_performanceMonitor.GetCPUProfiler().EndSample("Audio_Update");
        }
        
        audioCounter++;
    }
    
    void SimulateRenderingWork(float deltaTime) {
        // Симулируем рендеринг
        m_performanceMonitor.GetCPUProfiler().BeginSample("Render_Update");
        
        // Симулируем обновление рендер-состояния
        std::this_thread::sleep_for(std::chrono::microseconds(200));
        
        m_performanceMonitor.GetCPUProfiler().EndSample("Render_Update");
    }
    
    void SimulatePhysicsWork(float deltaTime) {
        // Симулируем физику
        m_performanceMonitor.GetCPUProfiler().BeginSample("Physics_Update");
        
        // Симулируем физические вычисления
        std::this_thread::sleep_for(std::chrono::microseconds(300));
        
        m_performanceMonitor.GetCPUProfiler().EndSample("Physics_Update");
    }
    
    void SimulateAIWork(float deltaTime) {
        // Симулируем AI
        m_performanceMonitor.GetCPUProfiler().BeginSample("AI_Update");
        
        // Симулируем AI вычисления
        std::this_thread::sleep_for(std::chrono::microseconds(150));
        
        m_performanceMonitor.GetCPUProfiler().EndSample("AI_Update");
    }
    
    void RenderDebugInfo(float fps) {
        // Рендерим отладочную информацию
        m_performanceMonitor.GetDebugRenderer().RenderFPS(fps);
        
        size_t currentMemory = m_performanceMonitor.GetMemoryProfiler().GetTotalMemoryUsage();
        size_t peakMemory = m_performanceMonitor.GetMemoryProfiler().GetPeakMemoryUsage();
        m_performanceMonitor.GetDebugRenderer().RenderMemoryUsage(currentMemory, peakMemory);
        
        // Рендерим данные профилировщика
        auto cpuMetrics = m_performanceMonitor.GetCPUProfiler().GetMetrics();
        m_performanceMonitor.GetDebugRenderer().RenderProfilerData(cpuMetrics);
    }
    
    void PrintFinalStats() {
        std::cout << "\n=== Final Performance Statistics ===" << std::endl;
        
        // CPU статистика
        auto cpuMetrics = m_performanceMonitor.GetCPUProfiler().GetMetrics();
        std::map<std::string, PerformanceStats> cpuStats;
        
        for (const auto& metric : cpuMetrics) {
            cpuStats[metric.name] = m_performanceMonitor.GetCPUProfiler().GetStats(metric.name);
        }
        
        std::cout << "CPU Performance:" << std::endl;
        for (const auto& pair : cpuStats) {
            const auto& stats = pair.second;
            std::cout << "  " << pair.first << ":" << std::endl;
            std::cout << "    Average: " << stats.average << "ms" << std::endl;
            std::cout << "    Min: " << stats.min << "ms" << std::endl;
            std::cout << "    Max: " << stats.max << "ms" << std::endl;
            std::cout << "    P95: " << stats.p95 << "ms" << std::endl;
            std::cout << "    P99: " << stats.p99 << "ms" << std::endl;
            std::cout << "    Samples: " << stats.sampleCount << std::endl;
        }
        
        // GPU статистика
        auto gpuMetrics = m_performanceMonitor.GetGPUProfiler().GetMetrics();
        std::map<std::string, PerformanceStats> gpuStats;
        
        for (const auto& metric : gpuMetrics) {
            gpuStats[metric.name] = m_performanceMonitor.GetGPUProfiler().GetStats(metric.name);
        }
        
        std::cout << "\nGPU Performance:" << std::endl;
        for (const auto& pair : gpuStats) {
            const auto& stats = pair.second;
            std::cout << "  " << pair.first << ":" << std::endl;
            std::cout << "    Average: " << stats.average << "ms" << std::endl;
            std::cout << "    Min: " << stats.min << "ms" << std::endl;
            std::cout << "    Max: " << stats.max << "ms" << std::endl;
            std::cout << "    P95: " << stats.p95 << "ms" << std::endl;
            std::cout << "    P99: " << stats.p99 << "ms" << std::endl;
            std::cout << "    Samples: " << stats.sampleCount << std::endl;
        }
        
        // Память
        std::cout << "\nMemory Usage:" << std::endl;
        std::cout << "  Current: " << m_performanceMonitor.GetMemoryProfiler().GetTotalMemoryUsage() / 1024 / 1024 << " MB" << std::endl;
        std::cout << "  Peak: " << m_performanceMonitor.GetMemoryProfiler().GetPeakMemoryUsage() / 1024 / 1024 << " MB" << std::endl;
        
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
    
    void ExportResults() {
        std::cout << "\n=== Exporting Results ===" << std::endl;
        
        // Экспортируем в различные форматы
        m_performanceMonitor.ExportToCSV("performance_report.csv");
        m_performanceMonitor.ExportToJSON(m_outputFile);
        m_performanceMonitor.ExportToHTML("performance_report.html");
        
        std::cout << "Results exported to:" << std::endl;
        std::cout << "  - performance_report.csv" << std::endl;
        std::cout << "  - " << m_outputFile << std::endl;
        std::cout << "  - performance_report.html" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    std::cout << "FastEngine Performance Test Application" << std::endl;
    std::cout << "========================================" << std::endl;
    
    PerformanceTestApp app;
    
    // Парсим аргументы командной строки
    float duration = 60.0f;
    std::string outputFile = "performance_report.json";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--duration" && i + 1 < argc) {
            duration = std::stof(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --duration <seconds>  Test duration in seconds (default: 60)" << std::endl;
            std::cout << "  --output <filename>   Output JSON file (default: performance_report.json)" << std::endl;
            std::cout << "  --help               Show this help message" << std::endl;
            return 0;
        }
    }
    
    app.SetDuration(duration);
    app.SetOutputFile(outputFile);
    
    if (!app.Initialize()) {
        std::cout << "Failed to initialize application" << std::endl;
        return 1;
    }
    
    app.Run();
    app.Shutdown();
    
    std::cout << "\nPerformance test completed successfully!" << std::endl;
    return 0;
}

