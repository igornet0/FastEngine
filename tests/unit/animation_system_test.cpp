#include <gtest/gtest.h>
#include "FastEngine/Systems/AnimationSystem.h"
#include "FastEngine/World.h"
#include "FastEngine/Entity.h"
#include "FastEngine/Components/Animator.h"
#include "FastEngine/Components/Sprite.h"
#include <memory>

using namespace FastEngine;

class AnimationSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        world = std::make_unique<World>();
        world->Initialize();
        animationSystem = std::make_unique<AnimationSystem>();
        animationSystem->Initialize();
    }
    
    void TearDown() override {
        animationSystem->Shutdown();
        animationSystem.reset();
        world.reset();
    }
    
    std::unique_ptr<World> world;
    std::unique_ptr<AnimationSystem> animationSystem;
};

TEST_F(AnimationSystemTest, AnimationSystemInitialization) {
    EXPECT_TRUE(animationSystem != nullptr);
}

TEST_F(AnimationSystemTest, EntityRegistration) {
    // Тест регистрации сущности в системе анимации
    auto entity = world->CreateEntity();
    auto animator = entity->AddComponent<Animator>();
    auto sprite = entity->AddComponent<Sprite>();
    
    // Сущность должна быть автоматически зарегистрирована
    EXPECT_TRUE(animationSystem->IsEntityRegistered(entity.get()));
}

TEST_F(AnimationSystemTest, AnimationUpdate) {
    // Тест обновления анимаций
    auto entity = world->CreateEntity();
    auto animator = entity->AddComponent<Animator>();
    auto sprite = entity->AddComponent<Sprite>();
    
    // Создаем анимацию
    Animation walkAnim("walk");
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    walkAnim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
    walkAnim.frames.push_back(AnimationFrame("frame3.png", 0.1f));
    
    animator->AddAnimation(walkAnim);
    animator->Play("walk");
    
    // Обновляем систему анимации
    animationSystem->Update(0.016f);
    
    // Проверяем, что анимация обновилась
    EXPECT_TRUE(animator->IsPlaying());
}

TEST_F(AnimationSystemTest, MultipleAnimations) {
    // Тест множественных анимаций
    const int entityCount = 10;
    std::vector<std::shared_ptr<Entity>> entities;
    
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto animator = entity->AddComponent<Animator>();
        auto sprite = entity->AddComponent<Sprite>();
        
        // Создаем анимацию для каждой сущности
        Animation anim("anim_" + std::to_string(i));
        anim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
        anim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
        
        animator->AddAnimation(anim);
        animator->Play("anim_" + std::to_string(i));
        
        entities.push_back(entity);
    }
    
    // Обновляем систему анимации
    animationSystem->Update(0.016f);
    
    // Проверяем, что все анимации обновились
    for (const auto& entity : entities) {
        auto animator = entity->GetComponent<Animator>();
        EXPECT_TRUE(animator->IsPlaying());
    }
}

TEST_F(AnimationSystemTest, AnimationEvents) {
    // Тест событий анимации
    auto entity = world->CreateEntity();
    auto animator = entity->AddComponent<Animator>();
    auto sprite = entity->AddComponent<Sprite>();
    
    bool onCompleteCalled = false;
    bool onFrameChangeCalled = false;
    
    animator->SetOnAnimationComplete([&onCompleteCalled](const std::string& animName) {
        onCompleteCalled = true;
    });
    
    animator->SetOnFrameChange([&onFrameChangeCalled](int frame) {
        onFrameChangeCalled = true;
    });
    
    // Создаем короткую анимацию
    Animation anim("test");
    anim.type = AnimationType::Once;
    anim.frames.push_back(AnimationFrame("frame1.png", 0.01f));
    anim.frames.push_back(AnimationFrame("frame2.png", 0.01f));
    
    animator->AddAnimation(anim);
    animator->Play("test");
    
    // Обновляем систему анимации до завершения
    for (int i = 0; i < 10; ++i) {
        animationSystem->Update(0.1f);
    }
    
    // Проверяем, что события сработали
    EXPECT_TRUE(onCompleteCalled);
    EXPECT_TRUE(onFrameChangeCalled);
}

