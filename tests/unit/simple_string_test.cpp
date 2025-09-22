#include <gtest/gtest.h>
#include <string>
#include <vector>

// Простые тесты строк без зависимостей от FastEngine
class StringTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_string = "Hello, World!";
    }
    
    void TearDown() override {
        // Очистка после каждого теста
    }
    
    std::string test_string;
};

// Тест базовых операций со строками
TEST_F(StringTest, BasicOperations) {
    EXPECT_EQ(test_string.length(), 13);
    EXPECT_EQ(test_string[0], 'H');
    EXPECT_EQ(test_string[12], '!');
}

// Тест поиска в строках
TEST_F(StringTest, StringSearch) {
    EXPECT_NE(test_string.find("World"), std::string::npos);
    EXPECT_EQ(test_string.find("World"), 7);
    EXPECT_EQ(test_string.find("Not Found"), std::string::npos);
}

// Тест модификации строк
TEST_F(StringTest, StringModification) {
    std::string copy = test_string;
    copy[0] = 'h';
    EXPECT_EQ(copy, "hello, World!");
    
    copy.append(" Test");
    EXPECT_EQ(copy, "hello, World! Test");
}

// Тест работы с векторами строк
TEST_F(StringTest, StringVector) {
    std::vector<std::string> words = {"Hello", "World", "Test"};
    EXPECT_EQ(words.size(), 3);
    EXPECT_EQ(words[0], "Hello");
    EXPECT_EQ(words[1], "World");
    EXPECT_EQ(words[2], "Test");
}

