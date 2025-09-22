#include "FastEngine/Profiling/PerformanceProfiler.h"
#include "FastEngine/Platform/FileSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace FastEngine {

// CPUProfiler implementation
CPUProfiler::CPUProfiler() 
    : m_profiling(false)
    , m_maxSamples(10000)
    , m_samplingRate(1.0f) {
}

void CPUProfiler::StartProfiling() {
    m_profiling = true;
    m_profilingStart = std::chrono::high_resolution_clock::now();
    std::cout << "CPUProfiler: Started profiling" << std::endl;
}

void CPUProfiler::StopProfiling() {
    m_profiling = false;
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_profilingStart);
    std::cout << "CPUProfiler: Stopped profiling after " << duration.count() << "ms" << std::endl;
}

void CPUProfiler::Reset() {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    m_samples.clear();
    m_metrics.clear();
    std::cout << "CPUProfiler: Reset" << std::endl;
}

void CPUProfiler::BeginSample(const std::string& name) {
    if (!m_profiling) return;
    
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    auto& sample = m_samples[name];
    sample.name = name;
    sample.startTime = std::chrono::high_resolution_clock::now();
    sample.active = true;
}

void CPUProfiler::EndSample(const std::string& name) {
    if (!m_profiling) return;
    
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    auto it = m_samples.find(name);
    if (it != m_samples.end() && it->second.active) {
        it->second.endTime = std::chrono::high_resolution_clock::now();
        it->second.duration = std::chrono::duration<double, std::milli>(
            it->second.endTime - it->second.startTime).count();
        it->second.active = false;
        
        AddMetric(name, it->second.duration);
    }
}

PerformanceStats CPUProfiler::GetStats(const std::string& name) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    PerformanceStats stats;
    
    std::vector<double> values;
    for (const auto& metric : m_metrics) {
        if (metric.name == name) {
            values.push_back(metric.value);
        }
    }
    
    if (values.empty()) {
        return stats;
    }
    
    std::sort(values.begin(), values.end());
    
    stats.min = values.front();
    stats.max = values.back();
    stats.average = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    stats.sampleCount = values.size();
    
    // Медиана
    if (values.size() % 2 == 0) {
        stats.median = (values[values.size() / 2 - 1] + values[values.size() / 2]) / 2.0;
    } else {
        stats.median = values[values.size() / 2];
    }
    
    // 95-й и 99-й перцентили
    if (values.size() > 0) {
        size_t p95Index = static_cast<size_t>(values.size() * 0.95);
        size_t p99Index = static_cast<size_t>(values.size() * 0.99);
        stats.p95 = values[std::min(p95Index, values.size() - 1)];
        stats.p99 = values[std::min(p99Index, values.size() - 1)];
    }
    
    return stats;
}

std::vector<PerformanceMetric> CPUProfiler::GetMetrics() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    return m_metrics;
}

void CPUProfiler::AddMetric(const std::string& name, double duration) {
    if (m_metrics.size() >= m_maxSamples) {
        m_metrics.erase(m_metrics.begin());
    }
    
    PerformanceMetric metric(name, ProfilerType::CPU, duration, "ms");
    m_metrics.push_back(metric);
}

// GPUProfiler implementation
GPUProfiler::GPUProfiler() 
    : m_profiling(false)
    , m_drawCalls(0)
    , m_triangles(0)
    , m_vertices(0)
    , m_textureMemory(0)
    , m_bufferMemory(0) {
}

void GPUProfiler::StartProfiling() {
    m_profiling = true;
    std::cout << "GPUProfiler: Started profiling" << std::endl;
}

void GPUProfiler::StopProfiling() {
    m_profiling = false;
    std::cout << "GPUProfiler: Stopped profiling" << std::endl;
}

void GPUProfiler::Reset() {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    m_queries.clear();
    m_metrics.clear();
    m_drawCalls = 0;
    m_triangles = 0;
    m_vertices = 0;
    m_textureMemory = 0;
    m_bufferMemory = 0;
    std::cout << "GPUProfiler: Reset" << std::endl;
}

void GPUProfiler::BeginQuery(const std::string& name) {
    if (!m_profiling) return;
    
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    auto& query = m_queries[name];
    query.name = name;
    query.queryId = CreateQuery();
    query.startTime = std::chrono::high_resolution_clock::now();
    query.active = true;
}

void GPUProfiler::EndQuery(const std::string& name) {
    if (!m_profiling) return;
    
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    auto it = m_queries.find(name);
    if (it != m_queries.end() && it->second.active) {
        it->second.endTime = std::chrono::high_resolution_clock::now();
        it->second.duration = std::chrono::duration<double, std::milli>(
            it->second.endTime - it->second.startTime).count();
        it->second.active = false;
        
        AddMetric(name, it->second.duration);
        DeleteQuery(it->second.queryId);
    }
}

