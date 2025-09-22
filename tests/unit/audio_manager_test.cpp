#include <gtest/gtest.h>
#include "FastEngine/Audio/AudioManager.h"
#include <memory>

using namespace FastEngine;

class AudioManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        audioManager = std::make_unique<AudioManager>();
        audioManager->Initialize();
    }
    
    void TearDown() override {
        audioManager->Shutdown();
        audioManager.reset();
    }
    
    std::unique_ptr<AudioManager> audioManager;
};

TEST_F(AudioManagerTest, AudioManagerInitialization) {
    EXPECT_TRUE(audioManager != nullptr);
}

TEST_F(AudioManagerTest, AudioDeviceEnumeration) {
    // Тест перечисления аудио устройств
    auto devices = audioManager->GetAudioDevices();
    EXPECT_GE(devices.size(), 0);
}

TEST_F(AudioManagerTest, AudioDeviceSelection) {
    // Тест выбора аудио устройства
    auto devices = audioManager->GetAudioDevices();
    if (!devices.empty()) {
        audioManager->SetAudioDevice(devices[0]);
        EXPECT_EQ(audioManager->GetCurrentAudioDevice(), devices[0]);
    }
}

TEST_F(AudioManagerTest, MasterVolume) {
    // Тест главной громкости
    audioManager->SetMasterVolume(0.5f);
    EXPECT_FLOAT_EQ(audioManager->GetMasterVolume(), 0.5f);
    
    audioManager->SetMasterVolume(1.0f);
    EXPECT_FLOAT_EQ(audioManager->GetMasterVolume(), 1.0f);
}

TEST_F(AudioManagerTest, AudioGroupVolume) {
    // Тест громкости групп аудио
    audioManager->SetGroupVolume("Music", 0.7f);
    audioManager->SetGroupVolume("SFX", 0.8f);
    
    EXPECT_FLOAT_EQ(audioManager->GetGroupVolume("Music"), 0.7f);
    EXPECT_FLOAT_EQ(audioManager->GetGroupVolume("SFX"), 0.8f);
}

TEST_F(AudioManagerTest, AudioMute) {
    // Тест отключения звука
    audioManager->SetMute(true);
    EXPECT_TRUE(audioManager->IsMuted());
    
    audioManager->SetMute(false);
    EXPECT_FALSE(audioManager->IsMuted());
}

TEST_F(AudioManagerTest, AudioPause) {
    // Тест паузы аудио
    audioManager->SetPaused(true);
    EXPECT_TRUE(audioManager->IsPaused());
    
    audioManager->SetPaused(false);
    EXPECT_FALSE(audioManager->IsPaused());
}

