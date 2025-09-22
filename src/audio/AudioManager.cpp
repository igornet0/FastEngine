#include "FastEngine/Audio/AudioManager.h"
#include "FastEngine/Audio/Sound.h"
#include <algorithm>

namespace FastEngine {
    AudioManager::AudioManager() 
        : m_masterVolume(1.0f)
        , m_muted(false)
        , m_initialized(false) {
    }
    
    AudioManager::~AudioManager() {
        Shutdown();
    }
    
    bool AudioManager::Initialize() {
        if (m_initialized) {
            return true;
        }
        
        // На мобильных платформах аудио инициализируется автоматически
        // Здесь можно добавить платформо-специфичную инициализацию
        
        m_initialized = true;
        return true;
    }
    
    void AudioManager::Shutdown() {
        if (!m_initialized) {
            return;
        }
        
        // Останавливаем все звуки
        for (auto& pair : m_sounds) {
            if (pair.second) {
                pair.second->Stop();
            }
        }
        
        // Очищаем все звуки
        m_sounds.clear();
        
        m_initialized = false;
    }
    
    Sound* AudioManager::LoadSound(const std::string& filePath) {
        // Проверяем, не загружен ли уже этот звук
        auto it = m_sounds.find(filePath);
        if (it != m_sounds.end()) {
            return it->second.get();
        }
        
        // Создаем новый звук
        auto sound = std::make_unique<Sound>();
        if (!sound->LoadFromFile(filePath)) {
            return nullptr;
        }
        
        // Сохраняем звук
        Sound* soundPtr = sound.get();
        m_sounds[filePath] = std::move(sound);
        
        return soundPtr;
    }
    
    void AudioManager::UnloadSound(const std::string& filePath) {
        auto it = m_sounds.find(filePath);
        if (it != m_sounds.end()) {
            it->second->Stop();
            m_sounds.erase(it);
        }
    }
    
    void AudioManager::UnloadAllSounds() {
        for (auto& pair : m_sounds) {
            if (pair.second) {
                pair.second->Stop();
            }
        }
        m_sounds.clear();
    }
    
    void AudioManager::SetMasterVolume(float volume) {
        m_masterVolume = std::clamp(volume, 0.0f, 1.0f);
        
        // Применяем громкость ко всем звукам
        for (auto& pair : m_sounds) {
            if (pair.second) {
                pair.second->SetVolume(pair.second->GetVolume() * m_masterVolume);
            }
        }
    }
    
    void AudioManager::SetMuted(bool muted) {
        m_muted = muted;
        
        if (muted) {
            // Останавливаем все звуки
            for (auto& pair : m_sounds) {
                if (pair.second) {
                    pair.second->Stop();
                }
            }
        }
    }
    
    Sound* AudioManager::GetSound(const std::string& name) const {
        auto it = m_sounds.find(name);
        if (it != m_sounds.end()) {
            return it->second.get();
        }
        return nullptr;
    }
}
