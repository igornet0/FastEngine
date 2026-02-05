#pragma once

#include <string>
#include <functional>
#include <memory>

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
        void Present();
        
        // Вызов каждый кадр (для iOS GLKView delegate)
        std::function<void()> OnDrawFrame;
        
        // Управление состоянием
        bool ShouldClose() const;
        void SetShouldClose(bool shouldClose);
        
        // Получение информации о платформе
        std::string GetPlatformName() const;
        std::string GetVersion() const;
        
        // Колбэки клавиш (desktop: проброс из SDL в InputManager)
        void SetKeyCallbacks(std::function<void(int)> onKeyDown, std::function<void(int)> onKeyUp);
        /// Колбэки мыши (desktop: проброс в InputManager как touch id 0). (x,y) в пикселях окна, button: 1=left.
        void SetMouseCallbacks(std::function<void(int, int, int)> onMouseDown,
                               std::function<void(int, int, int)> onMouseUp,
                               std::function<void(int, int)> onMouseMove);
        
    private:
        Platform() = default;
        ~Platform() = default;
        
        std::unique_ptr<Window> m_window;
        std::unique_ptr<FileSystem> m_fileSystem;
        std::unique_ptr<Timer> m_timer;
        
        std::function<void(int)> m_onKeyDown;
        std::function<void(int)> m_onKeyUp;
        std::function<void(int, int, int)> m_onMouseDown;
        std::function<void(int, int, int)> m_onMouseUp;
        std::function<void(int, int)> m_onMouseMove;
        
        bool m_initialized;
        bool m_shouldClose;
    };
}
