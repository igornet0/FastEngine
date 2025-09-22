#include "FastEngine/Audio/Sound.h"
#include "FastEngine/Platform/Platform.h"
#include <algorithm>

namespace FastEngine {
    Sound::Sound() 
        : m_volume(1.0f)
        , m_looping(false)
        , m_pitch(1.0f)
        , m_playing(false)
        , m_paused(false)
        , m_loaded(false)
        , m_audioData(nullptr) {
    }
    
    Sound::~Sound() {
        Destroy();
    }
    
    bool Sound::LoadFromFile(const std::string& filePath) {
        if (m_loaded) {
            Destroy();
        }
        
        m_filePath = filePath;
        
        // На мобильных платформах загрузка звука зависит от конкретной реализации
        // Здесь создаем заглушку для демонстрации
        
        // В реальном приложении здесь должна быть загрузка через:
        // - iOS: AVAudioPlayer или AudioToolbox
        // - Android: MediaPlayer или SoundPool
        // - Desktop: SDL_mixer или OpenAL
        
        m_loaded = true;
        return true;
    }
    
    void Sound::Destroy() {
        if (m_loaded) {
            Stop();
            
            // Освобождаем платформо-специфичные данные
            if (m_audioData) {
                // Здесь должно быть освобождение ресурсов
                m_audioData = nullptr;
            }
            
            m_loaded = false;
        }
    }
    
    void Sound::Play() {
        if (!m_loaded || m_playing) {
            return;
        }
        
        // В реальном приложении здесь должно быть воспроизведение звука
        // через платформо-специфичные API
        
        m_playing = true;
        m_paused = false;
    }
    
    void Sound::Stop() {
        if (!m_loaded || !m_playing) {
            return;
        }
        
        // В реальном приложении здесь должно быть остановка звука
        // через платформо-специфичные API
        
        m_playing = false;
        m_paused = false;
    }
    
    void Sound::Pause() {
        if (!m_loaded || !m_playing || m_paused) {
            return;
        }
        
        // В реальном приложении здесь должно быть приостановка звука
        // через платформо-специфичные API
        
        m_paused = true;
    }
    
    void Sound::Resume() {
        if (!m_loaded || !m_playing || !m_paused) {
            return;
        }
        
        // В реальном приложении здесь должно быть возобновление звука
        // через платформо-специфичные API
        
        m_paused = false;
    }
    
    void Sound::SetVolume(float volume) {
        m_volume = std::clamp(volume, 0.0f, 1.0f);
        
        // В реальном приложении здесь должно быть установка громкости
        // через платформо-специфичные API
    }
    
    void Sound::SetLooping(bool looping) {
        m_looping = looping;
        
        // В реальном приложении здесь должно быть установка зацикливания
        // через платформо-специфичные API
    }
    
    void Sound::SetPitch(float pitch) {
        m_pitch = std::clamp(pitch, 0.1f, 2.0f);
        
        // В реальном приложении здесь должно быть установка высоты тона
        // через платформо-специфичные API
    }
}
