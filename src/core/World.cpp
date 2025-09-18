#include "FastEngine/World.h"
#include "FastEngine/Entity.h"
#include "FastEngine/System.h"

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
}
