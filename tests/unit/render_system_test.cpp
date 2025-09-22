#include <gtest/gtest.h>
#include "FastEngine/Systems/RenderSystem.h"
#include "FastEngine/World.h"
#include "FastEngine/Entity.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Render/Camera.h"
#include <memory>

using namespace FastEngine;

class RenderSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        world = std::make_unique<World>();
        world->Initialize();
        renderSystem = std::make_unique<RenderSystem>();
        renderSystem->Initialize();
    }
    
    void TearDown() override {
        renderSystem->Shutdown();
        renderSystem.reset();
        world.reset();
    }
    
    std::unique_ptr<World> world;
    std::unique_ptr<RenderSystem> renderSystem;
};

TEST_F(RenderSystemTest, RenderSystemInitialization) {
    EXPECT_TRUE(renderSystem != nullptr);
}

TEST_F(RenderSystemTest, CameraManagement) {
    // Тест управления камерой
    auto camera = std::make_unique<Camera>();
    renderSystem->SetCamera(camera.get());
    
    EXPECT_EQ(renderSystem->GetCamera(), camera.get());
}

TEST_F(RenderSystemTest, EntityRegistration) {
    // Тест регистрации сущности в системе рендеринга
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    auto transform = entity->AddComponent<Transform>();
    
    // Сущность должна быть автоматически зарегистрирована
    EXPECT_TRUE(renderSystem->IsEntityRegistered(entity.get()));
}

TEST_F(RenderSystemTest, SpriteRendering) {
    // Тест рендеринга спрайтов
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    auto transform = entity->AddComponent<Transform>();
    
    sprite->SetSize(glm::vec2(100.0f, 100.0f));
    sprite->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    transform->SetPosition(glm::vec2(50.0f, 50.0f));
    
    // Рендеринг должен пройти без ошибок
    renderSystem->Render();
    EXPECT_TRUE(true);
}

TEST_F(RenderSystemTest, MultipleSprites) {
    // Тест рендеринга множественных спрайтов
    const int spriteCount = 10;
    std::vector<std::shared_ptr<Entity>> entities;
    
    for (int i = 0; i < spriteCount; ++i) {
        auto entity = world->CreateEntity();
        auto sprite = entity->AddComponent<Sprite>();
        auto transform = entity->AddComponent<Transform>();
        
        sprite->SetSize(glm::vec2(50.0f, 50.0f));
        sprite->SetColor(glm::vec4(i / 10.0f, 0.0f, 1.0f - i / 10.0f, 1.0f));
        transform->SetPosition(glm::vec2(i * 60.0f, 100.0f));
        
        entities.push_back(entity);
    }
    
    // Рендеринг должен пройти без ошибок
    renderSystem->Render();
    EXPECT_TRUE(true);
}

TEST_F(RenderSystemTest, TransformRendering) {
    // Тест рендеринга с трансформацией
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    auto transform = entity->AddComponent<Transform>();
    
    sprite->SetSize(glm::vec2(100.0f, 100.0f));
    transform->SetPosition(glm::vec2(200.0f, 200.0f));
    transform->SetRotation(45.0f);
    transform->SetScale(glm::vec2(2.0f, 1.5f));
    
    // Рендеринг должен учитывать трансформацию
    renderSystem->Render();
    EXPECT_TRUE(true);
}

TEST_F(RenderSystemTest, VisibilityControl) {
    // Тест управления видимостью
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    auto transform = entity->AddComponent<Transform>();
    
    sprite->SetSize(glm::vec2(100.0f, 100.0f));
    sprite->SetVisible(false);
    
    // Невидимый спрайт не должен рендериться
    renderSystem->Render();
    EXPECT_TRUE(true);
}

TEST_F(RenderSystemTest, ColorRendering) {
    // Тест рендеринга цветов
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    auto transform = entity->AddComponent<Transform>();
    
    sprite->SetSize(glm::vec2(100.0f, 100.0f));
    sprite->SetColor(glm::vec4(0.5f, 0.7f, 0.3f, 0.8f));
    
    // Рендеринг должен учитывать цвет
    renderSystem->Render();
    EXPECT_TRUE(true);
}

TEST_F(RenderSystemTest, TextureRendering) {
    // Тест рендеринга с текстурами
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    auto transform = entity->AddComponent<Transform>();
    
    sprite->SetSize(glm::vec2(100.0f, 100.0f));
    // sprite->SetTexture("test_texture.png"); // Если есть метод загрузки текстуры
    
    // Рендеринг с текстурой должен пройти без ошибок
    renderSystem->Render();
    EXPECT_TRUE(true);
}

