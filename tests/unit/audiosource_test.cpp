#include <gtest/gtest.h>
#include "FastEngine/Components/AudioSource.h"
#include "FastEngine/Entity.h"
#include "FastEngine/World.h"
#include <memory>

using namespace FastEngine;

class AudioSourceTest : public ::testing::Test {
protected:
    void SetUp() override {
        world = std::make_unique<World>();
        world->Initialize();
        entity = world->CreateEntity();
        audioSource = entity->AddComponent<AudioSource>();
    }
    
    void TearDown() override {
        audioSource = nullptr;
        entity = nullptr;
        world.reset();
    }
    
    std::unique_ptr<World> world;
    std::shared_ptr<Entity> entity;
    std::shared_ptr<AudioSource> audioSource;
};

TEST_F(AudioSourceTest, AudioSourceCreation) {
    EXPECT_TRUE(audioSource != nullptr);
    EXPECT_TRUE(entity->HasComponent<AudioSource>());
}

TEST_F(AudioSourceTest, AudioClipLoading) {
    // Тест загрузки аудио клипа
    std::string clipPath = "test_sound.wav";
    audioSource->SetAudioClip(clipPath);
    
    EXPECT_EQ(audioSource->GetAudioClip(), clipPath);
}

TEST_F(AudioSourceTest, PlaybackControl) {
    // Тест управления воспроизведением
    audioSource->SetAudioClip("test_sound.wav");
    
    // Тест воспроизведения
    audioSource->Play();
    EXPECT_TRUE(audioSource->IsPlaying());
    
    // Тест паузы
    audioSource->Pause();
    EXPECT_TRUE(audioSource->IsPaused());
    EXPECT_TRUE(audioSource->IsPlaying());
    
    // Тест возобновления
    audioSource->Resume();
    EXPECT_FALSE(audioSource->IsPaused());
    EXPECT_TRUE(audioSource->IsPlaying());
    
    // Тест остановки
    audioSource->Stop();
    EXPECT_FALSE(audioSource->IsPlaying());
    EXPECT_FALSE(audioSource->IsPaused());
}

TEST_F(AudioSourceTest, VolumeControl) {
    // Тест управления громкостью
    audioSource->SetVolume(0.5f);
    EXPECT_FLOAT_EQ(audioSource->GetVolume(), 0.5f);
    
    audioSource->SetVolume(1.0f);
    EXPECT_FLOAT_EQ(audioSource->GetVolume(), 1.0f);
    
    audioSource->SetVolume(0.0f);
    EXPECT_FLOAT_EQ(audioSource->GetVolume(), 0.0f);
}

TEST_F(AudioSourceTest, PitchControl) {
    // Тест управления высотой тона
    audioSource->SetPitch(1.5f);
    EXPECT_FLOAT_EQ(audioSource->GetPitch(), 1.5f);
    
    audioSource->SetPitch(0.5f);
    EXPECT_FLOAT_EQ(audioSource->GetPitch(), 0.5f);
    
    audioSource->SetPitch(1.0f);
    EXPECT_FLOAT_EQ(audioSource->GetPitch(), 1.0f);
}

TEST_F(AudioSourceTest, LoopControl) {
    // Тест управления зацикливанием
    audioSource->SetLoop(true);
    EXPECT_TRUE(audioSource->IsLooping());
    
    audioSource->SetLoop(false);
    EXPECT_FALSE(audioSource->IsLooping());
}

TEST_F(AudioSourceTest, MuteControl) {
    // Тест управления отключением звука
    audioSource->SetMute(true);
    EXPECT_TRUE(audioSource->IsMuted());
    
    audioSource->SetMute(false);
    EXPECT_FALSE(audioSource->IsMuted());
}

TEST_F(AudioSourceTest, SpatialAudio) {
    // Тест пространственного аудио
    audioSource->SetSpatialAudio(true);
    EXPECT_TRUE(audioSource->IsSpatialAudio());
    
    audioSource->SetSpatialAudio(false);
    EXPECT_FALSE(audioSource->IsSpatialAudio());
}

TEST_F(AudioSourceTest, AudioRange) {
    // Тест диапазона слышимости
    audioSource->SetMinDistance(10.0f);
    audioSource->SetMaxDistance(100.0f);
    
    EXPECT_FLOAT_EQ(audioSource->GetMinDistance(), 10.0f);
    EXPECT_FLOAT_EQ(audioSource->GetMaxDistance(), 100.0f);
}

TEST_F(AudioSourceTest, RolloffFactor) {
    // Тест фактора затухания
    audioSource->SetRolloffFactor(2.0f);
    EXPECT_FLOAT_EQ(audioSource->GetRolloffFactor(), 2.0f);
}

TEST_F(AudioSourceTest, AudioPriority) {
    // Тест приоритета аудио
    audioSource->SetPriority(5);
    EXPECT_EQ(audioSource->GetPriority(), 5);
}

