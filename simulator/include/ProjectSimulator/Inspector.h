#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <glm/glm.hpp>

// Forward declarations
namespace FastEngine {
    class Entity;
    class Component;
    class Transform;
    class Sprite;
    class Animator;
    class RigidBody;
    class Collider;
    class AudioSource;
    class Text;
}

namespace ProjectSimulator {

/**
 * Типы свойств компонентов
 */
enum class PropertyType {
    Float,
    Int,
    String,
    Bool,
    Vector2,
    Vector3,
    Color,
    Enum,
    File
};

/**
 * Свойство компонента
 */
struct ComponentProperty {
    std::string name;
    PropertyType type;
    std::string value;
    std::string defaultValue;
    std::vector<std::string> enumValues; // Для PropertyType::Enum
    std::string fileFilter; // Для PropertyType::File
    bool isReadOnly;
    
    ComponentProperty() : type(PropertyType::String), isReadOnly(false) {}
};

/**
 * Информация о компоненте
 */
struct ComponentInfo {
    std::string name;
    std::string displayName;
    std::vector<ComponentProperty> properties;
    bool isEnabled;
    
    ComponentInfo() : isEnabled(true) {}
};

/**
 * Инспектор компонентов
 * Позволяет редактировать свойства компонентов
 */
class Inspector {
public:
    Inspector();
    ~Inspector();

    /**
     * Инициализация инспектора
     * @return true если инициализация успешна
     */
    bool Initialize();

    /**
     * Обновление инспектора
     * @param deltaTime время между кадрами
     */
    void Update(float deltaTime);

    /**
     * Рендеринг инспектора
     * @param renderer рендерер
     */
    void Render(void* renderer);

    /**
     * Установка сущности для редактирования
     * @param entity сущность
     */
    void SetEntity(FastEngine::Entity* entity);

    /**
     * Получение текущей сущности
     */
    FastEngine::Entity* GetEntity() const { return m_currentEntity; }

    /**
     * Обновление свойств компонентов
     */
    void RefreshProperties();

    /**
     * Установка callback для обновления UI
     * @param callback функция обратного вызова
     */
    void SetUpdateCallback(std::function<void()> callback) { m_updateCallback = callback; }

    /**
     * Получение информации о компонентах
     */
    const std::vector<ComponentInfo>& GetComponents() const { return m_components; }

    /**
     * Добавление компонента к сущности
     * @param componentType тип компонента
     * @return true если компонент добавлен
     */
    bool AddComponent(const std::string& componentType);

    /**
     * Удаление компонента из сущности
     * @param componentType тип компонента
     * @return true если компонент удален
     */
    bool RemoveComponent(const std::string& componentType);

    /**
     * Включение/отключение компонента
     * @param componentType тип компонента
     * @param enabled включен ли компонент
     */
    void SetComponentEnabled(const std::string& componentType, bool enabled);

    /**
     * Получение состояния инспектора
     */
    bool IsInitialized() const { return m_initialized; }
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }

private:
    FastEngine::Entity* m_currentEntity;
    std::vector<ComponentInfo> m_components;
    
    bool m_initialized;
    bool m_visible;
    
    std::function<void()> m_updateCallback;
    
    // Вспомогательные методы
    void LoadTransformProperties();
    void LoadSpriteProperties();
    void LoadAnimatorProperties();
    void LoadRigidBodyProperties();
    void LoadColliderProperties();
    void LoadAudioSourceProperties();
    void LoadTextProperties();
    
    void UpdateTransformProperties();
    void UpdateSpriteProperties();
    void UpdateAnimatorProperties();
    void UpdateRigidBodyProperties();
    void UpdateColliderProperties();
    void UpdateAudioSourceProperties();
    void UpdateTextProperties();
    
    void RenderComponentProperties(const ComponentInfo& component);
    void RenderProperty(const ComponentProperty& property);
    
    bool ValidatePropertyValue(const ComponentProperty& property, const std::string& value);
    void ApplyPropertyValue(const ComponentProperty& property, const std::string& value);
    
    std::string GetPropertyValue(const ComponentProperty& property);
    void SetPropertyValue(const ComponentProperty& property, const std::string& value);
};

} // namespace ProjectSimulator

