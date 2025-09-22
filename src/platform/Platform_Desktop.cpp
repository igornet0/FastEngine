#include "FastEngine/Platform/Platform.h"
#include "FastEngine/Platform/Window.h"
#include "FastEngine/Platform/FileSystem.h"
#include "FastEngine/Platform/Timer.h"
#include <functional>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/wglext.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if !TARGET_OS_IPHONE
#include <OpenGL/gl.h>
#include <OpenGL/OpenGL.h>
#endif
#else
#include <GL/gl.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#endif

#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <chrono>

namespace FastEngine {
    // Platform Implementation
    Platform& Platform::GetInstance() {
        static Platform instance;
        std::cout << "Platform::GetInstance() called" << std::endl;
        return instance;
    }
    
    bool Platform::Initialize() {
        if (m_initialized) {
            return true;
        }
        
        m_shouldClose = false;
        std::cout << "Platform::Initialize() called" << std::endl;
        
        // Инициализация систем
        m_window = std::make_unique<Window>();
        m_fileSystem = std::make_unique<FileSystem>();
        m_timer = std::make_unique<Timer>();
        
        std::cout << "Systems created" << std::endl;
        
        if (!m_fileSystem->Initialize()) {
            std::cout << "FileSystem initialization failed" << std::endl;
            return false;
        }
        
        std::cout << "FileSystem initialized" << std::endl;
        
        if (!m_timer->Initialize()) {
            std::cout << "Timer initialization failed" << std::endl;
            return false;
        }
        
        std::cout << "Timer initialized" << std::endl;
        
        // Создаем окно
        Window::WindowConfig windowConfig;
        windowConfig.title = "FastEngine";
        windowConfig.width = 800;
        windowConfig.height = 600;
        windowConfig.fullscreen = false;
        windowConfig.resizable = true;
        windowConfig.vsync = true;
        
        if (!m_window->Create(windowConfig)) {
            std::cout << "Window creation failed" << std::endl;
            return false;
        }
        
        // Устанавливаем обработчик закрытия окна
        m_window->OnClose = [this]() {
            m_shouldClose = true;
        };
        
        std::cout << "Window created" << std::endl;
        
        m_initialized = true;
        std::cout << "Platform initialized successfully" << std::endl;
        return true;
    }
    
    void Platform::Shutdown() {
        if (!m_initialized) {
            return;
        }
        
        if (m_timer) {
            m_timer->Shutdown();
        }
        
        if (m_fileSystem) {
            m_fileSystem->Shutdown();
        }
        
        if (m_window) {
            m_window->Destroy();
        }
        
        m_timer.reset();
        m_fileSystem.reset();
        m_window.reset();
        
        m_initialized = false;
    }
    
    void Platform::PollEvents() {
        if (m_window) {
            m_window->PollEvents();
        }
    }
    
    void Platform::Present() {
        if (m_window && m_window->GetNativeWindow()) {
            SDL_GL_SwapWindow(static_cast<SDL_Window*>(m_window->GetNativeWindow()));
        }
    }
    
    bool Platform::ShouldClose() const {
        return m_shouldClose;
    }
    
    void Platform::SetShouldClose(bool shouldClose) {
        // В Desktop версии это обрабатывается через SDL_QUIT событие
    }
    
    std::string Platform::GetPlatformName() const {
        if (!m_initialized) {
            return "Unknown";
        }
#ifdef _WIN32
        return "Windows";
#elif defined(__APPLE__)
        return "macOS";
#else
        return "Linux";
#endif
    }
    
    std::string Platform::GetVersion() const {
        return "1.0.0";
    }
}