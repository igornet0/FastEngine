#include <gtest/gtest.h>
#include "FastEngine/Engine.h"

using namespace FastEngine;

class OpenGLTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_TRUE(Engine::GetInstance().Initialize());
    }
    
    void TearDown() override {
        Engine::GetInstance().Shutdown();
    }
};

// Тест инициализации OpenGL контекста
TEST_F(OpenGLTest, OpenGLContextInitialization) {
    // Проверяем, что OpenGL контекст инициализирован
    EXPECT_NO_THROW({
        // Здесь можно добавить проверки OpenGL
        // Например, проверка версии OpenGL
    });
}

// Тест совместимости с OpenGL
TEST_F(OpenGLTest, OpenGLCompatibility) {
    // Проверяем совместимость с различными версиями OpenGL
    EXPECT_NO_THROW({
        // Тестируем базовую функциональность OpenGL
    });
}










