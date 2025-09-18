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
        ~Window();
        
        // Создание и уничтожение окна
        bool Create(const WindowConfig& config);
        void Destroy();
        
        // Управление окном
        void Show();
        void Hide();
        void SetTitle(const std::string& title);
        void SetSize(int width, int height);
        void SetFullscreen(bool fullscreen);
        void SetVSync(bool enabled);
        
        // Получение информации
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        bool IsFullscreen() const { return m_fullscreen; }
        bool IsVSyncEnabled() const { return m_vsync; }
        
        // Обработка событий
        void PollEvents();
        
        // Получение нативного окна (для платформо-специфичного кода)
        void* GetNativeWindow() const { return m_nativeWindow; }
        
        // Обратные вызовы для событий
        std::function<void(int, int)> OnResize;
        std::function<void()> OnClose;
        
    private:
        void* m_nativeWindow;
        std::string m_title;
        int m_width;
        int m_height;
        bool m_fullscreen;
        bool m_vsync;
        bool m_created;
    };
}
