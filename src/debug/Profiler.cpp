#include "FastEngine/Debug/Profiler.h"
#include "FastEngine/Render/Renderer.h"
#include "FastEngine/Components/Text.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <sstream>

namespace FastEngine {
    Profiler& Profiler::GetInstance() {
        static Profiler instance;
        return instance;
    }
    
    bool Profiler::Initialize() {
        if (m_initialized) {
            return true;
        }
        
        m_enabled = true;
        m_initialized = true;
        m_displayMode = 0; // Simple mode
        m_maxHistory = 1000;
        m_minTime = 0.001; // 1ms minimum
        m_totalFrameTime = 0.0;
        m_frameCount = 0;
        
        return true;
    }
    
    void Profiler::Shutdown() {
        m_profiles.clear();
        m_activeProfiles.clear();
        m_frameTimes.clear();
        m_initialized = false;
    }
    
    void Profiler::StartProfile(const std::string& name) {
        if (!m_enabled || !m_initialized) {
            return;
        }
        
        m_activeProfiles[name] = std::chrono::high_resolution_clock::now();
    }
    
    void Profiler::EndProfile(const std::string& name) {
        if (!m_enabled || !m_initialized) {
            return;
        }
        
        auto it = m_activeProfiles.find(name);
        if (it == m_activeProfiles.end()) {
            return;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - it->second);
        double timeMs = duration.count() / 1000.0;
        
        UpdateProfileData(name, timeMs);
        m_activeProfiles.erase(it);
    }
    
    const ProfileData* Profiler::GetProfileData(const std::string& name) const {
        auto it = m_profiles.find(name);
        return it != m_profiles.end() ? &it->second : nullptr;
    }
    
    double Profiler::GetTotalTime() const {
        double total = 0.0;
        for (const auto& pair : m_profiles) {
            total += pair.second.totalTime;
        }
        return total;
    }
    
    double Profiler::GetAverageFPS() const {
        if (m_frameTimes.empty()) {
            return 0.0;
        }
        
        double totalTime = 0.0;
        for (double time : m_frameTimes) {
            totalTime += time;
        }
        
        return m_frameTimes.size() / (totalTime / 1000.0); // FPS
    }
    
    int Profiler::GetFrameCount() const {
        return m_frameCount;
    }
    
    void Profiler::Update(float deltaTime) {
        if (!m_enabled || !m_initialized) {
            return;
        }
        
        // Обновляем статистику кадров
        double frameTime = deltaTime * 1000.0; // Convert to milliseconds
        m_frameTimes.push_back(frameTime);
        m_totalFrameTime += frameTime;
        m_frameCount++;
        
        // Ограничиваем историю
        if (static_cast<int>(m_frameTimes.size()) > m_maxHistory) {
            m_frameTimes.erase(m_frameTimes.begin());
        }
    }
    
    void Profiler::Render(Renderer* renderer) {
        if (!m_enabled || !m_initialized || !renderer) {
            return;
        }
        
        switch (m_displayMode) {
            case 0:
                RenderSimple(renderer);
                break;
            case 1:
                RenderDetailed(renderer);
                break;
            case 2:
                RenderGraph(renderer);
                break;
        }
    }
    
    void Profiler::Clear() {
        m_profiles.clear();
        m_activeProfiles.clear();
        m_frameTimes.clear();
        m_totalFrameTime = 0.0;
        m_frameCount = 0;
    }
    
    void Profiler::ResetProfile(const std::string& name) {
        auto it = m_profiles.find(name);
        if (it != m_profiles.end()) {
            m_profiles.erase(it);
        }
    }
    
    void Profiler::ExportToCSV(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }
        
        file << "Name,Total Time,Average Time,Min Time,Max Time,Call Count,Last Time\n";
        
        for (const auto& pair : m_profiles) {
            const ProfileData& data = pair.second;
            file << data.name << ","
                 << data.totalTime << ","
                 << data.averageTime << ","
                 << data.minTime << ","
                 << data.maxTime << ","
                 << data.callCount << ","
                 << data.lastTime << "\n";
        }
        
