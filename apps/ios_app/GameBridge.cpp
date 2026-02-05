#include "FastEngine/Engine.h"
#include "FastEngine/Platform/Platform.h"
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
