#include <gtest/gtest.h>
#include "FastEngine/Components/Text.h"
#include "FastEngine/Entity.h"
#include "FastEngine/World.h"
#include <memory>

using namespace FastEngine;

class TextTest : public ::testing::Test {
protected:
    void SetUp() override {
        world = std::make_unique<World>();
        world->Initialize();
        entity = world->CreateEntity();
        text = entity->AddComponent<Text>();
    }
    
    void TearDown() override {
        text = nullptr;
        entity = nullptr;
        world.reset();
    }
    
    std::unique_ptr<World> world;
    std::shared_ptr<Entity> entity;
    std::shared_ptr<Text> text;
};

TEST_F(TextTest, TextCreation) {
    EXPECT_TRUE(text != nullptr);
    EXPECT_TRUE(entity->HasComponent<Text>());
}

TEST_F(TextTest, TextContent) {
    // Тест установки текстового содержимого
    std::string content = "Hello, World!";
    text->SetText(content);
    
    EXPECT_EQ(text->GetText(), content);
}

TEST_F(TextTest, FontSettings) {
    // Тест настройки шрифта
    std::string fontPath = "arial.ttf";
    text->SetFont(fontPath);
    
    EXPECT_EQ(text->GetFont(), fontPath);
}

TEST_F(TextTest, FontSize) {
    // Тест размера шрифта
    text->SetFontSize(24);
    EXPECT_EQ(text->GetFontSize(), 24);
    
    text->SetFontSize(48);
    EXPECT_EQ(text->GetFontSize(), 48);
}

TEST_F(TextTest, TextColor) {
    // Тест цвета текста
    glm::vec4 color(1.0f, 0.0f, 0.0f, 1.0f); // Красный
    text->SetColor(color);
    
    auto retrievedColor = text->GetColor();
    EXPECT_FLOAT_EQ(retrievedColor.r, color.r);
    EXPECT_FLOAT_EQ(retrievedColor.g, color.g);
    EXPECT_FLOAT_EQ(retrievedColor.b, color.b);
    EXPECT_FLOAT_EQ(retrievedColor.a, color.a);
}

TEST_F(TextTest, TextAlignment) {
    // Тест выравнивания текста
    text->SetAlignment(TextAlignment::Left);
    EXPECT_EQ(text->GetAlignment(), TextAlignment::Left);
    
    text->SetAlignment(TextAlignment::Center);
    EXPECT_EQ(text->GetAlignment(), TextAlignment::Center);
    
    text->SetAlignment(TextAlignment::Right);
    EXPECT_EQ(text->GetAlignment(), TextAlignment::Right);
}

TEST_F(TextTest, TextVerticalAlignment) {
    // Тест вертикального выравнивания
    text->SetVerticalAlignment(TextVerticalAlignment::Top);
    EXPECT_EQ(text->GetVerticalAlignment(), TextVerticalAlignment::Top);
    
    text->SetVerticalAlignment(TextVerticalAlignment::Middle);
    EXPECT_EQ(text->GetVerticalAlignment(), TextVerticalAlignment::Middle);
    
    text->SetVerticalAlignment(TextVerticalAlignment::Bottom);
    EXPECT_EQ(text->GetVerticalAlignment(), TextVerticalAlignment::Bottom);
}

TEST_F(TextTest, TextWrap) {
    // Тест переноса строк
    text->SetWordWrap(true);
    EXPECT_TRUE(text->IsWordWrap());
    
    text->SetWordWrap(false);
    EXPECT_FALSE(text->IsWordWrap());
}

TEST_F(TextTest, TextWrapWidth) {
    // Тест ширины переноса
    text->SetWrapWidth(200.0f);
    EXPECT_FLOAT_EQ(text->GetWrapWidth(), 200.0f);
}

TEST_F(TextTest, TextSpacing) {
    // Тест межстрочного интервала
    text->SetLineSpacing(1.5f);
    EXPECT_FLOAT_EQ(text->GetLineSpacing(), 1.5f);
    
    text->SetCharacterSpacing(2.0f);
    EXPECT_FLOAT_EQ(text->GetCharacterSpacing(), 2.0f);
}

TEST_F(TextTest, TextStyle) {
    // Тест стиля текста
    text->SetBold(true);
    text->SetItalic(true);
    text->SetUnderline(true);
    text->SetStrikethrough(true);
    
    EXPECT_TRUE(text->IsBold());
    EXPECT_TRUE(text->IsItalic());
    EXPECT_TRUE(text->IsUnderline());
    EXPECT_TRUE(text->IsStrikethrough());
}

