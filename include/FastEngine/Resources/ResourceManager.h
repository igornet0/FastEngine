#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

namespace FastEngine {
    class Texture;
    class Sound;
    class Shader;
    
    enum class ResourceType {
        Texture,
        Sound,
        Shader,
        Font,
        Mesh,
        Animation
    };
    
    struct ResourceInfo {
        ResourceType type;
        std::string path;
        std::string name;
        size_t size;
        std::chrono::system_clock::time_point lastAccess;
        bool loaded;
        bool persistent; // Не выгружается автоматически
        
        ResourceInfo() : type(ResourceType::Texture), size(0), loaded(false), persistent(false) {}
    };
    
    class ResourceManager {
    public:
        static ResourceManager& GetInstance();
        
        // Инициализация и завершение
        bool Initialize();
        void Shutdown();
        
        // Загрузка ресурсов
        std::shared_ptr<Texture> LoadTexture(const std::string& path, bool persistent = false);
        std::shared_ptr<Sound> LoadSound(const std::string& path, bool persistent = false);
        std::shared_ptr<Shader> LoadShader(const std::string& vertexPath, const std::string& fragmentPath, bool persistent = false);
        
        // Асинхронная загрузка
        void LoadTextureAsync(const std::string& path, std::function<void(std::shared_ptr<Texture>)> callback, bool persistent = false);
        void LoadSoundAsync(const std::string& path, std::function<void(std::shared_ptr<Sound>)> callback, bool persistent = false);
        
        // Выгрузка ресурсов
        void UnloadResource(const std::string& path);
        void UnloadResource(ResourceType type, const std::string& name);
        void UnloadAll();
        void UnloadUnused();
        
        // Получение ресурсов
        std::shared_ptr<Texture> GetTexture(const std::string& path);
        std::shared_ptr<Sound> GetSound(const std::string& path);
        std::shared_ptr<Shader> GetShader(const std::string& name);
        
        // Проверка существования
        bool HasResource(const std::string& path) const;
        bool IsResourceLoaded(const std::string& path) const;
        
        // Управление памятью
        void SetMaxMemory(size_t maxMemory) { m_maxMemory = maxMemory; }
        size_t GetMaxMemory() const { return m_maxMemory; }
        size_t GetUsedMemory() const { return m_usedMemory; }
        
        void SetAutoUnload(bool enabled) { m_autoUnload = enabled; }
        bool IsAutoUnload() const { return m_autoUnload; }
        
        // Hot Reload
        void EnableHotReload(bool enabled) { m_hotReload = enabled; }
        bool IsHotReloadEnabled() const { return m_hotReload; }
        
        void WatchFile(const std::string& path);
        void UnwatchFile(const std::string& path);
        
        // Обновление (проверка изменений файлов)
        void Update();
        
        // Получение информации
        std::vector<ResourceInfo> GetResourceInfo() const;
        size_t GetResourceCount() const;
        size_t GetLoadedResourceCount() const;
        
        // События
        void SetOnResourceLoaded(std::function<void(const std::string&, ResourceType)> callback);
        void SetOnResourceUnloaded(std::function<void(const std::string&, ResourceType)> callback);
        void SetOnResourceReloaded(std::function<void(const std::string&, ResourceType)> callback);
        
        // Утилиты
        std::string GetResourcePath(const std::string& name) const;
        std::string GetResourceName(const std::string& path) const;
        ResourceType GetResourceType(const std::string& path) const;
        
    private:
        ResourceManager() = default;
        ~ResourceManager() = default;
        
        // Запрещаем копирование
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;
        
        // Внутренние методы
        std::string GenerateResourceName(const std::string& path) const;
        void UpdateResourceAccess(const std::string& path);
        void CheckFileChanges();
        void ReloadResource(const std::string& path);
        
        // Хранилище ресурсов
        std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
        std::unordered_map<std::string, std::shared_ptr<Sound>> m_sounds;
        std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
        
        // Информация о ресурсах
        std::unordered_map<std::string, ResourceInfo> m_resourceInfo;
        
        // Управление памятью
        size_t m_maxMemory;
        size_t m_usedMemory;
        bool m_autoUnload;
        
        // Hot Reload
        bool m_hotReload;
        std::unordered_map<std::string, std::chrono::system_clock::time_point> m_fileTimestamps;
        std::vector<std::string> m_watchedFiles;
        
        // Асинхронная загрузка
        std::vector<std::thread> m_loadingThreads;
        std::mutex m_mutex;
        std::atomic<bool> m_shutdown;
        
        // События
        std::function<void(const std::string&, ResourceType)> m_onResourceLoaded;
        std::function<void(const std::string&, ResourceType)> m_onResourceUnloaded;
        std::function<void(const std::string&, ResourceType)> m_onResourceReloaded;
    };
}

