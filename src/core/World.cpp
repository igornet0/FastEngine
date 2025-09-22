#include "FastEngine/World.h"
#include "FastEngine/Entity.h"
#include "FastEngine/System.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Components/Animator.h"
#include "FastEngine/Components/RigidBody.h"
#include "FastEngine/Components/Collider.h"
#include <algorithm>

namespace FastEngine {
    World::World() = default;
    
    World::~World() {
        m_systems.clear();
        m_entities.clear();
    }
    
    Entity* World::CreateEntity() {
        auto entity = std::make_unique<Entity>(this);
        Entity* ptr = entity.get();
        m_entities.push_back(std::move(entity));
        return ptr;
    }
    
    void World::DestroyEntity(Entity* entity) {
        if (!entity) return;
        
        // Удаляем сущность из списка
        m_entities.erase(
            std::remove_if(m_entities.begin(), m_entities.end(),
                [entity](const std::unique_ptr<Entity>& e) {
                    return e.get() == entity;
                }),
            m_entities.end()
        );
    }
    
    void World::Update(float deltaTime) {
        // Обновляем все системы
        for (auto& system : m_systems) {
            if (system) {
                system->Update(deltaTime);
            }
        }
    }
    
    // Реализация шаблона GetEntitiesWithComponents
    template<typename... ComponentTypes>
    std::vector<Entity*> World::GetEntitiesWithComponents() {
        std::vector<Entity*> result;
        for (auto& entity : m_entities) {
            if (entity->HasComponents<ComponentTypes...>()) {
                result.push_back(entity.get());
            }
        }
        return result;
    }
    
    // Явная инстанциация для часто используемых типов
    template std::vector<Entity*> World::GetEntitiesWithComponents<Sprite, Transform>();
    template std::vector<Entity*> World::GetEntitiesWithComponents<Animator>();
    template std::vector<Entity*> World::GetEntitiesWithComponents<RigidBody>();
    template std::vector<Entity*> World::GetEntitiesWithComponents<Collider>();
}