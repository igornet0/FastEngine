#include "FastEngine/Cinematic/CinematicEditor.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace FastEngine {

// TimelineEvent implementation
TimelineEvent::TimelineEvent() 
    : m_time(0.0f)
    , m_duration(1.0f)
    , m_type(TimelineEventType::Custom) {
}

void TimelineEvent::SetData(const std::string& key, const std::string& value) {
    m_data[key] = value;
}

std::string TimelineEvent::GetData(const std::string& key) const {
    auto it = m_data.find(key);
    return (it != m_data.end()) ? it->second : "";
}

bool TimelineEvent::HasData(const std::string& key) const {
    return m_data.find(key) != m_data.end();
}

std::string TimelineEvent::Serialize() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"time\": " << m_time << ",\n";
    ss << "  \"duration\": " << m_duration << ",\n";
    ss << "  \"type\": " << static_cast<int>(m_type) << ",\n";
    ss << "  \"name\": \"" << m_name << "\"\n";
    ss << "}";
    return ss.str();
}

void TimelineEvent::Deserialize(const std::string& data) {
    std::cout << "Deserializing timeline event: " << m_name << std::endl;
}

// CameraMoveEvent implementation
CameraMoveEvent::CameraMoveEvent() 
    : m_startPosition(0.0f)
    , m_endPosition(0.0f)
    , m_easingType("linear")
    , m_started(false) {
    m_type = TimelineEventType::CameraMove;
    m_name = "CameraMove";
}

void CameraMoveEvent::Execute(float time, float deltaTime) {
    if (!m_started) {
        m_started = true;
        std::cout << "CameraMoveEvent: Started moving camera" << std::endl;
    }
    
    // В реальной реализации здесь бы обновлялась позиция камеры
    std::cout << "CameraMoveEvent: Moving camera at time " << time << std::endl;
}

// CameraRotateEvent implementation
CameraRotateEvent::CameraRotateEvent() 
    : m_startRotation(0.0f)
    , m_endRotation(0.0f)
    , m_easingType("linear")
    , m_started(false) {
    m_type = TimelineEventType::CameraRotate;
    m_name = "CameraRotate";
}

void CameraRotateEvent::Execute(float time, float deltaTime) {
    if (!m_started) {
        m_started = true;
        std::cout << "CameraRotateEvent: Started rotating camera" << std::endl;
    }
    
    // В реальной реализации здесь бы обновлялся поворот камеры
    std::cout << "CameraRotateEvent: Rotating camera at time " << time << std::endl;
}

// AudioPlayEvent implementation
AudioPlayEvent::AudioPlayEvent() 
    : m_volume(1.0f)
    , m_loop(false)
    , m_playing(false) {
    m_type = TimelineEventType::AudioPlay;
    m_name = "AudioPlay";
}

void AudioPlayEvent::Execute(float time, float deltaTime) {
    if (!m_playing) {
        m_playing = true;
        std::cout << "AudioPlayEvent: Playing audio: " << m_audioFile << std::endl;
    }
}

void AudioPlayEvent::Start() {
    std::cout << "AudioPlayEvent: Starting audio playback" << std::endl;
}

void AudioPlayEvent::End() {
    std::cout << "AudioPlayEvent: Ending audio playback" << std::endl;
}

// TextDisplayEvent implementation
TextDisplayEvent::TextDisplayEvent() 
    : m_position(0.0f)
    , m_fadeIn(0.5f)
    , m_fadeOut(0.5f)
    , m_visible(false) {
    m_type = TimelineEventType::TextDisplay;
    m_name = "TextDisplay";
}

void TextDisplayEvent::Execute(float time, float deltaTime) {
    if (!m_visible) {
        m_visible = true;
        std::cout << "TextDisplayEvent: Displaying text: " << m_text << std::endl;
    }
}

void TextDisplayEvent::Start() {
    std::cout << "TextDisplayEvent: Starting text display" << std::endl;
}

void TextDisplayEvent::End() {
    std::cout << "TextDisplayEvent: Ending text display" << std::endl;
}

