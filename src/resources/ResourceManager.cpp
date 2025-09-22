#include "FastEngine/Resources/ResourceManager.h"
#include "FastEngine/Render/Texture.h"
#include "FastEngine/Audio/Sound.h"
#include "FastEngine/Render/Shader.h"
#include "FastEngine/Platform/FileSystem.h"
#include <filesystem>
#include <algorithm>
#include <chrono>

namespace FastEngine {
    ResourceManager& ResourceManager::GetInstance() {
        static ResourceManager instance;
        return instance;
    }
    
    bool ResourceManager::Initialize() {
        m_maxMemory = 256 * 1024 * 1024; // 256 MB по умолчанию
        m_usedMemory = 0;
        m_autoUnload = true;
        m_hotReload = false;
        m_shutdown = false;
        
        return true;
    }
    
    void ResourceManager::Shutdown() {
        m_shutdown = true;
        
        // Ждем завершения всех потоков загрузки
        for (auto& thread : m_loadingThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        m_loadingThreads.clear();
        
        // Выгружаем все ресурсы
        UnloadAll();
    }
    
    std::shared_ptr<Texture> ResourceManager::LoadTexture(const std::string& path, bool persistent) {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        // Проверяем, уже загружена ли текстура
        auto it = m_textures.find(path);
        if (it != m_textures.end()) {
            UpdateResourceAccess(path);
            return it->second;
        }
        
        // Загружаем новую текстуру
        auto texture = std::make_shared<Texture>();
        if (texture->LoadFromFile(path)) {
            m_textures[path] = texture;
            
            // Обновляем информацию о ресурсе
            ResourceInfo info;
            info.type = ResourceType::Texture;
            info.path = path;
            info.name = GenerateResourceName(path);
            info.size = 0; // В реальной реализации вычислялся бы размер
            info.lastAccess = std::chrono::system_clock::now();
            info.loaded = true;
            info.persistent = persistent;
            m_resourceInfo[path] = info;
            
            if (m_onResourceLoaded) {
                m_onResourceLoaded(path, ResourceType::Texture);
            }
            
            return texture;
        }
        
        return nullptr;
    }
    
    std::shared_ptr<Sound> ResourceManager::LoadSound(const std::string& path, bool persistent) {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        auto it = m_sounds.find(path);
        if (it != m_sounds.end()) {
            UpdateResourceAccess(path);
            return it->second;
        }
        
        auto sound = std::make_shared<Sound>();
        if (sound->LoadFromFile(path)) {
            m_sounds[path] = sound;
            
            ResourceInfo info;
            info.type = ResourceType::Sound;
            info.path = path;
            info.name = GenerateResourceName(path);
            info.size = 0;
            info.lastAccess = std::chrono::system_clock::now();
            info.loaded = true;
            info.persistent = persistent;
            m_resourceInfo[path] = info;
            
            if (m_onResourceLoaded) {
                m_onResourceLoaded(path, ResourceType::Sound);
            }
            
            return sound;
        }
        
        return nullptr;
    }
    
    std::shared_ptr<Shader> ResourceManager::LoadShader(const std::string& vertexPath, const std::string& fragmentPath, bool persistent) {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        std::string name = GenerateResourceName(vertexPath + "|" + fragmentPath);
        
        auto it = m_shaders.find(name);
        if (it != m_shaders.end()) {
            UpdateResourceAccess(name);
            return it->second;
        }
        
        auto shader = std::make_shared<Shader>();
        if (shader->LoadFromFiles(vertexPath, fragmentPath)) {
            m_shaders[name] = shader;
            
            ResourceInfo info;
            info.type = ResourceType::Shader;
            info.path = name;
            info.name = name;
            info.size = 0;
            info.lastAccess = std::chrono::system_clock::now();
            info.loaded = true;
            info.persistent = persistent;
            m_resourceInfo[name] = info;
            
            if (m_onResourceLoaded) {
                m_onResourceLoaded(name, ResourceType::Shader);
            }
            
            return shader;
        }
        
        return nullptr;
    }
    
    void ResourceManager::LoadTextureAsync(const std::string& path, std::function<void(std::shared_ptr<Texture>)> callback, bool persistent) {
        if (m_shutdown) {
            callback(nullptr);
            return;
        }
        
        std::thread loadingThread([this, path, callback, persistent]() {
            auto texture = LoadTexture(path, persistent);
            callback(texture);
        });
        
        m_loadingThreads.push_back(std::move(loadingThread));
    }
    
    void ResourceManager::LoadSoundAsync(const std::string& path, std::function<void(std::shared_ptr<Sound>)> callback, bool persistent) {
        if (m_shutdown) {
            callback(nullptr);
            return;
        }
        
        std::thread loadingThread([this, path, callback, persistent]() {
            auto sound = LoadSound(path, persistent);
            callback(sound);
        });
        
        m_loadingThreads.push_back(std::move(loadingThread));
    }
    
    void ResourceManager::UnloadResource(const std::string& path) {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        auto infoIt = m_resourceInfo.find(path);
        if (infoIt == m_resourceInfo.end()) {
            return;
        }
        
        ResourceType type = infoIt->second.type;
        
        switch (type) {
            case ResourceType::Texture:
                m_textures.erase(path);
                break;
            case ResourceType::Sound:
                m_sounds.erase(path);
                break;
            case ResourceType::Shader:
                m_shaders.erase(path);
                break;
            default:
                break;
        }
        
        m_resourceInfo.erase(infoIt);
        
        if (m_onResourceUnloaded) {
            m_onResourceUnloaded(path, type);
        }
    }
    
    void ResourceManager::UnloadResource(ResourceType type, const std::string& name) {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        for (auto it = m_resourceInfo.begin(); it != m_resourceInfo.end();) {
            if (it->second.type == type && it->second.name == name) {
                std::string path = it->first;
                it = m_resourceInfo.erase(it);
                
                switch (type) {
                    case ResourceType::Texture:
                        m_textures.erase(path);
                        break;
                    case ResourceType::Sound:
                        m_sounds.erase(path);
                        break;
                    case ResourceType::Shader:
                        m_shaders.erase(path);
                        break;
                    default:
                        break;
                }
                
                if (m_onResourceUnloaded) {
                    m_onResourceUnloaded(path, type);
                }
            } else {
                ++it;
            }
        }
    }
    
    void ResourceManager::UnloadAll() {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        m_textures.clear();
        m_sounds.clear();
        m_shaders.clear();
        m_resourceInfo.clear();
        m_usedMemory = 0;
    }
    
    void ResourceManager::UnloadUnused() {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        auto now = std::chrono::system_clock::now();
        auto timeout = std::chrono::minutes(5); // 5 минут неиспользования
        
        for (auto it = m_resourceInfo.begin(); it != m_resourceInfo.end();) {
            if (!it->second.persistent && 
                now - it->second.lastAccess > timeout) {
                
                std::string path = it->first;
                ResourceType type = it->second.type;
                
                switch (type) {
                    case ResourceType::Texture:
                        m_textures.erase(path);
                        break;
                    case ResourceType::Sound:
                        m_sounds.erase(path);
                        break;
                    case ResourceType::Shader:
                        m_shaders.erase(path);
                        break;
                    default:
                        break;
                }
                
                it = m_resourceInfo.erase(it);
                
                if (m_onResourceUnloaded) {
                    m_onResourceUnloaded(path, type);
                }
            } else {
                ++it;
            }
        }
    }
    
    std::shared_ptr<Texture> ResourceManager::GetTexture(const std::string& path) {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        auto it = m_textures.find(path);
        if (it != m_textures.end()) {
            UpdateResourceAccess(path);
            return it->second;
        }
        
        return nullptr;
    }
    
    std::shared_ptr<Sound> ResourceManager::GetSound(const std::string& path) {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        auto it = m_sounds.find(path);
        if (it != m_sounds.end()) {
            UpdateResourceAccess(path);
            return it->second;
        }
        
        return nullptr;
    }
    
    std::shared_ptr<Shader> ResourceManager::GetShader(const std::string& name) {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        auto it = m_shaders.find(name);
        if (it != m_shaders.end()) {
            UpdateResourceAccess(name);
            return it->second;
        }
        
        return nullptr;
    }
    
    bool ResourceManager::HasResource(const std::string& path) const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        return m_resourceInfo.find(path) != m_resourceInfo.end();
    }
    
