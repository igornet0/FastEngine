#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <any>

namespace FastEngine {

/**
 * Интерфейс плагина
 */
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Инициализация и завершение
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    
    // Информация о плагине
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::string GetDescription() const = 0;
    
    // Обновление
    virtual void Update(float deltaTime) {}
    
    // События
    virtual void OnLoad() {}
    virtual void OnUnload() {}
    virtual void OnEnable() {}
    virtual void OnDisable() {}
};

/**
 * Информация о плагине
 */
struct PluginInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string path;
    bool enabled;
    bool loaded;
    
    PluginInfo() : enabled(false), loaded(false) {}
    PluginInfo(const std::string& n, const std::string& v, const std::string& d, const std::string& a)
        : name(n), version(v), description(d), author(a), enabled(false), loaded(false) {}
};

/**
 * Менеджер плагинов
 */
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Инициализация
    bool Initialize();
    void Shutdown();
    
    // Управление плагинами
    bool LoadPlugin(const std::string& path);
    bool UnloadPlugin(const std::string& name);
    bool ReloadPlugin(const std::string& name);
    
    // Получение плагинов
    std::shared_ptr<IPlugin> GetPlugin(const std::string& name) const;
    const std::vector<PluginInfo>& GetPluginInfos() const { return m_pluginInfos; }
    
    // Включение/отключение
    bool EnablePlugin(const std::string& name);
    bool DisablePlugin(const std::string& name);
    bool IsPluginEnabled(const std::string& name) const;
    
    // Обновление
    void Update(float deltaTime);
    
    // Сканирование директорий
    void ScanDirectory(const std::string& directory);
    void RefreshPlugins();
    
    // Callbacks
    void SetOnPluginLoaded(std::function<void(const std::string&)> callback) { m_onPluginLoaded = callback; }
    void SetOnPluginUnloaded(std::function<void(const std::string&)> callback) { m_onPluginUnloaded = callback; }
    void SetOnPluginEnabled(std::function<void(const std::string&)> callback) { m_onPluginEnabled = callback; }
    void SetOnPluginDisabled(std::function<void(const std::string&)> callback) { m_onPluginDisabled = callback; }
    
private:
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> m_plugins;
    std::vector<PluginInfo> m_pluginInfos;
    std::string m_pluginDirectory;
    bool m_initialized;
    
    // Callbacks
    std::function<void(const std::string&)> m_onPluginLoaded;
    std::function<void(const std::string&)> m_onPluginUnloaded;
    std::function<void(const std::string&)> m_onPluginEnabled;
    std::function<void(const std::string&)> m_onPluginDisabled;
    
    // Внутренние методы
    bool LoadPluginFromFile(const std::string& path);
    void UnloadPluginInternal(const std::string& name);
    PluginInfo ExtractPluginInfo(const std::string& path);
    bool ValidatePlugin(std::shared_ptr<IPlugin> plugin);
};

/**
 * API для плагинов
 */
class PluginAPI {
public:
    PluginAPI();
    ~PluginAPI() = default;
    
    // Регистрация функций
    void RegisterFunction(const std::string& name, std::function<std::any(const std::vector<std::any>&)> function);
    void UnregisterFunction(const std::string& name);
    
    // Вызов функций
    std::any CallFunction(const std::string& name, const std::vector<std::any>& args = {});
    bool HasFunction(const std::string& name) const;
    
    // Управление данными
    void SetData(const std::string& key, const std::any& value);
    std::any GetData(const std::string& key) const;
    bool HasData(const std::string& key) const;
    void RemoveData(const std::string& key);
    
    // События
    void EmitEvent(const std::string& eventName, const std::any& data = std::any{});
    void SubscribeToEvent(const std::string& eventName, std::function<void(const std::any&)> callback);
    void UnsubscribeFromEvent(const std::string& eventName, std::function<void(const std::any&)> callback);
    
private:
    std::unordered_map<std::string, std::function<std::any(const std::vector<std::any>&)>> m_functions;
    std::unordered_map<std::string, std::any> m_data;
    std::unordered_map<std::string, std::vector<std::function<void(const std::any&)>>> m_eventSubscribers;
};

/**
 * Базовый класс для плагинов
 */
class BasePlugin : public IPlugin {
public:
    BasePlugin();
    virtual ~BasePlugin() = default;
    
    // IPlugin interface
    bool Initialize() override;
    void Shutdown() override;
    std::string GetName() const override { return m_name; }
    std::string GetVersion() const override { return m_version; }
    std::string GetDescription() const override { return m_description; }
    void Update(float deltaTime) override;
    
    // Настройка плагина
    void SetName(const std::string& name) { m_name = name; }
    void SetVersion(const std::string& version) { m_version = version; }
    void SetDescription(const std::string& description) { m_description = description; }
    
    // API доступ
    PluginAPI* GetAPI() const { return m_api; }
    
protected:
    std::string m_name;
    std::string m_version;
    std::string m_description;
    PluginAPI* m_api;
    bool m_initialized;
    
    // Виртуальные методы для переопределения
    virtual bool OnInitialize() { return true; }
    virtual void OnShutdown() {}
    virtual void OnUpdate(float deltaTime) {}
};

/**
 * Marketplace интеграция
 */
class PluginMarketplace {
public:
    PluginMarketplace();
    ~PluginMarketplace() = default;
    
    // Инициализация
    bool Initialize();
    void Shutdown();
    
    // Поиск плагинов
    std::vector<PluginInfo> SearchPlugins(const std::string& query);
    std::vector<PluginInfo> GetFeaturedPlugins();
    std::vector<PluginInfo> GetPopularPlugins();
    std::vector<PluginInfo> GetRecentPlugins();
    
    // Управление плагинами
    bool DownloadPlugin(const std::string& pluginId);
    bool InstallPlugin(const std::string& pluginId);
    bool UninstallPlugin(const std::string& pluginId);
    
    // Рейтинг и отзывы
    bool RatePlugin(const std::string& pluginId, int rating);
    bool ReviewPlugin(const std::string& pluginId, const std::string& review);
    std::vector<std::string> GetPluginReviews(const std::string& pluginId);
    
    // Обновления
    std::vector<PluginInfo> CheckForUpdates();
    bool UpdatePlugin(const std::string& pluginId);
    
private:
    std::string m_marketplaceUrl;
    bool m_initialized;
    
    // Внутренние методы
    std::vector<PluginInfo> FetchPluginsFromAPI(const std::string& endpoint);
    bool DownloadPluginFile(const std::string& pluginId, const std::string& url);
    bool InstallPluginFile(const std::string& filePath);
};

} // namespace FastEngine

