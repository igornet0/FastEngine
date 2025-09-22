#include "FastEngine/Plugins/PluginManager.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace FastEngine {

// PluginAPI implementation
PluginAPI::PluginAPI() {}

void PluginAPI::RegisterFunction(const std::string& name, std::function<std::any(const std::vector<std::any>&)> function) {
    m_functions[name] = function;
    std::cout << "PluginAPI: Registered function: " << name << std::endl;
}

void PluginAPI::UnregisterFunction(const std::string& name) {
    m_functions.erase(name);
    std::cout << "PluginAPI: Unregistered function: " << name << std::endl;
}

std::any PluginAPI::CallFunction(const std::string& name, const std::vector<std::any>& args) {
    auto it = m_functions.find(name);
    if (it != m_functions.end()) {
        return it->second(args);
    }
    return std::any{};
}

bool PluginAPI::HasFunction(const std::string& name) const {
    return m_functions.find(name) != m_functions.end();
}

void PluginAPI::SetData(const std::string& key, const std::any& value) {
    m_data[key] = value;
    std::cout << "PluginAPI: Set data: " << key << std::endl;
}

std::any PluginAPI::GetData(const std::string& key) const {
    auto it = m_data.find(key);
    return (it != m_data.end()) ? it->second : std::any{};
}

bool PluginAPI::HasData(const std::string& key) const {
    return m_data.find(key) != m_data.end();
}

void PluginAPI::RemoveData(const std::string& key) {
    m_data.erase(key);
    std::cout << "PluginAPI: Removed data: " << key << std::endl;
}

void PluginAPI::EmitEvent(const std::string& eventName, const std::any& data) {
    auto it = m_eventSubscribers.find(eventName);
    if (it != m_eventSubscribers.end()) {
        for (auto& callback : it->second) {
            callback(data);
        }
    }
    std::cout << "PluginAPI: Emitted event: " << eventName << std::endl;
}

void PluginAPI::SubscribeToEvent(const std::string& eventName, std::function<void(const std::any&)> callback) {
    m_eventSubscribers[eventName].push_back(callback);
    std::cout << "PluginAPI: Subscribed to event: " << eventName << std::endl;
}

void PluginAPI::UnsubscribeFromEvent(const std::string& eventName, std::function<void(const std::any&)> callback) {
    auto it = m_eventSubscribers.find(eventName);
    if (it != m_eventSubscribers.end()) {
        // Используем remove_if с лямбда-функцией для сравнения
        it->second.erase(
            std::remove_if(it->second.begin(), it->second.end(),
                [&callback](const std::function<void(const std::any&)>& func) {
                    // Простое сравнение указателей на функции
                    return func.target<void(const std::any&)>() == callback.target<void(const std::any&)>();
                }),
            it->second.end()
        );
    }
    std::cout << "PluginAPI: Unsubscribed from event: " << eventName << std::endl;
}

// BasePlugin implementation
BasePlugin::BasePlugin() 
    : m_api(nullptr)
    , m_initialized(false) {
}

bool BasePlugin::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    m_initialized = OnInitialize();
    if (m_initialized) {
        OnLoad();
    }
    
    return m_initialized;
}

void BasePlugin::Shutdown() {
    if (m_initialized) {
        OnShutdown();
        OnUnload();
        m_initialized = false;
    }
}

void BasePlugin::Update(float deltaTime) {
    if (m_initialized) {
        OnUpdate(deltaTime);
    }
}

// PluginManager implementation
PluginManager::PluginManager() 
    : m_initialized(false) {
}

PluginManager::~PluginManager() {
    if (m_initialized) {
        Shutdown();
    }
}

bool PluginManager::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    m_initialized = true;
    std::cout << "PluginManager initialized successfully" << std::endl;
    return true;
}

void PluginManager::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    // Отключаем все плагины
    for (auto& pair : m_plugins) {
        pair.second->Shutdown();
    }
    
    m_plugins.clear();
    m_pluginInfos.clear();
    m_initialized = false;
    
    std::cout << "PluginManager shutdown" << std::endl;
}

bool PluginManager::LoadPlugin(const std::string& path) {
    if (!m_initialized) {
        std::cerr << "PluginManager: Not initialized" << std::endl;
        return false;
    }
    
    return LoadPluginFromFile(path);
}

bool PluginManager::UnloadPlugin(const std::string& name) {
    if (!m_initialized) {
        std::cerr << "PluginManager: Not initialized" << std::endl;
        return false;
    }
    
    auto it = m_plugins.find(name);
    if (it == m_plugins.end()) {
        std::cerr << "PluginManager: Plugin not found: " << name << std::endl;
        return false;
    }
    
    UnloadPluginInternal(name);
    return true;
}

bool PluginManager::ReloadPlugin(const std::string& name) {
    if (!m_initialized) {
        std::cerr << "PluginManager: Not initialized" << std::endl;
        return false;
    }
    
    auto it = m_plugins.find(name);
    if (it == m_plugins.end()) {
        std::cerr << "PluginManager: Plugin not found: " << name << std::endl;
        return false;
    }
    
    std::string path = it->second->GetName(); // В реальной реализации здесь бы сохранялся путь
    UnloadPluginInternal(name);
    return LoadPluginFromFile(path);
}

