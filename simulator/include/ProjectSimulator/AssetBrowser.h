#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <map>

// Forward declarations
namespace FastEngine {
    class ResourceManager;
}

namespace ProjectSimulator {

/**
 * Типы ассетов
 */
enum class AssetType {
    Unknown,
    Texture,
    Audio,
    Model,
    Scene,
    Script,
    Shader,
    Font,
    Animation
};

/**
 * Информация об ассете
 */
struct AssetInfo {
    std::string path;
    std::string name;
    std::string extension;
    AssetType type;
    size_t size;
    std::string lastModified;
    bool isLoaded;
    bool isDirty;
    
    AssetInfo() : type(AssetType::Unknown), size(0), isLoaded(false), isDirty(false) {}
};

/**
 * Фильтры ассетов
 */
struct AssetFilter {
    std::vector<AssetType> types;
    std::vector<std::string> extensions;
    std::string searchText;
    bool showHidden;
    
    AssetFilter() : showHidden(false) {}
};

/**
 * Браузер ассетов
 * Управляет просмотром и управлением ресурсами проекта
 */
class AssetBrowser {
public:
    AssetBrowser();
    ~AssetBrowser();

    /**
     * Инициализация браузера ассетов
     * @param resourceManager менеджер ресурсов
     * @param assetsPath путь к папке с ассетами
     * @return true если инициализация успешна
     */
    bool Initialize(FastEngine::ResourceManager* resourceManager, const std::string& assetsPath);

    /**
     * Обновление браузера ассетов
     * @param deltaTime время между кадрами
     */
    void Update(float deltaTime);

    /**
     * Рендеринг браузера ассетов
     * @param renderer рендерер
     */
    void Render(void* renderer);

    /**
     * Сканирование папки ассетов
     * @param path путь для сканирования
     * @return true если сканирование успешно
     */
    bool ScanAssets(const std::string& path);

    /**
     * Обновление списка ассетов
     */
    void RefreshAssets();

    /**
     * Загрузка ассета
     * @param assetInfo информация об ассете
     * @return true если загрузка успешна
     */
    bool LoadAsset(const AssetInfo& assetInfo);

    /**
     * Выгрузка ассета
     * @param assetInfo информация об ассете
     * @return true если выгрузка успешна
     */
    bool UnloadAsset(const AssetInfo& assetInfo);

    /**
     * Удаление ассета
     * @param assetInfo информация об ассете
     * @return true если удаление успешно
     */
    bool DeleteAsset(const AssetInfo& assetInfo);

    /**
     * Переименование ассета
     * @param assetInfo информация об ассете
     * @param newName новое имя
     * @return true если переименование успешно
     */
    bool RenameAsset(const AssetInfo& assetInfo, const std::string& newName);

    /**
     * Копирование ассета
     * @param assetInfo информация об ассете
     * @param destinationPath путь назначения
     * @return true если копирование успешно
     */
    bool CopyAsset(const AssetInfo& assetInfo, const std::string& destinationPath);

    /**
     * Перемещение ассета
     * @param assetInfo информация об ассете
     * @param destinationPath путь назначения
     * @return true если перемещение успешно
     */
    bool MoveAsset(const AssetInfo& assetInfo, const std::string& destinationPath);

    /**
     * Получение информации об ассете
     * @param path путь к ассету
     * @return информация об ассете
     */
    AssetInfo GetAssetInfo(const std::string& path);

    /**
     * Получение всех ассетов
     */
    const std::vector<AssetInfo>& GetAllAssets() const { return m_assets; }

    /**
     * Получение отфильтрованных ассетов
     */
    std::vector<AssetInfo> GetFilteredAssets() const;

    /**
     * Установка фильтра
     * @param filter фильтр
     */
    void SetFilter(const AssetFilter& filter) { m_filter = filter; }

    /**
     * Получение фильтра
     */
    const AssetFilter& GetFilter() const { return m_filter; }

    /**
     * Установка текущей папки
     * @param path путь к папке
     */
    void SetCurrentFolder(const std::string& path);

    /**
     * Получение текущей папки
     */
    const std::string& GetCurrentFolder() const { return m_currentFolder; }

    /**
     * Переход в родительскую папку
     */
    void GoToParentFolder();

    /**
     * Переход в дочернюю папку
     * @param folderName имя папки
     */
    void GoToChildFolder(const std::string& folderName);

    /**
     * Создание новой папки
     * @param folderName имя папки
     * @return true если папка создана
     */
    bool CreateFolder(const std::string& folderName);

    /**
     * Удаление папки
     * @param folderName имя папки
     * @return true если папка удалена
     */
    bool DeleteFolder(const std::string& folderName);

    /**
     * Установка callback для обновления UI
     * @param callback функция обратного вызова
     */
    void SetUpdateCallback(std::function<void()> callback) { m_updateCallback = callback; }

    /**
     * Установка callback для выбора ассета
     * @param callback функция обратного вызова
     */
    void SetSelectionCallback(std::function<void(const AssetInfo&)> callback) { m_selectionCallback = callback; }

    /**
     * Получение состояния браузера ассетов
     */
    bool IsInitialized() const { return m_initialized; }
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }

private:
    FastEngine::ResourceManager* m_resourceManager;
    std::string m_assetsPath;
    std::string m_currentFolder;
    std::vector<AssetInfo> m_assets;
    AssetFilter m_filter;
    
    bool m_initialized;
    bool m_visible;
    
    std::function<void()> m_updateCallback;
    std::function<void(const AssetInfo&)> m_selectionCallback;
    
    // Вспомогательные методы
    AssetType GetAssetTypeFromExtension(const std::string& extension);
    std::string GetAssetIcon(AssetType type);
    std::string GetAssetPreview(const AssetInfo& assetInfo);
    
    void ScanDirectory(const std::string& path, int depth = 0);
    bool IsAssetFiltered(const AssetInfo& assetInfo) const;
    
    void RenderAssetList();
    void RenderAssetItem(const AssetInfo& assetInfo);
    void RenderFolderTree();
    void RenderAssetPreview(const AssetInfo& assetInfo);
    
    bool HandleAssetInput(const AssetInfo& assetInfo, float mouseX, float mouseY, int mouseButton);
    void HandleDragAndDrop(const AssetInfo& assetInfo, float mouseX, float mouseY);
    
    std::string GetFileSizeString(size_t size) const;
    std::string GetLastModifiedString(const std::string& lastModified) const;
};

} // namespace ProjectSimulator

