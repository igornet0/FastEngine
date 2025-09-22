#pragma once

#include "FastEngine/Component.h"
#include <string>
#include <memory>
#include <functional>

namespace FastEngine {
    class Sound;
    
    enum class AudioState {
        Stopped,
        Playing,
        Paused
    };
    
    enum class AudioType {
        Music,      // Музыка (зацикленная)
        SFX,        // Звуковые эффекты
        Voice,      // Голос
        Ambient     // Амбиент
    };
    
    class AudioSource : public Component {
    public:
        AudioSource();
        ~AudioSource() override = default;
        
        // Управление воспроизведением
        void Play();
        void Stop();
        void Pause();
        void Resume();
        
        // Состояние
        AudioState GetState() const { return m_state; }
        bool IsPlaying() const { return m_state == AudioState::Playing; }
        bool IsPaused() const { return m_state == AudioState::Paused; }
        bool IsStopped() const { return m_state == AudioState::Stopped; }
        
        // Загрузка звука
        bool LoadSound(const std::string& filePath);
        void UnloadSound();
        
        // Свойства звука
        void SetVolume(float volume);
        float GetVolume() const { return m_volume; }
        
        void SetPitch(float pitch);
        float GetPitch() const { return m_pitch; }
        
        void SetPan(float pan); // -1.0 (лево) до 1.0 (право)
        float GetPan() const { return m_pan; }
        
        // Зацикливание
        void SetLooping(bool loop) { m_looping = loop; }
        bool IsLooping() const { return m_looping; }
        
        // 3D позиционирование
        void Set3DEnabled(bool enabled) { m_3DEnabled = enabled; }
        bool Is3DEnabled() const { return m_3DEnabled; }
        
        void SetMinDistance(float distance) { m_minDistance = distance; }
        float GetMinDistance() const { return m_minDistance; }
        
        void SetMaxDistance(float distance) { m_maxDistance = distance; }
        float GetMaxDistance() const { return m_maxDistance; }
        
        void SetRolloffFactor(float factor) { m_rolloffFactor = factor; }
        float GetRolloffFactor() const { return m_rolloffFactor; }
        
        // Приоритет (для управления количеством одновременно играющих звуков)
        void SetPriority(int priority) { m_priority = priority; }
        int GetPriority() const { return m_priority; }
        
        // Автоматическое воспроизведение
        void SetAutoPlay(bool autoPlay) { m_autoPlay = autoPlay; }
        bool IsAutoPlay() const { return m_autoPlay; }
        
        // Тип аудио
        void SetAudioType(AudioType type) { m_audioType = type; }
        AudioType GetAudioType() const { return m_audioType; }
        
        // Обновление (вызывается AudioSystem)
        void Update(float deltaTime) override;
        
        // События
        void SetOnPlaybackComplete(std::function<void()> callback);
        void SetOnPlaybackStart(std::function<void()> callback);
        
        // Получение информации о звуке
        std::string GetSoundPath() const { return m_soundPath; }
        bool HasSound() const { return m_sound != nullptr; }
        
        // Fade эффекты
        void FadeIn(float duration);
        void FadeOut(float duration);
        void StopFade();
        
        // Временные эффекты
        void SetTemporaryVolume(float volume, float duration);
        void SetTemporaryPitch(float pitch, float duration);
        
    private:
        std::shared_ptr<Sound> m_sound;
        std::string m_soundPath;
        AudioState m_state;
        AudioType m_audioType;
        
        // Основные свойства
        float m_volume;
        float m_pitch;
        float m_pan;
        bool m_looping;
        
        // 3D свойства
        bool m_3DEnabled;
        float m_minDistance;
        float m_maxDistance;
        float m_rolloffFactor;
        
        // Управление
        int m_priority;
        bool m_autoPlay;
        
        // Fade эффекты
        bool m_fadingIn;
        bool m_fadingOut;
        float m_fadeDuration;
        float m_fadeTimer;
        float m_originalVolume;
        
        // Временные эффекты
        bool m_tempVolumeActive;
        bool m_tempPitchActive;
        float m_tempVolume;
        float m_tempPitch;
        float m_tempDuration;
        float m_tempTimer;
        
        // События
        std::function<void()> m_onPlaybackComplete;
        std::function<void()> m_onPlaybackStart;
        
        // Вспомогательные методы
        void UpdateFade(float deltaTime);
        void UpdateTemporaryEffects(float deltaTime);
        void Apply3DPositioning();
    };
}