std::shared_ptr<IPlugin> PluginManager::GetPlugin(const std::string& name) const {
    auto it = m_plugins.find(name);
    return (it != m_plugins.end()) ? it->second : nullptr;
}

bool PluginManager::EnablePlugin(const std::string& name) {
    auto it = m_plugins.find(name);
    if (it == m_plugins.end()) {
        std::cerr << "PluginManager: Plugin not found: " << name << std::endl;
        return false;
    }
    
    auto pluginInfo = std::find_if(m_pluginInfos.begin(), m_pluginInfos.end(),
        [&name](const PluginInfo& info) {
            return info.name == name;
        });
    
    if (pluginInfo != m_pluginInfos.end()) {
        pluginInfo->enabled = true;
        it->second->OnEnable();
        
        if (m_onPluginEnabled) {
            m_onPluginEnabled(name);
        }
        
        std::cout << "PluginManager: Enabled plugin: " << name << std::endl;
        return true;
    }
    
    return false;
}

bool PluginManager::DisablePlugin(const std::string& name) {
    auto it = m_plugins.find(name);
    if (it == m_plugins.end()) {
        std::cerr << "PluginManager: Plugin not found: " << name << std::endl;
        return false;
    }
    
    auto pluginInfo = std::find_if(m_pluginInfos.begin(), m_pluginInfos.end(),
        [&name](const PluginInfo& info) {
            return info.name == name;
        });
    
    if (pluginInfo != m_pluginInfos.end()) {
        pluginInfo->enabled = false;
        it->second->OnDisable();
        
        if (m_onPluginDisabled) {
            m_onPluginDisabled(name);
        }
        
        std::cout << "PluginManager: Disabled plugin: " << name << std::endl;
        return true;
    }
    
    return false;
}

bool PluginManager::IsPluginEnabled(const std::string& name) const {
    auto pluginInfo = std::find_if(m_pluginInfos.begin(), m_pluginInfos.end(),
        [&name](const PluginInfo& info) {
            return info.name == name;
        });
    
    return (pluginInfo != m_pluginInfos.end()) ? pluginInfo->enabled : false;
}

void PluginManager::Update(float deltaTime) {
    if (!m_initialized) {
        return;
    }
    
    // Обновляем все включенные плагины
    for (auto& pair : m_plugins) {
        if (IsPluginEnabled(pair.first)) {
            pair.second->Update(deltaTime);
        }
    }
}

void PluginManager::ScanDirectory(const std::string& directory) {
    if (!m_initialized) {
        std::cerr << "PluginManager: Not initialized" << std::endl;
        return;
    }
    
    m_pluginDirectory = directory;
    std::cout << "PluginManager: Scanning directory: " << directory << std::endl;
    
    // В реальной реализации здесь бы сканировалась директория
    // и загружались все найденные плагины
}

void PluginManager::RefreshPlugins() {
    if (!m_initialized) {
        std::cerr << "PluginManager: Not initialized" << std::endl;
        return;
    }
    
    std::cout << "PluginManager: Refreshing plugins" << std::endl;
    
    // В реальной реализации здесь бы обновлялся список плагинов
}

bool PluginManager::LoadPluginFromFile(const std::string& path) {
    // В реальной реализации здесь бы загружался плагин из файла
    std::cout << "PluginManager: Loading plugin from: " << path << std::endl;
    
    // Создаем заглушку плагина
    auto plugin = std::make_shared<BasePlugin>();
    plugin->SetName("TestPlugin");
    plugin->SetVersion("1.0.0");
    plugin->SetDescription("Test plugin for demonstration");
    
    if (plugin->Initialize()) {
        m_plugins[plugin->GetName()] = plugin;
        
        PluginInfo info = ExtractPluginInfo(path);
        info.loaded = true;
        m_pluginInfos.push_back(info);
        
        if (m_onPluginLoaded) {
            m_onPluginLoaded(plugin->GetName());
        }
        
        return true;
    }
    
    return false;
}

void PluginManager::UnloadPluginInternal(const std::string& name) {
    auto it = m_plugins.find(name);
    if (it != m_plugins.end()) {
        it->second->Shutdown();
        m_plugins.erase(it);
        
        auto pluginInfo = std::find_if(m_pluginInfos.begin(), m_pluginInfos.end(),
            [&name](const PluginInfo& info) {
                return info.name == name;
            });
        
        if (pluginInfo != m_pluginInfos.end()) {
            pluginInfo->loaded = false;
        }
        
        if (m_onPluginUnloaded) {
            m_onPluginUnloaded(name);
        }
        
        std::cout << "PluginManager: Unloaded plugin: " << name << std::endl;
    }
}

PluginInfo PluginManager::ExtractPluginInfo(const std::string& path) {
    PluginInfo info;
    info.name = "TestPlugin";
    info.version = "1.0.0";
    info.description = "Test plugin for demonstration";
    info.author = "FastEngine";
    info.path = path;
    info.enabled = false;
    info.loaded = false;
    return info;
}

