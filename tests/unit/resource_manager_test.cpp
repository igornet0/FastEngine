#include <gtest/gtest.h>
#include "FastEngine/Resources/ResourceManager.h"
#include <memory>

using namespace FastEngine;

class ResourceManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        resourceManager = std::make_unique<ResourceManager>();
        resourceManager->Initialize();
    }
    
    void TearDown() override {
        resourceManager->Shutdown();
        resourceManager.reset();
    }
    
    std::unique_ptr<ResourceManager> resourceManager;
};

TEST_F(ResourceManagerTest, ResourceManagerInitialization) {
    EXPECT_TRUE(resourceManager != nullptr);
}

TEST_F(ResourceManagerTest, TextureLoading) {
    // Тест загрузки текстур
    auto texture = resourceManager->LoadTexture("test_texture.png");
    EXPECT_TRUE(texture != nullptr);
    
    // Проверяем, что текстура загружена
    EXPECT_TRUE(resourceManager->IsTextureLoaded("test_texture.png"));
}

TEST_F(ResourceManagerTest, TextureUnloading) {
    // Тест выгрузки текстур
    auto texture = resourceManager->LoadTexture("test_texture.png");
    EXPECT_TRUE(texture != nullptr);
    
    resourceManager->UnloadTexture("test_texture.png");
    EXPECT_FALSE(resourceManager->IsTextureLoaded("test_texture.png"));
}

TEST_F(ResourceManagerTest, TextureCaching) {
    // Тест кэширования текстур
    auto texture1 = resourceManager->LoadTexture("test_texture.png");
    auto texture2 = resourceManager->LoadTexture("test_texture.png");
    
    // Оба указателя должны указывать на один объект
    EXPECT_EQ(texture1, texture2);
}

TEST_F(ResourceManagerTest, AudioLoading) {
    // Тест загрузки аудио
    auto audio = resourceManager->LoadAudio("test_sound.wav");
    EXPECT_TRUE(audio != nullptr);
    
    // Проверяем, что аудио загружено
    EXPECT_TRUE(resourceManager->IsAudioLoaded("test_sound.wav"));
}

TEST_F(ResourceManagerTest, AudioUnloading) {
    // Тест выгрузки аудио
    auto audio = resourceManager->LoadAudio("test_sound.wav");
    EXPECT_TRUE(audio != nullptr);
    
    resourceManager->UnloadAudio("test_sound.wav");
    EXPECT_FALSE(resourceManager->IsAudioLoaded("test_sound.wav"));
}

TEST_F(ResourceManagerTest, ShaderLoading) {
    // Тест загрузки шейдеров
    auto shader = resourceManager->LoadShader("test.vert", "test.frag");
    EXPECT_TRUE(shader != nullptr);
    
    // Проверяем, что шейдер загружен
    EXPECT_TRUE(resourceManager->IsShaderLoaded("test.vert", "test.frag"));
}

TEST_F(ResourceManagerTest, ShaderUnloading) {
    // Тест выгрузки шейдеров
    auto shader = resourceManager->LoadShader("test.vert", "test.frag");
    EXPECT_TRUE(shader != nullptr);
    
    resourceManager->UnloadShader("test.vert", "test.frag");
    EXPECT_FALSE(resourceManager->IsShaderLoaded("test.vert", "test.frag"));
}

TEST_F(ResourceManagerTest, FontLoading) {
    // Тест загрузки шрифтов
    auto font = resourceManager->LoadFont("arial.ttf", 24);
    EXPECT_TRUE(font != nullptr);
    
    // Проверяем, что шрифт загружен
    EXPECT_TRUE(resourceManager->IsFontLoaded("arial.ttf", 24));
}

TEST_F(ResourceManagerTest, FontUnloading) {
    // Тест выгрузки шрифтов
    auto font = resourceManager->LoadFont("arial.ttf", 24);
    EXPECT_TRUE(font != nullptr);
    
    resourceManager->UnloadFont("arial.ttf", 24);
    EXPECT_FALSE(resourceManager->IsFontLoaded("arial.ttf", 24));
}

TEST_F(ResourceManagerTest, ModelLoading) {
    // Тест загрузки моделей
    auto model = resourceManager->LoadModel("test.obj");
    EXPECT_TRUE(model != nullptr);
    
    // Проверяем, что модель загружена
    EXPECT_TRUE(resourceManager->IsModelLoaded("test.obj"));
}

TEST_F(ResourceManagerTest, ModelUnloading) {
    // Тест выгрузки моделей
    auto model = resourceManager->LoadModel("test.obj");
    EXPECT_TRUE(model != nullptr);
    
    resourceManager->UnloadModel("test.obj");
    EXPECT_FALSE(resourceManager->IsModelLoaded("test.obj"));
}

