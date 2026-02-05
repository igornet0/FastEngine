#include "Game.h"
#include "GameUpdateSystem.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Components/Sprite.h"
#include "FastEngine/Input/InputManager.h"
#include "FastEngine/Platform/Platform.h"
#include "FastEngine/Platform/FileSystem.h"
#include "FastEngine/Render/Camera.h"
#include "FastEngine/Render/Renderer.h"
#include "FastEngine/Systems/RenderSystem.h"
#include <glm/glm.hpp>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <vector>
#include <string>

#if HAVE_SDL2_TTF
#include <SDL_ttf.h>
#endif

const float Game::CELL_SIZE = 40.0f;
const float Game::MOVE_INTERVAL = 0.12f;
const int Game::ID_BTN_RESTART = 1;
const int Game::ID_BTN_MENU = 2;
const int Game::ID_BTN_PLAY = 3;
const int Game::ID_BTN_QUIT = 4;

#if HAVE_SDL2_TTF
struct Game::UIFontState {
    TTF_Font* font = nullptr;
    FastEngine::Texture* texture = nullptr;
    ~UIFontState() {
        if (font) { TTF_CloseFont(font); font = nullptr; }
        if (texture) { texture->Destroy(); delete texture; texture = nullptr; }
        TTF_Quit();
    }
};
#else
struct Game::UIFontState {};
#endif

Game::Game(FastEngine::Engine* engine)
    : m_engine(engine)
    , m_world(nullptr)
    , m_camera(nullptr)
    , m_state(State::Playing)
    , m_direction(Right)
    , m_nextDirection(Right)
    , m_foodGx(0)
    , m_foodGy(0)
    , m_moveTimer(0.0f)
    , m_score(0)
    , m_highScore(0)
    , m_gameOver(false)
    , m_newRecordThisGame(false)
    , m_paused(false)
    , m_background(nullptr)
    , m_foodEntity(nullptr)
    , m_uiFont(nullptr) {
}

Game::~Game() = default;

float Game::CellToWorldX(int gx) const {
    return (gx + 0.5f) * CELL_SIZE;
}

float Game::CellToWorldY(int gy) const {
    return (gy + 0.5f) * CELL_SIZE;
}

void Game::Initialize() {
    static bool s_randSeeded = false;
    if (!s_randSeeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        s_randSeeded = true;
    }

    m_world = m_engine->GetWorld();

    m_camera = new FastEngine::Camera();
    m_camera->SetSize(800.0f, 600.0f);
    m_camera->SetPosition(400.0f, 300.0f);

    auto* renderSystem = m_engine->GetRenderSystem();
    if (renderSystem) {
        renderSystem->SetCamera(m_camera);
    }

    // Фон
    m_background = m_world->CreateEntity();
    m_background->AddComponent<FastEngine::Transform>(400.0f, 300.0f);
    auto* bgSprite = m_background->AddComponent<FastEngine::Sprite>("textures/background.bmp");
    bgSprite->SetSize(800.0f, 600.0f);
    bgSprite->SetColor(0.08f, 0.2f, 0.12f, 1.0f);

    // Стартовая змейка: 3 сегмента по центру, смотрит вправо
    m_snake.clear();
    int cx = GRID_COLS / 2;
    int cy = GRID_ROWS / 2;
    m_snake.push_back({ cx, cy });
    m_snake.push_back({ cx - 1, cy });
    m_snake.push_back({ cx - 2, cy });
    m_direction = Right;
    m_nextDirection = Right;
    m_moveTimer = 0.0f;
    m_score = 0;
    m_gameOver = false;
    m_newRecordThisGame = false;
    m_paused = false;

    // Загрузка лучшего счёта из файла
    auto* fs = FastEngine::Platform::GetInstance().GetFileSystem();
    if (fs) {
        std::string content = fs->ReadFile("snake_highscore.txt");
        if (!content.empty()) {
            try {
                int loaded = std::stoi(content);
                if (loaded >= 0) m_highScore = loaded;
            } catch (...) {}
        }
    }

    // Сущности для сегментов змейки
    for (size_t i = 0; i < m_snake.size(); ++i) {
        auto* e = m_world->CreateEntity();
        float wx = CellToWorldX(m_snake[i].first);
        float wy = CellToWorldY(m_snake[i].second);
        e->AddComponent<FastEngine::Transform>(wx, wy);
        auto* sprite = e->AddComponent<FastEngine::Sprite>("textures/player.bmp");
        sprite->SetSize(CELL_SIZE - 2.0f, CELL_SIZE - 2.0f);
        sprite->SetColor(0.2f, 0.85f, 0.3f, 1.0f);
        m_snakeEntities.push_back(e);
    }

    // Еда
    m_foodEntity = m_world->CreateEntity();
    SpawnFood();
    m_foodEntity->AddComponent<FastEngine::Transform>(CellToWorldX(m_foodGx), CellToWorldY(m_foodGy));
    auto* foodSprite = m_foodEntity->AddComponent<FastEngine::Sprite>("textures/player.bmp");
    foodSprite->SetSize(CELL_SIZE - 4.0f, CELL_SIZE - 4.0f);
    foodSprite->SetColor(0.95f, 0.2f, 0.2f, 1.0f);

    if (!m_world->GetSystem<GameUpdateSystem>()) {
        m_world->AddSystem<GameUpdateSystem>(m_world, this);
    }

    m_state = State::Playing;
    m_buttonManager = std::make_unique<FastEngine::ButtonManager>();
    SetupPlayingButtons();
    m_engine->GetRenderer()->SetGameSize(800, 600);

#if HAVE_SDL2_TTF
    m_uiFont = std::make_unique<UIFontState>();
    if (TTF_Init() == 0) {
        const char* fontPaths[] = {
            "/System/Library/Fonts/Supplemental/Arial.ttf",
            "/Library/Fonts/Arial.ttf",
            "/System/Library/Fonts/Helvetica.ttc",
        };
        for (const char* path : fontPaths) {
            m_uiFont->font = TTF_OpenFont(path, 24);
            if (m_uiFont->font) break;
        }
        if (m_uiFont->font) {
            m_uiFont->texture = new FastEngine::Texture();
        }
    }
#endif

    std::cout << "Snake (snake_cros_app): Game initialized. Use Arrow Keys or WASD, R to restart, ESC to quit." << std::endl;
}

