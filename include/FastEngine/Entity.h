#pragma once

#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace FastEngine {
    class Component;
    class World;
    
    class Entity {
    public:
        Entity(World* world);
        ~Entity();
        
        // Добавление и удаление компонентов
        template<typename T, typename... Args>
        T* AddComponent(Args&&... args) {
            static_assert(std::is_base_of_v<Component, T>, "T must be derived from Component");
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            T* ptr = component.get();
            m_components[std::type_index(typeid(T))] = std::move(component);
            return ptr;
        }
        
        template<typename T>
        T* GetComponent() {
            auto it = m_components.find(std::type_index(typeid(T)));
            if (it != m_components.end()) {
                return static_cast<T*>(it->second.get());
            }
            return nullptr;
        }
        
        template<typename T>
        bool HasComponent() const {
            return m_components.find(std::type_index(typeid(T))) != m_components.end();
        }
        
        template<typename T>
        void RemoveComponent() {
            m_components.erase(std::type_index(typeid(T)));
        }
        
        // Получение всех компонентов
        const std::unordered_map<std::type_index, std::unique_ptr<Component>>& GetComponents() const {
            return m_components;
        }
        
        // ID сущности
        size_t GetID() const { return m_id; }
        
    private:
        World* m_world;
        size_t m_id;
        std::unordered_map<std::type_index, std::unique_ptr<Component>> m_components;
        
        static size_t s_nextId;
    };
}