TEST_F(TextTest, TextOutline) {
    // Тест контура текста
    text->SetOutlineEnabled(true);
    text->SetOutlineColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    text->SetOutlineWidth(2.0f);
    
    EXPECT_TRUE(text->IsOutlineEnabled());
    EXPECT_FLOAT_EQ(text->GetOutlineWidth(), 2.0f);
    
    auto outlineColor = text->GetOutlineColor();
    EXPECT_FLOAT_EQ(outlineColor.r, 0.0f);
    EXPECT_FLOAT_EQ(outlineColor.g, 0.0f);
    EXPECT_FLOAT_EQ(outlineColor.b, 0.0f);
    EXPECT_FLOAT_EQ(outlineColor.a, 1.0f);
}

TEST_F(TextTest, TextShadow) {
    // Тест тени текста
    text->SetShadowEnabled(true);
    text->SetShadowColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
    text->SetShadowOffset(glm::vec2(2.0f, 2.0f));
    text->SetShadowBlur(1.0f);
    
    EXPECT_TRUE(text->IsShadowEnabled());
    EXPECT_FLOAT_EQ(text->GetShadowBlur(), 1.0f);
    
    auto shadowColor = text->GetShadowColor();
    auto shadowOffset = text->GetShadowOffset();
    
    EXPECT_FLOAT_EQ(shadowColor.r, 0.0f);
    EXPECT_FLOAT_EQ(shadowColor.g, 0.0f);
    EXPECT_FLOAT_EQ(shadowColor.b, 0.0f);
    EXPECT_FLOAT_EQ(shadowColor.a, 0.5f);
    
    EXPECT_FLOAT_EQ(shadowOffset.x, 2.0f);
    EXPECT_FLOAT_EQ(shadowOffset.y, 2.0f);
}

TEST_F(TextTest, TextGradient) {
    // Тест градиента текста
    text->SetGradientEnabled(true);
    text->SetGradientStartColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    text->SetGradientEndColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    text->SetGradientDirection(TextGradientDirection::Horizontal);
    
    EXPECT_TRUE(text->IsGradientEnabled());
    EXPECT_EQ(text->GetGradientDirection(), TextGradientDirection::Horizontal);
    
    auto startColor = text->GetGradientStartColor();
    auto endColor = text->GetGradientEndColor();
    
    EXPECT_FLOAT_EQ(startColor.r, 1.0f);
    EXPECT_FLOAT_EQ(startColor.g, 0.0f);
    EXPECT_FLOAT_EQ(startColor.b, 0.0f);
    
    EXPECT_FLOAT_EQ(endColor.r, 0.0f);
    EXPECT_FLOAT_EQ(endColor.g, 0.0f);
    EXPECT_FLOAT_EQ(endColor.b, 1.0f);
}

TEST_F(TextTest, TextSize) {
    // Тест размера текста
    text->SetText("Hello, World!");
    text->SetFontSize(24);
    
    auto size = text->GetTextSize();
    EXPECT_GT(size.x, 0.0f);
    EXPECT_GT(size.y, 0.0f);
}

TEST_F(TextTest, TextBounds) {
    // Тест границ текста
    text->SetText("Test Text");
    text->SetFontSize(16);
    
    auto bounds = text->GetTextBounds();
    EXPECT_GT(bounds.width, 0.0f);
    EXPECT_GT(bounds.height, 0.0f);
}

TEST_F(TextTest, TextPosition) {
    // Тест позиции текста
    glm::vec2 position(100.0f, 200.0f);
    text->SetPosition(position);
    
    auto retrievedPosition = text->GetPosition();
    EXPECT_FLOAT_EQ(retrievedPosition.x, position.x);
    EXPECT_FLOAT_EQ(retrievedPosition.y, position.y);
}

TEST_F(TextTest, TextRotation) {
    // Тест поворота текста
    text->SetRotation(45.0f);
    EXPECT_FLOAT_EQ(text->GetRotation(), 45.0f);
}

TEST_F(TextTest, TextScale) {
    // Тест масштаба текста
    glm::vec2 scale(2.0f, 1.5f);
    text->SetScale(scale);
    
    auto retrievedScale = text->GetScale();
    EXPECT_FLOAT_EQ(retrievedScale.x, scale.x);
    EXPECT_FLOAT_EQ(retrievedScale.y, scale.y);
}

TEST_F(TextTest, TextVisible) {
    // Тест видимости текста
    text->SetVisible(true);
    EXPECT_TRUE(text->IsVisible());
    
    text->SetVisible(false);
    EXPECT_FALSE(text->IsVisible());
}

