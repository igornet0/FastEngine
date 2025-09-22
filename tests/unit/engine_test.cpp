#include <gtest/gtest.h>
#include "FastEngine/Engine.h"

using namespace FastEngine;

class EngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Инициализация для каждого теста
    }
    
    void TearDown() override {
        // Очистка после каждого теста
    }
};

// Тест инициализации движка
TEST_F(EngineTest, EngineInitialization) {
    EXPECT_NO_THROW({
        Engine& engine = Engine::GetInstance();
        EXPECT_TRUE(engine.Initialize());
    });
}

// Тест завершения работы движка
TEST_F(EngineTest, EngineShutdown) {
    Engine& engine = Engine::GetInstance();
    EXPECT_NO_THROW(engine.Shutdown());
}

// Тест получения экземпляра движка
TEST_F(EngineTest, GetInstance) {
    Engine& engine1 = Engine::GetInstance();
    Engine& engine2 = Engine::GetInstance();
    EXPECT_EQ(&engine1, &engine2);
}

