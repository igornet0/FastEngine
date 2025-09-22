#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <functional>

// Forward declarations
namespace FastEngine {
    class Engine;
    class World;
    class Entity;
    class Component;
    class Renderer;
    class Camera;
}

namespace ProjectSimulator {

/**
 * Типы манипуляторов
 */
enum class ManipulatorType {
    None,
    Translate,
    Rotate,
    Scale
};

/**
 * Режимы отображения сцены
 */
enum class SceneViewMode {
    Wireframe,
    Solid,
    Textured,
    Lighting
};

/**
 * Выбранный объект в сцене
 */
struct SelectedObject {
    FastEngine::Entity* entity;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    bool isSelected;
    
    SelectedObject() : entity(nullptr), position(0.0f), rotation(0.0f), scale(1.0f), isSelected(false) {}
};

/**
 * Редактор сцен
 * Управляет визуальным редактированием сцен
 */
class SceneEditor {
public:
    SceneEditor();
    ~SceneEditor();

    /**
     * Инициализация редактора сцен
     * @param engine движок FastEngine
     * @param world мир для редактирования
     * @return true если инициализация успешна
     */
    bool Initialize(FastEngine::Engine* engine, FastEngine::World* world);

    /**
     * Обновление редактора
     * @param deltaTime время между кадрами
     */
    void Update(float deltaTime);

    /**
     * Рендеринг редактора
     * @param renderer рендерер
     */
    void Render(FastEngine::Renderer* renderer);

    /**
     * Обработка ввода
     * @param mouseX позиция мыши X
     * @param mouseY позиция мыши Y
     * @param mouseButton кнопка мыши
     * @param key клавиша
     * @return true если ввод обработан
     */
    bool HandleInput(float mouseX, float mouseY, int mouseButton, int key);

    /**
     * Создание нового объекта в сцене
     * @param type тип объекта
     * @param position позиция
     * @return созданная сущность
     */
    FastEngine::Entity* CreateObject(const std::string& type, const glm::vec3& position = glm::vec3(0.0f));

    /**
     * Удаление объекта из сцены
     * @param entity сущность для удаления
     */
    void DeleteObject(FastEngine::Entity* entity);

    /**
     * Выбор объекта в сцене
     * @param entity сущность для выбора
     */
    void SelectObject(FastEngine::Entity* entity);

    /**
     * Отмена выбора
     */
    void DeselectAll();

    /**
     * Получение выбранного объекта
     */
    SelectedObject& GetSelectedObject() { return m_selectedObject; }

    /**
     * Установка типа манипулятора
     * @param type тип манипулятора
     */
    void SetManipulatorType(ManipulatorType type) { m_manipulatorType = type; }

    /**
     * Получение типа манипулятора
     */
    ManipulatorType GetManipulatorType() const { return m_manipulatorType; }

    /**
     * Установка режима отображения
     * @param mode режим отображения
     */
    void SetViewMode(SceneViewMode mode) { m_viewMode = mode; }

    /**
     * Получение режима отображения
     */
    SceneViewMode GetViewMode() const { return m_viewMode; }

    /**
     * Установка камеры
     * @param camera камера
     */
    void SetCamera(FastEngine::Camera* camera) { m_camera = camera; }

    /**
     * Получение камеры
     */
    FastEngine::Camera* GetCamera() const { return m_camera; }

    /**
     * Установка callback для обновления UI
     * @param callback функция обратного вызова
     */
    void SetUpdateCallback(std::function<void()> callback) { m_updateCallback = callback; }

    /**
     * Получение всех объектов в сцене
     */
    const std::vector<FastEngine::Entity*>& GetSceneObjects() const { return m_sceneObjects; }

    /**
     * Очистка сцены
     */
    void ClearScene();

    /**
     * Сохранение сцены
     * @param filename имя файла
     * @return true если сохранение успешно
     */
    bool SaveScene(const std::string& filename);

    /**
     * Загрузка сцены
     * @param filename имя файла
     * @return true если загрузка успешна
     */
    bool LoadScene(const std::string& filename);

    /**
     * Получение состояния редактора
     */
    bool IsInitialized() const { return m_initialized; }
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }

private:
    FastEngine::Engine* m_engine;
    FastEngine::World* m_world;
    FastEngine::Camera* m_camera;
    
    std::vector<FastEngine::Entity*> m_sceneObjects;
    SelectedObject m_selectedObject;
    
    ManipulatorType m_manipulatorType;
    SceneViewMode m_viewMode;
    
    bool m_initialized;
    bool m_enabled;
    
    std::function<void()> m_updateCallback;
    
    // Вспомогательные методы
    void RenderManipulators(FastEngine::Renderer* renderer);
    void RenderGrid(FastEngine::Renderer* renderer);
    void RenderSelection(FastEngine::Renderer* renderer);
    
    bool RaycastToObject(float mouseX, float mouseY, FastEngine::Entity*& hitEntity);
    glm::vec3 ScreenToWorld(float mouseX, float mouseY);
    glm::vec3 WorldToScreen(const glm::vec3& worldPos);
    
    void UpdateManipulators(float deltaTime);
    void HandleManipulatorInput(float mouseX, float mouseY, int mouseButton);
};

} // namespace ProjectSimulator

