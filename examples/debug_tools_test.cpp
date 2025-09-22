#include "FastEngine/FastEngine.h"
#include "FastEngine/Debug/Console.h"
#include "FastEngine/Debug/Profiler.h"
#include "FastEngine/Debug/Wireframe.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace FastEngine;

class DebugToolsTestApp {
public:
    DebugToolsTestApp() {
        m_engine = nullptr;
    }
    
    ~DebugToolsTestApp() = default;
    
    bool Initialize() {
        // Инициализируем движок
        m_engine = std::make_unique<Engine>();
        if (!m_engine->Initialize("Debug Tools Test", 800, 600)) {
            return false;
        }
        
        // Инициализируем debug tools
        Console::GetInstance().Initialize();
        Profiler::GetInstance().Initialize();
        Wireframe::GetInstance().Initialize();
        
        // Настраиваем консоль
        Console::GetInstance().LogInfo("Debug Tools Test App initialized!");
        Console::GetInstance().LogWarning("This is a warning message");
        Console::GetInstance().LogError("This is an error message");
        Console::GetInstance().LogDebug("This is a debug message");
        
        // Регистрируем пользовательские команды
        Console::GetInstance().RegisterCommand("test", [](const std::vector<std::string>& args) {
            Console::GetInstance().LogInfo("Test command executed with " + std::to_string(args.size()) + " arguments");
        });
        
        Console::GetInstance().RegisterCommand("profile", [](const std::vector<std::string>& args) {
            Profiler::GetInstance().SetEnabled(!Profiler::GetInstance().IsEnabled());
            Console::GetInstance().LogInfo("Profiler " + std::string(Profiler::GetInstance().IsEnabled() ? "enabled" : "disabled"));
        });
        
        Console::GetInstance().RegisterCommand("wireframe", [](const std::vector<std::string>& args) {
            Wireframe::GetInstance().Toggle();
            Console::GetInstance().LogInfo("Wireframe " + std::string(Wireframe::GetInstance().IsEnabled() ? "enabled" : "disabled"));
        });
        
        // Настраиваем wireframe
        Wireframe::GetInstance().AddBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Wireframe::GetInstance().AddCircle(glm::vec3(3.0f, 0.0f, 0.0f), 1.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        Wireframe::GetInstance().AddSphere(glm::vec3(-3.0f, 0.0f, 0.0f), 1.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
        
        std::cout << "Debug Tools Test App initialized successfully!" << std::endl;
        std::cout << "Features tested:" << std::endl;
        std::cout << "- Console with logging and commands" << std::endl;
        std::cout << "- Profiler for performance monitoring" << std::endl;
        std::cout << "- Wireframe for debug visualization" << std::endl;
        std::cout << "Available console commands: help, clear, echo, quit, test, profile, wireframe" << std::endl;
        
        return true;
    }
    
    void Update(float deltaTime) {
        if (m_engine) {
            // Профилируем обновление
            PROFILE_SCOPE("Update");
            
            // Обновляем debug tools
            Console::GetInstance().Update(deltaTime);
            Profiler::GetInstance().Update(deltaTime);
            Wireframe::GetInstance().Update(deltaTime);
            
            // Симулируем некоторую работу
            SimulateWork();
        }
    }
    
    void Render() {
        if (m_engine) {
            // Профилируем рендеринг
            PROFILE_SCOPE("Render");
            
            // Рендерим wireframe
            Wireframe::GetInstance().Render(nullptr);
            
            // Рендерим profiler
            Profiler::GetInstance().Render(nullptr);
        }
    }
    
    void Shutdown() {
        // Экспортируем данные профилировщика
        Profiler::GetInstance().ExportToCSV("profiler_data.csv");
        Profiler::GetInstance().ExportToJSON("profiler_data.json");
        
        // Завершаем debug tools
        Wireframe::GetInstance().Shutdown();
        Profiler::GetInstance().Shutdown();
        Console::GetInstance().Shutdown();
        
        m_engine.reset();
    }
    
private:
    std::unique_ptr<Engine> m_engine;
    
    void SimulateWork() {
        PROFILE_SCOPE("SimulateWork");
        
        // Симулируем некоторую работу
        std::vector<int> data(1000);
        for (int i = 0; i < 1000; i++) {
            data[i] = i * i;
        }
        
        // Симулируем задержку
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
};

int main() {
    std::cout << "FastEngine Debug Tools Test Starting..." << std::endl;
    
    DebugToolsTestApp app;
    if (!app.Initialize()) {
        std::cerr << "Failed to initialize Debug Tools Test app" << std::endl;
        return -1;
    }
    
    std::cout << "Running Debug Tools Test for 10 seconds..." << std::endl;
    std::cout << "Try console commands: help, clear, echo, test, profile, wireframe" << std::endl;
    
    // Запускаем на 10 секунд
    auto start = std::chrono::high_resolution_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - start).count() < 10) {
        
        auto now = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(now - start).count();
        
        app.Update(deltaTime);
        app.Render();
        
        // Симулируем ввод команд
        static int commandCounter = 0;
        if (commandCounter % 100 == 0) {
            switch (commandCounter / 100) {
                case 1:
                    Console::GetInstance().ExecuteCommand("help");
                    break;
                case 2:
                    Console::GetInstance().ExecuteCommand("echo Hello from debug tools!");
                    break;
                case 3:
                    Console::GetInstance().ExecuteCommand("test arg1 arg2");
                    break;
                case 4:
                    Console::GetInstance().ExecuteCommand("profile");
                    break;
                case 5:
                    Console::GetInstance().ExecuteCommand("wireframe");
                    break;
            }
        }
        commandCounter++;
    }
    
    app.Shutdown();
    
    std::cout << "Debug Tools Test finished!" << std::endl;
    std::cout << "Check profiler_data.csv and profiler_data.json for performance data" << std::endl;
    return 0;
}