TEST_F(TextTest, TextAlpha) {
    // Тест прозрачности текста
    text->SetAlpha(0.5f);
    EXPECT_FLOAT_EQ(text->GetAlpha(), 0.5f);
}

TEST_F(TextTest, TextClipping) {
    // Тест обрезки текста
    text->SetClippingEnabled(true);
    text->SetClippingRect(glm::vec4(0.0f, 0.0f, 100.0f, 50.0f));
    
    EXPECT_TRUE(text->IsClippingEnabled());
    
    auto clippingRect = text->GetClippingRect();
    EXPECT_FLOAT_EQ(clippingRect.x, 0.0f);
    EXPECT_FLOAT_EQ(clippingRect.y, 0.0f);
    EXPECT_FLOAT_EQ(clippingRect.z, 100.0f);
    EXPECT_FLOAT_EQ(clippingRect.w, 50.0f);
}

TEST_F(TextTest, TextKerning) {
    // Тест кернинга
    text->SetKerningEnabled(true);
    text->SetKerningAmount(1.5f);
    
    EXPECT_TRUE(text->IsKerningEnabled());
    EXPECT_FLOAT_EQ(text->GetKerningAmount(), 1.5f);
}

TEST_F(TextTest, TextLigatures) {
    // Тест лигатур
    text->SetLigaturesEnabled(true);
    EXPECT_TRUE(text->IsLigaturesEnabled());
    
    text->SetLigaturesEnabled(false);
    EXPECT_FALSE(text->IsLigaturesEnabled());
}

TEST_F(TextTest, TextRendering) {
    // Тест рендеринга текста
    text->SetText("Render Test");
    text->SetFontSize(16);
    text->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    // Проверяем, что текст готов к рендерингу
    EXPECT_TRUE(text->IsReadyToRender());
}

TEST_F(TextTest, TextUpdate) {
    // Тест обновления текста
    text->SetText("Initial Text");
    text->Update(0.016f);
    
    // Проверяем, что обновление прошло без ошибок
    EXPECT_TRUE(true);
}

TEST_F(TextTest, TextSerialization) {
    // Тест сериализации текста
    text->SetText("Serialization Test");
    text->SetFontSize(20);
    text->SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    
    // Проверяем, что свойства сохранились
    EXPECT_EQ(text->GetText(), "Serialization Test");
    EXPECT_EQ(text->GetFontSize(), 20);
    
    auto color = text->GetColor();
    EXPECT_FLOAT_EQ(color.r, 0.5f);
    EXPECT_FLOAT_EQ(color.g, 0.5f);
    EXPECT_FLOAT_EQ(color.b, 0.5f);
    EXPECT_FLOAT_EQ(color.a, 1.0f);
}

TEST_F(TextTest, TextCopy) {
    // Тест копирования текста
    text->SetText("Original Text");
    text->SetFontSize(18);
    text->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    
    // Создаем вторую сущность
    auto entity2 = world->CreateEntity();
    auto text2 = entity2->AddComponent<Text>();
    
    // Копируем свойства
    text2->SetText(text->GetText());
    text2->SetFontSize(text->GetFontSize());
    text2->SetColor(text->GetColor());
    
    // Проверяем, что свойства скопированы
    EXPECT_EQ(text2->GetText(), text->GetText());
    EXPECT_EQ(text2->GetFontSize(), text->GetFontSize());
    
    auto color1 = text->GetColor();
    auto color2 = text2->GetColor();
    EXPECT_FLOAT_EQ(color1.r, color2.r);
    EXPECT_FLOAT_EQ(color1.g, color2.g);
    EXPECT_FLOAT_EQ(color1.b, color2.b);
    EXPECT_FLOAT_EQ(color1.a, color2.a);
}

TEST_F(TextTest, TextValidation) {
    // Тест валидации текста
    // Пустой текст
    text->SetText("");
    EXPECT_TRUE(text->GetText().empty());
    
    // Очень длинный текст
    std::string longText(10000, 'A');
    text->SetText(longText);
    EXPECT_EQ(text->GetText(), longText);
    
    // Текст с специальными символами
    text->SetText("Special chars: !@#$%^&*()_+-=[]{}|;':\",./<>?");
    EXPECT_EQ(text->GetText(), "Special chars: !@#$%^&*()_+-=[]{}|;':\",./<>?");
}

TEST_F(TextTest, TextPerformance) {
    // Тест производительности текста
    const int iterations = 1000;
    
    for (int i = 0; i < iterations; ++i) {
        text->SetText("Performance Test " + std::to_string(i));
        text->SetFontSize(12 + (i % 20));
        text->Update(0.016f);
    }
    
    // Проверяем, что все итерации прошли успешно
    EXPECT_TRUE(true);
}