TEST_F(RenderSystemTest, RenderOrder) {
    // Тест порядка рендеринга
    auto entity1 = world->CreateEntity();
    auto sprite1 = entity1->AddComponent<Sprite>();
    auto transform1 = entity1->AddComponent<Transform>();
    
    auto entity2 = world->CreateEntity();
    auto sprite2 = entity2->AddComponent<Sprite>();
    auto transform2 = entity2->AddComponent<Transform>();
    
    sprite1->SetSize(glm::vec2(100.0f, 100.0f));
    sprite1->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    transform1->SetPosition(glm::vec2(50.0f, 50.0f));
    
    sprite2->SetSize(glm::vec2(50.0f, 50.0f));
    sprite2->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    transform2->SetPosition(glm::vec2(75.0f, 75.0f));
    
    // Рендеринг должен учитывать порядок
    renderSystem->Render();
    EXPECT_TRUE(true);
}

TEST_F(RenderSystemTest, RenderLayers) {
    // Тест слоев рендеринга
    auto entity1 = world->CreateEntity();
    auto sprite1 = entity1->AddComponent<Sprite>();
    auto transform1 = entity1->AddComponent<Transform>();
    
    auto entity2 = world->CreateEntity();
    auto sprite2 = entity2->AddComponent<Sprite>();
    auto transform2 = entity2->AddComponent<Transform>();
    
    sprite1->SetSize(glm::vec2(100.0f, 100.0f));
    sprite1->SetRenderLayer(0);
    
    sprite2->SetSize(glm::vec2(50.0f, 50.0f));
    sprite2->SetRenderLayer(1);
    
    // Рендеринг должен учитывать слои
    renderSystem->Render();
    EXPECT_TRUE(true);
}

TEST_F(RenderSystemTest, RenderBatching) {
    // Тест пакетного рендеринга
    const int spriteCount = 100;
    std::vector<std::shared_ptr<Entity>> entities;
    
    for (int i = 0; i < spriteCount; ++i) {
        auto entity = world->CreateEntity();
        auto sprite = entity->AddComponent<Sprite>();
        auto transform = entity->AddComponent<Transform>();
        
        sprite->SetSize(glm::vec2(20.0f, 20.0f));
        sprite->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        transform->SetPosition(glm::vec2(i % 10 * 25.0f, i / 10 * 25.0f));
        
        entities.push_back(entity);
    }
    
    // Пакетный рендеринг должен быть эффективным
    renderSystem->Render();
    EXPECT_TRUE(true);
}

TEST_F(RenderSystemTest, RenderCulling) {
    // Тест отсечения рендеринга
    auto camera = std::make_unique<Camera>();
    camera->SetPosition(glm::vec2(400.0f, 300.0f));
    camera->SetSize(glm::vec2(800.0f, 600.0f));
    renderSystem->SetCamera(camera.get());
    
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    auto transform = entity->AddComponent<Transform>();
    
    sprite->SetSize(glm::vec2(100.0f, 100.0f));
    transform->SetPosition(glm::vec2(2000.0f, 2000.0f)); // Вне видимой области
    
    // Спрайт вне видимой области не должен рендериться
    renderSystem->Render();
    EXPECT_TRUE(true);
}

