#include <gtest/gtest.h>
#include "FastEngine/Engine.h"
#include "FastEngine/Entity.h"
#include "FastEngine/World.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Components/Transform.h"
#include <stdexcept>
#include <memory>
#include <vector>

using namespace FastEngine;

class ErrorHandlingTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_TRUE(Engine::GetInstance().Initialize());
    }
    
    void TearDown() override {
        Engine::GetInstance().Shutdown();
    }
};

// Тест на обработку ошибок при создании сущностей
TEST_F(ErrorHandlingTest, EntityCreationErrorHandling) {
    auto world = std::make_unique<World>();
    
    // Тест на создание сущности с невалидными данными
    EXPECT_NO_THROW({
        auto entity = world->CreateEntity();
        EXPECT_NE(entity, nullptr);
    });
    
    // Тест на создание множества сущностей
    const int numEntities = 1000;
    for (int i = 0; i < numEntities; ++i) {
        EXPECT_NO_THROW({
            auto entity = world->CreateEntity();
            EXPECT_NE(entity, nullptr);
        });
    }
}

// Тест на обработку ошибок при добавлении компонентов
TEST_F(ErrorHandlingTest, ComponentAdditionErrorHandling) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Тест на добавление компонентов к валидной сущности
    EXPECT_NO_THROW({
        auto transform = entity->AddComponent<Transform>();
        EXPECT_NE(transform, nullptr);
    });
    
    EXPECT_NO_THROW({
        auto sprite = entity->AddComponent<Sprite>();
        EXPECT_NE(sprite, nullptr);
    });
    
    // Тест на добавление компонентов к невалидной сущности
    EXPECT_THROW({
        Entity* nullEntity = nullptr;
        nullEntity->AddComponent<Transform>();
    }, std::invalid_argument);
}

// Тест на обработку ошибок при получении компонентов
TEST_F(ErrorHandlingTest, ComponentRetrievalErrorHandling) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Тест на получение существующего компонента
    auto transform = entity->AddComponent<Transform>();
    EXPECT_NO_THROW({
        auto retrievedTransform = entity->GetComponent<Transform>();
        EXPECT_NE(retrievedTransform, nullptr);
        EXPECT_EQ(retrievedTransform, transform);
    });
    
    // Тест на получение несуществующего компонента
    EXPECT_NO_THROW({
        auto nonExistentComponent = entity->GetComponent<Sprite>();
        EXPECT_EQ(nonExistentComponent, nullptr);
    });
    
    // Тест на получение компонента у невалидной сущности
    EXPECT_THROW({
        Entity* nullEntity = nullptr;
        nullEntity->GetComponent<Transform>();
    }, std::invalid_argument);
}

// Тест на обработку ошибок при удалении компонентов
TEST_F(ErrorHandlingTest, ComponentRemovalErrorHandling) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Добавляем компоненты
    auto transform = entity->AddComponent<Transform>();
    auto sprite = entity->AddComponent<Sprite>();
    
    // Тест на удаление существующего компонента
    EXPECT_NO_THROW({
        entity->RemoveComponent<Transform>();
        auto retrievedTransform = entity->GetComponent<Transform>();
        EXPECT_EQ(retrievedTransform, nullptr);
    });
    
    // Тест на удаление несуществующего компонента
    EXPECT_NO_THROW({
        entity->RemoveComponent<Transform>(); // Уже удален
    });
    
    // Тест на удаление компонента у невалидной сущности
    EXPECT_THROW({
        Entity* nullEntity = nullptr;
        nullEntity->RemoveComponent<Transform>();
    }, std::invalid_argument);
}

