#include "ProjectSimulator/Inspector.h"
#include "FastEngine/Entity.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Components/Animator.h"
#include "FastEngine/Components/RigidBody.h"
#include "FastEngine/Components/Collider.h"
#include "FastEngine/Components/AudioSource.h"
#include "FastEngine/Components/Text.h"
#include <iostream>
#include <sstream>

namespace ProjectSimulator {

Inspector::Inspector() 
    : m_currentEntity(nullptr)
    , m_initialized(false)
    , m_visible(true) {
}

Inspector::~Inspector() {
    if (m_initialized) {
        // Очистка ресурсов
    }
}

bool Inspector::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    m_initialized = true;
    
    std::cout << "Inspector initialized successfully" << std::endl;
    return true;
}

void Inspector::Update(float deltaTime) {
    if (!m_initialized || !m_visible) {
        return;
    }
    
    // Обновляем свойства компонентов
    if (m_currentEntity) {
        RefreshProperties();
    }
    
    // Вызываем callback для обновления UI
    if (m_updateCallback) {
        m_updateCallback();
    }
}

void Inspector::Render(void* renderer) {
    if (!m_initialized || !m_visible) {
        return;
    }
    
    // В реальной реализации здесь бы рендерился UI инспектора
    // Для простоты выводим информацию в консоль
    if (m_currentEntity) {
        std::cout << "=== Inspector ===" << std::endl;
        std::cout << "Entity: " << m_currentEntity << std::endl;
        
        for (const auto& component : m_components) {
            std::cout << "Component: " << component.displayName 
                      << " (enabled: " << (component.isEnabled ? "true" : "false") << ")" << std::endl;
            
            for (const auto& property : component.properties) {
                std::cout << "  " << property.name << ": " << property.value << std::endl;
            }
        }
        std::cout << "================" << std::endl;
    }
}

void Inspector::SetEntity(FastEngine::Entity* entity) {
    m_currentEntity = entity;
    RefreshProperties();
}

void Inspector::RefreshProperties() {
    if (!m_currentEntity) {
        m_components.clear();
        return;
    }
    
    m_components.clear();
    
    // Загружаем свойства всех компонентов
    LoadTransformProperties();
    LoadSpriteProperties();
    LoadAnimatorProperties();
    LoadRigidBodyProperties();
    LoadColliderProperties();
    LoadAudioSourceProperties();
    LoadTextProperties();
}

bool Inspector::AddComponent(const std::string& componentType) {
    if (!m_currentEntity) {
        return false;
    }
    
    if (componentType == "Transform") {
        m_currentEntity->AddComponent<FastEngine::Transform>();
    } else if (componentType == "Sprite") {
        m_currentEntity->AddComponent<FastEngine::Sprite>("textures/default.png");
    } else if (componentType == "Animator") {
        m_currentEntity->AddComponent<FastEngine::Animator>();
    } else if (componentType == "RigidBody") {
        m_currentEntity->AddComponent<FastEngine::RigidBody>();
    } else if (componentType == "Collider") {
        m_currentEntity->AddComponent<FastEngine::Collider>();
    } else if (componentType == "AudioSource") {
        m_currentEntity->AddComponent<FastEngine::AudioSource>();
    } else if (componentType == "Text") {
        m_currentEntity->AddComponent<FastEngine::Text>();
    } else {
        std::cerr << "Unknown component type: " << componentType << std::endl;
        return false;
    }
    
    RefreshProperties();
    std::cout << "Added component: " << componentType << std::endl;
    return true;
}

bool Inspector::RemoveComponent(const std::string& componentType) {
    if (!m_currentEntity) {
        return false;
    }
    
    // В реальной реализации здесь бы удалялся компонент
    // FastEngine пока не поддерживает удаление компонентов
    std::cout << "Remove component: " << componentType << " (not implemented)" << std::endl;
    return false;
}

void Inspector::SetComponentEnabled(const std::string& componentType, bool enabled) {
    // В реальной реализации здесь бы включался/отключался компонент
    std::cout << "Set component " << componentType << " enabled: " << enabled << std::endl;
}

void Inspector::LoadTransformProperties() {
    if (!m_currentEntity) return;
    
    auto* transform = m_currentEntity->GetComponent<FastEngine::Transform>();
    if (!transform) return;
    
    ComponentInfo info;
    info.name = "Transform";
    info.displayName = "Transform";
    info.isEnabled = true;
    
    // Position
    ComponentProperty posX;
    posX.name = "Position X";
    posX.type = PropertyType::Float;
    posX.value = std::to_string(transform->GetPosition().x);
    posX.defaultValue = "0.0";
    info.properties.push_back(posX);
    
    ComponentProperty posY;
    posY.name = "Position Y";
    posY.type = PropertyType::Float;
    posY.value = std::to_string(transform->GetPosition().y);
    posY.defaultValue = "0.0";
    info.properties.push_back(posY);
    
    // Scale
    ComponentProperty scaleX;
    scaleX.name = "Scale X";
    scaleX.type = PropertyType::Float;
    scaleX.value = std::to_string(transform->GetScale().x);
    scaleX.defaultValue = "1.0";
    info.properties.push_back(scaleX);
    
    ComponentProperty scaleY;
    scaleY.name = "Scale Y";
    scaleY.type = PropertyType::Float;
    scaleY.value = std::to_string(transform->GetScale().y);
    scaleY.defaultValue = "1.0";
    info.properties.push_back(scaleY);
    
    m_components.push_back(info);
}

