#include "FastEngine/Platform/Platform.h"
#include "FastEngine/Platform/Window.h"
#include "FastEngine/Platform/FileSystem.h"
#include "FastEngine/Platform/Timer.h"

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#define GLES_SILENCE_DEPRECATION 1
#include <UIKit/UIKit.h>
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#include <QuartzCore/QuartzCore.h>
#include <Foundation/Foundation.h>
#include <GLKit/GLKit.h>

// Делегат для GLKView
@interface GLKViewDelegate : NSObject <GLKViewDelegate>
@end

@implementation GLKViewDelegate
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    // Убеждаемся, что контекст текущий (GLKView обычно уже сделал это)
    if (view.context && [EAGLContext currentContext] != view.context) {
        [EAGLContext setCurrentContext:view.context];
    }
    // При смене ориентации drawable меняется — синхронизируем размер окна каждый кадр
    FastEngine::Window* win = FastEngine::Platform::GetInstance().GetWindow();
    if (win) {
        int w = (int)view.drawableWidth;
        int h = (int)view.drawableHeight;
        if (w > 0 && h > 0) {
            win->SetSize(w, h);
        }
    }
    if (FastEngine::Platform::GetInstance().OnDrawFrame) {
        FastEngine::Platform::GetInstance().OnDrawFrame();
    }
}
@end

namespace FastEngine {
    // iOS Window Implementation
    class iOSWindow : public Window {
    public:
        iOSWindow() : Window() {
            m_nativeWindow = nullptr;
            m_glContext = nullptr;
            m_layer = nullptr;
        }
        
        ~iOSWindow() override {
            Destroy();
        }
        
        bool Create(const WindowConfig& config) override {
            @autoreleasepool {
                // Получаем главное окно приложения
                UIWindow* window = [[UIApplication sharedApplication].windows firstObject];
                if (!window) {
                    return false;
                }
                
                // Создаем GLKView для OpenGL ES
                GLKView* glView = [[GLKView alloc] initWithFrame:window.bounds];
                if (!glView) {
                    return false;
                }
                
                // Настраиваем OpenGL ES контекст
                EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
                if (!context) {
                    // Пробуем OpenGL ES 2.0 если 3.0 недоступен
                    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
                    if (!context) {
                        return false;
                    }
                }
                
                // Устанавливаем контекст как текущий
                [EAGLContext setCurrentContext:context];
                
                glView.context = context;
                glView.drawableDepthFormat = GLKViewDrawableDepthFormat24;
                glView.drawableStencilFormat = GLKViewDrawableStencilFormat8;
                glView.enableSetNeedsDisplay = YES;
                glView.multipleTouchEnabled = YES;
                
                // Настраиваем делегат для обработки событий
                glView.delegate = (id<GLKViewDelegate>)[[GLKViewDelegate alloc] init];
                
                // Добавляем view к окну и выводим поверх остальных
                [window addSubview:glView];
                [window bringSubviewToFront:glView];
                glView.frame = window.bounds;
                
                m_nativeWindow = (__bridge void*)glView;
                m_glContext = (__bridge void*)context;
                m_layer = (__bridge void*)glView.layer;
                
                m_width = config.width;
                m_height = config.height;
                m_title = config.title;
                m_fullscreen = config.fullscreen;
                m_vsync = config.vsync;
                m_created = true;
                
                return true;
            }
        }
        
        void Destroy() override {
            if (m_glContext) {
                EAGLContext* context = (__bridge EAGLContext*)m_glContext;
                [EAGLContext setCurrentContext:nil];
                context = nil;
                m_glContext = nullptr;
            }
            
            if (m_nativeWindow) {
                GLKView* glView = (__bridge GLKView*)m_nativeWindow;
                [glView removeFromSuperview];
                glView = nil;
                m_nativeWindow = nullptr;
            }
            
            m_created = false;
        }
        
        void Show() override {
            if (m_nativeWindow) {
                GLKView* glView = (__bridge GLKView*)m_nativeWindow;
                glView.hidden = NO;
            }
        }
        
        void Hide() override {
            if (m_nativeWindow) {
                GLKView* glView = (__bridge GLKView*)m_nativeWindow;
                glView.hidden = YES;
            }
        }
        
        void SetTitle(const std::string& title) override {
            m_title = title;
            // На iOS заголовок окна не отображается
        }
        
        void SetSize(int width, int height) override {
            m_width = width;
            m_height = height;
            
            if (m_nativeWindow) {
                GLKView* glView = (__bridge GLKView*)m_nativeWindow;
                glView.frame = CGRectMake(0, 0, width, height);
            }
        }
        
        void SetFullscreen(bool fullscreen) override {
            m_fullscreen = fullscreen;
            // На iOS приложения обычно всегда полноэкранные
        }
        
        void SetVSync(bool enabled) override {
            m_vsync = enabled;
            // VSync управляется системой iOS
        }
        
        void PollEvents() override {
            // События обрабатываются автоматически iOS
        }
        
    private:
        void* m_glContext;
        void* m_layer;
    };
    
    // iOS FileSystem Implementation
    class iOSFileSystem : public FileSystem {
    public:
        bool Initialize() override {
            return true;
        }
        
        void Shutdown() override {
        }
        
