#include "ProjectSimulator/AssetBrowser.h"
#include "FastEngine/Resources/ResourceManager.h"
#include "FastEngine/Platform/FileSystem.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace ProjectSimulator {

AssetBrowser::AssetBrowser() 
    : m_resourceManager(nullptr)
    , m_assetsPath("assets")
    , m_currentFolder("assets")
    , m_initialized(false)
    , m_visible(true) {
}

AssetBrowser::~AssetBrowser() {
    if (m_initialized) {
        // Очистка ресурсов
    }
}

bool AssetBrowser::Initialize(FastEngine::ResourceManager* resourceManager, const std::string& assetsPath) {
    if (m_initialized) {
        return true;
    }
    
    if (!resourceManager) {
        std::cerr << "AssetBrowser: Invalid resource manager" << std::endl;
        return false;
    }
    
    m_resourceManager = resourceManager;
    m_assetsPath = assetsPath;
    m_currentFolder = assetsPath;
    
    // Сканируем ассеты
    ScanAssets(assetsPath);
    
    m_initialized = true;
    
    std::cout << "AssetBrowser initialized successfully" << std::endl;
    return true;
}

void AssetBrowser::Update(float deltaTime) {
    if (!m_initialized || !m_visible) {
        return;
    }
    
    // Обновляем список ассетов
    RefreshAssets();
    
    // Вызываем callback для обновления UI
    if (m_updateCallback) {
        m_updateCallback();
    }
}

void AssetBrowser::Render(void* renderer) {
    if (!m_initialized || !m_visible) {
        return;
    }
    
    // В реальной реализации здесь бы рендерился UI браузера ассетов
    // Для простоты выводим информацию в консоль
    std::cout << "=== Asset Browser ===" << std::endl;
    std::cout << "Current folder: " << m_currentFolder << std::endl;
    
    auto filteredAssets = GetFilteredAssets();
    std::cout << "Assets (" << filteredAssets.size() << "):" << std::endl;
    
    for (const auto& asset : filteredAssets) {
        std::cout << "  " << asset.name << " (" << asset.extension << ") - " 
                  << GetFileSizeString(asset.size) << std::endl;
    }
    
    std::cout << "===================" << std::endl;
}

bool AssetBrowser::ScanAssets(const std::string& path) {
    if (path.empty()) {
        return false;
    }
    
    m_assets.clear();
    ScanDirectory(path);
    
    std::cout << "Scanned " << m_assets.size() << " assets from " << path << std::endl;
    return true;
}

void AssetBrowser::RefreshAssets() {
    // В реальной реализации здесь бы обновлялся список ассетов
    // Для простоты пропускаем
}

bool AssetBrowser::LoadAsset(const AssetInfo& assetInfo) {
    if (!m_resourceManager) {
        return false;
    }
    
    // В реальной реализации здесь бы загружался ассет через ResourceManager
    std::cout << "Loading asset: " << assetInfo.path << std::endl;
    return true;
}

bool AssetBrowser::UnloadAsset(const AssetInfo& assetInfo) {
    if (!m_resourceManager) {
        return false;
    }
    
    // В реальной реализации здесь бы выгружался ассет через ResourceManager
    std::cout << "Unloading asset: " << assetInfo.path << std::endl;
    return true;
}

bool AssetBrowser::DeleteAsset(const AssetInfo& assetInfo) {
    try {
        std::filesystem::remove(assetInfo.path);
        std::cout << "Deleted asset: " << assetInfo.path << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to delete asset: " << e.what() << std::endl;
        return false;
    }
}

bool AssetBrowser::RenameAsset(const AssetInfo& assetInfo, const std::string& newName) {
    try {
        std::filesystem::path oldPath = assetInfo.path;
        std::filesystem::path newPath = oldPath.parent_path() / newName;
        
        std::filesystem::rename(oldPath, newPath);
        std::cout << "Renamed asset: " << assetInfo.path << " -> " << newPath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to rename asset: " << e.what() << std::endl;
        return false;
    }
}

