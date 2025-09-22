#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace FastEngine {
    class Sound;
    
    class AudioManager {
    public:
        AudioManager();
        ~AudioManager();
        
        // Инициализация и завершение работы
        bool Initialize();
        void Shutdown();
        
        // Управление звуками
        Sound* LoadSound(const std::string& filePath);
        void UnloadSound(const std::string& filePath);
        void UnloadAllSounds();
        
        // Глобальные настройки
        void SetMasterVolume(float volume);
        float GetMasterVolume() const { return m_masterVolume; }
        
        void SetMuted(bool muted);
        bool IsMuted() const { return m_muted; }
        
        // Получение звука по имени
        Sound* GetSound(const std::string& name) const;
        
    private:
        std::unordered_map<std::string, std::unique_ptr<Sound>> m_sounds;
        float m_masterVolume;
        bool m_muted;
        bool m_initialized;
    };
}
