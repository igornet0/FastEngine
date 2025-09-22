#include "FastEngine/Export/ProjectExporter.h"
#include "FastEngine/Platform/FileSystem.h"
#include "FastEngine/Platform/Platform.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <filesystem>
#include <algorithm>

namespace FastEngine {

// ProjectExporter implementation
ProjectExporter::ProjectExporter() {
    // Инициализируем настройки качества по умолчанию
    m_qualitySettings = QualitySettings();
    
    // Инициализируем настройки платформ по умолчанию
    InitializeDefaultPlatformSettings();
}

bool ProjectExporter::Initialize() {
    std::cout << "ProjectExporter: Initializing..." << std::endl;
    
    // Проверяем доступность необходимых инструментов
    // В реальной реализации здесь бы проверялись компиляторы, SDK и т.д.
    
    return true;
}

void ProjectExporter::Shutdown() {
    std::cout << "ProjectExporter: Shutting down..." << std::endl;
}

void ProjectExporter::InitializeDefaultPlatformSettings() {
    // Windows
    PlatformSettings windows("Windows");
    windows.outputDirectory = "windows";
    windows.executableName = "Game.exe";
    windows.buildConfiguration = "Release";
    windows.requiredLibraries = {"opengl32", "user32", "kernel32"};
    windows.platformSpecificSettings["arch"] = "x64";
    windows.platformSpecificSettings["runtime"] = "static";
    m_platformSettings["Windows"] = windows;
    
    // Linux
    PlatformSettings linux("Linux");
    linux.outputDirectory = "linux";
    linux.executableName = "Game";
    linux.buildConfiguration = "Release";
    linux.requiredLibraries = {"GL", "X11", "pthread"};
    linux.platformSpecificSettings["arch"] = "x64";
    linux.platformSpecificSettings["runtime"] = "static";
    m_platformSettings["Linux"] = linux;
    
    // macOS
    PlatformSettings macos("macOS");
    macos.outputDirectory = "macos";
    macos.executableName = "Game.app";
    macos.buildConfiguration = "Release";
    macos.requiredLibraries = {"OpenGL.framework", "Cocoa.framework"};
    macos.platformSpecificSettings["arch"] = "universal";
    macos.platformSpecificSettings["runtime"] = "static";
    m_platformSettings["macOS"] = macos;
    
    // Android
    PlatformSettings android("Android");
    android.outputDirectory = "android";
    android.executableName = "libgame.so";
    android.buildConfiguration = "Release";
    android.requiredLibraries = {"GLESv3", "log", "android"};
    android.platformSpecificSettings["minSdk"] = "21";
    android.platformSpecificSettings["targetSdk"] = "33";
    android.platformSpecificSettings["arch"] = "arm64-v8a";
    m_platformSettings["Android"] = android;
    
    // iOS
    PlatformSettings ios("iOS");
    ios.outputDirectory = "ios";
    ios.executableName = "Game.app";
    ios.buildConfiguration = "Release";
    ios.requiredLibraries = {"OpenGLES.framework", "UIKit.framework"};
    ios.platformSpecificSettings["minVersion"] = "12.0";
    ios.platformSpecificSettings["arch"] = "arm64";
    m_platformSettings["iOS"] = ios;
    
    // Web
    PlatformSettings web("Web");
    web.outputDirectory = "web";
    web.executableName = "index.html";
    web.buildConfiguration = "Release";
    web.requiredLibraries = {"emscripten"};
    web.platformSpecificSettings["target"] = "wasm32";
    web.platformSpecificSettings["optimization"] = "O3";
    m_platformSettings["Web"] = web;
}

void ProjectExporter::SetPlatformSettings(const std::string& platform, const PlatformSettings& settings) {
    m_platformSettings[platform] = settings;
}

PlatformSettings* ProjectExporter::GetPlatformSettings(const std::string& platform) {
    auto it = m_platformSettings.find(platform);
    return (it != m_platformSettings.end()) ? &it->second : nullptr;
}

ExportResult ProjectExporter::ExportProject(const std::string& projectPath, 
                                           const std::string& platform,
                                           const std::string& outputPath) {
    auto startTime = std::chrono::high_resolution_clock::now();
    ExportResult result;
    
    std::cout << "ProjectExporter: Starting export for platform: " << platform << std::endl;
    ReportProgress(0.0f, "Starting export...");
    
    // Валидация проекта
    if (!ValidateProject(projectPath)) {
        result.AddError("Project validation failed");
        return result;
    }
    
    // Проверка поддержки платформы
    if (!IsPlatformSupported(platform)) {
        result.AddError("Platform not supported: " + platform);
        return result;
    }
    
    // Создание выходной директории
    std::filesystem::create_directories(outputPath);
    
    ReportProgress(0.1f, "Processing resources...");
    
    // Обработка ресурсов
    if (!ProcessResources(projectPath, outputPath)) {
        result.AddError("Failed to process resources");
        return result;
    }
    
    ReportProgress(0.3f, "Generating platform files...");
    
    // Экспорт для конкретной платформы
    if (platform == "Windows") {
        result = ExportForWindows(projectPath, outputPath);
    } else if (platform == "Linux") {
        result = ExportForLinux(projectPath, outputPath);
    } else if (platform == "macOS") {
        result = ExportForMacOS(projectPath, outputPath);
    } else if (platform == "Android") {
        result = ExportForAndroid(projectPath, outputPath);
    } else if (platform == "iOS") {
        result = ExportForiOS(projectPath, outputPath);
    } else if (platform == "Web") {
        result = ExportForWeb(projectPath, outputPath);
    } else {
        result.AddError("Unknown platform: " + platform);
        return result;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.buildTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (result.success) {
        ReportProgress(1.0f, "Export completed successfully!");
        std::cout << "ProjectExporter: Export completed in " << result.buildTime.count() << "ms" << std::endl;
    } else {
        ReportProgress(0.0f, "Export failed!");
        std::cout << "ProjectExporter: Export failed with " << result.errors.size() << " errors" << std::endl;
    }
    
    return result;
}

std::vector<std::string> ProjectExporter::GetSupportedPlatforms() const {
    std::vector<std::string> platforms;
    for (const auto& pair : m_platformSettings) {
        platforms.push_back(pair.first);
    }
    return platforms;
}

bool ProjectExporter::IsPlatformSupported(const std::string& platform) const {
    return m_platformSettings.find(platform) != m_platformSettings.end();
}

bool ProjectExporter::ValidateProject(const std::string& projectPath) {
    std::cout << "ProjectExporter: Validating project at: " << projectPath << std::endl;
    
    // Проверяем существование основных файлов проекта
    std::vector<std::string> requiredFiles = {
        "project.json",
        "src/",
        "assets/"
    };
    
    for (const auto& file : requiredFiles) {
        std::string fullPath = projectPath + "/" + file;
        if (!std::filesystem::exists(fullPath)) {
            std::cout << "ProjectExporter: Missing required file: " << file << std::endl;
            return false;
        }
    }
    
    return true;
}

ExportResult ProjectExporter::ExportForWindows(const std::string& projectPath, const std::string& outputPath) {
    ExportResult result;
    std::cout << "ProjectExporter: Exporting for Windows..." << std::endl;
    
    auto* settings = GetPlatformSettings("Windows");
    if (!settings) {
        result.AddError("Windows platform settings not found");
        return result;
    }
    
    // Генерация CMakeLists.txt
    if (!GenerateCMakeLists(outputPath, *settings)) {
        result.AddError("Failed to generate CMakeLists.txt");
        return result;
    }
    
    // Генерация файлов проекта
    if (!GenerateProjectFiles(outputPath, *settings)) {
        result.AddError("Failed to generate project files");
        return result;
    }
    
    result.success = true;
    result.outputPath = outputPath;
    result.generatedFiles["CMakeLists.txt"] = outputPath + "/CMakeLists.txt";
    result.generatedFiles["project.vcxproj"] = outputPath + "/project.vcxproj";
    
    return result;
}

ExportResult ProjectExporter::ExportForLinux(const std::string& projectPath, const std::string& outputPath) {
    ExportResult result;
    std::cout << "ProjectExporter: Exporting for Linux..." << std::endl;
    
    auto* settings = GetPlatformSettings("Linux");
    if (!settings) {
        result.AddError("Linux platform settings not found");
        return result;
    }
    
    // Генерация CMakeLists.txt
    if (!GenerateCMakeLists(outputPath, *settings)) {
        result.AddError("Failed to generate CMakeLists.txt");
        return result;
    }
    
    // Генерация Makefile
    std::string makefilePath = outputPath + "/Makefile";
    std::ofstream makefile(makefilePath);
    if (makefile.is_open()) {
        makefile << "CC = g++\n";
        makefile << "CFLAGS = -std=c++17 -O3\n";
        makefile << "LIBS = -lGL -lX11 -lpthread\n";
        makefile << "TARGET = " << settings->executableName << "\n";
        makefile << "SOURCES = $(wildcard src/*.cpp)\n";
        makefile << "OBJECTS = $(SOURCES:.cpp=.o)\n\n";
        makefile << "$(TARGET): $(OBJECTS)\n";
        makefile << "\t$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)\n\n";
        makefile << "%.o: %.cpp\n";
        makefile << "\t$(CC) $(CFLAGS) -c $< -o $@\n\n";
        makefile << "clean:\n";
        makefile << "\trm -f $(OBJECTS) $(TARGET)\n";
        makefile.close();
        
        result.generatedFiles["Makefile"] = makefilePath;
    }
    
    result.success = true;
    result.outputPath = outputPath;
    
    return result;
}

ExportResult ProjectExporter::ExportForMacOS(const std::string& projectPath, const std::string& outputPath) {
    ExportResult result;
    std::cout << "ProjectExporter: Exporting for macOS..." << std::endl;
    
    auto* settings = GetPlatformSettings("macOS");
    if (!settings) {
        result.AddError("macOS platform settings not found");
        return result;
    }
    
    // Генерация CMakeLists.txt
    if (!GenerateCMakeLists(outputPath, *settings)) {
        result.AddError("Failed to generate CMakeLists.txt");
        return result;
    }
    
    // Генерация Xcode проекта
    std::string xcodePath = outputPath + "/project.xcodeproj";
    std::filesystem::create_directories(xcodePath);
    
    result.success = true;
    result.outputPath = outputPath;
    result.generatedFiles["CMakeLists.txt"] = outputPath + "/CMakeLists.txt";
    result.generatedFiles["project.xcodeproj"] = xcodePath;
    
    return result;
}

ExportResult ProjectExporter::ExportForAndroid(const std::string& projectPath, const std::string& outputPath) {
    ExportResult result;
    std::cout << "ProjectExporter: Exporting for Android..." << std::endl;
    
    auto* settings = GetPlatformSettings("Android");
    if (!settings) {
        result.AddError("Android platform settings not found");
        return result;
    }
    
    // Генерация AndroidManifest.xml
    if (!GenerateManifest(outputPath, *settings)) {
        result.AddError("Failed to generate AndroidManifest.xml");
        return result;
    }
    
    // Генерация CMakeLists.txt для Android
    if (!GenerateCMakeLists(outputPath, *settings)) {
        result.AddError("Failed to generate CMakeLists.txt");
        return result;
    }
    
    result.success = true;
    result.outputPath = outputPath;
    result.generatedFiles["AndroidManifest.xml"] = outputPath + "/AndroidManifest.xml";
    result.generatedFiles["CMakeLists.txt"] = outputPath + "/CMakeLists.txt";
    
    return result;
}

ExportResult ProjectExporter::ExportForiOS(const std::string& projectPath, const std::string& outputPath) {
    ExportResult result;
    std::cout << "ProjectExporter: Exporting for iOS..." << std::endl;
    
    auto* settings = GetPlatformSettings("iOS");
    if (!settings) {
        result.AddError("iOS platform settings not found");
        return result;
    }
    
    // Генерация Info.plist
    std::string plistPath = outputPath + "/Info.plist";
    std::ofstream plist(plistPath);
    if (plist.is_open()) {
        plist << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        plist << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
        plist << "<plist version=\"1.0\">\n";
        plist << "<dict>\n";
        plist << "    <key>CFBundleDisplayName</key>\n";
        plist << "    <string>Game</string>\n";
        plist << "    <key>CFBundleIdentifier</key>\n";
        plist << "    <string>com.fastengine.game</string>\n";
        plist << "    <key>CFBundleVersion</key>\n";
        plist << "    <string>1.0</string>\n";
        plist << "    <key>CFBundleShortVersionString</key>\n";
        plist << "    <string>1.0</string>\n";
        plist << "    <key>UIRequiredDeviceCapabilities</key>\n";
        plist << "    <array>\n";
        plist << "        <string>opengles-2</string>\n";
        plist << "    </array>\n";
        plist << "</dict>\n";
        plist << "</plist>\n";
        plist.close();
        
        result.generatedFiles["Info.plist"] = plistPath;
    }
    
    result.success = true;
    result.outputPath = outputPath;
    
    return result;
}

ExportResult ProjectExporter::ExportForWeb(const std::string& projectPath, const std::string& outputPath) {
    ExportResult result;
    std::cout << "ProjectExporter: Exporting for Web..." << std::endl;
    
    auto* settings = GetPlatformSettings("Web");
    if (!settings) {
        result.AddError("Web platform settings not found");
        return result;
    }
    
    // Генерация HTML файла
    std::string htmlPath = outputPath + "/index.html";
    std::ofstream html(htmlPath);
    if (html.is_open()) {
        html << "<!DOCTYPE html>\n";
        html << "<html>\n";
        html << "<head>\n";
        html << "    <title>FastEngine Game</title>\n";
        html << "    <meta charset=\"utf-8\">\n";
        html << "    <style>\n";
        html << "        body { margin: 0; padding: 0; background: #000; }\n";
        html << "        canvas { display: block; margin: 0 auto; }\n";
        html << "    </style>\n";
        html << "</head>\n";
        html << "<body>\n";
        html << "    <canvas id=\"canvas\"></canvas>\n";
        html << "    <script src=\"game.js\"></script>\n";
        html << "</body>\n";
        html << "</html>\n";
        html.close();
        
        result.generatedFiles["index.html"] = htmlPath;
    }
    
    result.success = true;
    result.outputPath = outputPath;
    
    return result;
}

bool ProjectExporter::ProcessResources(const std::string& projectPath, const std::string& outputPath) {
    std::cout << "ProjectExporter: Processing resources..." << std::endl;
    
    std::string assetsPath = projectPath + "/assets";
    std::string outputAssetsPath = outputPath + "/assets";
    
    if (!std::filesystem::exists(assetsPath)) {
        std::cout << "ProjectExporter: Assets directory not found" << std::endl;
        return false;
    }
    
    // Создаем выходную директорию для ресурсов
    std::filesystem::create_directories(outputAssetsPath);
    
    // Копируем ресурсы
    try {
        std::filesystem::copy(assetsPath, outputAssetsPath, std::filesystem::copy_options::recursive);
    } catch (const std::exception& e) {
        std::cout << "ProjectExporter: Failed to copy assets: " << e.what() << std::endl;
        return false;
    }
    
    // Оптимизируем текстуры
    if (!OptimizeTextures(outputAssetsPath, outputAssetsPath)) {
        std::cout << "ProjectExporter: Warning: Failed to optimize textures" << std::endl;
    }
    
    // Оптимизируем аудио
    if (!OptimizeAudio(outputAssetsPath, outputAssetsPath)) {
        std::cout << "ProjectExporter: Warning: Failed to optimize audio" << std::endl;
    }
    
    // Генерируем LOD'ы
    if (!GenerateLODs(outputAssetsPath, outputAssetsPath)) {
        std::cout << "ProjectExporter: Warning: Failed to generate LODs" << std::endl;
    }
    
    return true;
}

bool ProjectExporter::OptimizeTextures(const std::string& inputDir, const std::string& outputDir) {
    std::cout << "ProjectExporter: Optimizing textures..." << std::endl;
    
    // В реальной реализации здесь бы использовались библиотеки типа stb_image, 
    // FreeImage или ImageMagick для оптимизации текстур
    
    return true;
}

bool ProjectExporter::OptimizeAudio(const std::string& inputDir, const std::string& outputDir) {
    std::cout << "ProjectExporter: Optimizing audio..." << std::endl;
    
    // В реальной реализации здесь бы использовались библиотеки типа libsndfile
    // или FFmpeg для оптимизации аудио файлов
    
    return true;
}

bool ProjectExporter::GenerateLODs(const std::string& inputDir, const std::string& outputDir) {
    std::cout << "ProjectExporter: Generating LODs..." << std::endl;
    
    // В реальной реализации здесь бы генерировались LOD'ы для 3D моделей
    // с использованием библиотек типа Assimp или Open3D
    
    return true;
}

bool ProjectExporter::GenerateCMakeLists(const std::string& outputPath, const PlatformSettings& settings) {
    std::string cmakePath = outputPath + "/CMakeLists.txt";
    std::ofstream cmake(cmakePath);
    
    if (!cmake.is_open()) {
        return false;
    }
    
    cmake << "cmake_minimum_required(VERSION 3.16)\n";
    cmake << "project(FastEngineGame)\n\n";
    cmake << "set(CMAKE_CXX_STANDARD 17)\n";
    cmake << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n\n";
    
    if (settings.platformName == "Windows") {
        cmake << "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} /W3\")\n";
    } else {
        cmake << "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -Wall -Wextra\")\n";
    }
    
    cmake << "\n# Find required packages\n";
    cmake << "find_package(OpenGL REQUIRED)\n\n";
    
    cmake << "# Add executable\n";
    cmake << "add_executable(${PROJECT_NAME} src/main.cpp)\n\n";
    
    cmake << "# Link libraries\n";
    cmake << "target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})\n";
    
    for (const auto& lib : settings.requiredLibraries) {
        cmake << "target_link_libraries(${PROJECT_NAME} " << lib << ")\n";
    }
    
    cmake << "\n# Include directories\n";
    cmake << "target_include_directories(${PROJECT_NAME} PRIVATE include)\n\n";
    
    cmake << "# Copy assets\n";
    cmake << "file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})\n";
    
    cmake.close();
    return true;
}

bool ProjectExporter::GenerateProjectFiles(const std::string& outputPath, const PlatformSettings& settings) {
    // В реальной реализации здесь бы генерировались файлы проектов для различных IDE
    return true;
}

bool ProjectExporter::GenerateManifest(const std::string& outputPath, const PlatformSettings& settings) {
    std::string manifestPath = outputPath + "/AndroidManifest.xml";
    std::ofstream manifest(manifestPath);
    
    if (!manifest.is_open()) {
        return false;
    }
    
    manifest << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    manifest << "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"\n";
    manifest << "    package=\"com.fastengine.game\">\n\n";
    manifest << "    <uses-feature android:glEsVersion=\"0x00020000\" android:required=\"true\" />\n\n";
    manifest << "    <application\n";
    manifest << "        android:allowBackup=\"true\"\n";
    manifest << "        android:icon=\"@mipmap/ic_launcher\"\n";
    manifest << "        android:label=\"@string/app_name\">\n";
    manifest << "        <activity\n";
    manifest << "            android:name=\".MainActivity\"\n";
    manifest << "            android:exported=\"true\"\n";
    manifest << "            android:screenOrientation=\"landscape\">\n";
    manifest << "            <intent-filter>\n";
    manifest << "                <action android:name=\"android.intent.action.MAIN\" />\n";
    manifest << "                <category android:name=\"android.intent.category.LAUNCHER\" />\n";
    manifest << "            </intent-filter>\n";
    manifest << "        </activity>\n";
    manifest << "    </application>\n";
    manifest << "</manifest>\n";
    
    manifest.close();
    return true;
}

void ProjectExporter::ReportProgress(float progress, const std::string& message) {
    if (m_progressCallback) {
        m_progressCallback(progress, message);
    }
    std::cout << "ProjectExporter: [" << (int)(progress * 100) << "%] " << message << std::endl;
}

std::string ProjectExporter::GetPlatformExecutableExtension(const std::string& platform) const {
    if (platform == "Windows") return ".exe";
    if (platform == "macOS") return ".app";
    if (platform == "iOS") return ".app";
    if (platform == "Android") return ".apk";
    if (platform == "Web") return ".html";
    return "";
}

std::string ProjectExporter::GetPlatformLibraryExtension(const std::string& platform) const {
    if (platform == "Windows") return ".dll";
    if (platform == "Linux") return ".so";
    if (platform == "macOS") return ".dylib";
    if (platform == "iOS") return ".framework";
    if (platform == "Android") return ".so";
    if (platform == "Web") return ".wasm";
    return "";
}

// ExportManager implementation
ExportManager& ExportManager::GetInstance() {
    static ExportManager instance;
    return instance;
}

void ExportManager::RegisterExporter(const std::string& platform, std::unique_ptr<ProjectExporter> exporter) {
    m_exporters[platform] = std::move(exporter);
}

ProjectExporter* ExportManager::GetExporter(const std::string& platform) {
    auto it = m_exporters.find(platform);
    return (it != m_exporters.end()) ? it->second.get() : nullptr;
}

std::vector<ExportResult> ExportManager::ExportToAllPlatforms(const std::string& projectPath, 
                                                             const std::string& outputBasePath) {
    std::vector<ExportResult> results;
    
    for (const auto& pair : m_exporters) {
        const std::string& platform = pair.first;
        ProjectExporter* exporter = pair.second.get();
        
        std::string platformOutputPath = outputBasePath + "/" + platform;
        ExportResult result = exporter->ExportProject(projectPath, platform, platformOutputPath);
        results.push_back(result);
    }
    
    return results;
}

bool ExportManager::CreateProjectTemplate(const std::string& templateName, 
                                         const std::string& outputPath,
                                         const std::map<std::string, std::string>& parameters) {
    std::cout << "ExportManager: Creating project template: " << templateName << std::endl;
    
    // В реальной реализации здесь бы создавались шаблоны проектов
    // с базовой структурой файлов и конфигурацией
    
    return true;
}

} // namespace FastEngine