bool AssetBrowser::CopyAsset(const AssetInfo& assetInfo, const std::string& destinationPath) {
    try {
        std::filesystem::copy_file(assetInfo.path, destinationPath);
        std::cout << "Copied asset: " << assetInfo.path << " -> " << destinationPath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to copy asset: " << e.what() << std::endl;
        return false;
    }
}

bool AssetBrowser::MoveAsset(const AssetInfo& assetInfo, const std::string& destinationPath) {
    try {
        std::filesystem::rename(assetInfo.path, destinationPath);
        std::cout << "Moved asset: " << assetInfo.path << " -> " << destinationPath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to move asset: " << e.what() << std::endl;
        return false;
    }
}

AssetInfo AssetBrowser::GetAssetInfo(const std::string& path) {
    AssetInfo info;
    info.path = path;
    
    try {
        std::filesystem::path filePath(path);
        info.name = filePath.filename().string();
        info.extension = filePath.extension().string();
        info.type = GetAssetTypeFromExtension(info.extension);
        
        if (std::filesystem::exists(filePath)) {
            auto fileSize = std::filesystem::file_size(filePath);
            info.size = fileSize;
            
            auto lastWriteTime = std::filesystem::last_write_time(filePath);
            auto timeT = std::chrono::system_clock::to_time_t(
                std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    lastWriteTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
                )
            );
            
            std::stringstream ss;
            ss << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
            info.lastModified = ss.str();
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to get asset info: " << e.what() << std::endl;
    }
    
    return info;
}

std::vector<AssetInfo> AssetBrowser::GetFilteredAssets() const {
    std::vector<AssetInfo> filtered;
    
    for (const auto& asset : m_assets) {
        if (IsAssetFiltered(asset)) {
            filtered.push_back(asset);
        }
    }
    
    return filtered;
}

void AssetBrowser::SetCurrentFolder(const std::string& path) {
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
        m_currentFolder = path;
        std::cout << "Changed current folder to: " << path << std::endl;
    }
}

void AssetBrowser::GoToParentFolder() {
    std::filesystem::path currentPath(m_currentFolder);
    std::filesystem::path parentPath = currentPath.parent_path();
    
    if (parentPath != currentPath) {
        SetCurrentFolder(parentPath.string());
    }
}

void AssetBrowser::GoToChildFolder(const std::string& folderName) {
    std::filesystem::path newPath = std::filesystem::path(m_currentFolder) / folderName;
    SetCurrentFolder(newPath.string());
}

bool AssetBrowser::CreateFolder(const std::string& folderName) {
    try {
        std::filesystem::path newFolder = std::filesystem::path(m_currentFolder) / folderName;
        std::filesystem::create_directory(newFolder);
        std::cout << "Created folder: " << newFolder << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create folder: " << e.what() << std::endl;
        return false;
    }
}

bool AssetBrowser::DeleteFolder(const std::string& folderName) {
    try {
        std::filesystem::path folderPath = std::filesystem::path(m_currentFolder) / folderName;
        std::filesystem::remove_all(folderPath);
        std::cout << "Deleted folder: " << folderPath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to delete folder: " << e.what() << std::endl;
        return false;
    }
}

AssetType AssetBrowser::GetAssetTypeFromExtension(const std::string& extension) {
    std::string ext = extension;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga") {
        return AssetType::Texture;
    } else if (ext == ".wav" || ext == ".mp3" || ext == ".ogg") {
        return AssetType::Audio;
    } else if (ext == ".obj" || ext == ".fbx" || ext == ".dae") {
        return AssetType::Model;
    } else if (ext == ".scene") {
        return AssetType::Scene;
    } else if (ext == ".cpp" || ext == ".h" || ext == ".js" || ext == ".lua") {
        return AssetType::Script;
    } else if (ext == ".vert" || ext == ".frag" || ext == ".glsl") {
        return AssetType::Shader;
    } else if (ext == ".ttf" || ext == ".otf") {
        return AssetType::Font;
    } else if (ext == ".anim") {
        return AssetType::Animation;
    }
    
    return AssetType::Unknown;
}

