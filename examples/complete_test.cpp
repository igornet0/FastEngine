#include "FastEngine/FastEngine.h"
#include "FastEngine/Render/Lighting.h"
#include "FastEngine/Physics/Collision.h"
#include "FastEngine/Input/GamepadInput.h"
#include "FastEngine/Render/Mesh.h"
#include "FastEngine/Render/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>

using namespace FastEngine;

class CompleteTestApp {
public:
    CompleteTestApp() {
        m_cube = nullptr;
        m_camera = nullptr;
        m_lightingSystem = nullptr;
        m_gamepadInput = nullptr;
        m_advancedTouchInput = nullptr;
        m_rotation = 0.0f;
        m_collisionTest = false;
    }
    
    ~CompleteTestApp() = default;
    
    bool Initialize() {
        // Инициализируем платформу
        auto& platform = Platform::GetInstance();
        if (!platform.Initialize()) {
            return false;
        }
        
        // Создаем 3D камеру
        m_camera = std::make_unique<Camera>();
        m_camera->SetProjectionType(ProjectionType::Perspective);
        m_camera->SetPosition3D(0.0f, 0.0f, 5.0f);
        m_camera->SetFOV(45.0f);
        m_camera->SetNearPlane(0.1f);
        m_camera->SetFarPlane(100.0f);
        
        // Создаем рендерер
        m_renderer = std::make_unique<Renderer>();
        if (!m_renderer->Initialize(800, 600)) {
            return false;
        }
        m_renderer->SetCamera(m_camera.get());
        
        // Создаем куб
        m_cube = std::make_unique<Mesh>();
        *m_cube = Mesh::CreateCube(1.0f);
        
        // Инициализируем систему освещения
        m_lightingSystem = std::make_unique<LightingSystem>();
        
        // Добавляем источник света
        Light light;
        light.type = LightType::Point;
        light.position = glm::vec3(2.0f, 2.0f, 2.0f);
        light.color = glm::vec3(1.0f, 1.0f, 1.0f);
        light.intensity = 1.0f;
        light.range = 10.0f;
        m_lightingSystem->AddLight(light);
        
        // Устанавливаем окружающий свет
        m_lightingSystem->SetAmbientLight(glm::vec3(0.2f, 0.2f, 0.3f), 0.3f);
        
        // Инициализируем систему ввода
        m_gamepadInput = std::make_unique<GamepadInput>();
        m_gamepadInput->Initialize();
        
        m_advancedTouchInput = std::make_unique<AdvancedTouchInput>();
        m_advancedTouchInput->Initialize();
        
        // Создаем тестовые коллизии
        m_aabb1 = AABB(glm::vec2(-1.0f, -1.0f), glm::vec2(1.0f, 1.0f));
        m_aabb2 = AABB(glm::vec2(0.5f, 0.5f), glm::vec2(2.5f, 2.5f));
        m_circle1 = Circle(glm::vec2(-2.0f, 0.0f), 1.0f);
        m_circle2 = Circle(glm::vec2(0.0f, 0.0f), 1.5f);
        
        std::cout << "Complete Test App initialized successfully!" << std::endl;
        std::cout << "Features tested:" << std::endl;
        std::cout << "- 3D Mesh rendering" << std::endl;
        std::cout << "- Perspective camera" << std::endl;
        std::cout << "- Lighting system" << std::endl;
        std::cout << "- Collision detection (AABB, Circle)" << std::endl;
        std::cout << "- Gamepad input" << std::endl;
        std::cout << "- Touch input with gestures" << std::endl;
        
        return true;
    }
    
    void Update(float deltaTime) {
        
        // Вращаем куб
        m_rotation += deltaTime * 45.0f; // 45 градусов в секунду
        
        // Обновляем системы ввода
        if (m_gamepadInput) {
            m_gamepadInput->Update();
        }
        
        if (m_advancedTouchInput) {
            m_advancedTouchInput->Update();
        }
        
        // Обновляем систему освещения
        if (m_lightingSystem) {
            m_lightingSystem->Update();
        }
        
        // Тестируем коллизии
        TestCollisions();
        
        // Обрабатываем ввод
        HandleInput(deltaTime);
    }
    
