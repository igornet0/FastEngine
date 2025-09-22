#include <gtest/gtest.h>
#include "FastEngine/Components/Animator.h"
#include "FastEngine/Entity.h"
#include "FastEngine/World.h"
#include <memory>

using namespace FastEngine;

class AnimatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        world = std::make_unique<World>();
        world->Initialize();
        entity = world->CreateEntity();
        animator = entity->AddComponent<Animator>();
    }
    
    void TearDown() override {
        animator = nullptr;
        entity = nullptr;
        world.reset();
    }
    
    std::unique_ptr<World> world;
    std::shared_ptr<Entity> entity;
    std::shared_ptr<Animator> animator;
};

TEST_F(AnimatorTest, AnimationCreation) {
    // Тест создания анимации
    Animation walkAnim("walk");
    walkAnim.type = AnimationType::Loop;
    walkAnim.speed = 1.0f;
    walkAnim.autoPlay = false;
    
    // Добавляем кадры
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    walkAnim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
    walkAnim.frames.push_back(AnimationFrame("frame3.png", 0.1f));
    
    animator->AddAnimation(walkAnim);
    
    EXPECT_TRUE(animator->HasAnimation("walk"));
    EXPECT_EQ(animator->GetAnimationNames().size(), 1);
}

TEST_F(AnimatorTest, AnimationPlayback) {
    // Создаем анимацию
    Animation walkAnim("walk");
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    walkAnim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
    walkAnim.frames.push_back(AnimationFrame("frame3.png", 0.1f));
    
    animator->AddAnimation(walkAnim);
    
    // Тест воспроизведения
    animator->Play("walk");
    EXPECT_TRUE(animator->IsPlaying());
    EXPECT_EQ(animator->GetCurrentAnimation(), "walk");
    EXPECT_EQ(animator->GetCurrentFrame(), 0);
}

TEST_F(AnimatorTest, AnimationStop) {
    // Создаем и запускаем анимацию
    Animation walkAnim("walk");
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    walkAnim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
    
    animator->AddAnimation(walkAnim);
    animator->Play("walk");
    
    // Останавливаем анимацию
    animator->Stop();
    EXPECT_FALSE(animator->IsPlaying());
    EXPECT_EQ(animator->GetCurrentAnimation(), "");
}

TEST_F(AnimatorTest, AnimationPauseResume) {
    // Создаем и запускаем анимацию
    Animation walkAnim("walk");
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    walkAnim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
    
    animator->AddAnimation(walkAnim);
    animator->Play("walk");
    
    // Пауза
    animator->Pause();
    EXPECT_TRUE(animator->IsPaused());
    EXPECT_TRUE(animator->IsPlaying());
    
    // Возобновление
    animator->Resume();
    EXPECT_FALSE(animator->IsPaused());
    EXPECT_TRUE(animator->IsPlaying());
}

TEST_F(AnimatorTest, AnimationUpdate) {
    // Создаем анимацию с короткими кадрами
    Animation walkAnim("walk");
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.05f));
    walkAnim.frames.push_back(AnimationFrame("frame2.png", 0.05f));
    walkAnim.frames.push_back(AnimationFrame("frame3.png", 0.05f));
    
    animator->AddAnimation(walkAnim);
    animator->Play("walk");
    
    // Обновляем анимацию
    animator->Update(0.1f); // Больше времени кадра
    EXPECT_EQ(animator->GetCurrentFrame(), 2); // Должен перейти на последний кадр
}

TEST_F(AnimatorTest, AnimationLoop) {
    // Создаем зацикленную анимацию
    Animation walkAnim("walk");
    walkAnim.type = AnimationType::Loop;
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.05f));
    walkAnim.frames.push_back(AnimationFrame("frame2.png", 0.05f));
    
    animator->AddAnimation(walkAnim);
    animator->Play("walk");
    
    // Обновляем анимацию несколько раз
    for (int i = 0; i < 5; ++i) {
        animator->Update(0.1f);
    }
    
    // Анимация должна продолжать работать
    EXPECT_TRUE(animator->IsPlaying());
}

TEST_F(AnimatorTest, AnimationOnce) {
    // Создаем одноразовую анимацию
    Animation walkAnim("walk");
    walkAnim.type = AnimationType::Once;
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.05f));
    walkAnim.frames.push_back(AnimationFrame("frame2.png", 0.05f));
    
    animator->AddAnimation(walkAnim);
    animator->Play("walk");
    
    // Обновляем анимацию до завершения
    animator->Update(0.2f);
    
    // Анимация должна остановиться
    EXPECT_FALSE(animator->IsPlaying());
}

TEST_F(AnimatorTest, AnimationPingPong) {
    // Создаем анимацию туда-обратно
    Animation walkAnim("walk");
    walkAnim.type = AnimationType::PingPong;
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.05f));
    walkAnim.frames.push_back(AnimationFrame("frame2.png", 0.05f));
    
    animator->AddAnimation(walkAnim);
    animator->Play("walk");
    
    // Обновляем анимацию
    animator->Update(0.1f);
    EXPECT_TRUE(animator->IsPlaying());
}