void Inspector::LoadSpriteProperties() {
    if (!m_currentEntity) return;
    
    auto* sprite = m_currentEntity->GetComponent<FastEngine::Sprite>();
    if (!sprite) return;
    
    ComponentInfo info;
    info.name = "Sprite";
    info.displayName = "Sprite";
    info.isEnabled = true;
    
    // Texture
    ComponentProperty texture;
    texture.name = "Texture";
    texture.type = PropertyType::File;
    texture.value = sprite->GetTexturePath();
    texture.defaultValue = "textures/default.png";
    texture.fileFilter = "Image Files (*.png *.jpg *.bmp)";
    info.properties.push_back(texture);
    
    // Color
    ComponentProperty colorR;
    colorR.name = "Color R";
    colorR.type = PropertyType::Float;
    colorR.value = "1.0";
    colorR.defaultValue = "1.0";
    info.properties.push_back(colorR);
    
    ComponentProperty colorG;
    colorG.name = "Color G";
    colorG.type = PropertyType::Float;
    colorG.value = "1.0";
    colorG.defaultValue = "1.0";
    info.properties.push_back(colorG);
    
    ComponentProperty colorB;
    colorB.name = "Color B";
    colorB.type = PropertyType::Float;
    colorB.value = "1.0";
    colorB.defaultValue = "1.0";
    info.properties.push_back(colorB);
    
    ComponentProperty colorA;
    colorA.name = "Color A";
    colorA.type = PropertyType::Float;
    colorA.value = "1.0";
    colorA.defaultValue = "1.0";
    info.properties.push_back(colorA);
    
    m_components.push_back(info);
}

void Inspector::LoadAnimatorProperties() {
    if (!m_currentEntity) return;
    
    auto* animator = m_currentEntity->GetComponent<FastEngine::Animator>();
    if (!animator) return;
    
    ComponentInfo info;
    info.name = "Animator";
    info.displayName = "Animator";
    info.isEnabled = true;
    
    // Animation Speed
    ComponentProperty speed;
    speed.name = "Speed";
    speed.type = PropertyType::Float;
    speed.value = "1.0";
    speed.defaultValue = "1.0";
    info.properties.push_back(speed);
    
    // Loop
    ComponentProperty loop;
    loop.name = "Loop";
    loop.type = PropertyType::Bool;
    loop.value = "true";
    loop.defaultValue = "true";
    info.properties.push_back(loop);
    
    m_components.push_back(info);
}

void Inspector::LoadRigidBodyProperties() {
    if (!m_currentEntity) return;
    
    auto* rigidBody = m_currentEntity->GetComponent<FastEngine::RigidBody>();
    if (!rigidBody) return;
    
    ComponentInfo info;
    info.name = "RigidBody";
    info.displayName = "Rigid Body";
    info.isEnabled = true;
    
    // Body Type
    ComponentProperty bodyType;
    bodyType.name = "Body Type";
    bodyType.type = PropertyType::Enum;
    bodyType.value = "Dynamic";
    bodyType.defaultValue = "Dynamic";
    bodyType.enumValues = {"Static", "Dynamic", "Kinematic"};
    info.properties.push_back(bodyType);
    
    // Mass
    ComponentProperty mass;
    mass.name = "Mass";
    mass.type = PropertyType::Float;
    mass.value = "1.0";
    mass.defaultValue = "1.0";
    info.properties.push_back(mass);
    
    // Gravity Scale
    ComponentProperty gravityScale;
    gravityScale.name = "Gravity Scale";
    gravityScale.type = PropertyType::Float;
    gravityScale.value = "1.0";
    gravityScale.defaultValue = "1.0";
    info.properties.push_back(gravityScale);
    
    m_components.push_back(info);
}

void Inspector::LoadColliderProperties() {
    if (!m_currentEntity) return;
    
    auto* collider = m_currentEntity->GetComponent<FastEngine::Collider>();
    if (!collider) return;
    
    ComponentInfo info;
    info.name = "Collider";
    info.displayName = "Collider";
    info.isEnabled = true;
    
    // Collider Type
    ComponentProperty colliderType;
    colliderType.name = "Type";
    colliderType.type = PropertyType::Enum;
    colliderType.value = "Box";
    colliderType.defaultValue = "Box";
    colliderType.enumValues = {"Box", "Circle", "Polygon"};
    info.properties.push_back(colliderType);
    
    // Is Trigger
    ComponentProperty isTrigger;
    isTrigger.name = "Is Trigger";
    isTrigger.type = PropertyType::Bool;
    isTrigger.value = "false";
    isTrigger.defaultValue = "false";
    info.properties.push_back(isTrigger);
    
    m_components.push_back(info);
}