// Timeline implementation
Timeline::Timeline() 
    : m_currentTime(0.0f)
    , m_duration(0.0f)
    , m_playing(false)
    , m_paused(false) {
}

void Timeline::AddEvent(std::shared_ptr<TimelineEvent> event) {
    if (event) {
        m_events.push_back(event);
        UpdateDuration();
        SortEvents();
        std::cout << "Timeline: Added event: " << event->GetName() << std::endl;
    }
}

void Timeline::RemoveEvent(std::shared_ptr<TimelineEvent> event) {
    if (event) {
        m_events.erase(
            std::remove(m_events.begin(), m_events.end(), event),
            m_events.end()
        );
        UpdateDuration();
        std::cout << "Timeline: Removed event: " << event->GetName() << std::endl;
    }
}

void Timeline::ClearEvents() {
    m_events.clear();
    m_duration = 0.0f;
    std::cout << "Timeline: Cleared all events" << std::endl;
}

void Timeline::Play() {
    m_playing = true;
    m_paused = false;
    std::cout << "Timeline: Started playing" << std::endl;
}

void Timeline::Pause() {
    m_paused = true;
    std::cout << "Timeline: Paused" << std::endl;
}

void Timeline::Stop() {
    m_playing = false;
    m_paused = false;
    m_currentTime = 0.0f;
    std::cout << "Timeline: Stopped" << std::endl;
}

void Timeline::Seek(float time) {
    m_currentTime = time;
    std::cout << "Timeline: Seeked to time " << time << std::endl;
}

void Timeline::Update(float deltaTime) {
    if (!m_playing || m_paused) {
        return;
    }
    
    m_currentTime += deltaTime;
    
    // Выполняем события в текущем времени
    auto eventsAtTime = GetEventsAtTime(m_currentTime);
    for (auto& event : eventsAtTime) {
        event->Execute(m_currentTime, deltaTime);
    }
    
    // Проверяем, достигли ли конца
    if (m_currentTime >= m_duration) {
        Stop();
    }
}

std::vector<std::shared_ptr<TimelineEvent>> Timeline::GetEventsAtTime(float time) const {
    std::vector<std::shared_ptr<TimelineEvent>> result;
    
    for (const auto& event : m_events) {
        if (time >= event->GetTime() && time <= event->GetTime() + event->GetDuration()) {
            result.push_back(event);
        }
    }
    
    return result;
}

