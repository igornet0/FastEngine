#pragma once

#include <string>
#include <functional>

namespace FastEngine {
    class Window {
    public:
        struct WindowConfig {
            std::string title;
            int width;
            int height;
            bool fullscreen;
            bool resizable;
            bool vsync;
        };
        
        Window();
        virtual ~Window();
        
        // Создание и уничтожение окна
        virtual bool Create(const WindowConfig& config);
        virtual void Destroy();
        
        // Управление окном
        virtual void Show();
        virtual void Hide();
        virtual void SetTitle(const std::string& title);
        virtual void SetSize(int width, int height);
        virtual void SetFullscreen(bool fullscreen);
        virtual void SetVSync(bool enabled);
        
        // Получение информации
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        bool IsFullscreen() const { return m_fullscreen; }
        bool IsVSyncEnabled() const { return m_vsync; }
        
        // Обработка событий
        virtual void PollEvents();
        
        // Получение нативного окна (для платформо-специфичного кода)
        void* GetNativeWindow() const { return m_nativeWindow; }
        
        // Обратные вызовы для событий
        std::function<void(int, int)> OnResize;
        std::function<void()> OnClose;
        std::function<void(int)> OnKeyDown;
        std::function<void(int)> OnKeyUp;
        /// Мышь: (x, y) в пикселях окна, button: 1=left, 2=middle, 3=right
        std::function<void(int x, int y, int button)> OnMouseDown;
        std::function<void(int x, int y, int button)> OnMouseUp;
        std::function<void(int x, int y)> OnMouseMove;
        
    protected:
        void* m_nativeWindow;
        std::string m_title;
        int m_width;
        int m_height;
        bool m_fullscreen;
        bool m_vsync;
        bool m_created;
    };
}