TEST_F(AudioSourceTest, PlaybackPosition) {
    // Тест позиции воспроизведения
    audioSource->SetAudioClip("test_sound.wav");
    audioSource->Play();
    
    // Устанавливаем позицию
    audioSource->SetPlaybackPosition(1.5f);
    EXPECT_FLOAT_EQ(audioSource->GetPlaybackPosition(), 1.5f);
}

TEST_F(AudioSourceTest, PlaybackSpeed) {
    // Тест скорости воспроизведения
    audioSource->SetPlaybackSpeed(1.5f);
    EXPECT_FLOAT_EQ(audioSource->GetPlaybackSpeed(), 1.5f);
}

TEST_F(AudioSourceTest, AudioFade) {
    // Тест затухания аудио
    audioSource->SetAudioClip("test_sound.wav");
    audioSource->Play();
    
    // Затухание в течение 2 секунд
    audioSource->FadeOut(2.0f);
    EXPECT_TRUE(audioSource->IsFadingOut());
    
    // Появление в течение 1 секунды
    audioSource->FadeIn(1.0f);
    EXPECT_TRUE(audioSource->IsFadingIn());
}

TEST_F(AudioSourceTest, AudioGroup) {
    // Тест группы аудио
    audioSource->SetAudioGroup("Music");
    EXPECT_EQ(audioSource->GetAudioGroup(), "Music");
}

TEST_F(AudioSourceTest, AudioEffect) {
    // Тест аудио эффектов
    audioSource->SetReverbLevel(0.5f);
    audioSource->SetEchoLevel(0.3f);
    audioSource->SetChorusLevel(0.2f);
    
    EXPECT_FLOAT_EQ(audioSource->GetReverbLevel(), 0.5f);
    EXPECT_FLOAT_EQ(audioSource->GetEchoLevel(), 0.3f);
    EXPECT_FLOAT_EQ(audioSource->GetChorusLevel(), 0.2f);
}

TEST_F(AudioSourceTest, AudioFilter) {
    // Тест аудио фильтров
    audioSource->SetLowPassFilter(1000.0f);
    audioSource->SetHighPassFilter(100.0f);
    audioSource->SetBandPassFilter(500.0f, 2000.0f);
    
    EXPECT_FLOAT_EQ(audioSource->GetLowPassFilter(), 1000.0f);
    EXPECT_FLOAT_EQ(audioSource->GetHighPassFilter(), 100.0f);
    
    auto bandPass = audioSource->GetBandPassFilter();
    EXPECT_FLOAT_EQ(bandPass.first, 500.0f);
    EXPECT_FLOAT_EQ(bandPass.second, 2000.0f);
}

TEST_F(AudioSourceTest, Audio3DPosition) {
    // Тест 3D позиции
    glm::vec3 position(10.0f, 20.0f, 30.0f);
    audioSource->Set3DPosition(position);
    
    auto retrievedPosition = audioSource->Get3DPosition();
    EXPECT_FLOAT_EQ(retrievedPosition.x, position.x);
    EXPECT_FLOAT_EQ(retrievedPosition.y, position.y);
    EXPECT_FLOAT_EQ(retrievedPosition.z, position.z);
}

TEST_F(AudioSourceTest, Audio3DVelocity) {
    // Тест 3D скорости
    glm::vec3 velocity(5.0f, 10.0f, 15.0f);
    audioSource->Set3DVelocity(velocity);
    
    auto retrievedVelocity = audioSource->Get3DVelocity();
    EXPECT_FLOAT_EQ(retrievedVelocity.x, velocity.x);
    EXPECT_FLOAT_EQ(retrievedVelocity.y, velocity.y);
    EXPECT_FLOAT_EQ(retrievedVelocity.z, velocity.z);
}

TEST_F(AudioSourceTest, Audio3DDirection) {
    // Тест 3D направления
    glm::vec3 direction(0.0f, 0.0f, 1.0f);
    audioSource->Set3DDirection(direction);
    
    auto retrievedDirection = audioSource->Get3DDirection();
    EXPECT_FLOAT_EQ(retrievedDirection.x, direction.x);
    EXPECT_FLOAT_EQ(retrievedDirection.y, direction.y);
    EXPECT_FLOAT_EQ(retrievedDirection.z, direction.z);
}

TEST_F(AudioSourceTest, AudioCone) {
    // Тест аудио конуса
    audioSource->SetConeInnerAngle(45.0f);
    audioSource->SetConeOuterAngle(90.0f);
    audioSource->SetConeOuterGain(0.5f);
    
    EXPECT_FLOAT_EQ(audioSource->GetConeInnerAngle(), 45.0f);
    EXPECT_FLOAT_EQ(audioSource->GetConeOuterAngle(), 90.0f);
    EXPECT_FLOAT_EQ(audioSource->GetConeOuterGain(), 0.5f);
}

TEST_F(AudioSourceTest, AudioDoppler) {
    // Тест эффекта Доплера
    audioSource->SetDopplerFactor(1.0f);
    EXPECT_FLOAT_EQ(audioSource->GetDopplerFactor(), 1.0f);
}

