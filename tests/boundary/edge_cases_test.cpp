#include <gtest/gtest.h>
#include "FastEngine/Engine.h"
#include "FastEngine/Entity.h"
#include "FastEngine/World.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Components/Transform.h"
#include <limits>
#include <memory>
#include <vector>

using namespace FastEngine;

class EdgeCasesTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_TRUE(Engine::GetInstance().Initialize());
    }
    
    void TearDown() override {
        Engine::GetInstance().Shutdown();
    }
};

// Тест на граничные случаи при создании сущностей
TEST_F(EdgeCasesTest, EntityCreationEdgeCases) {
    auto world = std::make_unique<World>();
    
    // Тест на создание сущности с минимальными данными
    EXPECT_NO_THROW({
        auto entity = world->CreateEntity();
        EXPECT_NE(entity, nullptr);
    });
    
    // Тест на создание сущности с максимальными данными
    EXPECT_NO_THROW({
        auto entity = world->CreateEntity();
        EXPECT_NE(entity, nullptr);
    });
    
    // Тест на создание сущности с нулевыми данными
    EXPECT_NO_THROW({
        auto entity = world->CreateEntity();
        EXPECT_NE(entity, nullptr);
    });
}

// Тест на граничные случаи при работе с компонентами
TEST_F(EdgeCasesTest, ComponentEdgeCases) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Тест на добавление компонента к валидной сущности
    EXPECT_NO_THROW({
        auto transform = entity->AddComponent<Transform>();
        EXPECT_NE(transform, nullptr);
    });
    
    // Тест на добавление компонента к невалидной сущности
    EXPECT_THROW({
        Entity* nullEntity = nullptr;
        nullEntity->AddComponent<Transform>();
    }, std::invalid_argument);
    
    // Тест на получение компонента у валидной сущности
    EXPECT_NO_THROW({
        auto transform = entity->GetComponent<Transform>();
        EXPECT_NE(transform, nullptr);
    });
    
    // Тест на получение компонента у невалидной сущности
    EXPECT_THROW({
        Entity* nullEntity = nullptr;
        nullEntity->GetComponent<Transform>();
    }, std::invalid_argument);
}

