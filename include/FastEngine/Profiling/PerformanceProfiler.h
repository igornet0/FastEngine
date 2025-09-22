#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>

namespace FastEngine {

/**
 * Типы профилирования
 */
enum class ProfilerType {
    CPU,
    GPU,
    Memory,
    Network,
    Audio,
    Rendering,
    Physics,
    AI
};

/**
 * Метрика производительности
 */
struct PerformanceMetric {
    std::string name;
    ProfilerType type;
    double value;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string unit;
    std::map<std::string, std::string> metadata;
    
    PerformanceMetric() : value(0.0) {}
    PerformanceMetric(const std::string& n, ProfilerType t, double v, const std::string& u = "")
        : name(n), type(t), value(v), unit(u), timestamp(std::chrono::high_resolution_clock::now()) {}
};

/**
 * Статистика производительности
 */
struct PerformanceStats {
    double min = 0.0;
    double max = 0.0;
    double average = 0.0;
    double median = 0.0;
    double p95 = 0.0;
    double p99 = 0.0;
    size_t sampleCount = 0;
    std::chrono::milliseconds totalTime;
    
    void Update(double value) {
        if (sampleCount == 0) {
            min = max = average = value;
        } else {
            min = std::min(min, value);
            max = std::max(max, value);
            average = (average * sampleCount + value) / (sampleCount + 1);
        }
        sampleCount++;
    }
};

/**
 * CPU Profiler
 */
class CPUProfiler {
public:
    CPUProfiler();
    ~CPUProfiler() = default;
    
    // Управление профилированием
    void StartProfiling();
    void StopProfiling();
    void Reset();
    
    // Измерение времени выполнения
    void BeginSample(const std::string& name);
    void EndSample(const std::string& name);
    
    // Получение статистики
    PerformanceStats GetStats(const std::string& name) const;
    std::vector<PerformanceMetric> GetMetrics() const;
    
    // Настройки
    void SetMaxSamples(size_t maxSamples) { m_maxSamples = maxSamples; }
    void SetSamplingRate(float rate) { m_samplingRate = rate; }
    
private:
    struct Sample {
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        double duration;
        bool active;
        
        Sample() : duration(0.0), active(false) {}
    };
    
    std::map<std::string, Sample> m_samples;
    std::vector<PerformanceMetric> m_metrics;
    std::mutex m_mutex;
    std::atomic<bool> m_profiling;
    size_t m_maxSamples;
    float m_samplingRate;
    std::chrono::high_resolution_clock::time_point m_profilingStart;
    
    void AddMetric(const std::string& name, double duration);
};

/**
 * GPU Profiler
 */
class GPUProfiler {
public:
    GPUProfiler();
    ~GPUProfiler() = default;
    
    // Управление профилированием
    void StartProfiling();
    void StopProfiling();
    void Reset();
    
    // Измерение GPU операций
    void BeginQuery(const std::string& name);
    void EndQuery(const std::string& name);
    
    // Получение статистики
    PerformanceStats GetStats(const std::string& name) const;
    std::vector<PerformanceMetric> GetMetrics() const;
    
    // GPU специфичные метрики
    void RecordDrawCalls(int count);
    void RecordTriangles(int count);
    void RecordVertices(int count);
    void RecordTextureMemory(size_t bytes);
    void RecordBufferMemory(size_t bytes);
    
private:
    struct GPUQuery {
        std::string name;
        unsigned int queryId;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        double duration;
        bool active;
        
        GPUQuery() : queryId(0), duration(0.0), active(false) {}
    };
    
    std::map<std::string, GPUQuery> m_queries;
    std::vector<PerformanceMetric> m_metrics;
    std::mutex m_mutex;
    std::atomic<bool> m_profiling;
    
    // GPU статистика
    int m_drawCalls;
    int m_triangles;
    int m_vertices;
    size_t m_textureMemory;
    size_t m_bufferMemory;
    
    void AddMetric(const std::string& name, double duration);
    unsigned int CreateQuery();
    void DeleteQuery(unsigned int queryId);
};

/**
 * Memory Profiler
 */
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler() = default;
    
    // Управление профилированием
    void StartProfiling();
    void StopProfiling();
    void Reset();
    
    // Измерение памяти
    void RecordAllocation(const std::string& category, size_t size);
    void RecordDeallocation(const std::string& category, size_t size);
    void RecordMemoryUsage(const std::string& category, size_t current, size_t peak);
    
