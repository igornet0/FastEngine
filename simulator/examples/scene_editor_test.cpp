#include "ProjectSimulator/SceneEditorSimulator.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace ProjectSimulator;

int main() {
    std::cout << "FastEngine Scene Editor Test Starting..." << std::endl;
    
    // Создаем симулятор с редактором сцен
    SceneEditorSimulator simulator;
    
    // Инициализируем
    if (!simulator.Initialize(1200, 800)) {
        std::cerr << "Failed to initialize SceneEditorSimulator" << std::endl;
        return -1;
    }
    
    std::cout << "SceneEditorSimulator initialized successfully!" << std::endl;
    std::cout << "Features available:" << std::endl;
    std::cout << "- Scene Editor with manipulators" << std::endl;
    std::cout << "- Inspector for component editing" << std::endl;
    std::cout << "- Hierarchy for object management" << std::endl;
    std::cout << "- Asset Browser for resource management" << std::endl;
    std::cout << "- Real-time preview" << std::endl;
    
    // Создаем новый проект
    std::string projectName = "TestProject";
    std::string projectPath = "test_project";
    
    if (simulator.CreateNewProject(projectName, projectPath)) {
        std::cout << "Created new project: " << projectName << std::endl;
        
        // Создаем несколько объектов в сцене
        std::cout << "Creating scene objects..." << std::endl;
        
        // Создаем спрайт
        auto sprite1 = simulator.CreateSceneObject("Sprite", glm::vec3(0.0f, 0.0f, 0.0f));
        if (sprite1) {
            std::cout << "Created sprite object" << std::endl;
        }
        
        // Создаем еще один спрайт
        auto sprite2 = simulator.CreateSceneObject("Sprite", glm::vec3(2.0f, 1.0f, 0.0f));
        if (sprite2) {
            std::cout << "Created second sprite object" << std::endl;
        }
        
        // Выбираем первый объект
        simulator.SelectSceneObject(sprite1);
        std::cout << "Selected first sprite object" << std::endl;
        
        // Сохраняем сцену
        if (simulator.SaveScene("MainScene")) {
            std::cout << "Scene saved successfully" << std::endl;
        }
        
        // Запускаем симулятор на 10 секунд
        std::cout << "Running SceneEditorSimulator for 10 seconds..." << std::endl;
        std::cout << "Try switching between edit and simulation modes" << std::endl;
        
        // Запускаем в отдельном потоке
        std::thread simulatorThread([&simulator]() {
            simulator.Run();
        });
        
        // Ждем 10 секунд
        std::this_thread::sleep_for(std::chrono::seconds(10));
        
        // Останавливаем симулятор
        simulator.Shutdown();
        
        // Ждем завершения потока
        if (simulatorThread.joinable()) {
            simulatorThread.join();
        }
        
        std::cout << "SceneEditorSimulator test completed!" << std::endl;
    } else {
        std::cerr << "Failed to create new project" << std::endl;
        return -1;
    }
    
    return 0;
}