void Game::SpawnFood() {
    for (int attempt = 0; attempt < 500; ++attempt) {
        int gx = std::rand() % GRID_COLS;
        int gy = std::rand() % GRID_ROWS;
        bool onSnake = false;
        for (const auto& p : m_snake) {
            if (p.first == gx && p.second == gy) {
                onSnake = true;
                break;
            }
        }
        if (!onSnake) {
            m_foodGx = gx;
            m_foodGy = gy;
            return;
        }
    }
    m_foodGx = 0;
    m_foodGy = 0;
}

void Game::CreateMenuScene() {
    m_world = m_engine->GetWorld();
    m_camera = new FastEngine::Camera();
    m_camera->SetSize(800.0f, 600.0f);
    m_camera->SetPosition(400.0f, 300.0f);
    auto* renderSystem = m_engine->GetRenderSystem();
    if (renderSystem) renderSystem->SetCamera(m_camera);
    m_background = m_world->CreateEntity();
    m_background->AddComponent<FastEngine::Transform>(400.0f, 300.0f);
    auto* bgSprite = m_background->AddComponent<FastEngine::Sprite>("textures/background.bmp");
    bgSprite->SetSize(800.0f, 600.0f);
    bgSprite->SetColor(0.08f, 0.2f, 0.12f, 1.0f);
    m_state = State::MainMenu;
}

void Game::SetupGameOverButtons() {
    m_buttonManager->Clear();
    m_buttonManager->AddButton(ID_BTN_RESTART, 300.0f, 250.0f, 200.0f, 50.0f,
        [this]() { Shutdown(); Initialize(); },
        glm::vec4(0.2f, 0.7f, 0.3f, 0.95f));
    m_buttonManager->AddButton(ID_BTN_MENU, 300.0f, 180.0f, 200.0f, 50.0f,
        [this]() { Shutdown(); CreateMenuScene(); m_buttonManager = std::make_unique<FastEngine::ButtonManager>(); SetupMainMenuButtons(); m_engine->GetRenderer()->SetGameSize(800, 600); },
        glm::vec4(0.4f, 0.5f, 0.6f, 0.95f));
}

void Game::SetupMainMenuButtons() {
    m_buttonManager->Clear();
    m_buttonManager->AddButton(ID_BTN_PLAY, 300.0f, 280.0f, 200.0f, 50.0f,
        [this]() { Shutdown(); Initialize(); },
        glm::vec4(0.2f, 0.7f, 0.3f, 0.95f));
    m_buttonManager->AddButton(ID_BTN_QUIT, 300.0f, 210.0f, 200.0f, 50.0f,
        [this]() { m_engine->Stop(); },
        glm::vec4(0.7f, 0.25f, 0.25f, 0.95f));
}