void Inspector::LoadAudioSourceProperties() {
    if (!m_currentEntity) return;
    
    auto* audioSource = m_currentEntity->GetComponent<FastEngine::AudioSource>();
    if (!audioSource) return;
    
    ComponentInfo info;
    info.name = "AudioSource";
    info.displayName = "Audio Source";
    info.isEnabled = true;
    
    // Audio Clip
    ComponentProperty clip;
    clip.name = "Audio Clip";
    clip.type = PropertyType::File;
    clip.value = "";
    clip.defaultValue = "";
    clip.fileFilter = "Audio Files (*.wav *.mp3 *.ogg)";
    info.properties.push_back(clip);
    
    // Volume
    ComponentProperty volume;
    volume.name = "Volume";
    volume.type = PropertyType::Float;
    volume.value = "1.0";
    volume.defaultValue = "1.0";
    info.properties.push_back(volume);
    
    // Loop
    ComponentProperty loop;
    loop.name = "Loop";
    loop.type = PropertyType::Bool;
    loop.value = "false";
    loop.defaultValue = "false";
    info.properties.push_back(loop);
    
    m_components.push_back(info);
}

void Inspector::LoadTextProperties() {
    if (!m_currentEntity) return;
    
    auto* text = m_currentEntity->GetComponent<FastEngine::Text>();
    if (!text) return;
    
    ComponentInfo info;
    info.name = "Text";
    info.displayName = "Text";
    info.isEnabled = true;
    
    // Text Content
    ComponentProperty content;
    content.name = "Text";
    content.type = PropertyType::String;
    content.value = "";
    content.defaultValue = "";
    info.properties.push_back(content);
    
    // Font Size
    ComponentProperty fontSize;
    fontSize.name = "Font Size";
    fontSize.type = PropertyType::Int;
    fontSize.value = "16";
    fontSize.defaultValue = "16";
    info.properties.push_back(fontSize);
    
    // Color
    ComponentProperty colorR;
    colorR.name = "Color R";
    colorR.type = PropertyType::Float;
    colorR.value = "1.0";
    colorR.defaultValue = "1.0";
    info.properties.push_back(colorR);
    
    ComponentProperty colorG;
    colorG.name = "Color G";
    colorG.type = PropertyType::Float;
    colorG.value = "1.0";
    colorG.defaultValue = "1.0";
    info.properties.push_back(colorG);
    
    ComponentProperty colorB;
    colorB.name = "Color B";
    colorB.type = PropertyType::Float;
    colorB.value = "1.0";
    colorB.defaultValue = "1.0";
    info.properties.push_back(colorB);
    
    m_components.push_back(info);
}

void Inspector::RenderComponentProperties(const ComponentInfo& component) {
    // В реальной реализации здесь бы рендерились свойства компонента
    std::cout << "Rendering component: " << component.displayName << std::endl;
}

void Inspector::RenderProperty(const ComponentProperty& property) {
    // В реальной реализации здесь бы рендерилось свойство
    std::cout << "Rendering property: " << property.name << " = " << property.value << std::endl;
}

bool Inspector::ValidatePropertyValue(const ComponentProperty& property, const std::string& value) {
    // Валидация значения свойства
    switch (property.type) {
        case PropertyType::Float:
            try {
                std::stof(value);
                return true;
            } catch (...) {
                return false;
            }
        case PropertyType::Int:
            try {
                std::stoi(value);
                return true;
            } catch (...) {
                return false;
            }
        case PropertyType::Bool:
            return value == "true" || value == "false";
        default:
            return true;
    }
}

void Inspector::ApplyPropertyValue(const ComponentProperty& property, const std::string& value) {
    if (!ValidatePropertyValue(property, value)) {
        std::cerr << "Invalid property value: " << property.name << " = " << value << std::endl;
        return;
    }
    
    // Применяем значение к компоненту
    // В реальной реализации здесь бы обновлялись свойства компонентов
    std::cout << "Applied property: " << property.name << " = " << value << std::endl;
}

std::string Inspector::GetPropertyValue(const ComponentProperty& property) {
    return property.value;
}

void Inspector::SetPropertyValue(const ComponentProperty& property, const std::string& value) {
    if (ValidatePropertyValue(property, value)) {
        // В реальной реализации здесь бы обновлялось значение свойства
        std::cout << "Set property: " << property.name << " = " << value << std::endl;
    }
}

} // namespace ProjectSimulator