    // Получение статистики
    PerformanceStats GetStats(const std::string& category) const;
    std::vector<PerformanceMetric> GetMetrics() const;
    size_t GetTotalMemoryUsage() const;
    size_t GetPeakMemoryUsage() const;
    
    // Анализ утечек памяти
    void DetectMemoryLeaks();
    std::vector<std::string> GetMemoryLeaks() const;
    
private:
    struct MemoryCategory {
        size_t current;
        size_t peak;
        size_t totalAllocated;
        size_t totalDeallocated;
        size_t allocationCount;
        size_t deallocationCount;
        
        MemoryCategory() : current(0), peak(0), totalAllocated(0), totalDeallocated(0), 
                          allocationCount(0), deallocationCount(0) {}
    };
    
    std::map<std::string, MemoryCategory> m_categories;
    std::vector<PerformanceMetric> m_metrics;
    std::mutex m_mutex;
    std::atomic<bool> m_profiling;
    size_t m_totalMemory;
    size_t m_peakMemory;
    
    void AddMetric(const std::string& category, double value, const std::string& unit = "bytes");
};

/**
 * Debug Renderer
 */
class DebugRenderer {
public:
    DebugRenderer();
    ~DebugRenderer() = default;
    
    // Инициализация
    bool Initialize();
    void Shutdown();
    
    // Рендеринг отладочной информации
    void Render();
    void RenderFPS(float fps);
    void RenderMemoryUsage(size_t current, size_t peak);
    void RenderProfilerData(const std::vector<PerformanceMetric>& metrics);
    void RenderWireframe(bool enabled);
    void RenderBoundingBoxes(bool enabled);
    void RenderCollisionShapes(bool enabled);
    
    // Настройки
    void SetFontSize(float size) { m_fontSize = size; }
    void SetTextColor(float r, float g, float b, float a = 1.0f);
    void SetLineColor(float r, float g, float b, float a = 1.0f);
    void SetBackgroundColor(float r, float g, float b, float a = 0.8f);
    
private:
    bool m_initialized;
    float m_fontSize;
    float m_textColor[4];
    float m_lineColor[4];
    float m_backgroundColor[4];
    bool m_wireframeEnabled;
    bool m_boundingBoxesEnabled;
    bool m_collisionShapesEnabled;
    
    void RenderText(const std::string& text, float x, float y);
    void RenderLine(float x1, float y1, float x2, float y2);
    void RenderBox(float x, float y, float width, float height);
};

/**
 * Performance Monitor
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor() = default;
    
    // Инициализация
    bool Initialize();
    void Shutdown();
    
    // Управление профилированием
    void StartMonitoring();
    void StopMonitoring();
    void Reset();
    
    // Получение профилировщиков
    CPUProfiler& GetCPUProfiler() { return m_cpuProfiler; }
    GPUProfiler& GetGPUProfiler() { return m_gpuProfiler; }
    MemoryProfiler& GetMemoryProfiler() { return m_memoryProfiler; }
    DebugRenderer& GetDebugRenderer() { return m_debugRenderer; }
    
    // Обновление
    void Update(float deltaTime);
    
    // Экспорт данных
    void ExportToCSV(const std::string& filename) const;
    void ExportToJSON(const std::string& filename) const;
    void ExportToHTML(const std::string& filename) const;
    
    // Настройки
    void SetMonitoringEnabled(bool enabled) { m_monitoringEnabled = enabled; }
    void SetExportEnabled(bool enabled) { m_exportEnabled = enabled; }
    void SetExportInterval(float interval) { m_exportInterval = interval; }
    
    // Callbacks
    void SetOnPerformanceAlert(std::function<void(const std::string&, double)> callback) {
        m_performanceAlertCallback = callback;
    }
    
private:
    CPUProfiler m_cpuProfiler;
    GPUProfiler m_gpuProfiler;
    MemoryProfiler m_memoryProfiler;
    DebugRenderer m_debugRenderer;
    
    std::atomic<bool> m_monitoringEnabled;
    std::atomic<bool> m_exportEnabled;
    float m_exportInterval;
    float m_exportTimer;
    
    std::function<void(const std::string&, double)> m_performanceAlertCallback;
    
    void CheckPerformanceAlerts();
    void ExportData();
};

} // namespace FastEngine