TEST_F(AnimatorTest, AnimationSpeed) {
    // Создаем анимацию
    Animation walkAnim("walk");
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    walkAnim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
    
    animator->AddAnimation(walkAnim);
    animator->SetSpeed(2.0f);
    animator->Play("walk");
    
    EXPECT_FLOAT_EQ(animator->GetSpeed(), 2.0f);
}

TEST_F(AnimatorTest, AnimationRemoval) {
    // Создаем несколько анимаций
    Animation walkAnim("walk");
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    
    Animation runAnim("run");
    runAnim.frames.push_back(AnimationFrame("run1.png", 0.1f));
    
    animator->AddAnimation(walkAnim);
    animator->AddAnimation(runAnim);
    
    EXPECT_EQ(animator->GetAnimationNames().size(), 2);
    EXPECT_TRUE(animator->HasAnimation("walk"));
    EXPECT_TRUE(animator->HasAnimation("run"));
    
    // Удаляем одну анимацию
    animator->RemoveAnimation("walk");
    
    EXPECT_EQ(animator->GetAnimationNames().size(), 1);
    EXPECT_FALSE(animator->HasAnimation("walk"));
    EXPECT_TRUE(animator->HasAnimation("run"));
}

TEST_F(AnimatorTest, AnimationClear) {
    // Создаем несколько анимаций
    Animation walkAnim("walk");
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    
    Animation runAnim("run");
    runAnim.frames.push_back(AnimationFrame("run1.png", 0.1f));
    
    animator->AddAnimation(walkAnim);
    animator->AddAnimation(runAnim);
    
    EXPECT_EQ(animator->GetAnimationNames().size(), 2);
    
    // Очищаем все анимации
    animator->ClearAnimations();
    
    EXPECT_EQ(animator->GetAnimationNames().size(), 0);
    EXPECT_FALSE(animator->HasAnimation("walk"));
    EXPECT_FALSE(animator->HasAnimation("run"));
}

TEST_F(AnimatorTest, AnimationEvents) {
    // Создаем анимацию
    Animation walkAnim("walk");
    walkAnim.frames.push_back(AnimationFrame("frame1.png", 0.1f));
    walkAnim.frames.push_back(AnimationFrame("frame2.png", 0.1f));
    
    animator->AddAnimation(walkAnim);
    
    // Устанавливаем callback для завершения анимации
    bool animationCompleted = false;
    animator->SetOnAnimationComplete([&animationCompleted](const std::string& animName) {
        animationCompleted = true;
    });
    
    // Устанавливаем callback для смены кадра
    int frameChangeCount = 0;
    animator->SetOnFrameChange([&frameChangeCount](int frame) {
        frameChangeCount++;
    });
    
    animator->Play("walk");
    animator->Update(0.2f); // Обновляем до завершения
    
    // Проверяем, что события сработали
    EXPECT_TRUE(animationCompleted);
    EXPECT_GT(frameChangeCount, 0);
}

TEST_F(AnimatorTest, InvalidAnimationName) {
    // Пытаемся воспроизвести несуществующую анимацию
    animator->Play("nonexistent");
    EXPECT_FALSE(animator->IsPlaying());
    
    // Пытаемся получить несуществующую анимацию
    auto anim = animator->GetAnimation("nonexistent");
    EXPECT_TRUE(anim == nullptr);
}

TEST_F(AnimatorTest, AnimationFrameData) {
    // Создаем анимацию с детальными кадрами
    Animation walkAnim("walk");
    
    AnimationFrame frame1("frame1.png", 0.1f);
    frame1.offset = glm::vec2(10.0f, 20.0f);
    frame1.size = glm::vec2(100.0f, 150.0f);
    
    AnimationFrame frame2("frame2.png", 0.15f);
    frame2.offset = glm::vec2(15.0f, 25.0f);
    frame2.size = glm::vec2(110.0f, 160.0f);
    
    walkAnim.frames.push_back(frame1);
    walkAnim.frames.push_back(frame2);
    
    animator->AddAnimation(walkAnim);
    animator->Play("walk");
    
    // Получаем данные текущего кадра
    auto currentFrame = animator->GetCurrentFrameData();
    EXPECT_EQ(currentFrame.texturePath, "frame1.png");
    EXPECT_FLOAT_EQ(currentFrame.duration, 0.1f);
    EXPECT_FLOAT_EQ(currentFrame.offset.x, 10.0f);
    EXPECT_FLOAT_EQ(currentFrame.offset.y, 20.0f);
    EXPECT_FLOAT_EQ(currentFrame.size.x, 100.0f);
    EXPECT_FLOAT_EQ(currentFrame.size.y, 150.0f);
}
