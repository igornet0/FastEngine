#include "FastEngine/Entity.h"
#include "FastEngine/Component.h"
#include "FastEngine/World.h"

namespace FastEngine {
    size_t Entity::s_nextId = 0;
    
    Entity::Entity(World* world) 
        : m_world(world)
        , m_id(s_nextId++) {
    }
    
    Entity::~Entity() {
        // Компоненты автоматически удаляются через unique_ptr
    }
}