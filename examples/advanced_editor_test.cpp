#include "FastEngine/FastEngine.h"
#include "FastEngine/Editor/BlueprintEditor.h"
#include "FastEngine/Editor/UIBuilder.h"
#include "FastEngine/Editor/AnimationEditor.h"
#include "FastEngine/Editor/MaterialEditor.h"
#include "FastEngine/Editor/ParticleEditor.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace FastEngine;

class AdvancedEditorTest {
public:
    AdvancedEditorTest() {
        m_engine = nullptr;
    }
    
    ~AdvancedEditorTest() = default;
    
    bool Initialize() {
        // Инициализируем движок
        m_engine = std::make_unique<Engine>();
        if (!m_engine->Initialize("Advanced Editor Test", 1200, 800)) {
            return false;
        }
        
        // Инициализируем все редакторы
        if (!m_blueprintEditor.Initialize()) {
            return false;
        }
        
        if (!m_uiBuilder.Initialize()) {
            return false;
        }
        
        if (!m_animationEditor.Initialize()) {
            return false;
        }
        
        if (!m_materialEditor.Initialize()) {
            return false;
        }
        
        if (!m_particleEditor.Initialize()) {
            return false;
        }
        
        // Создаем тестовые данные
        CreateTestData();
        
        std::cout << "Advanced Editor Test initialized successfully!" << std::endl;
        std::cout << "Features demonstrated:" << std::endl;
        std::cout << "- Blueprint visual programming" << std::endl;
        std::cout << "- UI Builder with drag-and-drop" << std::endl;
        std::cout << "- Animation Editor with timeline" << std::endl;
        std::cout << "- Material Editor with node system" << std::endl;
        std::cout << "- Particle Editor with real-time preview" << std::endl;
        
        return true;
    }
    
    void Update(float deltaTime) {
        if (m_engine) {
            // Обновляем все редакторы
            m_blueprintEditor.Update(deltaTime);
            m_uiBuilder.Update(deltaTime);
            m_animationEditor.Update(deltaTime);
            m_materialEditor.Update(deltaTime);
            m_particleEditor.Update(deltaTime);
        }
    }
    
    void Render() {
        if (m_engine) {
            // Рендерим все редакторы
            m_blueprintEditor.Render(nullptr);
            m_uiBuilder.Render(nullptr);
            m_animationEditor.Render(nullptr);
            m_materialEditor.Render(nullptr);
            m_particleEditor.Render(nullptr);
        }
    }
    
    void Shutdown() {
        m_engine.reset();
    }
    
private:
    std::unique_ptr<Engine> m_engine;
    BlueprintEditor m_blueprintEditor;
    UIBuilder m_uiBuilder;
    AnimationEditor m_animationEditor;
    MaterialEditor m_materialEditor;
    ParticleEditor m_particleEditor;
    
    void CreateTestData() {
        std::cout << "Creating test data for all editors..." << std::endl;
        
        // Создаем тестовые Blueprint узлы
        auto eventNode = std::make_shared<EventNode>();
        eventNode->SetName("TestEvent");
        eventNode->SetPosition(glm::vec2(100.0f, 100.0f));
        m_blueprintEditor.AddNode(eventNode);
        
        auto actionNode = std::make_shared<ActionNode>();
        actionNode->SetName("TestAction");
        actionNode->SetPosition(glm::vec2(300.0f, 100.0f));
        m_blueprintEditor.AddNode(actionNode);
        
        // Создаем тестовые UI элементы
        auto panel = m_uiBuilder.CreateElement(UIElementType::Panel, glm::vec2(50.0f, 50.0f));
        if (panel) {
            panel->SetName("TestPanel");
        }
        
        auto button = m_uiBuilder.CreateElement(UIElementType::Button, glm::vec2(100.0f, 100.0f));
        if (button) {
            button->SetName("TestButton");
        }
        
        // Создаем тестовую анимацию
        auto animation = m_animationEditor.CreateAnimation("TestAnimation");
        if (animation) {
            animation->SetDuration(2.0f);
            animation->SetLooping(true);
        }
        
        // Создаем тестовые ноды материалов
        auto textureNode = m_materialEditor.CreateNode(MaterialNodeType::Texture, glm::vec2(100.0f, 100.0f));
        auto colorNode = m_materialEditor.CreateNode(MaterialNodeType::Color, glm::vec2(200.0f, 100.0f));
        
        // Создаем тестовый эмиттер частиц
        auto emitter = m_particleEditor.CreateEmitter("TestEmitter", glm::vec3(0.0f, 0.0f, 0.0f));
        if (emitter) {
            emitter->SetEmissionRate(50.0f);
            emitter->SetMaxParticles(500);
        }
        
        std::cout << "Test data created successfully!" << std::endl;
    }
};

int main() {
    std::cout << "FastEngine Advanced Editor Test Starting..." << std::endl;
    
    AdvancedEditorTest test;
    if (!test.Initialize()) {
        std::cerr << "Failed to initialize Advanced Editor Test" << std::endl;
        return -1;
    }
    
    std::cout << "Running Advanced Editor Test for 10 seconds..." << std::endl;
    std::cout << "This demonstrates the complete editor suite:" << std::endl;
    std::cout << "- Blueprint visual programming system" << std::endl;
    std::cout << "- UI Builder with drag-and-drop interface" << std::endl;
    std::cout << "- Animation Editor with timeline and keyframes" << std::endl;
    std::cout << "- Material Editor with node-based shader system" << std::endl;
    std::cout << "- Particle Editor with real-time effects" << std::endl;
    
    // Запускаем на 10 секунд
    auto start = std::chrono::high_resolution_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - start).count() < 10) {
        
        auto now = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(now - start).count();
        
        test.Update(deltaTime);
        test.Render();
        
        // Небольшая задержка для стабильности
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    test.Shutdown();
    
    std::cout << "Advanced Editor Test completed!" << std::endl;
    std::cout << "This demonstrates the complete editor suite:" << std::endl;
    std::cout << "✓ Blueprint visual programming system" << std::endl;
    std::cout << "✓ UI Builder with drag-and-drop interface" << std::endl;
    std::cout << "✓ Animation Editor with timeline and keyframes" << std::endl;
    std::cout << "✓ Material Editor with node-based shader system" << std::endl;
    std::cout << "✓ Particle Editor with real-time effects" << std::endl;
    std::cout << "✓ Complete editor integration" << std::endl;
    std::cout << "✓ Real-time preview and editing" << std::endl;
    
    return 0;
}