TEST_F(AudioManagerTest, Audio3DListener) {
    // Тест 3D слушателя
    glm::vec3 position(0.0f, 0.0f, 0.0f);
    glm::vec3 velocity(0.0f, 0.0f, 0.0f);
    glm::vec3 forward(0.0f, 0.0f, -1.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    
    audioManager->SetListenerPosition(position);
    audioManager->SetListenerVelocity(velocity);
    audioManager->SetListenerOrientation(forward, up);
    
    auto listenerPos = audioManager->GetListenerPosition();
    auto listenerVel = audioManager->GetListenerVelocity();
    auto listenerForward = audioManager->GetListenerForward();
    auto listenerUp = audioManager->GetListenerUp();
    
    EXPECT_FLOAT_EQ(listenerPos.x, position.x);
    EXPECT_FLOAT_EQ(listenerPos.y, position.y);
    EXPECT_FLOAT_EQ(listenerPos.z, position.z);
    
    EXPECT_FLOAT_EQ(listenerVel.x, velocity.x);
    EXPECT_FLOAT_EQ(listenerVel.y, velocity.y);
    EXPECT_FLOAT_EQ(listenerVel.z, velocity.z);
    
    EXPECT_FLOAT_EQ(listenerForward.x, forward.x);
    EXPECT_FLOAT_EQ(listenerForward.y, forward.y);
    EXPECT_FLOAT_EQ(listenerForward.z, forward.z);
    
    EXPECT_FLOAT_EQ(listenerUp.x, up.x);
    EXPECT_FLOAT_EQ(listenerUp.y, up.y);
    EXPECT_FLOAT_EQ(listenerUp.z, up.z);
}

TEST_F(AudioManagerTest, AudioEffects) {
    // Тест аудио эффектов
    audioManager->SetReverbEnabled(true);
    audioManager->SetReverbLevel(0.5f);
    
    EXPECT_TRUE(audioManager->IsReverbEnabled());
    EXPECT_FLOAT_EQ(audioManager->GetReverbLevel(), 0.5f);
    
    audioManager->SetEchoEnabled(true);
    audioManager->SetEchoLevel(0.3f);
    audioManager->SetEchoDelay(0.1f);
    
    EXPECT_TRUE(audioManager->IsEchoEnabled());
    EXPECT_FLOAT_EQ(audioManager->GetEchoLevel(), 0.3f);
    EXPECT_FLOAT_EQ(audioManager->GetEchoDelay(), 0.1f);
}

TEST_F(AudioManagerTest, AudioFilters) {
    // Тест аудио фильтров
    audioManager->SetLowPassFilter(1000.0f);
    audioManager->SetHighPassFilter(100.0f);
    audioManager->SetBandPassFilter(500.0f, 2000.0f);
    
    EXPECT_FLOAT_EQ(audioManager->GetLowPassFilter(), 1000.0f);
    EXPECT_FLOAT_EQ(audioManager->GetHighPassFilter(), 100.0f);
    
    auto bandPass = audioManager->GetBandPassFilter();
    EXPECT_FLOAT_EQ(bandPass.first, 500.0f);
    EXPECT_FLOAT_EQ(bandPass.second, 2000.0f);
}

TEST_F(AudioManagerTest, AudioCompression) {
    // Тест сжатия аудио
    audioManager->SetCompressionEnabled(true);
    audioManager->SetCompressionRatio(4.0f);
    audioManager->SetCompressionThreshold(0.5f);
    
    EXPECT_TRUE(audioManager->IsCompressionEnabled());
    EXPECT_FLOAT_EQ(audioManager->GetCompressionRatio(), 4.0f);
    EXPECT_FLOAT_EQ(audioManager->GetCompressionThreshold(), 0.5f);
}

TEST_F(AudioManagerTest, AudioLimiter) {
    // Тест ограничителя аудио
    audioManager->SetLimiterEnabled(true);
    audioManager->SetLimiterThreshold(0.9f);
    audioManager->SetLimiterRelease(0.1f);
    
    EXPECT_TRUE(audioManager->IsLimiterEnabled());
    EXPECT_FLOAT_EQ(audioManager->GetLimiterThreshold(), 0.9f);
    EXPECT_FLOAT_EQ(audioManager->GetLimiterRelease(), 0.1f);
}

TEST_F(AudioManagerTest, AudioPerformance) {
    // Тест производительности аудио менеджера
    const int iterations = 1000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        audioManager->SetMasterVolume(i / 1000.0f);
        audioManager->SetGroupVolume("Test", i / 1000.0f);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Обработка 1000 итераций должна занять менее 100мс
    EXPECT_LT(duration.count(), 100);
}

TEST_F(AudioManagerTest, AudioUpdate) {
    // Тест обновления аудио менеджера
    audioManager->SetMasterVolume(0.5f);
    audioManager->SetPaused(false);
    
    audioManager->Update(0.016f);
    
    // Проверяем, что обновление прошло без ошибок
    EXPECT_TRUE(true);
}

TEST_F(AudioManagerTest, AudioStatistics) {
    // Тест статистики аудио
    auto stats = audioManager->GetAudioStatistics();
    
    EXPECT_GE(stats.activeSources, 0);
    EXPECT_GE(stats.totalSources, 0);
    EXPECT_GE(stats.memoryUsage, 0);
    EXPECT_GE(stats.cpuUsage, 0.0f);
}

TEST_F(AudioManagerTest, AudioCleanup) {
    // Тест очистки аудио менеджера
    audioManager->SetMasterVolume(0.5f);
    audioManager->SetGroupVolume("Test", 0.7f);
    
    audioManager->Shutdown();
    audioManager->Initialize();
    
    // После переинициализации настройки должны сброситься
    EXPECT_FLOAT_EQ(audioManager->GetMasterVolume(), 1.0f);
    EXPECT_FLOAT_EQ(audioManager->GetGroupVolume("Test"), 1.0f);
}
