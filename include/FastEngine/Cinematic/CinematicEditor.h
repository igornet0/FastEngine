#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include <unordered_map>

namespace FastEngine {

/**
 * Типы событий на таймлайне
 */
enum class TimelineEventType {
    CameraMove,
    CameraRotate,
    CameraZoom,
    ObjectMove,
    ObjectRotate,
    ObjectScale,
    AudioPlay,
    AudioStop,
    TextDisplay,
    Custom
};

/**
 * Событие на таймлайне
 */
class TimelineEvent {
public:
    TimelineEvent();
    ~TimelineEvent() = default;
    
    // Свойства события
    float GetTime() const { return m_time; }
    void SetTime(float time) { m_time = time; }
    
    float GetDuration() const { return m_duration; }
    void SetDuration(float duration) { m_duration = duration; }
    
    TimelineEventType GetType() const { return m_type; }
    void SetType(TimelineEventType type) { m_type = type; }
    
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    // Данные события
    void SetData(const std::string& key, const std::string& value);
    std::string GetData(const std::string& key) const;
    bool HasData(const std::string& key) const;
    
    // Выполнение
    virtual void Execute(float time, float deltaTime) = 0;
    virtual void Start() {}
    virtual void End() {}
    
    // Сериализация
    virtual std::string Serialize() const;
    virtual void Deserialize(const std::string& data);
    
protected:
    float m_time;
    float m_duration;
    TimelineEventType m_type;
    std::string m_name;
    std::unordered_map<std::string, std::string> m_data;
};

/**
 * Событие движения камеры
 */
class CameraMoveEvent : public TimelineEvent {
public:
    CameraMoveEvent();
    ~CameraMoveEvent() = default;
    
    void Execute(float time, float deltaTime) override;
    
    void SetStartPosition(const glm::vec3& position) { m_startPosition = position; }
    void SetEndPosition(const glm::vec3& position) { m_endPosition = position; }
    void SetEasingType(const std::string& type) { m_easingType = type; }
    
private:
    glm::vec3 m_startPosition;
    glm::vec3 m_endPosition;
    std::string m_easingType;
    bool m_started;
};

/**
 * Событие поворота камеры
 */
class CameraRotateEvent : public TimelineEvent {
public:
    CameraRotateEvent();
    ~CameraRotateEvent() = default;
    
    void Execute(float time, float deltaTime) override;
    
    void SetStartRotation(const glm::vec3& rotation) { m_startRotation = rotation; }
    void SetEndRotation(const glm::vec3& rotation) { m_endRotation = rotation; }
    void SetEasingType(const std::string& type) { m_easingType = type; }
    
private:
    glm::vec3 m_startRotation;
    glm::vec3 m_endRotation;
    std::string m_easingType;
    bool m_started;
};

/**
 * Событие воспроизведения аудио
 */
class AudioPlayEvent : public TimelineEvent {
public:
    AudioPlayEvent();
    ~AudioPlayEvent() = default;
    
    void Execute(float time, float deltaTime) override;
    void Start() override;
    void End() override;
    
    void SetAudioFile(const std::string& file) { m_audioFile = file; }
    void SetVolume(float volume) { m_volume = volume; }
    void SetLoop(bool loop) { m_loop = loop; }
    
private:
    std::string m_audioFile;
    float m_volume;
    bool m_loop;
    bool m_playing;
};

/**
 * Событие отображения текста
 */
class TextDisplayEvent : public TimelineEvent {
public:
    TextDisplayEvent();
    ~TextDisplayEvent() = default;
    
    void Execute(float time, float deltaTime) override;
    void Start() override;
    void End() override;
    
    void SetText(const std::string& text) { m_text = text; }
    void SetPosition(const glm::vec2& position) { m_position = position; }
    void SetDuration(float duration) { m_duration = duration; }
    void SetFadeIn(float fadeIn) { m_fadeIn = fadeIn; }
    void SetFadeOut(float fadeOut) { m_fadeOut = fadeOut; }
    
private:
    std::string m_text;
    glm::vec2 m_position;
    float m_fadeIn;
    float m_fadeOut;
    bool m_visible;
};

/**
 * Таймлайн
 */
class Timeline {
public:
    Timeline();
    ~Timeline() = default;
    
    // Управление событиями
    void AddEvent(std::shared_ptr<TimelineEvent> event);
    void RemoveEvent(std::shared_ptr<TimelineEvent> event);
    void ClearEvents();
    