PerformanceStats GPUProfiler::GetStats(const std::string& name) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    PerformanceStats stats;
    
    std::vector<double> values;
    for (const auto& metric : m_metrics) {
        if (metric.name == name) {
            values.push_back(metric.value);
        }
    }
    
    if (values.empty()) {
        return stats;
    }
    
    std::sort(values.begin(), values.end());
    
    stats.min = values.front();
    stats.max = values.back();
    stats.average = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    stats.sampleCount = values.size();
    
    if (values.size() % 2 == 0) {
        stats.median = (values[values.size() / 2 - 1] + values[values.size() / 2]) / 2.0;
    } else {
        stats.median = values[values.size() / 2];
    }
    
    if (values.size() > 0) {
        size_t p95Index = static_cast<size_t>(values.size() * 0.95);
        size_t p99Index = static_cast<size_t>(values.size() * 0.99);
        stats.p95 = values[std::min(p95Index, values.size() - 1)];
        stats.p99 = values[std::min(p99Index, values.size() - 1)];
    }
    
    return stats;
}

std::vector<PerformanceMetric> GPUProfiler::GetMetrics() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    return m_metrics;
}

void GPUProfiler::RecordDrawCalls(int count) {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    m_drawCalls += count;
}

void GPUProfiler::RecordTriangles(int count) {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    m_triangles += count;
}

void GPUProfiler::RecordVertices(int count) {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    m_vertices += count;
}

void GPUProfiler::RecordTextureMemory(size_t bytes) {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    m_textureMemory += bytes;
}

void GPUProfiler::RecordBufferMemory(size_t bytes) {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    m_bufferMemory += bytes;
}

void GPUProfiler::AddMetric(const std::string& name, double duration) {
    PerformanceMetric metric(name, ProfilerType::GPU, duration, "ms");
    m_metrics.push_back(metric);
}

unsigned int GPUProfiler::CreateQuery() {
    // В реальной реализации здесь бы создавались OpenGL queries
    static unsigned int nextId = 1;
    return nextId++;
}

void GPUProfiler::DeleteQuery(unsigned int queryId) {
    // В реальной реализации здесь бы удалялись OpenGL queries
}

// MemoryProfiler implementation
MemoryProfiler::MemoryProfiler() 
    : m_profiling(false)
    , m_totalMemory(0)
    , m_peakMemory(0) {
}

void MemoryProfiler::StartProfiling() {
    m_profiling = true;
    std::cout << "MemoryProfiler: Started profiling" << std::endl;
}

void MemoryProfiler::StopProfiling() {
    m_profiling = false;
    std::cout << "MemoryProfiler: Stopped profiling" << std::endl;
}

void MemoryProfiler::Reset() {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    m_categories.clear();
    m_metrics.clear();
    m_totalMemory = 0;
    m_peakMemory = 0;
    std::cout << "MemoryProfiler: Reset" << std::endl;
}

void MemoryProfiler::RecordAllocation(const std::string& category, size_t size) {
    if (!m_profiling) return;
    
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    auto& cat = m_categories[category];
    cat.current += size;
    cat.totalAllocated += size;
    cat.allocationCount++;
    cat.peak = std::max(cat.peak, cat.current);
    
    m_totalMemory += size;
    m_peakMemory = std::max(m_peakMemory, m_totalMemory);
    
    AddMetric(category + "_allocated", static_cast<double>(size), "bytes");
}

void MemoryProfiler::RecordDeallocation(const std::string& category, size_t size) {
    if (!m_profiling) return;
    
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    auto& cat = m_categories[category];
    cat.current -= size;
    cat.totalDeallocated += size;
    cat.deallocationCount++;
    
    m_totalMemory -= size;
    
    AddMetric(category + "_deallocated", static_cast<double>(size), "bytes");
}

void MemoryProfiler::RecordMemoryUsage(const std::string& category, size_t current, size_t peak) {
    if (!m_profiling) return;
    
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    auto& cat = m_categories[category];
    cat.current = current;
    cat.peak = std::max(cat.peak, peak);
    
    AddMetric(category + "_current", static_cast<double>(current), "bytes");
    AddMetric(category + "_peak", static_cast<double>(peak), "bytes");
}