void Game::SetupPlayingButtons() {
    m_buttonManager->Clear();
    m_buttonManager->AddButton(ID_BTN_MENU, 700.0f, 550.0f, 80.0f, 40.0f,
        [this]() { Shutdown(); CreateMenuScene(); m_buttonManager = std::make_unique<FastEngine::ButtonManager>(); SetupMainMenuButtons(); m_engine->GetRenderer()->SetGameSize(800, 600); },
        glm::vec4(0.4f, 0.5f, 0.6f, 0.9f));
}

void Game::DrawButtonLabel(const char* text, float rectX, float rectY, float rectW, float rectH) {
#if !HAVE_SDL2_TTF
    (void)text;
    (void)rectX;
    (void)rectY;
    (void)rectW;
    (void)rectH;
    return;
#else
    if (!m_uiFont || !m_uiFont->font || !m_uiFont->texture || !m_engine->GetRenderer()) return;
    SDL_Surface* surf = TTF_RenderText_Blended(m_uiFont->font, text, SDL_Color{255, 255, 255, 255});
    if (!surf) return;
    SDL_Surface* rgba = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surf);
    if (!rgba) return;
    const int w = rgba->w;
    const int h = rgba->h;
    const int pitch = rgba->pitch;
    std::vector<unsigned char> flipped(static_cast<size_t>(w) * h * 4);
    for (int y = 0; y < h; ++y) {
        const unsigned char* srcRow = static_cast<const unsigned char*>(rgba->pixels) + (h - 1 - y) * pitch;
        unsigned char* dstRow = flipped.data() + y * w * 4;
        memcpy(dstRow, srcRow, static_cast<size_t>(w) * 4);
    }
    m_uiFont->texture->Create(w, h, flipped.data());
    SDL_FreeSurface(rgba);
    float heightWorld = rectH * 0.75f;
    if (h > 0) {
        float widthWorld = heightWorld * static_cast<float>(w) / static_cast<float>(h);
        if (widthWorld > rectW * 0.95f) {
            widthWorld = rectW * 0.95f;
            heightWorld = widthWorld * static_cast<float>(h) / static_cast<float>(w);
        }
        float centerX = rectX + rectW * 0.5f;
        float centerY = rectY + rectH * 0.5f;
        m_engine->GetRenderer()->SetBlendMode(true);
        m_engine->GetRenderer()->DrawTexture(m_uiFont->texture, glm::vec2(centerX, centerY), glm::vec2(widthWorld, heightWorld), glm::vec4(1.0f));
    }
#endif
}

void Game::DrawButtonLabels() {
#if !HAVE_SDL2_TTF
    return;
#else
    if (!m_uiFont || !m_uiFont->font) return;
    char buf[64];
    switch (m_state) {
        case State::GameOver: {
            DrawButtonLabel("Restart", 300.0f, 250.0f, 200.0f, 50.0f);
            DrawButtonLabel("Menu", 300.0f, 180.0f, 200.0f, 50.0f);
            snprintf(buf, sizeof(buf), "Best: %d", m_highScore);
            DrawButtonLabel(buf, 300.0f, 320.0f, 200.0f, 28.0f);
            if (m_newRecordThisGame) {
                DrawButtonLabel("New record!", 300.0f, 355.0f, 200.0f, 24.0f);
            }
            break;
        }
        case State::MainMenu:
            DrawButtonLabel("Play", 300.0f, 280.0f, 200.0f, 50.0f);
            DrawButtonLabel("Quit", 300.0f, 210.0f, 200.0f, 50.0f);
            break;
        case State::Playing: {
            snprintf(buf, sizeof(buf), "Score: %d", m_score);
            // Верхний левый угол: экран 800x600, Y растёт вверх — отступ 10px от верха
            DrawButtonLabel(buf, 10.0f, 562.0f, 180.0f, 28.0f);
            DrawButtonLabel("Menu", 700.0f, 550.0f, 80.0f, 40.0f);
            if (m_paused) {
                DrawButtonLabel("Paused", 300.0f, 280.0f, 200.0f, 40.0f);
            }
            break;
        }
    }
#endif
}

void Game::SyncSnakeEntities() {
    while (m_snakeEntities.size() < m_snake.size()) {
        auto* e = m_world->CreateEntity();
        e->AddComponent<FastEngine::Transform>(0.0f, 0.0f);
        auto* sprite = e->AddComponent<FastEngine::Sprite>("textures/player.bmp");
        sprite->SetSize(CELL_SIZE - 2.0f, CELL_SIZE - 2.0f);
        sprite->SetColor(0.2f, 0.85f, 0.3f, 1.0f);
        m_snakeEntities.push_back(e);
    }
    for (size_t i = 0; i < m_snake.size(); ++i) {
        auto* t = m_snakeEntities[i]->GetComponent<FastEngine::Transform>();
        if (t) {
            t->SetPosition(CellToWorldX(m_snake[i].first), CellToWorldY(m_snake[i].second));
        }
    }
    if (m_foodEntity) {
        auto* t = m_foodEntity->GetComponent<FastEngine::Transform>();
        if (t) {
            t->SetPosition(CellToWorldX(m_foodGx), CellToWorldY(m_foodGy));
        }
    }
}