        bool FileExists(const std::string& path) const override {
            @autoreleasepool {
                NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];
                return [[NSFileManager defaultManager] fileExistsAtPath:nsPath];
            }
        }
        
        std::string ReadFile(const std::string& path) const override {
            @autoreleasepool {
                NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];
                NSError* error = nil;
                NSString* content = [NSString stringWithContentsOfFile:nsPath
                                                              encoding:NSUTF8StringEncoding
                                                                 error:&error];
                if (error) {
                    return "";
                }
                return [content UTF8String];
            }
        }
        
        bool WriteFile(const std::string& path, const std::string& content) const override {
            @autoreleasepool {
                NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];
                NSString* nsContent = [NSString stringWithUTF8String:content.c_str()];
                NSError* error = nil;
                BOOL success = [nsContent writeToFile:nsPath
                                           atomically:YES
                                             encoding:NSUTF8StringEncoding
                                                error:&error];
                return success == YES;
            }
        }
        
        std::string GetResourcePath(const std::string& resource) const override {
            @autoreleasepool {
                std::string name = resource;
                std::string dir;
                size_t slash = resource.find_last_of('/');
                if (slash != std::string::npos) {
                    dir = resource.substr(0, slash);
                    name = resource.substr(slash + 1);
                }
                std::string base = name;
                std::string ext;
                size_t dot = name.find_last_of('.');
                if (dot != std::string::npos) {
                    base = name.substr(0, dot);
                    ext = name.substr(dot + 1);
                }
                NSString* nsBase = [NSString stringWithUTF8String:base.c_str()];
                NSString* nsExt = ext.empty() ? nil : [NSString stringWithUTF8String:ext.c_str()];
                NSString* nsDir = dir.empty() ? nil : [NSString stringWithUTF8String:dir.c_str()];
                NSString* path = [[NSBundle mainBundle] pathForResource:nsBase ofType:nsExt inDirectory:nsDir];
                if (path) {
                    return [path UTF8String];
                }
                path = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:resource.c_str()] ofType:nil];
                if (path) {
                    return [path UTF8String];
                }
                return "";
            }
        }
        
        std::string GetDocumentsPath() const override {
            @autoreleasepool {
                NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
                NSString* documentsDirectory = [paths objectAtIndex:0];
                return [documentsDirectory UTF8String];
            }
        }
    };
    
    // iOS Timer Implementation
    class iOSTimer : public Timer {
    public:
        iOSTimer() : Timer() {
            m_startTime = 0.0f;
            m_lastTime = 0.0f;
            m_deltaTime = 0.0f;
            m_fps = 0.0f;
            m_paused = false;
            m_initialized = false;
        }
        
        bool Initialize() override {
            m_startTime = CACurrentMediaTime();
            m_lastTime = m_startTime;
            m_initialized = true;
            return true;
        }
        
        void Shutdown() override {
            m_initialized = false;
        }
        
        float GetTime() const override {
            if (!m_initialized) return 0.0f;
            return CACurrentMediaTime() - m_startTime;
        }
        
        float GetDeltaTime() const override {
            return m_deltaTime;
        }
        
        float GetFPS() const override {
            return m_fps;
        }
        
        void Reset() override {
            m_startTime = CACurrentMediaTime();
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
            
            float currentTime = CACurrentMediaTime();
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
        
        m_window = std::make_unique<iOSWindow>();
        m_fileSystem = std::make_unique<iOSFileSystem>();
        m_timer = std::make_unique<iOSTimer>();
        
        if (!m_fileSystem->Initialize()) {
            return false;
        }
        
        if (!m_timer->Initialize()) {
            return false;
        }
        
        CGRect bounds = [[UIScreen mainScreen] bounds];
        Window::WindowConfig config;
        config.title = "FastEngine";
        config.width = (int)(bounds.size.width * [[UIScreen mainScreen] scale]);
        config.height = (int)(bounds.size.height * [[UIScreen mainScreen] scale]);
        config.fullscreen = true;
        config.resizable = false;
        config.vsync = true;
        if (!m_window->Create(config)) {
            return false;
        }
        
        m_initialized = true;
        return true;
    }
    
    void Platform::Present() {
        // GLKView swaps buffers automatically; no explicit swap needed on iOS
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
            void* native = m_window->GetNativeWindow();
            if (native) {
                EAGLContext* ctx = [(__bridge GLKView*)native context];
                if (ctx) {
                    [EAGLContext setCurrentContext:ctx];
                }
            }
            m_window->PollEvents();
        }
    }
    
    bool Platform::ShouldClose() const {
        // На iOS приложение может быть закрыто системой
        return false;
    }
    
    void Platform::SetShouldClose(bool shouldClose) {
        // На iOS управление закрытием приложения осуществляется системой
    }
    
    std::string Platform::GetPlatformName() const {
        return "iOS";
    }
    
    std::string Platform::GetVersion() const {
        @autoreleasepool {
            NSString* version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
            if (version) {
                return [version UTF8String];
            }
            return "1.0.0";
        }
    }
    
    void Platform::SetKeyCallbacks(std::function<void(int)> /* onKeyDown */, std::function<void(int)> /* onKeyUp */) {
        // iOS: ввод обрабатывается через touch / внешний цикл
    }
}

#endif // TARGET_OS_IPHONE
#endif // __APPLE__