PerformanceStats MemoryProfiler::GetStats(const std::string& category) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    PerformanceStats stats;
    
    auto it = m_categories.find(category);
    if (it != m_categories.end()) {
        stats.average = static_cast<double>(it->second.current);
        stats.max = static_cast<double>(it->second.peak);
        stats.sampleCount = it->second.allocationCount;
    }
    
    return stats;
}

std::vector<PerformanceMetric> MemoryProfiler::GetMetrics() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    return m_metrics;
}

size_t MemoryProfiler::GetTotalMemoryUsage() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    return m_totalMemory;
}

size_t MemoryProfiler::GetPeakMemoryUsage() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    return m_peakMemory;
}

void MemoryProfiler::DetectMemoryLeaks() {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    
    for (const auto& pair : m_categories) {
        const auto& category = pair.first;
        const auto& stats = pair.second;
        
        if (stats.current > 0 && stats.allocationCount > stats.deallocationCount) {
            std::cout << "MemoryProfiler: Potential memory leak detected in category: " 
                      << category << " (current: " << stats.current << " bytes)" << std::endl;
        }
    }
}

std::vector<std::string> MemoryProfiler::GetMemoryLeaks() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    std::vector<std::string> leaks;
    
    for (const auto& pair : m_categories) {
        const auto& category = pair.first;
        const auto& stats = pair.second;
        
        if (stats.current > 0 && stats.allocationCount > stats.deallocationCount) {
            leaks.push_back(category + ": " + std::to_string(stats.current) + " bytes");
        }
    }
    
    return leaks;
}

void MemoryProfiler::AddMetric(const std::string& category, double value, const std::string& unit) {
    PerformanceMetric metric(category, ProfilerType::Memory, value, unit);
    m_metrics.push_back(metric);
}

// DebugRenderer implementation
DebugRenderer::DebugRenderer() 
    : m_initialized(false)
    , m_fontSize(16.0f)
    , m_wireframeEnabled(false)
    , m_boundingBoxesEnabled(false)
    , m_collisionShapesEnabled(false) {
    m_textColor[0] = 1.0f; m_textColor[1] = 1.0f; m_textColor[2] = 1.0f; m_textColor[3] = 1.0f;
    m_lineColor[0] = 0.0f; m_lineColor[1] = 1.0f; m_lineColor[2] = 0.0f; m_lineColor[3] = 1.0f;
    m_backgroundColor[0] = 0.0f; m_backgroundColor[1] = 0.0f; m_backgroundColor[2] = 0.0f; m_backgroundColor[3] = 0.8f;
}

bool DebugRenderer::Initialize() {
    if (m_initialized) return true;
    
    // В реальной реализации здесь бы инициализировался OpenGL для отладочного рендеринга
    m_initialized = true;
    std::cout << "DebugRenderer: Initialized" << std::endl;
    return true;
}

void DebugRenderer::Shutdown() {
    if (!m_initialized) return;
    
    m_initialized = false;
    std::cout << "DebugRenderer: Shutdown" << std::endl;
}

void DebugRenderer::Render() {
    if (!m_initialized) return;
    
    // В реальной реализации здесь бы рендерилась отладочная информация
}

void DebugRenderer::RenderFPS(float fps) {
    std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
    RenderText(fpsText, 10.0f, 30.0f);
}

void DebugRenderer::RenderMemoryUsage(size_t current, size_t peak) {
    std::string memoryText = "Memory: " + std::to_string(current / 1024 / 1024) + "MB / " + 
                            std::to_string(peak / 1024 / 1024) + "MB";
    RenderText(memoryText, 10.0f, 50.0f);
}

void DebugRenderer::RenderProfilerData(const std::vector<PerformanceMetric>& metrics) {
    float y = 70.0f;
    for (const auto& metric : metrics) {
        std::string text = metric.name + ": " + std::to_string(metric.value) + " " + metric.unit;
        RenderText(text, 10.0f, y);
        y += 20.0f;
    }
}

void DebugRenderer::RenderWireframe(bool enabled) {
    m_wireframeEnabled = enabled;
}

void DebugRenderer::RenderBoundingBoxes(bool enabled) {
    m_boundingBoxesEnabled = enabled;
}

void DebugRenderer::RenderCollisionShapes(bool enabled) {
    m_collisionShapesEnabled = enabled;
}

void DebugRenderer::SetTextColor(float r, float g, float b, float a) {
    m_textColor[0] = r; m_textColor[1] = g; m_textColor[2] = b; m_textColor[3] = a;
}

void DebugRenderer::SetLineColor(float r, float g, float b, float a) {
    m_lineColor[0] = r; m_lineColor[1] = g; m_lineColor[2] = b; m_lineColor[3] = a;
}