std::string Timeline::Serialize() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"currentTime\": " << m_currentTime << ",\n";
    ss << "  \"duration\": " << m_duration << ",\n";
    ss << "  \"playing\": " << (m_playing ? "true" : "false") << ",\n";
    ss << "  \"paused\": " << (m_paused ? "true" : "false") << ",\n";
    ss << "  \"events\": [\n";
    
    for (size_t i = 0; i < m_events.size(); ++i) {
        ss << "    " << m_events[i]->Serialize();
        if (i < m_events.size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "  ]\n";
    ss << "}";
    return ss.str();
}

void Timeline::Deserialize(const std::string& data) {
    std::cout << "Deserializing timeline..." << std::endl;
}

void Timeline::UpdateDuration() {
    m_duration = 0.0f;
    for (const auto& event : m_events) {
        float eventEnd = event->GetTime() + event->GetDuration();
        if (eventEnd > m_duration) {
            m_duration = eventEnd;
        }
    }
}

void Timeline::SortEvents() {
    std::sort(m_events.begin(), m_events.end(),
        [](const std::shared_ptr<TimelineEvent>& a, const std::shared_ptr<TimelineEvent>& b) {
            return a->GetTime() < b->GetTime();
        });
}

// Cutscene implementation
Cutscene::Cutscene() 
    : m_currentTime(0.0f)
    , m_duration(0.0f)
    , m_playing(false)
    , m_paused(false) {
}

void Cutscene::AddTimeline(std::shared_ptr<Timeline> timeline) {
    if (timeline) {
        m_timelines.push_back(timeline);
        UpdateDuration();
        std::cout << "Cutscene: Added timeline" << std::endl;
    }
}

void Cutscene::RemoveTimeline(std::shared_ptr<Timeline> timeline) {
    if (timeline) {
        m_timelines.erase(
            std::remove(m_timelines.begin(), m_timelines.end(), timeline),
            m_timelines.end()
        );
        UpdateDuration();
        std::cout << "Cutscene: Removed timeline" << std::endl;
    }
}

void Cutscene::ClearTimelines() {
    m_timelines.clear();
    m_duration = 0.0f;
    std::cout << "Cutscene: Cleared all timelines" << std::endl;
}

void Cutscene::Play() {
    m_playing = true;
    m_paused = false;
    for (auto& timeline : m_timelines) {
        timeline->Play();
    }
    std::cout << "Cutscene: Started playing" << std::endl;
}

void Cutscene::Pause() {
    m_paused = true;
    for (auto& timeline : m_timelines) {
        timeline->Pause();
    }
    std::cout << "Cutscene: Paused" << std::endl;
}

void Cutscene::Stop() {
    m_playing = false;
    m_paused = false;
    m_currentTime = 0.0f;
    for (auto& timeline : m_timelines) {
        timeline->Stop();
    }
    std::cout << "Cutscene: Stopped" << std::endl;
}

void Cutscene::Seek(float time) {
    m_currentTime = time;
    for (auto& timeline : m_timelines) {
        timeline->Seek(time);
    }
    std::cout << "Cutscene: Seeked to time " << time << std::endl;
}

void Cutscene::Update(float deltaTime) {
    if (!m_playing || m_paused) {
        return;
    }
    
    m_currentTime += deltaTime;
    
    for (auto& timeline : m_timelines) {
        timeline->Update(deltaTime);
    }
    
    if (m_currentTime >= m_duration) {
        Stop();
    }
}

bool Cutscene::IsPlaying() const {
    return m_playing && !m_paused;
}

bool Cutscene::IsPaused() const {
    return m_paused;
}

float Cutscene::GetCurrentTime() const {
    return m_currentTime;
}

float Cutscene::GetDuration() const {
    return m_duration;
}

std::string Cutscene::Serialize() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"currentTime\": " << m_currentTime << ",\n";
    ss << "  \"duration\": " << m_duration << ",\n";
    ss << "  \"playing\": " << (m_playing ? "true" : "false") << ",\n";
    ss << "  \"paused\": " << (m_paused ? "true" : "false") << ",\n";
    ss << "  \"timelines\": [\n";
    
    for (size_t i = 0; i < m_timelines.size(); ++i) {
        ss << "    " << m_timelines[i]->Serialize();
        if (i < m_timelines.size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "  ]\n";
    ss << "}";
    return ss.str();
}

void Cutscene::Deserialize(const std::string& data) {
    std::cout << "Deserializing cutscene..." << std::endl;
}

void Cutscene::UpdateDuration() {
    m_duration = 0.0f;
    for (const auto& timeline : m_timelines) {
        if (timeline->GetDuration() > m_duration) {
            m_duration = timeline->GetDuration();
        }
    }
}

// CinematicEditor implementation
CinematicEditor::CinematicEditor() 
    : m_initialized(false) {
}

bool CinematicEditor::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    m_initialized = true;
    std::cout << "CinematicEditor initialized successfully" << std::endl;
    return true;
}

void CinematicEditor::Shutdown() {
    m_cutscenes.clear();
    m_timelines.clear();
    m_initialized = false;
    std::cout << "CinematicEditor shutdown" << std::endl;
}

std::shared_ptr<Cutscene> CinematicEditor::CreateCutscene(const std::string& name) {
    auto cutscene = std::make_shared<Cutscene>();
    m_cutscenes[name] = cutscene;
    std::cout << "Created cutscene: " << name << std::endl;
    return cutscene;
}

void CinematicEditor::RemoveCutscene(const std::string& name) {
    auto it = m_cutscenes.find(name);
    if (it != m_cutscenes.end()) {
        m_cutscenes.erase(it);
        std::cout << "Removed cutscene: " << name << std::endl;
    }
}

std::shared_ptr<Cutscene> CinematicEditor::GetCutscene(const std::string& name) const {
    auto it = m_cutscenes.find(name);
    return (it != m_cutscenes.end()) ? it->second : nullptr;
}

std::shared_ptr<Timeline> CinematicEditor::CreateTimeline(const std::string& name) {
    auto timeline = std::make_shared<Timeline>();
    m_timelines[name] = timeline;
    std::cout << "Created timeline: " << name << std::endl;
    return timeline;
}

void CinematicEditor::RemoveTimeline(const std::string& name) {
    auto it = m_timelines.find(name);
    if (it != m_timelines.end()) {
        m_timelines.erase(it);
        std::cout << "Removed timeline: " << name << std::endl;
    }
}

std::shared_ptr<Timeline> CinematicEditor::GetTimeline(const std::string& name) const {
    auto it = m_timelines.find(name);
    return (it != m_timelines.end()) ? it->second : nullptr;
}

std::shared_ptr<CameraMoveEvent> CinematicEditor::CreateCameraMoveEvent(float time, float duration) {
    auto event = std::make_shared<CameraMoveEvent>();
    event->SetTime(time);
    event->SetDuration(duration);
    return event;
}

std::shared_ptr<CameraRotateEvent> CinematicEditor::CreateCameraRotateEvent(float time, float duration) {
    auto event = std::make_shared<CameraRotateEvent>();
    event->SetTime(time);
    event->SetDuration(duration);
    return event;
}

std::shared_ptr<AudioPlayEvent> CinematicEditor::CreateAudioPlayEvent(float time, float duration) {
    auto event = std::make_shared<AudioPlayEvent>();
    event->SetTime(time);
    event->SetDuration(duration);
    return event;
}

std::shared_ptr<TextDisplayEvent> CinematicEditor::CreateTextDisplayEvent(float time, float duration) {
    auto event = std::make_shared<TextDisplayEvent>();
    event->SetTime(time);
    event->SetDuration(duration);
    return event;
}

void CinematicEditor::PlayCutscene(const std::string& name) {
    auto cutscene = GetCutscene(name);
    if (cutscene) {
        cutscene->Play();
        m_currentCutscene = name;
        std::cout << "Playing cutscene: " << name << std::endl;
    }
}

void CinematicEditor::PauseCutscene() {
    if (!m_currentCutscene.empty()) {
        auto cutscene = GetCutscene(m_currentCutscene);
        if (cutscene) {
            cutscene->Pause();
            std::cout << "Paused cutscene: " << m_currentCutscene << std::endl;
        }
    }
}

void CinematicEditor::StopCutscene() {
    if (!m_currentCutscene.empty()) {
        auto cutscene = GetCutscene(m_currentCutscene);
        if (cutscene) {
            cutscene->Stop();
            std::cout << "Stopped cutscene: " << m_currentCutscene << std::endl;
        }
        m_currentCutscene.clear();
    }
}

void CinematicEditor::SeekCutscene(float time) {
    if (!m_currentCutscene.empty()) {
        auto cutscene = GetCutscene(m_currentCutscene);
        if (cutscene) {
            cutscene->Seek(time);
            std::cout << "Seeked cutscene to time: " << time << std::endl;
        }
    }
}

void CinematicEditor::Update(float deltaTime) {
    if (!m_initialized) {
        return;
    }
    
    // Обновляем все cutscenes
    for (auto& pair : m_cutscenes) {
        pair.second->Update(deltaTime);
    }
    
    // Обновляем все timelines
    for (auto& pair : m_timelines) {
        pair.second->Update(deltaTime);
    }
}

bool CinematicEditor::IsPlaying() const {
    if (!m_currentCutscene.empty()) {
        auto cutscene = GetCutscene(m_currentCutscene);
        return cutscene && cutscene->IsPlaying();
    }
    return false;
}

} // namespace FastEngine