TEST_F(AudioSourceTest, AudioAttenuation) {
    // Тест затухания аудио
    audioSource->SetAttenuationModel(AttenuationModel::Inverse);
    EXPECT_EQ(audioSource->GetAttenuationModel(), AttenuationModel::Inverse);
    
    audioSource->SetAttenuationModel(AttenuationModel::Linear);
    EXPECT_EQ(audioSource->GetAttenuationModel(), AttenuationModel::Linear);
    
    audioSource->SetAttenuationModel(AttenuationModel::Exponential);
    EXPECT_EQ(audioSource->GetAttenuationModel(), AttenuationModel::Exponential);
}

TEST_F(AudioSourceTest, AudioPlaybackEvents) {
    // Тест событий воспроизведения
    bool onPlayCalled = false;
    bool onStopCalled = false;
    bool onPauseCalled = false;
    bool onResumeCalled = false;
    bool onFinishedCalled = false;
    
    audioSource->SetOnPlay([&onPlayCalled]() {
        onPlayCalled = true;
    });
    
    audioSource->SetOnStop([&onStopCalled]() {
        onStopCalled = true;
    });
    
    audioSource->SetOnPause([&onPauseCalled]() {
        onPauseCalled = true;
    });
    
    audioSource->SetOnResume([&onResumeCalled]() {
        onResumeCalled = true;
    });
    
    audioSource->SetOnFinished([&onFinishedCalled]() {
        onFinishedCalled = true;
    });
    
    // Тестируем события
    audioSource->SetAudioClip("test_sound.wav");
    audioSource->Play();
    EXPECT_TRUE(onPlayCalled);
    
    audioSource->Pause();
    EXPECT_TRUE(onPauseCalled);
    
    audioSource->Resume();
    EXPECT_TRUE(onResumeCalled);
    
    audioSource->Stop();
    EXPECT_TRUE(onStopCalled);
}

TEST_F(AudioSourceTest, AudioState) {
    // Тест состояния аудио
    audioSource->SetAudioClip("test_sound.wav");
    
    // Начальное состояние
    EXPECT_FALSE(audioSource->IsPlaying());
    EXPECT_FALSE(audioSource->IsPaused());
    
    // После воспроизведения
    audioSource->Play();
    EXPECT_TRUE(audioSource->IsPlaying());
    EXPECT_FALSE(audioSource->IsPaused());
    
    // После паузы
    audioSource->Pause();
    EXPECT_TRUE(audioSource->IsPlaying());
    EXPECT_TRUE(audioSource->IsPaused());
    
    // После остановки
    audioSource->Stop();
    EXPECT_FALSE(audioSource->IsPlaying());
    EXPECT_FALSE(audioSource->IsPaused());
}

TEST_F(AudioSourceTest, AudioClipValidation) {
    // Тест валидации аудио клипа
    // Попытка воспроизведения без клипа
    audioSource->Play();
    EXPECT_FALSE(audioSource->IsPlaying());
    
    // Установка пустого пути
    audioSource->SetAudioClip("");
    audioSource->Play();
    EXPECT_FALSE(audioSource->IsPlaying());
    
    // Установка несуществующего файла
    audioSource->SetAudioClip("nonexistent.wav");
    audioSource->Play();
    EXPECT_FALSE(audioSource->IsPlaying());
}

TEST_F(AudioSourceTest, AudioProperties) {
    // Тест свойств аудио
    audioSource->SetAudioClip("test_sound.wav");
    
    // Получаем свойства аудио
    auto duration = audioSource->GetDuration();
    auto sampleRate = audioSource->GetSampleRate();
    auto channels = audioSource->GetChannels();
    auto bitsPerSample = audioSource->GetBitsPerSample();
    
    // Проверяем, что свойства получены
    EXPECT_GE(duration, 0.0f);
    EXPECT_GT(sampleRate, 0);
    EXPECT_GT(channels, 0);
    EXPECT_GT(bitsPerSample, 0);
}

TEST_F(AudioSourceTest, AudioMixing) {
    // Тест микширования аудио
    audioSource->SetAudioClip("test_sound.wav");
    
    // Устанавливаем параметры микширования
    audioSource->SetMixLevel(0.8f);
    audioSource->SetPan(0.5f); // Слегка вправо
    
    EXPECT_FLOAT_EQ(audioSource->GetMixLevel(), 0.8f);
    EXPECT_FLOAT_EQ(audioSource->GetPan(), 0.5f);
}

TEST_F(AudioSourceTest, AudioCompression) {
    // Тест сжатия аудио
    audioSource->SetCompressionEnabled(true);
    audioSource->SetCompressionRatio(4.0f);
    audioSource->SetCompressionThreshold(0.5f);
    
    EXPECT_TRUE(audioSource->IsCompressionEnabled());
    EXPECT_FLOAT_EQ(audioSource->GetCompressionRatio(), 4.0f);
    EXPECT_FLOAT_EQ(audioSource->GetCompressionThreshold(), 0.5f);
}
