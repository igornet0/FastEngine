#pragma once

#include <string>

namespace FastEngine {
    class Sound {
    public:
        Sound();
        ~Sound();
        
        // Загрузка и создание звука
        bool LoadFromFile(const std::string& filePath);
        void Destroy();
        
        // Воспроизведение
        void Play();
        void Stop();
        void Pause();
        void Resume();
        
        // Настройки
        void SetVolume(float volume);
        float GetVolume() const { return m_volume; }
        
        void SetLooping(bool looping);
        bool IsLooping() const { return m_looping; }
        
        void SetPitch(float pitch);
        float GetPitch() const { return m_pitch; }
        
        // Состояние
        bool IsPlaying() const { return m_playing; }
        bool IsPaused() const { return m_paused; }
        
        // Получение информации
        const std::string& GetFilePath() const { return m_filePath; }
        
    private:
        std::string m_filePath;
        float m_volume;
        bool m_looping;
        float m_pitch;
        bool m_playing;
        bool m_paused;
        bool m_loaded;
        
        // Платформо-специфичные данные
        void* m_audioData;
    };
}