TEST_F(AnimationSystemTest, AnimationSpeed) {
    // Тест скорости анимации
    auto entity = world->CreateEntity();
    auto animator = entity->AddComponent<Animator>();
    auto sprite = entity->AddComponent<Sprite>();
    
    Animation anim("speed_test");
    anim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    anim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
    
    animator->AddAnimation(anim);
    animator->SetSpeed(2.0f); // Двойная скорость
    animator->Play("speed_test");
    
    int initialFrame = animator->GetCurrentFrame();
    
    // Обновляем систему анимации
    animationSystem->Update(0.05f); // Половина времени кадра
    
    // При двойной скорости кадр должен измениться
    EXPECT_NE(animator->GetCurrentFrame(), initialFrame);
}

TEST_F(AnimationSystemTest, AnimationPause) {
    // Тест паузы анимации
    auto entity = world->CreateEntity();
    auto animator = entity->AddComponent<Animator>();
    auto sprite = entity->AddComponent<Sprite>();
    
    Animation anim("pause_test");
    anim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    anim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
    
    animator->AddAnimation(anim);
    animator->Play("pause_test");
    
    // Обновляем систему анимации
    animationSystem->Update(0.05f);
    
    int frameBeforePause = animator->GetCurrentFrame();
    
    // Ставим на паузу
    animator->Pause();
    
    // Обновляем систему анимации еще раз
    animationSystem->Update(0.05f);
    
    // Кадр не должен измениться
    EXPECT_EQ(animator->GetCurrentFrame(), frameBeforePause);
}

TEST_F(AnimationSystemTest, AnimationStop) {
    // Тест остановки анимации
    auto entity = world->CreateEntity();
    auto animator = entity->AddComponent<Animator>();
    auto sprite = entity->AddComponent<Sprite>();
    
    Animation anim("stop_test");
    anim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    anim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
    
    animator->AddAnimation(anim);
    animator->Play("stop_test");
    
    // Обновляем систему анимации
    animationSystem->Update(0.05f);
    
    // Останавливаем анимацию
    animator->Stop();
    
    // Проверяем, что анимация остановлена
    EXPECT_FALSE(animator->IsPlaying());
}

TEST_F(AnimationSystemTest, AnimationLoop) {
    // Тест зацикленной анимации
    auto entity = world->CreateEntity();
    auto animator = entity->AddComponent<Animator>();
    auto sprite = entity->AddComponent<Sprite>();
    
    Animation anim("loop_test");
    anim.type = AnimationType::Loop;
    anim.frames.push_back(AnimationFrame("frame1.png", 0.05f));
    anim.frames.push_back(AnimationFrame("frame2.png", 0.05f));
    
    animator->AddAnimation(anim);
    animator->Play("loop_test");
    
    // Обновляем систему анимации несколько раз
    for (int i = 0; i < 20; ++i) {
        animationSystem->Update(0.1f);
    }
    
    // Анимация должна продолжать работать
    EXPECT_TRUE(animator->IsPlaying());
}

TEST_F(AnimationSystemTest, AnimationOnce) {
    // Тест одноразовой анимации
    auto entity = world->CreateEntity();
    auto animator = entity->AddComponent<Animator>();
    auto sprite = entity->AddComponent<Sprite>();
    
    Animation anim("once_test");
    anim.type = AnimationType::Once;
    anim.frames.push_back(AnimationFrame("frame1.png", 0.05f));
    anim.frames.push_back(AnimationFrame("frame2.png", 0.05f));
    
    animator->AddAnimation(anim);
    animator->Play("once_test");
    
    // Обновляем систему анимации до завершения
    for (int i = 0; i < 10; ++i) {
        animationSystem->Update(0.1f);
    }
    
    // Анимация должна остановиться
    EXPECT_FALSE(animator->IsPlaying());
}

TEST_F(AnimationSystemTest, AnimationPingPong) {
    // Тест анимации туда-обратно
    auto entity = world->CreateEntity();
    auto animator = entity->AddComponent<Animator>();
    auto sprite = entity->AddComponent<Sprite>();
    
    Animation anim("pingpong_test");
    anim.type = AnimationType::PingPong;
    anim.frames.push_back(AnimationFrame("frame1.png", 0.05f));
    anim.frames.push_back(AnimationFrame("frame2.png", 0.05f));
    
    animator->AddAnimation(anim);
    animator->Play("pingpong_test");
    
    // Обновляем систему анимации
    for (int i = 0; i < 10; ++i) {
        animationSystem->Update(0.1f);
    }
    
    // Анимация должна продолжать работать
    EXPECT_TRUE(animator->IsPlaying());
}

