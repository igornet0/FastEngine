#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace FastEngine {

/**
 * Настройки качества для экспорта
 */
struct QualitySettings {
    // LOD настройки
    int maxLODLevels = 4;
    float lodDistance0 = 0.0f;
    float lodDistance1 = 50.0f;
    float lodDistance2 = 100.0f;
    float lodDistance3 = 200.0f;
    
    // Mipmaps
    bool generateMipmaps = true;
    int maxMipmapLevels = 8;
    
    // Тени
    bool enableShadows = true;
    int shadowMapSize = 1024;
    float shadowDistance = 100.0f;
    
    // Текстуры
    int maxTextureSize = 2048;
    bool compressTextures = true;
    std::string textureFormat = "DXT5"; // DXT1, DXT5, ETC2, ASTC
    
    // Аудио
    int audioSampleRate = 44100;
    int audioBitRate = 128;
    std::string audioFormat = "OGG"; // OGG, MP3, WAV
    
    // Общие настройки
    bool stripDebugInfo = true;
    bool optimizeForSize = true;
    bool enableProfiling = false;
};

/**
 * Настройки платформы
 */
struct PlatformSettings {
    std::string platformName;
    std::string outputDirectory;
    std::string executableName;
    std::vector<std::string> requiredLibraries;
    std::map<std::string, std::string> platformSpecificSettings;
    
    // Настройки сборки
    std::string buildConfiguration; // Debug, Release, Shipping
    std::vector<std::string> compilerFlags;
    std::vector<std::string> linkerFlags;
    
    // Настройки ресурсов
    bool bundleResources = true;
    std::string resourceCompression = "LZ4"; // None, LZ4, Zlib
    
    PlatformSettings() : platformName("") {}
    PlatformSettings(const std::string& name) : platformName(name) {}
};

/**
 * Результат экспорта
 */
struct ExportResult {
    bool success = false;
    std::string outputPath;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    std::map<std::string, std::string> generatedFiles;
    size_t totalSize = 0;
    std::chrono::milliseconds buildTime;
    
    void AddWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    void AddError(const std::string& error) {
        errors.push_back(error);
    }
};

/**
 * Система экспорта проектов
 */
class ProjectExporter {
public:
    ProjectExporter();
    ~ProjectExporter() = default;
    
    // Инициализация
    bool Initialize();
    void Shutdown();
    
    // Настройки качества
    void SetQualitySettings(const QualitySettings& settings) { m_qualitySettings = settings; }
    const QualitySettings& GetQualitySettings() const { return m_qualitySettings; }
    
    // Настройки платформы
    void SetPlatformSettings(const std::string& platform, const PlatformSettings& settings);
    PlatformSettings* GetPlatformSettings(const std::string& platform);
    
    // Экспорт проекта
    ExportResult ExportProject(const std::string& projectPath, 
                              const std::string& platform,
                              const std::string& outputPath);
    
    // Поддерживаемые платформы
    std::vector<std::string> GetSupportedPlatforms() const;
    bool IsPlatformSupported(const std::string& platform) const;
    
    // Callbacks для прогресса
    void SetProgressCallback(std::function<void(float, const std::string&)> callback) {
        m_progressCallback = callback;
    }
    
    // Валидация проекта
    bool ValidateProject(const std::string& projectPath);
    
private:
    QualitySettings m_qualitySettings;
    std::map<std::string, PlatformSettings> m_platformSettings;
    std::function<void(float, const std::string&)> m_progressCallback;
    
    // Внутренние методы
    void InitializeDefaultPlatformSettings();
    ExportResult ExportForWindows(const std::string& projectPath, const std::string& outputPath);
    ExportResult ExportForLinux(const std::string& projectPath, const std::string& outputPath);
    ExportResult ExportForMacOS(const std::string& projectPath, const std::string& outputPath);
    ExportResult ExportForAndroid(const std::string& projectPath, const std::string& outputPath);
    ExportResult ExportForiOS(const std::string& projectPath, const std::string& outputPath);
    ExportResult ExportForWeb(const std::string& projectPath, const std::string& outputPath);
    
    // Обработка ресурсов
    bool ProcessResources(const std::string& projectPath, const std::string& outputPath);
    bool OptimizeTextures(const std::string& inputDir, const std::string& outputDir);
    bool OptimizeAudio(const std::string& inputDir, const std::string& outputDir);
    bool GenerateLODs(const std::string& inputDir, const std::string& outputDir);
    
    // Генерация файлов проекта
    bool GenerateCMakeLists(const std::string& outputPath, const PlatformSettings& settings);
    bool GenerateProjectFiles(const std::string& outputPath, const PlatformSettings& settings);
    bool GenerateManifest(const std::string& outputPath, const PlatformSettings& settings);
    
    // Утилиты
    void ReportProgress(float progress, const std::string& message);
    std::string GetPlatformExecutableExtension(const std::string& platform) const;
    std::string GetPlatformLibraryExtension(const std::string& platform) const;
};

/**
 * Менеджер экспорта
 */
class ExportManager {
public:
    static ExportManager& GetInstance();
    
    // Управление экспортерами
    void RegisterExporter(const std::string& platform, std::unique_ptr<ProjectExporter> exporter);
    ProjectExporter* GetExporter(const std::string& platform);
    
    // Батч экспорт
    std::vector<ExportResult> ExportToAllPlatforms(const std::string& projectPath, 
                                                   const std::string& outputBasePath);
    
    // Шаблоны проектов
    bool CreateProjectTemplate(const std::string& templateName, 
                              const std::string& outputPath,
                              const std::map<std::string, std::string>& parameters);
    
private:
    ExportManager() = default;
    std::map<std::string, std::unique_ptr<ProjectExporter>> m_exporters;
};

} // namespace FastEngine
