#include "FastEngine/Platform/Window.h"
#include <iostream>

#if defined(__APPLE__) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
/* iOS: Window implementation is in Platform_iOS.cpp (iOSWindow). Base class stubs only. */
#else
#include <SDL2/SDL.h>
#ifdef __APPLE__
#include <TargetConditionals.h>
#if !TARGET_OS_IPHONE
#include <OpenGL/gl.h>
#endif
#else
#include <GL/gl.h>
#endif
#endif

namespace FastEngine {
    Window::Window() 
        : m_nativeWindow(nullptr)
        , m_title("")
        , m_width(800)
        , m_height(600)
        , m_fullscreen(false)
        , m_vsync(false)
        , m_created(false) {
    }
    
    Window::~Window() {
        Destroy();
    }
    
#if defined(__APPLE__) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    bool Window::Create(const WindowConfig& config) {
        (void)config;
        return false; /* iOS uses iOSWindow::Create */
    }
    void Window::Destroy() {}
    void Window::Show() {}
    void Window::Hide() {}
    void Window::SetTitle(const std::string& title) { m_title = title; }
    void Window::SetSize(int width, int height) { m_width = width; m_height = height; }
    void Window::SetFullscreen(bool fullscreen) { m_fullscreen = fullscreen; }
    void Window::SetVSync(bool enabled) { m_vsync = enabled; }
    void Window::PollEvents() {}
}
#else
    bool FastEngine::Window::Create(const WindowConfig& config) {
        if (m_created) {
            return true;
        }
        
        // Инициализация SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            return false;
        }
        
        // Настройка OpenGL атрибутов для macOS
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        
        // Создание окна
        Uint32 flags = SDL_WINDOW_OPENGL;
        if (config.fullscreen) {
            flags |= SDL_WINDOW_FULLSCREEN;
        }
        if (config.resizable) {
            flags |= SDL_WINDOW_RESIZABLE;
        }
        
        SDL_Window* window = SDL_CreateWindow(
            config.title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            config.width,
            config.height,
            flags
        );
        
        if (!window) {
            std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }
        
        // Создание OpenGL контекста
        SDL_GLContext glContext = SDL_GL_CreateContext(window);
        if (!glContext) {
            std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        
        // Настройка VSync
        if (config.vsync) {
            SDL_GL_SetSwapInterval(1);
        } else {
            SDL_GL_SetSwapInterval(0);
        }
        
        m_nativeWindow = window;
        m_title = config.title;
        m_width = config.width;
        m_height = config.height;
        m_fullscreen = config.fullscreen;
        m_vsync = config.vsync;
        m_created = true;
        
        std::cout << "Window created successfully: " << config.title << " (" << config.width << "x" << config.height << ")" << std::endl;
        
        return true;
    }
    
    void FastEngine::Window::Destroy() {
        if (m_created && m_nativeWindow) {
            SDL_DestroyWindow(static_cast<SDL_Window*>(m_nativeWindow));
            SDL_Quit();
        }
        m_created = false;
        m_nativeWindow = nullptr;
    }
    
    void FastEngine::Window::Show() {
        // Заглушка
    }
    
    void FastEngine::Window::Hide() {
        // Заглушка
    }
    
    void FastEngine::Window::SetTitle(const std::string& title) {
        m_title = title;
    }
    
    void FastEngine::Window::SetSize(int width, int height) {
        m_width = width;
        m_height = height;
    }
    
    void FastEngine::Window::SetFullscreen(bool fullscreen) {
        m_fullscreen = fullscreen;
    }
    
    void FastEngine::Window::SetVSync(bool enabled) {
        m_vsync = enabled;
    }
    
    void FastEngine::Window::PollEvents() {
        if (!m_created) return;
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    if (OnClose) {
                        OnClose();
                    }
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        m_width = event.window.data1;
                        m_height = event.window.data2;
                        if (OnResize) {
                            OnResize(m_width, m_height);
                        }
                    }
                    break;
                case SDL_KEYDOWN:
                    if (OnKeyDown) {
                        OnKeyDown(static_cast<int>(event.key.keysym.scancode));
                    }
                    break;
                case SDL_KEYUP:
                    if (OnKeyUp) {
                        OnKeyUp(static_cast<int>(event.key.keysym.scancode));
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (OnMouseDown) {
                        OnMouseDown(event.button.x, event.button.y, static_cast<int>(event.button.button));
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (OnMouseUp) {
                        OnMouseUp(event.button.x, event.button.y, static_cast<int>(event.button.button));
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (OnMouseMove) {
                        OnMouseMove(event.motion.x, event.motion.y);
                    }
                    break;
            }
        }
    }
} // namespace FastEngine
#endif /* !iOS */