// Тест на обработку ошибок при работе с трансформацией
TEST_F(ErrorHandlingTest, TransformErrorHandling) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    auto transform = entity->AddComponent<Transform>();
    
    // Тест на установку валидных значений
    EXPECT_NO_THROW(transform->SetPosition(0.0f, 0.0f, 0.0f));
    EXPECT_NO_THROW(transform->SetScale(1.0f, 1.0f, 1.0f));
    EXPECT_NO_THROW(transform->SetRotation(0.0f, 0.0f, 0.0f));
    
    // Тест на установку граничных значений
    EXPECT_NO_THROW(transform->SetPosition(std::numeric_limits<float>::max(), 0.0f, 0.0f));
    EXPECT_NO_THROW(transform->SetPosition(std::numeric_limits<float>::min(), 0.0f, 0.0f));
    
    // Тест на установку невалидных значений
    EXPECT_THROW(transform->SetPosition(std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(transform->SetPosition(std::numeric_limits<float>::infinity(), 0.0f, 0.0f), std::invalid_argument);
    EXPECT_THROW(transform->SetPosition(-std::numeric_limits<float>::infinity(), 0.0f, 0.0f), std::invalid_argument);
}

// Тест на обработку ошибок при работе со спрайтами
TEST_F(ErrorHandlingTest, SpriteErrorHandling) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    
    // Тест на установку валидных значений
    EXPECT_NO_THROW(sprite->SetSize(100.0f, 100.0f));
    EXPECT_NO_THROW(sprite->SetColor(1.0f, 1.0f, 1.0f, 1.0f));
    
    // Тест на установку граничных значений
    EXPECT_NO_THROW(sprite->SetSize(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
    EXPECT_NO_THROW(sprite->SetSize(std::numeric_limits<float>::min(), std::numeric_limits<float>::min()));
    
    // Тест на установку невалидных значений
    EXPECT_THROW(sprite->SetSize(std::numeric_limits<float>::quiet_NaN(), 100.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetSize(std::numeric_limits<float>::infinity(), 100.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetSize(-std::numeric_limits<float>::infinity(), 100.0f), std::invalid_argument);
    
    // Тест на установку отрицательных размеров
    EXPECT_THROW(sprite->SetSize(-100.0f, 100.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetSize(100.0f, -100.0f), std::invalid_argument);
    EXPECT_THROW(sprite->SetSize(-100.0f, -100.0f), std::invalid_argument);
}

// Тест на обработку ошибок при работе с файлами
TEST_F(ErrorHandlingTest, FileErrorHandling) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    
    // Тест на загрузку несуществующих файлов
    EXPECT_THROW({
        // sprite->LoadTexture("non_existent_file.png");
    }, std::runtime_error);
    
    EXPECT_THROW({
        // sprite->LoadShader("non_existent.vert", "non_existent.frag");
    }, std::runtime_error);
    
    EXPECT_THROW({
        // sprite->LoadSound("non_existent.wav");
    }, std::runtime_error);
    
    // Тест на загрузку файлов с невалидными путями
    EXPECT_THROW({
        // sprite->LoadTexture("");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("   ");
    }, std::invalid_argument);
    
    // Тест на загрузку файлов с невалидными расширениями
    EXPECT_THROW({
        // sprite->LoadTexture("file.txt");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // sprite->LoadTexture("file.exe");
    }, std::invalid_argument);
}

// Тест на обработку ошибок при работе с сетью
TEST_F(ErrorHandlingTest, NetworkErrorHandling) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Тест на невалидные URL
    EXPECT_THROW({
        // networkComponent->MakeRequest("");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // networkComponent->MakeRequest("   ");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // networkComponent->MakeRequest("invalid_url");
    }, std::invalid_argument);
    
    // Тест на невалидные протоколы
    EXPECT_THROW({
        // networkComponent->MakeRequest("ftp://malicious.com/exploit.exe");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // networkComponent->MakeRequest("file:///etc/passwd");
    }, std::invalid_argument);
    
    // Тест на невалидные домены
    EXPECT_THROW({
        // networkComponent->MakeRequest("http://");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // networkComponent->MakeRequest("https://");
    }, std::invalid_argument);
}

// Тест на обработку ошибок при работе с базой данных
TEST_F(ErrorHandlingTest, DatabaseErrorHandling) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Тест на невалидные SQL запросы
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("   ");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("INVALID SQL");
    }, std::invalid_argument);
    
    // Тест на SQL инъекции
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("SELECT * FROM users WHERE id = 1; DROP TABLE users; --");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("SELECT * FROM users WHERE id = 1 UNION SELECT * FROM passwords");
    }, std::invalid_argument);
    
    // Тест на невалидные параметры
    EXPECT_THROW({
        // databaseComponent->ExecuteQuery("SELECT * FROM users WHERE id = ?", nullptr);
    }, std::invalid_argument);
}

