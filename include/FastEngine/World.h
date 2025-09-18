#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>

namespace FastEngine {
    class Entity;
    class Component;
    class System;
    
    class World {
    public:
        World();
        ~World();
        
        // Управление сущностями
        Entity* CreateEntity();
        void DestroyEntity(Entity* entity);
        
        // Управление системами
        template<typename T, typename... Args>
        T* AddSystem(Args&&... args) {
            static_assert(std::is_base_of_v<System, T>, "T must be derived from System");
            auto system = std::make_unique<T>(std::forward<Args>(args)...);
            T* ptr = system.get();
            m_systems.push_back(std::move(system));
            return ptr;
        }
        
        template<typename T>
        T* GetSystem() {
            for (auto& system : m_systems) {
                if (auto* ptr = dynamic_cast<T*>(system.get())) {
                    return ptr;
                }
            }
            return nullptr;
        }
        
        // Обновление систем
        void Update(float deltaTime);
        
        // Получение всех сущностей
        const std::vector<std::unique_ptr<Entity>>& GetEntities() const { return m_entities; }
        
    private:
        std::vector<std::unique_ptr<Entity>> m_entities;
        std::vector<std::unique_ptr<System>> m_systems;
    };
}
