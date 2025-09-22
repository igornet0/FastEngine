#include "FastEngine/FastEngine.h"
#include <iostream>

using namespace FastEngine;

class Test3DApp : public Engine {
public:
    Test3DApp() : Engine() {
        m_cube = nullptr;
        m_camera = nullptr;
        m_rotation = 0.0f;
    }
    
    ~Test3DApp() = default;
    
    bool Initialize() override {
        if (!Engine::Initialize()) {
            return false;
        }
        
        // Создаем 3D камеру
        m_camera = std::make_unique<Camera>();
        m_camera->SetProjectionType(ProjectionType::Perspective);
        m_camera->SetPosition3D(0.0f, 0.0f, 5.0f);
        m_camera->SetFOV(45.0f);
        m_camera->SetNearPlane(0.1f);
        m_camera->SetFarPlane(100.0f);
        
        // Устанавливаем камеру в рендерер
        GetRenderer()->SetCamera(m_camera.get());
        
        // Создаем куб
        m_cube = std::make_unique<Mesh>();
        *m_cube = Mesh::CreateCube(1.0f);
        
        std::cout << "3D Test App initialized successfully!" << std::endl;
        return true;
    }
    
    void Update(float deltaTime) override {
        Engine::Update(deltaTime);
        
        // Вращаем куб
        m_rotation += deltaTime * 45.0f; // 45 градусов в секунду
    }
    
    void Render() override {
        // Очищаем экран
        GetRenderer()->Clear(0.1f, 0.1f, 0.2f, 1.0f);
        
        // Создаем матрицу трансформации для куба
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::rotate(transform, glm::radians(m_rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(m_rotation * 0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
        
        // Отрисовываем куб
        if (m_cube && m_cube->IsLoaded()) {
            // Здесь нужно будет добавить метод DrawMesh в Renderer
            // GetRenderer()->DrawMesh(m_cube.get(), transform);
        }
        
        Engine::Render();
    }
    
    void Shutdown() override {
        m_cube.reset();
        m_camera.reset();
        Engine::Shutdown();
    }
    
private:
    std::unique_ptr<Mesh> m_cube;
    std::unique_ptr<Camera> m_camera;
    float m_rotation;
};

int main() {
    Test3DApp app;
    
    if (!app.Initialize()) {
        std::cerr << "Failed to initialize 3D test app" << std::endl;
        return -1;
    }
    
    app.Run();
    app.Shutdown();
    
    return 0;
}