bool PluginManager::ValidatePlugin(std::shared_ptr<IPlugin> plugin) {
    if (!plugin) {
        return false;
    }
    
    // Проверяем, что плагин имеет все необходимые методы
    return !plugin->GetName().empty() && 
           !plugin->GetVersion().empty() && 
           !plugin->GetDescription().empty();
}

// PluginMarketplace implementation
PluginMarketplace::PluginMarketplace() 
    : m_marketplaceUrl("https://marketplace.fastengine.com")
    , m_initialized(false) {
}

bool PluginMarketplace::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    m_initialized = true;
    std::cout << "PluginMarketplace initialized successfully" << std::endl;
    return true;
}

void PluginMarketplace::Shutdown() {
    m_initialized = false;
    std::cout << "PluginMarketplace shutdown" << std::endl;
}

std::vector<PluginInfo> PluginMarketplace::SearchPlugins(const std::string& query) {
    std::cout << "PluginMarketplace: Searching plugins for: " << query << std::endl;
    
    // В реальной реализации здесь бы выполнялся поиск в API
    std::vector<PluginInfo> results;
    return results;
}

std::vector<PluginInfo> PluginMarketplace::GetFeaturedPlugins() {
    std::cout << "PluginMarketplace: Getting featured plugins" << std::endl;
    
    // В реальной реализации здесь бы получались рекомендуемые плагины
    std::vector<PluginInfo> results;
    return results;
}

std::vector<PluginInfo> PluginMarketplace::GetPopularPlugins() {
    std::cout << "PluginMarketplace: Getting popular plugins" << std::endl;
    
    // В реальной реализации здесь бы получались популярные плагины
    std::vector<PluginInfo> results;
    return results;
}

std::vector<PluginInfo> PluginMarketplace::GetRecentPlugins() {
    std::cout << "PluginMarketplace: Getting recent plugins" << std::endl;
    
    // В реальной реализации здесь бы получались недавние плагины
    std::vector<PluginInfo> results;
    return results;
}

bool PluginMarketplace::DownloadPlugin(const std::string& pluginId) {
    std::cout << "PluginMarketplace: Downloading plugin: " << pluginId << std::endl;
    
    // В реальной реализации здесь бы скачивался плагин
    return true;
}

bool PluginMarketplace::InstallPlugin(const std::string& pluginId) {
    std::cout << "PluginMarketplace: Installing plugin: " << pluginId << std::endl;
    
    // В реальной реализации здесь бы устанавливался плагин
    return true;
}

bool PluginMarketplace::UninstallPlugin(const std::string& pluginId) {
    std::cout << "PluginMarketplace: Uninstalling plugin: " << pluginId << std::endl;
    
    // В реальной реализации здесь бы удалялся плагин
    return true;
}

bool PluginMarketplace::RatePlugin(const std::string& pluginId, int rating) {
    std::cout << "PluginMarketplace: Rating plugin " << pluginId << " with " << rating << " stars" << std::endl;
    
    // В реальной реализации здесь бы отправлялся рейтинг
    return true;
}

bool PluginMarketplace::ReviewPlugin(const std::string& pluginId, const std::string& review) {
    std::cout << "PluginMarketplace: Reviewing plugin " << pluginId << ": " << review << std::endl;
    
    // В реальной реализации здесь бы отправлялся отзыв
    return true;
}

std::vector<std::string> PluginMarketplace::GetPluginReviews(const std::string& pluginId) {
    std::cout << "PluginMarketplace: Getting reviews for plugin: " << pluginId << std::endl;
    
    // В реальной реализации здесь бы получались отзывы
    std::vector<std::string> reviews;
    return reviews;
}

std::vector<PluginInfo> PluginMarketplace::CheckForUpdates() {
    std::cout << "PluginMarketplace: Checking for updates" << std::endl;
    
    // В реальной реализации здесь бы проверялись обновления
    std::vector<PluginInfo> updates;
    return updates;
}

bool PluginMarketplace::UpdatePlugin(const std::string& pluginId) {
    std::cout << "PluginMarketplace: Updating plugin: " << pluginId << std::endl;
    
    // В реальной реализации здесь бы обновлялся плагин
    return true;
}

std::vector<PluginInfo> PluginMarketplace::FetchPluginsFromAPI(const std::string& endpoint) {
    std::cout << "PluginMarketplace: Fetching plugins from API: " << endpoint << std::endl;
    
    // В реальной реализации здесь бы выполнялся запрос к API
    std::vector<PluginInfo> plugins;
    return plugins;
}

bool PluginMarketplace::DownloadPluginFile(const std::string& pluginId, const std::string& url) {
    std::cout << "PluginMarketplace: Downloading plugin file: " << pluginId << " from " << url << std::endl;
    
    // В реальной реализации здесь бы скачивался файл
    return true;
}

bool PluginMarketplace::InstallPluginFile(const std::string& filePath) {
    std::cout << "PluginMarketplace: Installing plugin file: " << filePath << std::endl;
    
    // В реальной реализации здесь бы устанавливался файл
    return true;
}

} // namespace FastEngine