void Game::Update(float deltaTime) {
    auto* inputManager = m_engine->GetInputManager();
    auto* renderer = m_engine->GetRenderer();

    if (inputManager->IsKeyPressed(41)) {
        m_engine->Stop();
        return;
    }

    if (m_state == State::MainMenu) {
        if (m_buttonManager) m_buttonManager->Update(inputManager, renderer);
        return;
    }

    if (m_state == State::GameOver) {
        if (m_buttonManager) m_buttonManager->Update(inputManager, renderer);
        return;
    }

    if (m_gameOver) {
        if (m_score > m_highScore) {
            m_highScore = m_score;
            m_newRecordThisGame = true;
            auto* fs = FastEngine::Platform::GetInstance().GetFileSystem();
            if (fs) {
                fs->WriteFile("snake_highscore.txt", std::to_string(m_highScore));
            }
        }
        m_state = State::GameOver;
        SetupGameOverButtons();
        if (m_buttonManager) m_buttonManager->Update(inputManager, renderer);
        return;
    }

    // Playing: R — рестарт, Space (44) — пауза
    if (inputManager->IsKeyPressed(21)) {
        Shutdown();
        Initialize();
        return;
    }
    if (inputManager->IsKeyPressed(44)) {
        m_paused = !m_paused;
        SyncSnakeEntities();
        if (m_buttonManager) m_buttonManager->Update(inputManager, renderer);
        return;
    }
    if (m_paused) {
        SyncSnakeEntities();
        if (m_buttonManager) m_buttonManager->Update(inputManager, renderer);
        return;
    }

    // Ввод направления (нельзя развернуться на 180°)
    if (inputManager->IsKeyPressed(79) || inputManager->IsKeyPressed(7))  m_nextDirection = Right;  // Right / D
    if (inputManager->IsKeyPressed(80) || inputManager->IsKeyPressed(4))  m_nextDirection = Left;   // Left / A
    if (inputManager->IsKeyPressed(82) || inputManager->IsKeyPressed(26)) m_nextDirection = Up;     // Up / W
    if (inputManager->IsKeyPressed(81) || inputManager->IsKeyPressed(22)) m_nextDirection = Down;   // Down / S

    if ((m_nextDirection + 2) % 4 != m_direction) {
        m_direction = m_nextDirection;
    }

    m_moveTimer += deltaTime;
    if (m_moveTimer < MOVE_INTERVAL) {
        SyncSnakeEntities();
        return;
    }
    m_moveTimer -= MOVE_INTERVAL;

    int headX = m_snake[0].first;
    int headY = m_snake[0].second;
    switch (m_direction) {
        case Right: headX++; break;
        case Left:  headX--; break;
        case Up:    headY++; break;
        case Down:  headY--; break;
    }

    // Стены
    if (headX < 0 || headX >= GRID_COLS || headY < 0 || headY >= GRID_ROWS) {
        m_gameOver = true;
        std::cout << "Game Over (wall). Score: " << m_score << std::endl;
        return;
    }

    // Сам в себя
    for (size_t i = 0; i < m_snake.size(); ++i) {
        if (m_snake[i].first == headX && m_snake[i].second == headY) {
            m_gameOver = true;
            std::cout << "Game Over (self). Score: " << m_score << std::endl;
            return;
        }
    }

    m_snake.insert(m_snake.begin(), { headX, headY });

    if (headX == m_foodGx && headY == m_foodGy) {
        m_score++;
        SpawnFood();
    } else {
        m_snake.pop_back();
    }

    SyncSnakeEntities();

    if (m_buttonManager) m_buttonManager->Update(inputManager, renderer);
}

void Game::Render() {
    if (m_buttonManager && m_engine->GetRenderer()) {
        m_buttonManager->Draw(m_engine->GetRenderer());
        DrawButtonLabels();
    }
}

void Game::Shutdown() {
    if (m_camera) {
        delete m_camera;
        m_camera = nullptr;
    }
    for (auto* e : m_snakeEntities) {
        if (m_world) m_world->DestroyEntity(e);
    }
    m_snakeEntities.clear();
    if (m_foodEntity && m_world) {
        m_world->DestroyEntity(m_foodEntity);
        m_foodEntity = nullptr;
    }
    if (m_background && m_world) {
        m_world->DestroyEntity(m_background);
        m_background = nullptr;
    }
    m_snake.clear();
    m_world = nullptr;
}
