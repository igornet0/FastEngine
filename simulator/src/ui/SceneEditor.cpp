#include "ProjectSimulator/SceneEditor.h"
#include "FastEngine/Engine.h"
#include "FastEngine/World.h"
#include "FastEngine/Entity.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Render/Renderer.h"
#include "FastEngine/Render/Camera.h"
#include <iostream>
#include <fstream>
#include <json/json.h>

namespace ProjectSimulator {

SceneEditor::SceneEditor() 
    : m_engine(nullptr)
    , m_world(nullptr)
    , m_camera(nullptr)
    , m_manipulatorType(ManipulatorType::None)
    , m_viewMode(SceneViewMode::Solid)
    , m_initialized(false)
    , m_enabled(true) {
}

SceneEditor::~SceneEditor() {
    if (m_initialized) {
        Shutdown();
    }
}

bool SceneEditor::Initialize(FastEngine::Engine* engine, FastEngine::World* world) {
    if (m_initialized) {
        return true;
    }
    
    if (!engine || !world) {
        std::cerr << "SceneEditor: Invalid engine or world" << std::endl;
        return false;
    }
    
    m_engine = engine;
    m_world = world;
    m_initialized = true;
    
    std::cout << "SceneEditor initialized successfully" << std::endl;
    return true;
}

void SceneEditor::Update(float deltaTime) {
    if (!m_initialized || !m_enabled) {
        return;
    }
    
    // Обновляем манипуляторы
    UpdateManipulators(deltaTime);
    
    // Обновляем выбранный объект
    if (m_selectedObject.entity && m_selectedObject.isSelected) {
        // Здесь можно добавить логику обновления выбранного объекта
    }
    
    // Вызываем callback для обновления UI
    if (m_updateCallback) {
        m_updateCallback();
    }
}

void SceneEditor::Render(FastEngine::Renderer* renderer) {
    if (!m_initialized || !m_enabled || !renderer) {
        return;
    }
    
    // Рендерим сетку
    RenderGrid(renderer);
    
    // Рендерим объекты сцены
    for (auto* entity : m_sceneObjects) {
        if (entity) {
            // Здесь бы рендерились объекты сцены
            // В реальной реализации это делалось бы через RenderSystem
        }
    }
    
    // Рендерим выделение
    RenderSelection(renderer);
    
    // Рендерим манипуляторы
    RenderManipulators(renderer);
}

bool SceneEditor::HandleInput(float mouseX, float mouseY, int mouseButton, int key) {
    if (!m_initialized || !m_enabled) {
        return false;
    }
    
    // Обработка клика мыши
    if (mouseButton == 0) { // Левая кнопка мыши
        FastEngine::Entity* hitEntity = nullptr;
        if (RaycastToObject(mouseX, mouseY, hitEntity)) {
            SelectObject(hitEntity);
            return true;
        } else {
            DeselectAll();
        }
    }
    
    // Обработка манипуляторов
    if (m_selectedObject.isSelected) {
        HandleManipulatorInput(mouseX, mouseY, mouseButton);
    }
    
    // Обработка клавиатуры
    switch (key) {
        case 'T':
            SetManipulatorType(ManipulatorType::Translate);
            return true;
        case 'R':
            SetManipulatorType(ManipulatorType::Rotate);
            return true;
        case 'S':
            SetManipulatorType(ManipulatorType::Scale);
            return true;
        case 'G':
            SetManipulatorType(ManipulatorType::None);
            return true;
        case 'W':
            SetViewMode(SceneViewMode::Wireframe);
            return true;
        case '1':
            SetViewMode(SceneViewMode::Solid);
            return true;
        case '2':
            SetViewMode(SceneViewMode::Textured);
            return true;
        case '3':
            SetViewMode(SceneViewMode::Lighting);
            return true;
    }
    
    return false;
}

FastEngine::Entity* SceneEditor::CreateObject(const std::string& type, const glm::vec3& position) {
    if (!m_initialized || !m_world) {
        return nullptr;
    }
    
    FastEngine::Entity* entity = m_world->CreateEntity();
    if (!entity) {
        return nullptr;
    }
    
    // Добавляем Transform компонент
    auto* transform = entity->AddComponent<FastEngine::Transform>();
    if (transform) {
        transform->SetPosition(glm::vec2(position.x, position.y));
        transform->SetScale(glm::vec2(1.0f));
    }
    
    // Добавляем компоненты в зависимости от типа
    if (type == "Sprite") {
        auto* sprite = entity->AddComponent<FastEngine::Sprite>("textures/default.png");
        if (sprite) {
            // Настройки спрайта
        }
    }
    
    m_sceneObjects.push_back(entity);
    
    std::cout << "Created object of type: " << type << " at position: " 
              << position.x << ", " << position.y << ", " << position.z << std::endl;
    
    return entity;
}

void SceneEditor::DeleteObject(FastEngine::Entity* entity) {
    if (!entity) {
        return;
    }
    
    // Удаляем из списка объектов сцены
    auto it = std::find(m_sceneObjects.begin(), m_sceneObjects.end(), entity);
    if (it != m_sceneObjects.end()) {
        m_sceneObjects.erase(it);
    }
    
    // Если это выбранный объект, отменяем выбор
    if (m_selectedObject.entity == entity) {
        DeselectAll();
    }
    
    // Удаляем сущность из мира
    m_world->DestroyEntity(entity);
    
    std::cout << "Deleted object" << std::endl;
}

void SceneEditor::SelectObject(FastEngine::Entity* entity) {
    if (!entity) {
        return;
    }
    
    m_selectedObject.entity = entity;
    m_selectedObject.isSelected = true;
    
    // Получаем Transform компонент
    auto* transform = entity->GetComponent<FastEngine::Transform>();
    if (transform) {
        glm::vec2 pos2d = transform->GetPosition();
        glm::vec2 scale2d = transform->GetScale();
        
        m_selectedObject.position = glm::vec3(pos2d.x, pos2d.y, 0.0f);
        m_selectedObject.scale = glm::vec3(scale2d.x, scale2d.y, 1.0f);
        m_selectedObject.rotation = glm::vec3(0.0f); // Transform не имеет rotation в 2D
    }
    
    std::cout << "Selected object" << std::endl;
}

void SceneEditor::DeselectAll() {
    m_selectedObject.entity = nullptr;
    m_selectedObject.isSelected = false;
    
    std::cout << "Deselected all objects" << std::endl;
}

void SceneEditor::ClearScene() {
    // Удаляем все объекты
    for (auto* entity : m_sceneObjects) {
        if (entity) {
            m_world->DestroyEntity(entity);
        }
    }
    
    m_sceneObjects.clear();
    DeselectAll();
    
    std::cout << "Scene cleared" << std::endl;
}

bool SceneEditor::SaveScene(const std::string& filename) {
    if (!m_initialized) {
        return false;
    }
    
    Json::Value root;
    Json::Value objects(Json::arrayValue);
    
    // Сохраняем все объекты сцены
    for (auto* entity : m_sceneObjects) {
        if (!entity) continue;
        
        Json::Value obj;
        obj["id"] = static_cast<int>(reinterpret_cast<uintptr_t>(entity));
        
        // Сохраняем Transform
        auto* transform = entity->GetComponent<FastEngine::Transform>();
        if (transform) {
            glm::vec2 pos = transform->GetPosition();
            glm::vec2 scale = transform->GetScale();
            
            obj["position"]["x"] = pos.x;
            obj["position"]["y"] = pos.y;
            obj["scale"]["x"] = scale.x;
            obj["scale"]["y"] = scale.y;
        }
        
        // Сохраняем Sprite
        auto* sprite = entity->GetComponent<FastEngine::Sprite>();
        if (sprite) {
            obj["type"] = "Sprite";
            obj["texture"] = sprite->GetTexturePath();
        }
        
        objects.append(obj);
    }
    
    root["objects"] = objects;
    root["version"] = "1.0";
    
    // Записываем в файл
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    Json::StreamWriterBuilder builder;
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write(root, &file);
    file.close();
    
    std::cout << "Scene saved to: " << filename << std::endl;
    return true;
}

bool SceneEditor::LoadScene(const std::string& filename) {
    if (!m_initialized) {
        return false;
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << filename << std::endl;
        return false;
    }
    
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errors;
    
    if (!Json::parseFromStream(builder, file, &root, &errors)) {
        std::cerr << "Failed to parse JSON: " << errors << std::endl;
        return false;
    }
    
    // Очищаем текущую сцену
    ClearScene();
    
    // Загружаем объекты
    const Json::Value& objects = root["objects"];
    for (const auto& obj : objects) {
        std::string type = obj.get("type", "Sprite").asString();
        
        glm::vec3 position(0.0f);
        if (obj.isMember("position")) {
            position.x = obj["position"].get("x", 0.0).asFloat();
            position.y = obj["position"].get("y", 0.0).asFloat();
        }
        
        FastEngine::Entity* entity = CreateObject(type, position);
        if (entity) {
            // Применяем масштаб
            if (obj.isMember("scale")) {
                auto* transform = entity->GetComponent<FastEngine::Transform>();
                if (transform) {
                    float scaleX = obj["scale"].get("x", 1.0).asFloat();
                    float scaleY = obj["scale"].get("y", 1.0).asFloat();
                    transform->SetScale(glm::vec2(scaleX, scaleY));
                }
            }
        }
    }
    
    std::cout << "Scene loaded from: " << filename << std::endl;
    return true;
}

void SceneEditor::RenderManipulators(FastEngine::Renderer* renderer) {
    if (!m_selectedObject.isSelected || !renderer) {
        return;
    }
    
    // В реальной реализации здесь бы рендерились манипуляторы
    // (стрелки для перемещения, кольца для поворота, кубы для масштабирования)
    std::cout << "Rendering manipulators for selected object" << std::endl;
}

void SceneEditor::RenderGrid(FastEngine::Renderer* renderer) {
    if (!renderer) {
        return;
    }
    
    // В реальной реализации здесь бы рендерилась сетка
    std::cout << "Rendering grid" << std::endl;
}

void SceneEditor::RenderSelection(FastEngine::Renderer* renderer) {
    if (!m_selectedObject.isSelected || !renderer) {
        return;
    }
    
    // В реальной реализации здесь бы рендерилось выделение объекта
    std::cout << "Rendering selection" << std::endl;
}

bool SceneEditor::RaycastToObject(float mouseX, float mouseY, FastEngine::Entity*& hitEntity) {
    // В реальной реализации здесь бы выполнялся raycast
    // Для простоты возвращаем false
    hitEntity = nullptr;
    return false;
}

glm::vec3 SceneEditor::ScreenToWorld(float mouseX, float mouseY) {
    // В реальной реализации здесь бы выполнялось преобразование экранных координат в мировые
    return glm::vec3(mouseX, mouseY, 0.0f);
}

glm::vec3 SceneEditor::WorldToScreen(const glm::vec3& worldPos) {
    // В реальной реализации здесь бы выполнялось преобразование мировых координат в экранные
    return worldPos;
}

void SceneEditor::UpdateManipulators(float deltaTime) {
    if (!m_selectedObject.isSelected) {
        return;
    }
    
    // Обновляем манипуляторы
    // В реальной реализации здесь бы обновлялись позиции и состояния манипуляторов
}

void SceneEditor::HandleManipulatorInput(float mouseX, float mouseY, int mouseButton) {
    if (!m_selectedObject.isSelected) {
        return;
    }
    
    // Обрабатываем ввод для манипуляторов
    // В реальной реализации здесь бы обрабатывалось перетаскивание манипуляторов
}

} // namespace ProjectSimulator