void DebugRenderer::SetBackgroundColor(float r, float g, float b, float a) {
    m_backgroundColor[0] = r; m_backgroundColor[1] = g; m_backgroundColor[2] = b; m_backgroundColor[3] = a;
}

void DebugRenderer::RenderText(const std::string& text, float x, float y) {
    // В реальной реализации здесь бы рендерился текст с помощью OpenGL
}

void DebugRenderer::RenderLine(float x1, float y1, float x2, float y2) {
    // В реальной реализации здесь бы рендерилась линия с помощью OpenGL
}

void DebugRenderer::RenderBox(float x, float y, float width, float height) {
    // В реальной реализации здесь бы рендерился прямоугольник с помощью OpenGL
}

// PerformanceMonitor implementation
PerformanceMonitor::PerformanceMonitor() 
    : m_monitoringEnabled(false)
    , m_exportEnabled(false)
    , m_exportInterval(5.0f)
    , m_exportTimer(0.0f) {
}

bool PerformanceMonitor::Initialize() {
    if (!m_debugRenderer.Initialize()) {
        return false;
    }
    
    std::cout << "PerformanceMonitor: Initialized" << std::endl;
    return true;
}

void PerformanceMonitor::Shutdown() {
    m_debugRenderer.Shutdown();
    std::cout << "PerformanceMonitor: Shutdown" << std::endl;
}

void PerformanceMonitor::StartMonitoring() {
    m_monitoringEnabled = true;
    m_cpuProfiler.StartProfiling();
    m_gpuProfiler.StartProfiling();
    m_memoryProfiler.StartProfiling();
    std::cout << "PerformanceMonitor: Started monitoring" << std::endl;
}

void PerformanceMonitor::StopMonitoring() {
    m_monitoringEnabled = false;
    m_cpuProfiler.StopProfiling();
    m_gpuProfiler.StopProfiling();
    m_memoryProfiler.StopProfiling();
    std::cout << "PerformanceMonitor: Stopped monitoring" << std::endl;
}

void PerformanceMonitor::Reset() {
    m_cpuProfiler.Reset();
    m_gpuProfiler.Reset();
    m_memoryProfiler.Reset();
    std::cout << "PerformanceMonitor: Reset" << std::endl;
}

void PerformanceMonitor::Update(float deltaTime) {
    if (!m_monitoringEnabled) return;
    
    // Проверяем алерты производительности
    CheckPerformanceAlerts();
    
    // Экспортируем данные если нужно
    if (m_exportEnabled) {
        m_exportTimer += deltaTime;
        if (m_exportTimer >= m_exportInterval) {
            ExportData();
            m_exportTimer = 0.0f;
        }
    }
}

void PerformanceMonitor::ExportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "PerformanceMonitor: Failed to open file for CSV export: " << filename << std::endl;
        return;
    }
    
    file << "Name,Type,Value,Unit,Timestamp\n";
    
    auto cpuMetrics = m_cpuProfiler.GetMetrics();
    for (const auto& metric : cpuMetrics) {
        file << metric.name << ",CPU," << metric.value << "," << metric.unit << "," 
             << std::chrono::duration_cast<std::chrono::milliseconds>(metric.timestamp.time_since_epoch()).count() << "\n";
    }
    
    auto gpuMetrics = m_gpuProfiler.GetMetrics();
    for (const auto& metric : gpuMetrics) {
        file << metric.name << ",GPU," << metric.value << "," << metric.unit << "," 
             << std::chrono::duration_cast<std::chrono::milliseconds>(metric.timestamp.time_since_epoch()).count() << "\n";
    }
    
    auto memoryMetrics = m_memoryProfiler.GetMetrics();
    for (const auto& metric : memoryMetrics) {
        file << metric.name << ",Memory," << metric.value << "," << metric.unit << "," 
             << std::chrono::duration_cast<std::chrono::milliseconds>(metric.timestamp.time_since_epoch()).count() << "\n";
    }
    
    file.close();
    std::cout << "PerformanceMonitor: Exported data to CSV: " << filename << std::endl;
}

