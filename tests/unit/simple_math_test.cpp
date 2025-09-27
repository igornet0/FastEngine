#include <gtest/gtest.h>
#include <cmath>

// Простые математические тесты без зависимостей от FastEngine
class MathTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Инициализация для каждого теста
    }
    
    void TearDown() override {
        // Очистка после каждого теста
    }
};

// Тест базовых математических операций
TEST_F(MathTest, BasicArithmetic) {
    EXPECT_EQ(2 + 2, 4);
    EXPECT_EQ(5 - 3, 2);
    EXPECT_EQ(3 * 4, 12);
    EXPECT_EQ(8 / 2, 4);
}

// Тест математических функций
TEST_F(MathTest, MathFunctions) {
    EXPECT_NEAR(std::sin(0), 0.0, 0.001);
    EXPECT_NEAR(std::cos(0), 1.0, 0.001);
    EXPECT_NEAR(std::sqrt(4), 2.0, 0.001);
    EXPECT_NEAR(std::pow(2, 3), 8.0, 0.001);
}

// Тест работы с числами с плавающей точкой
TEST_F(MathTest, FloatingPoint) {
    EXPECT_FLOAT_EQ(0.1 + 0.2, 0.3);
    EXPECT_DOUBLE_EQ(1.0 / 3.0, 0.3333333333333333);
}

// Тест граничных случаев
TEST_F(MathTest, EdgeCases) {
    EXPECT_EQ(std::abs(-5), 5);
    EXPECT_EQ(std::abs(5), 5);
    EXPECT_TRUE(std::isnan(std::sqrt(-1)));
    EXPECT_TRUE(std::isinf(1.0 / 0.0));
}


