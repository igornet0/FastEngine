#include "FastEngine/Platform/Platform.h"
#include "FastEngine/Platform/Window.h"
#include "FastEngine/Platform/FileSystem.h"
#include "FastEngine/Platform/Timer.h"

#ifdef __ANDROID__
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <time.h>
#include <unistd.h>

#define LOG_TAG "FastEngine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace FastEngine {
    // Android Window Implementation
    class AndroidWindow : public Window {
    public:
        AndroidWindow() : Window() {
            m_nativeWindow = nullptr;
            m_display = EGL_NO_DISPLAY;
            m_surface = EGL_NO_SURFACE;
            m_context = EGL_NO_CONTEXT;
        }
        
        ~AndroidWindow() override {
            Destroy();
        }
        
        bool Create(const WindowConfig& config) override {
            // Получаем нативное окно из JNI
            JNIEnv* env = GetJNIEnv();
            if (!env) {
                LOGE("Failed to get JNI environment");
                return false;
            }
            
            // Получаем ANativeWindow из Java
            jobject surface = GetSurfaceFromJava();
            if (!surface) {
                LOGE("Failed to get surface from Java");
                return false;
            }
            
            ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);
            if (!nativeWindow) {
                LOGE("Failed to get ANativeWindow from surface");
                return false;
            }
            
            // Инициализация EGL
            if (!InitializeEGL(nativeWindow)) {
                LOGE("Failed to initialize EGL");
                return false;
            }
            
            m_nativeWindow = nativeWindow;
            m_width = config.width;
            m_height = config.height;
            m_title = config.title;
            m_fullscreen = config.fullscreen;
            m_vsync = config.vsync;
            m_created = true;
            
            return true;
        }
        
        void Destroy() override {
            if (m_context != EGL_NO_CONTEXT) {
                eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                eglDestroyContext(m_display, m_context);
                m_context = EGL_NO_CONTEXT;
            }
            
            if (m_surface != EGL_NO_SURFACE) {
                eglDestroySurface(m_display, m_surface);
                m_surface = EGL_NO_SURFACE;
            }
            
            if (m_display != EGL_NO_DISPLAY) {
                eglTerminate(m_display);
                m_display = EGL_NO_DISPLAY;
            }
            
            if (m_nativeWindow) {
                ANativeWindow_release((ANativeWindow*)m_nativeWindow);
                m_nativeWindow = nullptr;
            }
            
            m_created = false;
        }
        
        void Show() override {
            // На Android окно всегда видимо
        }
        
        void Hide() override {
            // На Android окно всегда видимо
        }
        
        void SetTitle(const std::string& title) override {
            m_title = title;
            // На Android заголовок окна не отображается
        }
        
        void SetSize(int width, int height) override {
            m_width = width;
            m_height = height;
            
            if (m_nativeWindow) {
                ANativeWindow_setBuffersGeometry((ANativeWindow*)m_nativeWindow, 
                                                width, height, 
                                                WINDOW_FORMAT_RGBA_8888);
            }
        }
        
        void SetFullscreen(bool fullscreen) override {
            m_fullscreen = fullscreen;
            // На Android приложения обычно всегда полноэкранные
        }
        
        void SetVSync(bool enabled) override {
            m_vsync = enabled;
            eglSwapInterval(m_display, enabled ? 1 : 0);
        }
        
        void PollEvents() override {
            // События обрабатываются через JNI
        }
        
    private:
        bool InitializeEGL(ANativeWindow* window) {
            // Получение EGL дисплея
            m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            if (m_display == EGL_NO_DISPLAY) {
                LOGE("Failed to get EGL display");
                return false;
            }
            
            // Инициализация EGL
            EGLint major, minor;
            if (!eglInitialize(m_display, &major, &minor)) {
                LOGE("Failed to initialize EGL");
                return false;
            }
            
            LOGI("EGL initialized: %d.%d", major, minor);
            
            // Выбор конфигурации для OpenGL ES 3.0
            EGLint configAttribs[] = {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_DEPTH_SIZE, 24,
                EGL_STENCIL_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_NONE
            };
            
            EGLConfig config;
            EGLint numConfigs;
            if (!eglChooseConfig(m_display, configAttribs, &config, 1, &numConfigs)) {
                LOGE("Failed to choose EGL config, trying ES 2.0");
                
                // Пробуем OpenGL ES 2.0 если 3.0 недоступен
                EGLint configAttribs2[] = {
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                    EGL_BLUE_SIZE, 8,
                    EGL_GREEN_SIZE, 8,
                    EGL_RED_SIZE, 8,
                    EGL_ALPHA_SIZE, 8,
                    EGL_DEPTH_SIZE, 24,
                    EGL_STENCIL_SIZE, 8,
                    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                    EGL_NONE
                };
                
                if (!eglChooseConfig(m_display, configAttribs2, &config, 1, &numConfigs)) {
                    LOGE("Failed to choose EGL config for ES 2.0");
                    return false;
                }
            }
            
            // Создание поверхности
            EGLint surfaceAttribs[] = { EGL_NONE };
            m_surface = eglCreateWindowSurface(m_display, config, window, surfaceAttribs);
            if (m_surface == EGL_NO_SURFACE) {
                LOGE("Failed to create EGL surface");
                return false;
            }
            
            // Создание контекста
            EGLint contextAttribs[] = {
                EGL_CONTEXT_CLIENT_VERSION, 3,
                EGL_NONE
            };
            
            m_context = eglCreateContext(m_display, config, EGL_NO_CONTEXT, contextAttribs);
            if (m_context == EGL_NO_CONTEXT) {
                // Пробуем ES 2.0 контекст
                EGLint contextAttribs2[] = {
                    EGL_CONTEXT_CLIENT_VERSION, 2,
                    EGL_NONE
                };
                
                m_context = eglCreateContext(m_display, config, EGL_NO_CONTEXT, contextAttribs2);
                if (m_context == EGL_NO_CONTEXT) {
                    LOGE("Failed to create EGL context");
                    return false;
                }
            }
            
            // Активация контекста
            if (!eglMakeCurrent(m_display, m_surface, m_surface, m_context)) {
                LOGE("Failed to make EGL context current");
                return false;
            }
            
            // Проверяем версию OpenGL ES
            const char* version = (const char*)glGetString(GL_VERSION);
            LOGI("OpenGL ES version: %s", version ? version : "Unknown");
            
            return true;
        }
        
        JNIEnv* GetJNIEnv() {
            // Получение JNI environment из глобального состояния
            // В реальном приложении это должно быть передано из Java
            static JavaVM* g_vm = nullptr;
            if (g_vm == nullptr) {
                return nullptr;
            }
            
            JNIEnv* env = nullptr;
            if (g_vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
                return nullptr;
            }
            return env;
        }
        
        jobject GetSurfaceFromJava() {
            // Получение Surface из Java
            // В реальном приложении это должно быть передано из Java
            JNIEnv* env = GetJNIEnv();
            if (!env) {
                return nullptr;
            }
            
            // Здесь должна быть логика получения Surface из Java
            // Пока возвращаем nullptr
            return nullptr;
        }
        
        // Статический метод для установки JNI
        static void SetJavaVM(JavaVM* vm) {
            g_vm = vm;
        }
        
        // Статический метод для установки Surface
        static void SetSurface(jobject surface) {
            g_surface = surface;
        }
        
    private:
        static JavaVM* g_vm;
        static jobject g_surface;
        
        EGLDisplay m_display;
        EGLSurface m_surface;
        EGLContext m_context;
    };
    
    // Инициализация статических переменных
    JavaVM* AndroidWindow::g_vm = nullptr;
    jobject AndroidWindow::g_surface = nullptr;
    
    // Android FileSystem Implementation
    class AndroidFileSystem : public FileSystem {
    public:
        AndroidFileSystem() : m_assetManager(nullptr) {}
        
        bool Initialize() override {
            // Asset Manager должен быть передан из Java
            return true;
        }
        
        void Shutdown() override {
            m_assetManager = nullptr;
        }
        
        void SetAssetManager(AAssetManager* assetManager) {
            m_assetManager = assetManager;
        }
        
        bool FileExists(const std::string& path) override {
            if (m_assetManager) {
                AAsset* asset = AAssetManager_open(m_assetManager, path.c_str(), AASSET_MODE_UNKNOWN);
                if (asset) {
                    AAsset_close(asset);
                    return true;
                }
            }
            
            // Проверка в файловой системе
            return access(path.c_str(), F_OK) == 0;
        }
        
        std::string ReadFile(const std::string& path) override {
            // Сначала пробуем прочитать из assets
            if (m_assetManager) {
                AAsset* asset = AAssetManager_open(m_assetManager, path.c_str(), AASSET_MODE_BUFFER);
                if (asset) {
                    size_t length = AAsset_getLength(asset);
                    const char* buffer = (const char*)AAsset_getBuffer(asset);
                    std::string content(buffer, length);
                    AAsset_close(asset);
                    return content;
                }
            }
            
            // Если не найден в assets, читаем из файловой системы
            FILE* file = fopen(path.c_str(), "rb");
            if (!file) {
                return "";
            }
            
            fseek(file, 0, SEEK_END);
            long length = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            std::string content(length, 0);
            fread(&content[0], 1, length, file);
            fclose(file);
            
            return content;
        }
        
        bool WriteFile(const std::string& path, const std::string& content) override {
            FILE* file = fopen(path.c_str(), "wb");
            if (!file) {
                return false;
            }
            
            size_t written = fwrite(content.c_str(), 1, content.length(), file);
            fclose(file);
            
            return written == content.length();
        }
        
        std::string GetResourcePath(const std::string& resource) override {
            // На Android ресурсы находятся в assets
            return "assets/" + resource;
        }
        
        std::string GetDocumentsPath() override {
            // Получение пути к документам через JNI
            // В реальном приложении это должно быть реализовано через JNI
            return "/data/data/com.yourapp/documents/";
        }
        
    private:
        AAssetManager* m_assetManager;
    };
    
    // Android Timer Implementation
    class AndroidTimer : public Timer {
    public:
        AndroidTimer() : Timer() {
            m_startTime = 0.0f;
            m_lastTime = 0.0f;
            m_deltaTime = 0.0f;
            m_fps = 0.0f;
            m_paused = false;
            m_initialized = false;
        }
        
        bool Initialize() override {
            m_startTime = GetCurrentTime();
            m_lastTime = m_startTime;
            m_initialized = true;
            return true;
        }
        
        void Shutdown() override {
            m_initialized = false;
        }
        
        float GetTime() const override {
            if (!m_initialized) return 0.0f;
            return GetCurrentTime() - m_startTime;
        }
        
        float GetDeltaTime() const override {
            return m_deltaTime;
        }
        
        float GetFPS() const override {
            return m_fps;
        }
        
        void Reset() override {
            m_startTime = GetCurrentTime();
            m_lastTime = m_startTime;
        }
        
        void Pause() override {
            m_paused = true;
        }
        
        void Resume() override {
            m_paused = false;
        }
        
        void Update() override {
            if (!m_initialized || m_paused) return;
            
            float currentTime = GetCurrentTime();
            m_deltaTime = currentTime - m_lastTime;
            m_lastTime = currentTime;
            
            // Обновляем FPS
            static float fpsTimer = 0.0f;
            static int frameCount = 0;
            fpsTimer += m_deltaTime;
            frameCount++;
            
            if (fpsTimer >= 1.0f) {
                m_fps = frameCount / fpsTimer;
                frameCount = 0;
                fpsTimer = 0.0f;
            }
        }
        
    private:
        float GetCurrentTime() const {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return ts.tv_sec + ts.tv_nsec / 1000000000.0f;
        }
    };
    
    // Platform Implementation
    Platform& Platform::GetInstance() {
        static Platform instance;
        return instance;
    }
    
    bool Platform::Initialize() {
        if (m_initialized) {
            return true;
        }
        
        // Инициализация систем
        m_window = std::make_unique<AndroidWindow>();
        m_fileSystem = std::make_unique<AndroidFileSystem>();
        m_timer = std::make_unique<AndroidTimer>();
        
        if (!m_fileSystem->Initialize()) {
            return false;
        }
        
        if (!m_timer->Initialize()) {
            return false;
        }
        
        m_initialized = true;
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
    
    bool Platform::ShouldClose() const {
        // На Android приложение может быть закрыто системой
        return false;
    }
    
    void Platform::SetShouldClose(bool shouldClose) {
        // На Android управление закрытием приложения осуществляется системой
    }
    
    std::string Platform::GetPlatformName() const {
        return "Android";
    }
    
    std::string Platform::GetVersion() const {
        return "1.0.0";
    }
    
    void Platform::SetKeyCallbacks(std::function<void(int)> /* onKeyDown */, std::function<void(int)> /* onKeyUp */) {
        // Android: ввод обрабатывается через touch / JNI
    }
}

#endif // __ANDROID__