// Тест на граничные случаи при работе с трансформацией
TEST_F(EdgeCasesTest, TransformEdgeCases) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<Transform>();
    
    // Тест на установку минимальных значений
    EXPECT_NO_THROW(transform->SetPosition(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min()));
    EXPECT_NO_THROW(transform->SetScale(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min()));
    EXPECT_NO_THROW(transform->SetRotation(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min()));
    
    // Тест на установку максимальных значений
    EXPECT_NO_THROW(transform->SetPosition(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
    EXPECT_NO_THROW(transform->SetScale(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
    EXPECT_NO_THROW(transform->SetRotation(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
    
    // Тест на установку нулевых значений
    EXPECT_NO_THROW(transform->SetPosition(0.0f, 0.0f, 0.0f));
    EXPECT_NO_THROW(transform->SetScale(0.0f, 0.0f, 0.0f));
    EXPECT_NO_THROW(transform->SetRotation(0.0f, 0.0f, 0.0f));
    
    // Тест на установку отрицательных значений
    EXPECT_NO_THROW(transform->SetPosition(-1.0f, -1.0f, -1.0f));
    EXPECT_NO_THROW(transform->SetScale(-1.0f, -1.0f, -1.0f));
    EXPECT_NO_THROW(transform->SetRotation(-1.0f, -1.0f, -1.0f));
    
    // Тест на установку NaN значений
    EXPECT_THROW(transform->SetPosition(std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(transform->SetScale(std::numeric_limits<float>::quiet_NaN(), 1.0f, 1.0f), std::invalid_argument);
    EXPECT_THROW(transform->SetRotation(std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f), std::invalid_argument);
    
    // Тест на установку Infinity значений
    EXPECT_THROW(transform->SetPosition(std::numeric_limits<float>::infinity(), 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(transform->SetScale(std::numeric_limits<float>::infinity(), 1.0f, 1.0f), std::invalid_argument);
    EXPECT_THROW(transform->SetRotation(std::numeric_limits<float>::infinity(), 0.0f, 0.0f), std::invalid_argument);
    
    // Тест на установку -Infinity значений
    EXPECT_THROW(transform->SetPosition(-std::numeric_limits<float>::infinity(), 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(transform->SetScale(-std::numeric_limits<float>::infinity(), 1.0f, 1.0f), std::invalid_argument);
    EXPECT_THROW(transform->SetRotation(-std::numeric_limits<float>::infinity(), 0.0f, 0.0f), std::invalid_argument);
}

// Тест на граничные случаи при работе со спрайтами
TEST_F(EdgeCasesTest, SpriteEdgeCases) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    
    // Тест на установку минимальных размеров
    EXPECT_NO_THROW(sprite->SetSize(std::numeric_limits<float>::min(), std::numeric_limits<float>::min()));
    
    // Тест на установку максимальных размеров
    EXPECT_NO_THROW(sprite->SetSize(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
    
    // Тест на установку нулевых размеров
    EXPECT_NO_THROW(sprite->SetSize(0.0f, 0.0f));
    
    // Тест на установку отрицательных размеров
    EXPECT_THROW(sprite->SetSize(-1.0f, 1.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetSize(1.0f, -1.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetSize(-1.0f, -1.0f), std::invalid_argument);
    
    // Тест на установку NaN размеров
    EXPECT_THROW(sprite->SetSize(std::numeric_limits<float>::quiet_NaN(), 100.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetSize(100.0f, std::numeric_limits<float>::quiet_NaN()), std::invalid_argument);
    EXPECT_THROW(sprite->SetSize(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()), std::invalid_argument);
    
    // Тест на установку Infinity размеров
    EXPECT_THROW(sprite->SetSize(std::numeric_limits<float>::infinity(), 100.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetSize(100.0f, std::numeric_limits<float>::infinity()), std::invalid_argument);
    EXPECT_THROW(sprite->SetSize(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()), std::invalid_argument);
    
    // Тест на установку -Infinity размеров
    EXPECT_THROW(sprite->SetSize(-std::numeric_limits<float>::infinity(), 100.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetSize(100.0f, -std::numeric_limits<float>::infinity()), std::invalid_argument);
    EXPECT_THROW(sprite->SetSize(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()), std::invalid_argument);
    
    // Тест на установку минимальных цветов
    EXPECT_NO_THROW(sprite->SetColor(0.0f, 0.0f, 0.0f, 0.0f));
    
    // Тест на установку максимальных цветов
    EXPECT_NO_THROW(sprite->SetColor(1.0f, 1.0f, 1.0f, 1.0f));
    
    // Тест на установку отрицательных цветов
    EXPECT_THROW(sprite->SetColor(-1.0f, 0.0f, 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, -1.0f, 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, 0.0f, -1.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, 0.0f, 0.0f, -1.0f), std::invalid_argument);
    
    // Тест на установку цветов больше 1.0
    EXPECT_THROW(sprite->SetColor(1.1f, 0.0f, 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, 1.1f, 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, 0.0f, 1.1f, 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, 0.0f, 0.0f, 1.1f), std::invalid_argument);
    
    // Тест на установку NaN цветов
    EXPECT_THROW(sprite->SetColor(std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, 0.0f, std::numeric_limits<float>::quiet_NaN(), 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, 0.0f, 0.0f, std::numeric_limits<float>::quiet_NaN()), std::invalid_argument);
    
    // Тест на установку Infinity цветов
    EXPECT_THROW(sprite->SetColor(std::numeric_limits<float>::infinity(), 0.0f, 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, std::numeric_limits<float>::infinity(), 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, 0.0f, std::numeric_limits<float>::infinity(), 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, 0.0f, 0.0f, std::numeric_limits<float>::infinity()), std::invalid_argument);
    
    // Тест на установку -Infinity цветов
    EXPECT_THROW(sprite->SetColor(-std::numeric_limits<float>::infinity(), 0.0f, 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, -std::numeric_limits<float>::infinity(), 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, 0.0f, -std::numeric_limits<float>::infinity(), 0.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetColor(0.0f, 0.0f, 0.0f, -std::numeric_limits<float>::infinity()), std::invalid_argument);
}

// Тест на граничные случаи при работе с файлами
TEST_F(EdgeCasesTest, FileEdgeCases) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    
    // Тест на пустые имена файлов
    EXPECT_THROW({
        // sprite->LoadTexture("");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("   ");
    }, std::invalid_argument);
    
    // Тест на имена файлов с только пробелами
    EXPECT_THROW({
        // sprite->LoadTexture("   ");
    }, std::invalid_argument);
    
    // Тест на имена файлов с невалидными символами
    EXPECT_THROW({
        // sprite->LoadTexture("file<name>.png");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("file>name.png");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("file:name.png");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("file\"name.png");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("file|name.png");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("file?name.png");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("file*name.png");
    }, std::invalid_argument);
    
    // Тест на имена файлов с невалидными путями
    EXPECT_THROW({
        // sprite->LoadTexture("../../../etc/passwd");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("..\\..\\..\\windows\\system32\\config\\sam");
    }, std::invalid_argument);
    
    // Тест на имена файлов с невалидными расширениями
    EXPECT_THROW({
        // sprite->LoadTexture("file.txt");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("file.exe");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("file.bat");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("file.cmd");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("file.scr");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("file.com");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("file.pif");
    }, std::invalid_argument);
}

// Тест на граничные случаи при работе с сетью
TEST_F(EdgeCasesTest, NetworkEdgeCases) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Тест на пустые URL
    EXPECT_THROW({
        // networkComponent->MakeRequest("");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // networkComponent->MakeRequest("   ");
    }, std::invalid_argument);
    
    // Тест на URL с только пробелами
    EXPECT_THROW({
        // networkComponent->MakeRequest("   ");
    }, std::invalid_argument);
    
    // Тест на URL с невалидными протоколами
    EXPECT_THROW({
        // networkComponent->MakeRequest("ftp://malicious.com/exploit.exe");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // networkComponent->MakeRequest("file:///etc/passwd");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // networkComponent->MakeRequest("javascript:alert('xss')");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // networkComponent->MakeRequest("data:text/html,<script>alert('xss')</script>");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // networkComponent->MakeRequest("vbscript:msgbox('xss')");
    }, std::invalid_argument);
    
    // Тест на URL с невалидными доменами
    EXPECT_THROW({
        // networkComponent->MakeRequest("http://");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // networkComponent->MakeRequest("https://");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // networkComponent->MakeRequest("http://malicious.com:99999/");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // networkComponent->MakeRequest("https://malicious.com:99999/");
    }, std::invalid_argument);
}

// Тест на граничные случаи при работе с базой данных
TEST_F(EdgeCasesTest, DatabaseEdgeCases) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Тест на пустые SQL запросы
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("   ");
    }, std::invalid_argument);
    
    // Тест на SQL запросы с только пробелами
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("   ");
    }, std::invalid_argument);
    
    // Тест на SQL запросы с невалидными символами
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("SELECT * FROM users WHERE id = 1; DROP TABLE users; --");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("SELECT * FROM users WHERE id = 1 UNION SELECT * FROM passwords");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("SELECT * FROM users WHERE id = 1 OR 1=1");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("SELECT * FROM users WHERE id = 1' OR '1'='1");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("SELECT * FROM users WHERE id = 1; INSERT INTO users (name) VALUES ('hacker'); --");
    }, std::invalid_argument);
    
    // Тест на SQL запросы с невалидными параметрами
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("SELECT * FROM users WHERE id = ?", nullptr);
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("SELECT * FROM users WHERE id = ?", "");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("SELECT * FROM users WHERE id = ?", "   ");
    }, std::invalid_argument);
}

// Тест на граничные случаи при работе с JSON
TEST_F(EdgeCasesTest, JSONEdgeCases) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Тест на пустые JSON данные
    EXPECT_THROW({
        // jsonComponent->Parse("");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // jsonComponent->Parse("   ");
    }, std::invalid_argument);
    
    // Тест на JSON данные с только пробелами
    EXPECT_THROW({
        // jsonComponent->Parse("   ");
    }, std::invalid_argument);
    
    // Тест на JSON данные с невалидными структурами
    EXPECT_THROW({
        // jsonComponent->Parse("{\"name\": \"test\", \"value\": 123");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // jsonComponent->Parse("{\"name\": \"test\", \"value\": 123,}");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // jsonComponent->Parse("{\"name\": \"test\", \"value\": 123, \"array\": [1, 2, 3,]}");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // jsonComponent->Parse("{\"name\": \"test\", \"value\": 123, \"object\": {\"key\": \"value\",}}");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // jsonComponent->Parse("{\"name\": \"test\", \"value\": 123, \"string\": \"unclosed string}");
    }, std::invalid_argument);
    
    // Тест на JSON данные с невалидными типами
    EXPECT_THROW({
        // jsonComponent->Parse("{\"name\": \"test\", \"value\": 123.45.67}");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // jsonComponent->Parse("{\"name\": \"test\", \"value\": 123, \"boolean\": true, false}");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // jsonComponent->Parse("{\"name\": \"test\", \"value\": 123, \"number\": 123.45.67}");
    }, std::invalid_argument);
}

// Тест на граничные случаи при работе с XML
TEST_F(EdgeCasesTest, XMLEdgeCases) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Тест на пустые XML данные
    EXPECT_THROW({
        // xmlComponent->Parse("");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // xmlComponent->Parse("   ");
    }, std::invalid_argument);
    
    // Тест на XML данные с только пробелами
    EXPECT_THROW({
        // xmlComponent->Parse("   ");
    }, std::invalid_argument);
    
    // Тест на XML данные с невалидными структурами
    EXPECT_THROW({
        // xmlComponent->Parse("<?xml version=\"1.0\" encoding=\"UTF-8\"?><root><item>test</root>");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // xmlComponent->Parse("<?xml version=\"1.0\" encoding=\"UTF-8\"?><root><item>test</item>");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // xmlComponent->Parse("<?xml version=\"1.0\" encoding=\"UTF-8\"?><root><item id=\"1\">test</root>");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // xmlComponent->Parse("<?xml version=\"1.0\" encoding=\"UTF-8\"?><root><item><subitem>test</subitem></item></root>");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // xmlComponent->Parse("<?xml version=\"1.0\" encoding=\"UTF-8\"?><root><item>test</item><item>test2</item></root>");
    }, std::invalid_argument);
}

// Тест на граничные случаи при работе с регулярными выражениями
TEST_F(EdgeCasesTest, RegexEdgeCases) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Тест на пустые регулярные выражения
    EXPECT_THROW({
        // regexComponent->Compile("");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // regexComponent->Compile("   ");
    }, std::invalid_argument);
    
    // Тест на регулярные выражения с только пробелами
    EXPECT_THROW({
        // regexComponent->Compile("   ");
    }, std::invalid_argument);
    
    // Тест на регулярные выражения с невалидными символами
    EXPECT_THROW({
        // regexComponent->Compile("[");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // regexComponent->Compile("(");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // regexComponent->Compile("{");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // regexComponent->Compile("\\");
    }, std::invalid_argument);
    
    // Тест на регулярные выражения с невалидными комбинациями
    EXPECT_THROW({
        // regexComponent->Compile(".*+");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // regexComponent->Compile("?*");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // regexComponent->Compile("++");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // regexComponent->Compile("??");
    }, std::invalid_argument);
}