        file.close();
    }
    
    void Profiler::ExportToJSON(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }
        
        file << "{\n";
        file << "  \"profiles\": [\n";
        
        bool first = true;
        for (const auto& pair : m_profiles) {
            if (!first) {
                file << ",\n";
            }
            first = false;
            
            const ProfileData& data = pair.second;
            file << "    {\n";
            file << "      \"name\": \"" << data.name << "\",\n";
            file << "      \"totalTime\": " << data.totalTime << ",\n";
            file << "      \"averageTime\": " << data.averageTime << ",\n";
            file << "      \"minTime\": " << data.minTime << ",\n";
            file << "      \"maxTime\": " << data.maxTime << ",\n";
            file << "      \"callCount\": " << data.callCount << ",\n";
            file << "      \"lastTime\": " << data.lastTime << "\n";
            file << "    }";
        }
        
        file << "\n  ],\n";
        file << "  \"frameCount\": " << m_frameCount << ",\n";
        file << "  \"averageFPS\": " << GetAverageFPS() << "\n";
        file << "}\n";
        
        file.close();
    }
    
    void Profiler::OnFrameStart() {
        StartProfile("Frame");
    }
    
    void Profiler::OnFrameEnd() {
        EndProfile("Frame");
    }
    
    void Profiler::UpdateProfileData(const std::string& name, double time) {
        if (time < m_minTime) {
            return;
        }
        
        ProfileData& data = m_profiles[name];
        data.name = name;
        data.totalTime += time;
        data.callCount++;
        data.averageTime = data.totalTime / data.callCount;
        data.lastTime = time;
        
        if (data.callCount == 1) {
            data.minTime = time;
            data.maxTime = time;
        } else {
            data.minTime = std::min(data.minTime, time);
            data.maxTime = std::max(data.maxTime, time);
        }
    }
    
    void Profiler::RenderSimple(Renderer* renderer) {
        // В реальной реализации здесь бы рендерилась простая статистика
        // Для простоты выводим в консоль
        std::cout << "=== Profiler (Simple) ===" << std::endl;
        std::cout << "FPS: " << std::fixed << std::setprecision(1) << GetAverageFPS() << std::endl;
        std::cout << "Frame Count: " << m_frameCount << std::endl;
        
        for (const auto& pair : m_profiles) {
            const ProfileData& data = pair.second;
            std::cout << data.name << ": " << std::fixed << std::setprecision(2) 
                      << data.lastTime << "ms (avg: " << data.averageTime << "ms)" << std::endl;
        }
        std::cout << "========================" << std::endl;
    }
    
    void Profiler::RenderDetailed(Renderer* renderer) {
        // В реальной реализации здесь бы рендерилась детальная статистика
        std::cout << "=== Profiler (Detailed) ===" << std::endl;
        std::cout << "FPS: " << std::fixed << std::setprecision(1) << GetAverageFPS() << std::endl;
        std::cout << "Frame Count: " << m_frameCount << std::endl;
        std::cout << "Total Time: " << std::fixed << std::setprecision(2) << GetTotalTime() << "ms" << std::endl;
        
        for (const auto& pair : m_profiles) {
            const ProfileData& data = pair.second;
            std::cout << data.name << ":" << std::endl;
            std::cout << "  Last: " << std::fixed << std::setprecision(2) << data.lastTime << "ms" << std::endl;
            std::cout << "  Avg:  " << std::fixed << std::setprecision(2) << data.averageTime << "ms" << std::endl;
            std::cout << "  Min:  " << std::fixed << std::setprecision(2) << data.minTime << "ms" << std::endl;
            std::cout << "  Max:  " << std::fixed << std::setprecision(2) << data.maxTime << "ms" << std::endl;
            std::cout << "  Calls: " << data.callCount << std::endl;
        }
        std::cout << "=========================" << std::endl;
    }
    
    void Profiler::RenderGraph(Renderer* renderer) {
        // В реальной реализации здесь бы рендерился график производительности
        std::cout << "=== Profiler (Graph) ===" << std::endl;
        std::cout << "Graph rendering not implemented yet" << std::endl;
        std::cout << "=======================" << std::endl;
    }
    
    std::string Profiler::FormatTime(double time) const {
        std::stringstream ss;
        if (time < 1.0) {
            ss << std::fixed << std::setprecision(2) << time << "ms";
        } else {
            ss << std::fixed << std::setprecision(2) << time / 1000.0 << "s";
        }
        return ss.str();
    }
    
    glm::vec4 Profiler::GetColorForTime(double time) const {
        if (time < 1.0) {
            return glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
        } else if (time < 5.0) {
            return glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
        } else {
            return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
        }
    }
}
