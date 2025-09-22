#include "FastEngine/Components/AudioSource.h"
#include "FastEngine/Audio/Sound.h"
#include <algorithm>

namespace FastEngine {
    AudioSource::AudioSource() 
        : m_state(AudioState::Stopped)
        , m_audioType(AudioType::SFX)
        , m_volume(1.0f)
        , m_pitch(1.0f)
        , m_pan(0.0f)
        , m_looping(false)
        , m_3DEnabled(false)
        , m_minDistance(1.0f)
        , m_maxDistance(100.0f)
        , m_rolloffFactor(1.0f)
        , m_priority(0)
        , m_autoPlay(false)
        , m_fadingIn(false)
        , m_fadingOut(false)
        , m_fadeDuration(0.0f)
        , m_fadeTimer(0.0f)
        , m_originalVolume(1.0f)
        , m_tempVolumeActive(false)
        , m_tempPitchActive(false)
        , m_tempVolume(1.0f)
        , m_tempPitch(1.0f)
        , m_tempDuration(0.0f)
        , m_tempTimer(0.0f) {
    }
    
    void AudioSource::Play() {
        if (!m_sound) {
            return;
        }
        
        if (m_state == AudioState::Paused) {
            Resume();
            return;
        }
        
        m_state = AudioState::Playing;
        
        // Здесь должен быть вызов m_sound->Play()
        // В реальной реализации это будет зависеть от конкретной аудио системы
        
        if (m_onPlaybackStart) {
            m_onPlaybackStart();
        }
    }
    
    void AudioSource::Stop() {
        if (m_state == AudioState::Stopped) {
            return;
        }
        
        m_state = AudioState::Stopped;
        StopFade();
        
        // Здесь должен быть вызов m_sound->Stop()
    }
    
    void AudioSource::Pause() {
        if (m_state != AudioState::Playing) {
            return;
        }
        
        m_state = AudioState::Paused;
        
        // Здесь должен быть вызов m_sound->Pause()
    }
    
    void AudioSource::Resume() {
        if (m_state != AudioState::Paused) {
            return;
        }
        
        m_state = AudioState::Playing;
        
        // Здесь должен быть вызов m_sound->Resume()
    }
    
    bool AudioSource::LoadSound(const std::string& filePath) {
        // В реальной реализации здесь бы создавался Sound объект
        // Для простоты просто сохраняем путь
        m_soundPath = filePath;
        m_sound = std::make_shared<Sound>();
        
        // Здесь должен быть вызов m_sound->LoadFromFile(filePath)
        
        if (m_autoPlay) {
            Play();
        }
        
        return true;
    }
    
    void AudioSource::UnloadSound() {
        Stop();
        m_sound.reset();
        m_soundPath.clear();
    }
    
    void AudioSource::SetVolume(float volume) {
        m_volume = std::clamp(volume, 0.0f, 1.0f);
        
        if (m_sound) {
            // Здесь должен быть вызов m_sound->SetVolume(m_volume)
        }
    }
    
    void AudioSource::SetPitch(float pitch) {
        m_pitch = std::max(0.1f, pitch);
        
        if (m_sound) {
            // Здесь должен быть вызов m_sound->SetPitch(m_pitch)
        }
    }
    
    void AudioSource::SetPan(float pan) {
        m_pan = std::clamp(pan, -1.0f, 1.0f);
        
        if (m_sound) {
            // Здесь должен быть вызов m_sound->SetPan(m_pan)
        }
    }
    
    void AudioSource::Update(float deltaTime) {
        if (m_state != AudioState::Playing) {
            return;
        }
        
        UpdateFade(deltaTime);
        UpdateTemporaryEffects(deltaTime);
        Apply3DPositioning();
        
        // Проверяем, завершилось ли воспроизведение
        // В реальной реализации это проверялось бы через аудио систему
        // if (m_sound && m_sound->IsFinished()) {
        //     if (m_looping) {
        //         Play();
        //     } else {
        //         Stop();
        //         if (m_onPlaybackComplete) {
        //             m_onPlaybackComplete();
        //         }
        //     }
        // }
    }
    