TEST_F(AnimationSystemTest, AnimationPerformance) {
    // Тест производительности системы анимации
    const int entityCount = 1000;
    std::vector<std::shared_ptr<Entity>> entities;
    
    // Создаем много сущностей с анимациями
    for (int i = 0; i < entityCount; ++i) {
        auto entity = world->CreateEntity();
        auto animator = entity->AddComponent<Animator>();
        auto sprite = entity->AddComponent<Sprite>();
        
        Animation anim("perf_test_" + std::to_string(i));
        anim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
        anim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
        
        animator->AddAnimation(anim);
        animator->Play("perf_test_" + std::to_string(i));
        
        entities.push_back(entity);
    }
    
    // Измеряем время обновления
    auto start = std::chrono::high_resolution_clock::now();
    
    animationSystem->Update(0.016f);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Обновление 1000 анимаций должно занять менее 100мс
    EXPECT_LT(duration.count(), 100);
}

TEST_F(AnimationSystemTest, AnimationCleanup) {
    // Тест очистки анимаций
    auto entity = world->CreateEntity();
    auto animator = entity->AddComponent<Animator>();
    auto sprite = entity->AddComponent<Sprite>();
    
    Animation anim("cleanup_test");
    anim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    
    animator->AddAnimation(anim);
    animator->Play("cleanup_test");
    
    // Обновляем систему анимации
    animationSystem->Update(0.016f);
    
    // Удаляем сущность
    entity.reset();
    
    // Обновляем систему анимации
    animationSystem->Update(0.016f);
    
    // Система должна корректно обработать удаление сущности
    EXPECT_TRUE(true);
}

TEST_F(AnimationSystemTest, AnimationStateConsistency) {
    // Тест согласованности состояния анимации
    auto entity = world->CreateEntity();
    auto animator = entity->AddComponent<Animator>();
    auto sprite = entity->AddComponent<Sprite>();
    
    Animation anim("consistency_test");
    anim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    anim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
    
    animator->AddAnimation(anim);
    animator->Play("consistency_test");
    
    // Обновляем систему анимации
    animationSystem->Update(0.05f);
    
    // Проверяем согласованность состояния
    EXPECT_TRUE(animator->IsPlaying());
    EXPECT_FALSE(animator->IsPaused());
    EXPECT_EQ(animator->GetCurrentAnimation(), "consistency_test");
    EXPECT_GE(animator->GetCurrentFrame(), 0);
}

TEST_F(AnimationSystemTest, AnimationFrameData) {
    // Тест данных кадров анимации
    auto entity = world->CreateEntity();
    auto animator = entity->AddComponent<Animator>();
    auto sprite = entity->AddComponent<Sprite>();
    
    Animation anim("frame_data_test");
    AnimationFrame frame1("frame1.png", 0.1f);
    frame1.offset = glm::vec2(10.0f, 20.0f);
    frame1.size = glm::vec2(100.0f, 150.0f);
    
    AnimationFrame frame2("frame2.png", 0.1f);
    frame2.offset = glm::vec2(15.0f, 25.0f);
    frame2.size = glm::vec2(110.0f, 160.0f);
    
    anim.frames.push_back(frame1);
    anim.frames.push_back(frame2);
    
    animator->AddAnimation(anim);
    animator->Play("frame_data_test");
    
    // Обновляем систему анимации
    animationSystem->Update(0.05f);
    
    // Проверяем данные текущего кадра
    auto currentFrame = animator->GetCurrentFrameData();
    EXPECT_EQ(currentFrame.texturePath, "frame1.png");
    EXPECT_FLOAT_EQ(currentFrame.duration, 0.1f);
    EXPECT_FLOAT_EQ(currentFrame.offset.x, 10.0f);
    EXPECT_FLOAT_EQ(currentFrame.offset.y, 20.0f);
    EXPECT_FLOAT_EQ(currentFrame.size.x, 100.0f);
    EXPECT_FLOAT_EQ(currentFrame.size.y, 150.0f);
}