    // Воспроизведение
    void Play();
    void Pause();
    void Stop();
    void Seek(float time);
    
    // Обновление
    void Update(float deltaTime);
    
    // Состояние
    bool IsPlaying() const { return m_playing; }
    bool IsPaused() const { return m_paused; }
    float GetCurrentTime() const { return m_currentTime; }
    float GetDuration() const { return m_duration; }
    
    // Получение событий
    const std::vector<std::shared_ptr<TimelineEvent>>& GetEvents() const { return m_events; }
    std::vector<std::shared_ptr<TimelineEvent>> GetEventsAtTime(float time) const;
    
    // Сериализация
    std::string Serialize() const;
    void Deserialize(const std::string& data);
    
private:
    std::vector<std::shared_ptr<TimelineEvent>> m_events;
    float m_currentTime;
    float m_duration;
    bool m_playing;
    bool m_paused;
    
    void UpdateDuration();
    void SortEvents();
};

/**
 * Cutscene
 */
class Cutscene {
public:
    Cutscene();
    ~Cutscene() = default;
    
    // Управление
    void AddTimeline(std::shared_ptr<Timeline> timeline);
    void RemoveTimeline(std::shared_ptr<Timeline> timeline);
    void ClearTimelines();
    
    // Воспроизведение
    void Play();
    void Pause();
    void Stop();
    void Seek(float time);
    
    // Обновление
    void Update(float deltaTime);
    
    // Состояние
    bool IsPlaying() const;
    bool IsPaused() const;
    float GetCurrentTime() const;
    float GetDuration() const;
    
    // Получение таймлайнов
    const std::vector<std::shared_ptr<Timeline>>& GetTimelines() const { return m_timelines; }
    
    // Сериализация
    std::string Serialize() const;
    void Deserialize(const std::string& data);
    
private:
    std::vector<std::shared_ptr<Timeline>> m_timelines;
    float m_currentTime;
    float m_duration;
    bool m_playing;
    bool m_paused;
    
    void UpdateDuration();
};

/**
 * Редактор кинематики
 */
class CinematicEditor {
public:
    CinematicEditor();
    ~CinematicEditor() = default;
    
    // Инициализация
    bool Initialize();
    void Shutdown();
    
    // Управление cutscene
    std::shared_ptr<Cutscene> CreateCutscene(const std::string& name);
    void RemoveCutscene(const std::string& name);
    std::shared_ptr<Cutscene> GetCutscene(const std::string& name) const;
    
    // Управление таймлайнами
    std::shared_ptr<Timeline> CreateTimeline(const std::string& name);
    void RemoveTimeline(const std::string& name);
    std::shared_ptr<Timeline> GetTimeline(const std::string& name) const;
    
    // Создание событий
    std::shared_ptr<CameraMoveEvent> CreateCameraMoveEvent(float time, float duration);
    std::shared_ptr<CameraRotateEvent> CreateCameraRotateEvent(float time, float duration);
    std::shared_ptr<AudioPlayEvent> CreateAudioPlayEvent(float time, float duration);
    std::shared_ptr<TextDisplayEvent> CreateTextDisplayEvent(float time, float duration);
    
    // Воспроизведение
    void PlayCutscene(const std::string& name);
    void PauseCutscene();
    void StopCutscene();
    void SeekCutscene(float time);
    
    // Обновление
    void Update(float deltaTime);
    
    // Получение информации
    const std::string& GetCurrentCutscene() const { return m_currentCutscene; }
    bool IsPlaying() const;
    
private:
    std::unordered_map<std::string, std::shared_ptr<Cutscene>> m_cutscenes;
    std::unordered_map<std::string, std::shared_ptr<Timeline>> m_timelines;
    std::string m_currentCutscene;
    bool m_initialized;
    
    // Callbacks
    std::function<void()> m_onCutsceneComplete;
    std::function<void()> m_onCutscenePaused;
    std::function<void()> m_onCutsceneStopped;
    
public:
    void SetOnCutsceneComplete(std::function<void()> callback) { m_onCutsceneComplete = callback; }
    void SetOnCutscenePaused(std::function<void()> callback) { m_onCutscenePaused = callback; }
    void SetOnCutsceneStopped(std::function<void()> callback) { m_onCutsceneStopped = callback; }
};

} // namespace FastEngine