    bool ResourceManager::IsResourceLoaded(const std::string& path) const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        auto it = m_resourceInfo.find(path);
        return it != m_resourceInfo.end() && it->second.loaded;
    }
    
    void ResourceManager::Update() {
        if (m_hotReload) {
            CheckFileChanges();
        }
        
        if (m_autoUnload) {
            UnloadUnused();
        }
        
        // Очищаем завершенные потоки
        m_loadingThreads.erase(
            std::remove_if(m_loadingThreads.begin(), m_loadingThreads.end(),
                [](std::thread& t) {
                    return !t.joinable();
                }),
            m_loadingThreads.end()
        );
    }
    
    std::vector<ResourceInfo> ResourceManager::GetResourceInfo() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        std::vector<ResourceInfo> info;
        for (const auto& pair : m_resourceInfo) {
            info.push_back(pair.second);
        }
        
        return info;
    }
    
    size_t ResourceManager::GetResourceCount() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        return m_resourceInfo.size();
    }
    
    size_t ResourceManager::GetLoadedResourceCount() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        size_t count = 0;
        for (const auto& pair : m_resourceInfo) {
            if (pair.second.loaded) {
                count++;
            }
        }
        
        return count;
    }
    
    void ResourceManager::SetOnResourceLoaded(std::function<void(const std::string&, ResourceType)> callback) {
        m_onResourceLoaded = callback;
    }
    
    void ResourceManager::SetOnResourceUnloaded(std::function<void(const std::string&, ResourceType)> callback) {
        m_onResourceUnloaded = callback;
    }
    
    void ResourceManager::SetOnResourceReloaded(std::function<void(const std::string&, ResourceType)> callback) {
        m_onResourceReloaded = callback;
    }
    
    std::string ResourceManager::GetResourcePath(const std::string& name) const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        for (const auto& pair : m_resourceInfo) {
            if (pair.second.name == name) {
                return pair.first;
            }
        }
        
        return "";
    }
    
    std::string ResourceManager::GetResourceName(const std::string& path) const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        auto it = m_resourceInfo.find(path);
        return it != m_resourceInfo.end() ? it->second.name : "";
    }
    
    ResourceType ResourceManager::GetResourceType(const std::string& path) const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
        
        auto it = m_resourceInfo.find(path);
        return it != m_resourceInfo.end() ? it->second.type : ResourceType::Texture;
    }
    
    std::string ResourceManager::GenerateResourceName(const std::string& path) const {
        std::filesystem::path filePath(path);
        return filePath.stem().string();
    }
    
    void ResourceManager::UpdateResourceAccess(const std::string& path) {
        auto it = m_resourceInfo.find(path);
        if (it != m_resourceInfo.end()) {
            it->second.lastAccess = std::chrono::system_clock::now();
        }
    }
    
    void ResourceManager::CheckFileChanges() {
        // Упрощенная реализация проверки изменений файлов
        // В реальной реализации использовался бы файловый вотчер
    }
    
    void ResourceManager::ReloadResource(const std::string& path) {
        // Упрощенная реализация перезагрузки ресурса
        // В реальной реализации ресурс бы перезагружался
    }
}
