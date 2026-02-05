#define GLES_SILENCE_DEPRECATION 1
#import "GameViewController.h"
#import <GLKit/GLKit.h>

#include "FastEngine/Engine.h"
#include "FastEngine/Platform/Platform.h"
#include "FastEngine/Platform/Window.h"
#include "Game.h"

static FastEngine::Engine* g_engine = nullptr;
static Game* g_game = nullptr;

extern "C" void StartFastEngine(int width, int height) {
    FastEngine::Platform::GetInstance().OnDrawFrame = []() {
        if (g_game) {
            g_game->Update(g_engine->GetDeltaTime());
        }
        if (g_engine) {
            g_engine->RunOneFrame();
        }
    };

    g_engine = new FastEngine::Engine();
    if (!g_engine->Initialize("FastEngine", width, height)) {
        delete g_engine;
        g_engine = nullptr;
        return;
    }

    g_game = new Game(g_engine);
    g_game->Initialize();
}

extern "C" void ShutdownFastEngine(void) {
    if (g_game) {
        g_game->Shutdown();
        delete g_game;
        g_game = nullptr;
    }
    if (g_engine) {
        g_engine->Shutdown();
        delete g_engine;
        g_engine = nullptr;
    }
}

@interface GameViewController ()
@property (strong, nonatomic) CADisplayLink* displayLink;
@end

@implementation GameViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor clearColor];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    
    CGRect bounds = self.view.bounds;
    CGFloat scale = [[UIScreen mainScreen] scale];
    int width = (int)(bounds.size.width * scale);
    int height = (int)(bounds.size.height * scale);
    
    StartFastEngine(width, height);
    
    // Получаем GLKView из движка (надёжнее, чем поиск по window.subviews)
    GLKView* glView = nil;
    auto* window = FastEngine::Platform::GetInstance().GetWindow();
    if (window && window->GetNativeWindow()) {
        glView = (__bridge GLKView*)window->GetNativeWindow();
    }
    if (!glView) {
        for (UIView* subview in self.view.window.subviews) {
            if ([subview isKindOfClass:[GLKView class]]) {
                glView = (GLKView*)subview;
                break;
            }
        }
    }
    
    if (glView) {
        self.displayLink = [CADisplayLink displayLinkWithTarget:glView selector:@selector(setNeedsDisplay)];
        [self.displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
    }
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.displayLink invalidate];
    self.displayLink = nil;
    ShutdownFastEngine();
}

@end
