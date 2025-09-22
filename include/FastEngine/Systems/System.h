#pragma once

#include "System.h"

namespace FastEngine {
    class World;
    
    class System {
    public:
        System(World* world) : m_world(world) {}
        virtual ~System() = default;
        
        // Виртуальные методы для инициализации и обновления
        virtual void Initialize() {}
        virtual void Update(float deltaTime) {}
        virtual void Cleanup() {}
        
    protected:
        World* m_world;
    };
}