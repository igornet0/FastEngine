#pragma once

#include <string>
#include <functional>

namespace FastEngine {
    class Window;
    class FileSystem;
    class Timer;
    
    class Platform {
    public:
        static Platform& GetInstance();
        
        // Инициализация и завершение работы
        bool Initialize();
        void Shutdown();
        
        // Получение основных систем
        Window* GetWindow() const { return m_window.get(); }
        FileSystem* GetFileSystem() const { return m_fileSystem.get(); }
        Timer* GetTimer() const { return m_timer.get(); }
        
        // Обработка событий
        void PollEvents();
        
        // Управление состоянием
        bool ShouldClose() const;
        void SetShouldClose(bool shouldClose);
        
        // Получение информации о платформе
        std::string GetPlatformName() const;
        std::string GetVersion() const;
        
    private:
        Platform() = default;
        ~Platform() = default;
        
        std::unique_ptr<Window> m_window;
        std::unique_ptr<FileSystem> m_fileSystem;
        std::unique_ptr<Timer> m_timer;
        
        bool m_initialized;
    };
}