std::string AssetBrowser::GetAssetIcon(AssetType type) {
    switch (type) {
        case AssetType::Texture:
            return "🖼️";
        case AssetType::Audio:
            return "🎵";
        case AssetType::Model:
            return "📦";
        case AssetType::Scene:
            return "🎬";
        case AssetType::Script:
            return "📝";
        case AssetType::Shader:
            return "⚡";
        case AssetType::Font:
            return "🔤";
        case AssetType::Animation:
            return "🎭";
        default:
            return "📄";
    }
}

std::string AssetBrowser::GetAssetPreview(const AssetInfo& assetInfo) {
    // В реальной реализации здесь бы генерировался превью ассета
    return GetAssetIcon(assetInfo.type);
}

void AssetBrowser::ScanDirectory(const std::string& path, int depth) {
    if (depth > 10) { // Ограничиваем глубину рекурсии
        return;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                AssetInfo info = GetAssetInfo(entry.path().string());
                m_assets.push_back(info);
            } else if (entry.is_directory()) {
                // Рекурсивно сканируем подпапки
                ScanDirectory(entry.path().string(), depth + 1);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to scan directory: " << e.what() << std::endl;
    }
}

bool AssetBrowser::IsAssetFiltered(const AssetInfo& assetInfo) const {
    // Фильтр по типу
    if (!m_filter.types.empty()) {
        if (std::find(m_filter.types.begin(), m_filter.types.end(), assetInfo.type) == m_filter.types.end()) {
            return false;
        }
    }
    
    // Фильтр по расширению
    if (!m_filter.extensions.empty()) {
        if (std::find(m_filter.extensions.begin(), m_filter.extensions.end(), assetInfo.extension) == m_filter.extensions.end()) {
            return false;
        }
    }
    
    // Фильтр по тексту поиска
    if (!m_filter.searchText.empty()) {
        std::string searchText = m_filter.searchText;
        std::string assetName = assetInfo.name;
        
        std::transform(searchText.begin(), searchText.end(), searchText.begin(), ::tolower);
        std::transform(assetName.begin(), assetName.end(), assetName.begin(), ::tolower);
        
        if (assetName.find(searchText) == std::string::npos) {
            return false;
        }
    }
    
    // Фильтр скрытых файлов
    if (!m_filter.showHidden) {
        if (assetInfo.name[0] == '.') {
            return false;
        }
    }
    
    return true;
}

void AssetBrowser::RenderAssetList() {
    // В реальной реализации здесь бы рендерился список ассетов
    std::cout << "Rendering asset list" << std::endl;
}

void AssetBrowser::RenderAssetItem(const AssetInfo& assetInfo) {
    // В реальной реализации здесь бы рендерился элемент ассета
    std::cout << "Rendering asset item: " << assetInfo.name << std::endl;
}

void AssetBrowser::RenderFolderTree() {
    // В реальной реализации здесь бы рендерилось дерево папок
    std::cout << "Rendering folder tree" << std::endl;
}

void AssetBrowser::RenderAssetPreview(const AssetInfo& assetInfo) {
    // В реальной реализации здесь бы рендерилось превью ассета
    std::cout << "Rendering asset preview: " << assetInfo.name << std::endl;
}

bool AssetBrowser::HandleAssetInput(const AssetInfo& assetInfo, float mouseX, float mouseY, int mouseButton) {
    // В реальной реализации здесь бы обрабатывался ввод для ассета
    return false;
}

void AssetBrowser::HandleDragAndDrop(const AssetInfo& assetInfo, float mouseX, float mouseY) {
    // В реальной реализации здесь бы обрабатывался drag and drop
    std::cout << "Drag and drop asset: " << assetInfo.name << std::endl;
}

std::string AssetBrowser::GetFileSizeString(size_t size) const {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unit = 0;
    double fileSize = static_cast<double>(size);
    
    while (fileSize >= 1024.0 && unit < 3) {
        fileSize /= 1024.0;
        unit++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << fileSize << " " << units[unit];
    return ss.str();
}

std::string AssetBrowser::GetLastModifiedString(const std::string& lastModified) const {
    return lastModified;
}

} // namespace ProjectSimulator