void PerformanceMonitor::ExportToJSON(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "PerformanceMonitor: Failed to open file for JSON export: " << filename << std::endl;
        return;
    }
    
    file << "{\n";
    file << "  \"performance_data\": {\n";
    file << "    \"cpu_metrics\": [\n";
    
    auto cpuMetrics = m_cpuProfiler.GetMetrics();
    for (size_t i = 0; i < cpuMetrics.size(); ++i) {
        const auto& metric = cpuMetrics[i];
        file << "      {\n";
        file << "        \"name\": \"" << metric.name << "\",\n";
        file << "        \"value\": " << metric.value << ",\n";
        file << "        \"unit\": \"" << metric.unit << "\",\n";
        file << "        \"timestamp\": " << std::chrono::duration_cast<std::chrono::milliseconds>(metric.timestamp.time_since_epoch()).count() << "\n";
        file << "      }";
        if (i < cpuMetrics.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "    ],\n";
    file << "    \"gpu_metrics\": [\n";
    
    auto gpuMetrics = m_gpuProfiler.GetMetrics();
    for (size_t i = 0; i < gpuMetrics.size(); ++i) {
        const auto& metric = gpuMetrics[i];
        file << "      {\n";
        file << "        \"name\": \"" << metric.name << "\",\n";
        file << "        \"value\": " << metric.value << ",\n";
        file << "        \"unit\": \"" << metric.unit << "\",\n";
        file << "        \"timestamp\": " << std::chrono::duration_cast<std::chrono::milliseconds>(metric.timestamp.time_since_epoch()).count() << "\n";
        file << "      }";
        if (i < gpuMetrics.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "    ],\n";
    file << "    \"memory_metrics\": [\n";
    
    auto memoryMetrics = m_memoryProfiler.GetMetrics();
    for (size_t i = 0; i < memoryMetrics.size(); ++i) {
        const auto& metric = memoryMetrics[i];
        file << "      {\n";
        file << "        \"name\": \"" << metric.name << "\",\n";
        file << "        \"value\": " << metric.value << ",\n";
        file << "        \"unit\": \"" << metric.unit << "\",\n";
        file << "        \"timestamp\": " << std::chrono::duration_cast<std::chrono::milliseconds>(metric.timestamp.time_since_epoch()).count() << "\n";
        file << "      }";
        if (i < memoryMetrics.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "    ]\n";
    file << "  }\n";
    file << "}\n";
    
    file.close();
    std::cout << "PerformanceMonitor: Exported data to JSON: " << filename << std::endl;
}

void PerformanceMonitor::ExportToHTML(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "PerformanceMonitor: Failed to open file for HTML export: " << filename << std::endl;
        return;
    }
    
    file << "<!DOCTYPE html>\n";
    file << "<html>\n";
    file << "<head>\n";
    file << "    <title>Performance Report</title>\n";
    file << "    <style>\n";
    file << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
    file << "        table { border-collapse: collapse; width: 100%; }\n";
    file << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    file << "        th { background-color: #f2f2f2; }\n";
    file << "    </style>\n";
    file << "</head>\n";
    file << "<body>\n";
    file << "    <h1>Performance Report</h1>\n";
    file << "    <h2>CPU Metrics</h2>\n";
    file << "    <table>\n";
    file << "        <tr><th>Name</th><th>Value</th><th>Unit</th><th>Timestamp</th></tr>\n";
    
    auto cpuMetrics = m_cpuProfiler.GetMetrics();
    for (const auto& metric : cpuMetrics) {
        file << "        <tr><td>" << metric.name << "</td><td>" << metric.value 
             << "</td><td>" << metric.unit << "</td><td>" 
             << std::chrono::duration_cast<std::chrono::milliseconds>(metric.timestamp.time_since_epoch()).count() 
             << "</td></tr>\n";
    }
    
    file << "    </table>\n";
    file << "</body>\n";
    file << "</html>\n";
    
    file.close();
    std::cout << "PerformanceMonitor: Exported data to HTML: " << filename << std::endl;
}

void PerformanceMonitor::CheckPerformanceAlerts() {
    if (!m_performanceAlertCallback) return;
    
    // Проверяем CPU метрики
    auto cpuMetrics = m_cpuProfiler.GetMetrics();
    for (const auto& metric : cpuMetrics) {
        if (metric.value > 16.67) { // 60 FPS threshold
            m_performanceAlertCallback(metric.name, metric.value);
        }
    }
    
    // Проверяем память
    size_t currentMemory = m_memoryProfiler.GetTotalMemoryUsage();
    size_t peakMemory = m_memoryProfiler.GetPeakMemoryUsage();
    
    if (currentMemory > 100 * 1024 * 1024) { // 100MB threshold
        m_performanceAlertCallback("Memory Usage", static_cast<double>(currentMemory));
    }
}

void PerformanceMonitor::ExportData() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    std::stringstream filename;
    filename << "performance_" << std::put_time(&tm, "%Y%m%d_%H%M%S");
    
    ExportToCSV(filename.str() + ".csv");
    ExportToJSON(filename.str() + ".json");
    ExportToHTML(filename.str() + ".html");
}

} // namespace FastEngine