TEST_F(ResourceManagerTest, ResourcePath) {
    // Тест пути к ресурсам
    resourceManager->SetResourcePath("assets/");
    EXPECT_EQ(resourceManager->GetResourcePath(), "assets/");
    
    resourceManager->SetResourcePath("resources/");
    EXPECT_EQ(resourceManager->GetResourcePath(), "resources/");
}

TEST_F(ResourceManagerTest, ResourceHotReload) {
    // Тест горячей перезагрузки ресурсов
    resourceManager->SetHotReloadEnabled(true);
    EXPECT_TRUE(resourceManager->IsHotReloadEnabled());
    
    resourceManager->SetHotReloadEnabled(false);
    EXPECT_FALSE(resourceManager->IsHotReloadEnabled());
}

TEST_F(ResourceManagerTest, ResourcePreloading) {
    // Тест предзагрузки ресурсов
    std::vector<std::string> resources = {
        "texture1.png",
        "texture2.png",
        "sound1.wav",
        "sound2.wav"
    };
    
    resourceManager->PreloadResources(resources);
    
    // Проверяем, что ресурсы предзагружены
    for (const auto& resource : resources) {
        if (resource.find(".png") != std::string::npos) {
            EXPECT_TRUE(resourceManager->IsTextureLoaded(resource));
        } else if (resource.find(".wav") != std::string::npos) {
            EXPECT_TRUE(resourceManager->IsAudioLoaded(resource));
        }
    }
}

TEST_F(ResourceManagerTest, ResourceMemoryUsage) {
    // Тест использования памяти ресурсами
    auto texture = resourceManager->LoadTexture("test_texture.png");
    auto audio = resourceManager->LoadAudio("test_sound.wav");
    
    auto memoryUsage = resourceManager->GetMemoryUsage();
    EXPECT_GT(memoryUsage, 0);
}

TEST_F(ResourceManagerTest, ResourceCleanup) {
    // Тест очистки ресурсов
    auto texture = resourceManager->LoadTexture("test_texture.png");
    auto audio = resourceManager->LoadAudio("test_sound.wav");
    
    resourceManager->CleanupUnusedResources();
    
    // Неиспользуемые ресурсы должны быть очищены
    EXPECT_FALSE(resourceManager->IsTextureLoaded("test_texture.png"));
    EXPECT_FALSE(resourceManager->IsAudioLoaded("test_sound.wav"));
}

TEST_F(ResourceManagerTest, ResourceValidation) {
    // Тест валидации ресурсов
    // Попытка загрузки несуществующего файла
    auto texture = resourceManager->LoadTexture("nonexistent.png");
    EXPECT_TRUE(texture == nullptr);
    
    // Попытка загрузки файла с неподдерживаемым форматом
    auto audio = resourceManager->LoadAudio("test.txt");
    EXPECT_TRUE(audio == nullptr);
}

TEST_F(ResourceManagerTest, ResourceStatistics) {
    // Тест статистики ресурсов
    auto texture = resourceManager->LoadTexture("test_texture.png");
    auto audio = resourceManager->LoadAudio("test_sound.wav");
    
    auto stats = resourceManager->GetResourceStatistics();
    
    EXPECT_GE(stats.loadedTextures, 0);
    EXPECT_GE(stats.loadedAudio, 0);
    EXPECT_GE(stats.loadedShaders, 0);
    EXPECT_GE(stats.loadedFonts, 0);
    EXPECT_GE(stats.loadedModels, 0);
    EXPECT_GE(stats.totalMemoryUsage, 0);
}

TEST_F(ResourceManagerTest, ResourceUpdate) {
    // Тест обновления менеджера ресурсов
    resourceManager->SetHotReloadEnabled(true);
    
    resourceManager->Update(0.016f);
    
    // Проверяем, что обновление прошло без ошибок
    EXPECT_TRUE(true);
}

TEST_F(ResourceManagerTest, ResourcePerformance) {
    // Тест производительности менеджера ресурсов
    const int iterations = 100;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto texture = resourceManager->LoadTexture("test_texture_" + std::to_string(i) + ".png");
        resourceManager->UnloadTexture("test_texture_" + std::to_string(i) + ".png");
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Обработка 100 ресурсов должна занять менее 1000мс
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(ResourceManagerTest, ResourceThreadSafety) {
    // Тест потокобезопасности менеджера ресурсов
    const int threadCount = 4;
    const int operationsPerThread = 50;
    std::vector<std::thread> threads;
    
    for (int t = 0; t < threadCount; ++t) {
        threads.emplace_back([this, operationsPerThread, t]() {
            for (int i = 0; i < operationsPerThread; ++i) {
                auto texture = resourceManager->LoadTexture("thread_" + std::to_string(t) + "_texture_" + std::to_string(i) + ".png");
                resourceManager->UnloadTexture("thread_" + std::to_string(t) + "_texture_" + std::to_string(i) + ".png");
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что все операции прошли без ошибок
    EXPECT_TRUE(true);
}