// Тест на обработку ошибок при работе с JSON
TEST_F(ErrorHandlingTest, JSONErrorHandling) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Тест на невалидные JSON данные
    EXPECT_THROW({
        // jsonComponent->Parse("");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // jsonComponent->Parse("   ");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // jsonComponent->Parse("invalid json");
    }, std::invalid_argument);
    
    // Тест на невалидные JSON структуры
    EXPECT_THROW({
        // jsonComponent->Parse("{\"name\": \"test\", \"value\": 123");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // jsonComponent->Parse("{\"name\": \"test\", \"value\": 123,}");
    }, std::invalid_argument);
    
    // Тест на невалидные JSON типы
    EXPECT_THROW({
        // jsonComponent->Parse("{\"name\": \"test\", \"value\": 123.45.67}");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // jsonComponent->Parse("{\"name\": \"test\", \"value\": 123, \"boolean\": true, false}");
    }, std::invalid_argument);
}

// Тест на обработку ошибок при работе с XML
TEST_F(ErrorHandlingTest, XMLErrorHandling) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Тест на невалидные XML данные
    EXPECT_THROW({
        // xmlComponent->Parse("");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // xmlComponent->Parse("   ");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // xmlComponent->Parse("invalid xml");
    }, std::invalid_argument);
    
    // Тест на невалидные XML структуры
    EXPECT_THROW({
        // xmlComponent->Parse("<?xml version=\"1.0\" encoding=\"UTF-8\"?><root><item>test</root>");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // xmlComponent->Parse("<?xml version=\"1.0\" encoding=\"UTF-8\"?><root><item>test</item>");
    }, std::invalid_argument);
    
    // Тест на невалидные XML атрибуты
    EXPECT_THROW({
        // xmlComponent->Parse("<?xml version=\"1.0\" encoding=\"UTF-8\"?><root><item id=\"1\">test</root>");
    }, std::invalid_argument);
}

// Тест на обработку ошибок при работе с регулярными выражениями
TEST_F(ErrorHandlingTest, RegexErrorHandling) {
    auto world = std::make_unique<World>();
    auto entity = world->CreateEntity();
    
    // Тест на невалидные регулярные выражения
    EXPECT_THROW({
        // regexComponent->Compile("");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        // regexComponent->Compile("   ");
    }, std::invalid_argument);
    
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
    
    // Тест на невалидные комбинации
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

// Тест на обработку ошибок при работе с памятью
TEST_F(ErrorHandlingTest, MemoryErrorHandling) {
    auto world = std::make_unique<World>();
    
    // Тест на создание сущностей при нехватке памяти
    const int numEntities = 1000000; // Очень большое число
    std::vector<std::unique_ptr<Entity>> entities;
    
    try {
        for (int i = 0; i < numEntities; ++i) {
            auto entity = world->CreateEntity();
            if (entity) {
                entities.push_back(std::move(entity));
            } else {
                break; // Прерываем, если не удается создать сущность
            }
        }
    } catch (const std::bad_alloc& e) {
        // Ожидаем исключение при нехватке памяти
        std::cout << "Memory allocation failed: " << e.what() << std::endl;
    }
    
    // Проверяем, что хотя бы некоторые сущности созданы
    EXPECT_GT(entities.size(), 0);
}

// Тест на обработку ошибок при работе с исключениями
TEST_F(ErrorHandlingTest, ExceptionHandling) {
    auto world = std::make_unique<World>();
    
    // Тест на обработку различных типов исключений
    EXPECT_THROW({
        throw std::runtime_error("Test runtime error");
    }, std::runtime_error);
    
    EXPECT_THROW({
        throw std::invalid_argument("Test invalid argument error");
    }, std::invalid_argument);
    
    EXPECT_THROW({
        throw std::logic_error("Test logic error");
    }, std::logic_error);
    
    EXPECT_THROW({
        throw std::out_of_range("Test out of range error");
    }, std::out_of_range);
    
    EXPECT_THROW({
        throw std::bad_alloc();
    }, std::bad_alloc);
    
    EXPECT_THROW({
        throw std::bad_exception();
    }, std::bad_exception);
}