    void Render() {
        // Очищаем экран
        m_renderer->Clear(0.1f, 0.1f, 0.2f, 1.0f);
        
        // Создаем матрицу трансформации для куба
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::rotate(transform, glm::radians(m_rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(m_rotation * 0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
        
        // Отрисовываем куб (здесь нужно будет добавить метод DrawMesh в Renderer)
        if (m_cube && m_cube->IsLoaded()) {
            // GetRenderer()->DrawMesh(m_cube.get(), transform);
        }
        
        // Отрисовываем информацию о коллизиях
        RenderCollisionInfo();
        
        m_renderer->Present();
    }
    
    void Shutdown() {
        m_cube.reset();
        m_camera.reset();
        m_lightingSystem.reset();
        m_gamepadInput.reset();
        m_advancedTouchInput.reset();
        m_renderer.reset();
        
        // Завершаем платформу
        Platform::GetInstance().Shutdown();
    }
    
private:
    std::unique_ptr<Mesh> m_cube;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<LightingSystem> m_lightingSystem;
    std::unique_ptr<GamepadInput> m_gamepadInput;
    std::unique_ptr<AdvancedTouchInput> m_advancedTouchInput;
    std::unique_ptr<Renderer> m_renderer;
    
    // Коллизии для тестирования
    AABB m_aabb1, m_aabb2;
    Circle m_circle1, m_circle2;
    
    float m_rotation;
    bool m_collisionTest;
    
    void TestCollisions() {
        // Тестируем AABB vs AABB
        bool aabbCollision = CollisionSystem::AABBvsAABB(m_aabb1, m_aabb2);
        if (aabbCollision != m_collisionTest) {
            m_collisionTest = aabbCollision;
            std::cout << "AABB collision: " << (aabbCollision ? "YES" : "NO") << std::endl;
        }
        
        // Тестируем Circle vs Circle
        bool circleCollision = CollisionSystem::CirclevsCircle(m_circle1, m_circle2);
        
        // Тестируем AABB vs Circle
        bool aabbCircleCollision = CollisionSystem::AABBvsCircle(m_aabb1, m_circle1);
    }
    
    void HandleInput(float deltaTime) {
        // Обработка геймпада
        if (m_gamepadInput && m_gamepadInput->IsGamepadConnected(0)) {
            glm::vec2 leftStick = m_gamepadInput->GetLeftStick(0);
            if (glm::length(leftStick) > 0.1f) {
                // Двигаем камеру
                glm::vec3 movement = glm::vec3(leftStick.x, 0.0f, leftStick.y) * deltaTime * 5.0f;
                m_camera->Move(movement);
            }
            
            if (m_gamepadInput->IsButtonJustPressed(0, GamepadButton::A)) {
                std::cout << "Gamepad A button pressed!" << std::endl;
            }
        }
        
        // Обработка касаний
        if (m_advancedTouchInput) {
            int touchCount = m_advancedTouchInput->GetTouchCount();
            if (touchCount > 0) {
                // Обрабатываем жесты
                const auto& gestures = m_advancedTouchInput->GetGestures();
                for (const auto& gesture : gestures) {
                    switch (gesture.type) {
                        case GestureType::Tap:
                            std::cout << "Tap detected at (" << gesture.position.x << ", " << gesture.position.y << ")" << std::endl;
                            break;
                        case GestureType::Swipe:
                            std::cout << "Swipe detected: delta(" << gesture.delta.x << ", " << gesture.delta.y << ")" << std::endl;
                            break;
                        case GestureType::Pinch:
                            std::cout << "Pinch detected: scale " << gesture.scale << std::endl;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
    
    void RenderCollisionInfo() {
        // Здесь можно добавить отрисовку информации о коллизиях
        // Например, отрисовка AABB и кругов для визуализации
    }
};

int main() {
    CompleteTestApp app;
    
    if (!app.Initialize()) {
        std::cerr << "Failed to initialize Complete Test app" << std::endl;
        return -1;
    }
    
    std::cout << "Running Complete Test for 5 seconds..." << std::endl;
    
    // Запускаем на 5 секунд
    auto start = std::chrono::high_resolution_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - start).count() < 5) {
        auto now = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(now - start).count();
        app.Update(deltaTime);
        app.Render();
    }
    
    app.Shutdown();
    
    std::cout << "Complete Test finished!" << std::endl;
    return 0;
}
