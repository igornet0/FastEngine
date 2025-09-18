#pragma once

namespace FastEngine {
    class Component {
    public:
        Component() = default;
        virtual ~Component() = default;
        
        // Виртуальные методы для инициализации и обновления
        virtual void Initialize() {}
        virtual void Update(float deltaTime) {}
        virtual void Cleanup() {}
    };
}