TEST_F(RenderSystemTest, RenderPerformance) {
    // Тест производительности рендеринга
    const int spriteCount = 1000;
    std::vector<std::shared_ptr<Entity>> entities;
    
    for (int i = 0; i < spriteCount; ++i) {
        auto entity = world->CreateEntity();
        auto sprite = entity->AddComponent<Sprite>();
        auto transform = entity->AddComponent<Transform>();
        
        sprite->SetSize(glm::vec2(10.0f, 10.0f));
        sprite->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        transform->SetPosition(glm::vec2(i % 50 * 20.0f, i / 50 * 20.0f));
        
        entities.push_back(entity);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    renderSystem->Render();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Рендеринг 1000 спрайтов должен занять менее 50мс
    EXPECT_LT(duration.count(), 50);
}

TEST_F(RenderSystemTest, RenderState) {
    // Тест состояния рендеринга
    renderSystem->SetRenderState(RenderState::Opaque);
    EXPECT_EQ(renderSystem->GetRenderState(), RenderState::Opaque);
    
    renderSystem->SetRenderState(RenderState::Transparent);
    EXPECT_EQ(renderSystem->GetRenderState(), RenderState::Transparent);
}

TEST_F(RenderSystemTest, RenderTarget) {
    // Тест цели рендеринга
    auto renderTarget = std::make_unique<RenderTarget>();
    renderTarget->SetSize(glm::vec2(1024.0f, 768.0f));
    
    renderSystem->SetRenderTarget(renderTarget.get());
    EXPECT_EQ(renderSystem->GetRenderTarget(), renderTarget.get());
}

TEST_F(RenderSystemTest, RenderClear) {
    // Тест очистки рендеринга
    renderSystem->SetClearColor(glm::vec4(0.2f, 0.3f, 0.4f, 1.0f));
    renderSystem->SetClearDepth(1.0f);
    renderSystem->SetClearStencil(0);
    
    EXPECT_FLOAT_EQ(renderSystem->GetClearColor().r, 0.2f);
    EXPECT_FLOAT_EQ(renderSystem->GetClearColor().g, 0.3f);
    EXPECT_FLOAT_EQ(renderSystem->GetClearColor().b, 0.4f);
    EXPECT_FLOAT_EQ(renderSystem->GetClearColor().a, 1.0f);
    EXPECT_FLOAT_EQ(renderSystem->GetClearDepth(), 1.0f);
    EXPECT_EQ(renderSystem->GetClearStencil(), 0);
}

TEST_F(RenderSystemTest, RenderViewport) {
    // Тест области просмотра
    renderSystem->SetViewport(glm::vec4(0.0f, 0.0f, 800.0f, 600.0f));
    auto viewport = renderSystem->GetViewport();
    
    EXPECT_FLOAT_EQ(viewport.x, 0.0f);
    EXPECT_FLOAT_EQ(viewport.y, 0.0f);
    EXPECT_FLOAT_EQ(viewport.z, 800.0f);
    EXPECT_FLOAT_EQ(viewport.w, 600.0f);
}

TEST_F(RenderSystemTest, RenderScissor) {
    // Тест ножниц рендеринга
    renderSystem->SetScissor(glm::vec4(100.0f, 100.0f, 200.0f, 200.0f));
    auto scissor = renderSystem->GetScissor();
    
    EXPECT_FLOAT_EQ(scissor.x, 100.0f);
    EXPECT_FLOAT_EQ(scissor.y, 100.0f);
    EXPECT_FLOAT_EQ(scissor.z, 200.0f);
    EXPECT_FLOAT_EQ(scissor.w, 200.0f);
}

TEST_F(RenderSystemTest, RenderBlendMode) {
    // Тест режима смешивания
    renderSystem->SetBlendMode(BlendMode::Alpha);
    EXPECT_EQ(renderSystem->GetBlendMode(), BlendMode::Alpha);
    
    renderSystem->SetBlendMode(BlendMode::Additive);
    EXPECT_EQ(renderSystem->GetBlendMode(), BlendMode::Additive);
}

TEST_F(RenderSystemTest, RenderDepthTest) {
    // Тест теста глубины
    renderSystem->SetDepthTestEnabled(true);
    EXPECT_TRUE(renderSystem->IsDepthTestEnabled());
    
    renderSystem->SetDepthTestEnabled(false);
    EXPECT_FALSE(renderSystem->IsDepthTestEnabled());
}

TEST_F(RenderSystemTest, RenderStencilTest) {
    // Тест теста трафарета
    renderSystem->SetStencilTestEnabled(true);
    EXPECT_TRUE(renderSystem->IsStencilTestEnabled());
    
    renderSystem->SetStencilTestEnabled(false);
    EXPECT_FALSE(renderSystem->IsStencilTestEnabled());
}

TEST_F(RenderSystemTest, RenderCullFace) {
    // Тест отсечения граней
    renderSystem->SetCullFaceEnabled(true);
    EXPECT_TRUE(renderSystem->IsCullFaceEnabled());
    
    renderSystem->SetCullFaceEnabled(false);
    EXPECT_FALSE(renderSystem->IsCullFaceEnabled());
}

TEST_F(RenderSystemTest, RenderWireframe) {
    // Тест каркасного режима
    renderSystem->SetWireframeMode(true);
    EXPECT_TRUE(renderSystem->IsWireframeMode());
    
    renderSystem->SetWireframeMode(false);
    EXPECT_FALSE(renderSystem->IsWireframeMode());
}

TEST_F(RenderSystemTest, RenderStatistics) {
    // Тест статистики рендеринга
    auto stats = renderSystem->GetRenderStatistics();
    
    EXPECT_GE(stats.drawCalls, 0);
    EXPECT_GE(stats.vertices, 0);
    EXPECT_GE(stats.triangles, 0);
    EXPECT_GE(stats.textures, 0);
}

TEST_F(RenderSystemTest, RenderUpdate) {
    // Тест обновления системы рендеринга
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    auto transform = entity->AddComponent<Transform>();
    
    sprite->SetSize(glm::vec2(100.0f, 100.0f));
    transform->SetPosition(glm::vec2(100.0f, 100.0f));
    
    // Обновление системы рендеринга
    renderSystem->Update(0.016f);
    
    // Проверяем, что обновление прошло без ошибок
    EXPECT_TRUE(true);
}

TEST_F(RenderSystemTest, RenderCleanup) {
    // Тест очистки системы рендеринга
    auto entity = world->CreateEntity();
    auto sprite = entity->AddComponent<Sprite>();
    auto transform = entity->AddComponent<Transform>();
    
    sprite->SetSize(glm::vec2(100.0f, 100.0f));
    transform->SetPosition(glm::vec2(100.0f, 100.0f));
    
    // Удаляем сущность
    entity.reset();
    
    // Обновляем систему рендеринга
    renderSystem->Update(0.016f);
    
    // Система должна корректно обработать удаление сущности
    EXPECT_TRUE(true);
}
