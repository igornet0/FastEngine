#include "FastEngine/Platform/Window.h"
#include <SDL2/SDL.h>
#include <iostream>

#ifdef __APPLE__
#include <TargetConditionals.h>
#if !TARGET_OS_IPHONE
#include <OpenGL/gl.h>
#endif
#else
#include <GL/gl.h>
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
    
    bool Window::Create(const WindowConfig& config) {
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
    
    void Window::Destroy() {
        if (m_created && m_nativeWindow) {
            SDL_DestroyWindow(static_cast<SDL_Window*>(m_nativeWindow));
            SDL_Quit();
        }
        m_created = false;
        m_nativeWindow = nullptr;
    }
    
    void Window::Show() {
        // Заглушка
    }
    
    void Window::Hide() {
        // Заглушка
    }
    
    void Window::SetTitle(const std::string& title) {
        m_title = title;
    }
    
    void Window::SetSize(int width, int height) {
        m_width = width;
        m_height = height;
    }
    
    void Window::SetFullscreen(bool fullscreen) {
        m_fullscreen = fullscreen;
    }
    
    void Window::SetVSync(bool enabled) {
        m_vsync = enabled;
    }
    
    void Window::PollEvents() {
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
            }
        }
    }
}