    void AudioSource::SetOnPlaybackComplete(std::function<void()> callback) {
        m_onPlaybackComplete = callback;
    }
    
    void AudioSource::SetOnPlaybackStart(std::function<void()> callback) {
        m_onPlaybackStart = callback;
    }
    
    void AudioSource::FadeIn(float duration) {
        if (duration <= 0.0f) {
            return;
        }
        
        m_fadingIn = true;
        m_fadingOut = false;
        m_fadeDuration = duration;
        m_fadeTimer = 0.0f;
        m_originalVolume = m_volume;
        m_volume = 0.0f;
        
        if (m_sound) {
            // m_sound->SetVolume(0.0f);
        }
    }
    
    void AudioSource::FadeOut(float duration) {
        if (duration <= 0.0f) {
            Stop();
            return;
        }
        
        m_fadingOut = true;
        m_fadingIn = false;
        m_fadeDuration = duration;
        m_fadeTimer = 0.0f;
        m_originalVolume = m_volume;
    }
    
    void AudioSource::StopFade() {
        m_fadingIn = false;
        m_fadingOut = false;
        m_fadeTimer = 0.0f;
    }
    
    void AudioSource::SetTemporaryVolume(float volume, float duration) {
        m_tempVolume = std::clamp(volume, 0.0f, 1.0f);
        m_tempDuration = duration;
        m_tempTimer = 0.0f;
        m_tempVolumeActive = true;
    }
    
    void AudioSource::SetTemporaryPitch(float pitch, float duration) {
        m_tempPitch = std::max(0.1f, pitch);
        m_tempDuration = duration;
        m_tempTimer = 0.0f;
        m_tempPitchActive = true;
    }
    
    void AudioSource::UpdateFade(float deltaTime) {
        if (!m_fadingIn && !m_fadingOut) {
            return;
        }
        
        m_fadeTimer += deltaTime;
        float progress = m_fadeTimer / m_fadeDuration;
        
        if (progress >= 1.0f) {
            progress = 1.0f;
            m_fadingIn = false;
            m_fadingOut = false;
        }
        
        if (m_fadingIn) {
            m_volume = m_originalVolume * progress;
        } else if (m_fadingOut) {
            m_volume = m_originalVolume * (1.0f - progress);
            if (progress >= 1.0f) {
                Stop();
            }
        }
        
        if (m_sound) {
            // m_sound->SetVolume(m_volume);
        }
    }
    
    void AudioSource::UpdateTemporaryEffects(float deltaTime) {
        if (m_tempVolumeActive) {
            m_tempTimer += deltaTime;
            if (m_tempTimer >= m_tempDuration) {
                m_tempVolumeActive = false;
                m_tempTimer = 0.0f;
                // Восстанавливаем оригинальный volume
                if (m_sound) {
                    // m_sound->SetVolume(m_volume);
                }
            } else {
                // Применяем временный volume
                if (m_sound) {
                    // m_sound->SetVolume(m_tempVolume);
                }
            }
        }
        
        if (m_tempPitchActive) {
            m_tempTimer += deltaTime;
            if (m_tempTimer >= m_tempDuration) {
                m_tempPitchActive = false;
                m_tempTimer = 0.0f;
                // Восстанавливаем оригинальный pitch
                if (m_sound) {
                    // m_sound->SetPitch(m_pitch);
                }
            } else {
                // Применяем временный pitch
                if (m_sound) {
                    // m_sound->SetPitch(m_tempPitch);
                }
            }
        }
    }
    
    void AudioSource::Apply3DPositioning() {
        if (!m_3DEnabled || !m_sound) {
            return;
        }
        
        // В реальной реализации здесь бы вычислялись 3D параметры
        // на основе позиции объекта, слушателя и настроек дистанции
        
        // Получаем позицию объекта (через Transform компонент)
        // glm::vec3 position = GetEntity()->GetComponent<Transform>()->GetPosition();
        
        // Вычисляем расстояние до слушателя
        // float distance = glm::length(position - listenerPosition);
        
        // Применяем rolloff
        // float volumeMultiplier = CalculateVolumeRolloff(distance);
        // m_sound->SetVolume(m_volume * volumeMultiplier);
    }
}

