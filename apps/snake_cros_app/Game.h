#pragma once

#include "FastEngine/Engine.h"
#include "FastEngine/World.h"
#include "FastEngine/Entity.h"
#include "FastEngine/Render/Camera.h"
#include "FastEngine/UI/ButtonManager.h"
#include "FastEngine/Render/Texture.h"
#include <memory>
#include <vector>

class Game {
public:
    enum class State { MainMenu, Playing, GameOver };

    Game(FastEngine::Engine* engine);
    ~Game();

    void Initialize();
    void Update(float deltaTime);
    void Render();
    void Shutdown();

private:
    void SpawnFood();
    void SyncSnakeEntities();
    void CreateMenuScene();
    void SetupGameOverButtons();
    void SetupMainMenuButtons();
    void SetupPlayingButtons();
    void DrawButtonLabels();
    void DrawButtonLabel(const char* text, float rectX, float rectY, float rectW, float rectH);
    float CellToWorldX(int gx) const;
    float CellToWorldY(int gy) const;

    FastEngine::Engine* m_engine;
    FastEngine::World* m_world;
    FastEngine::Camera* m_camera;
    State m_state;
    std::unique_ptr<FastEngine::ButtonManager> m_buttonManager;

    static const int GRID_COLS = 20;
    static const int GRID_ROWS = 15;
    static const float CELL_SIZE;
    static const float MOVE_INTERVAL;

    enum Dir { Right = 0, Up = 1, Left = 2, Down = 3 };

    std::vector<std::pair<int, int>> m_snake;
    int m_direction;
    int m_nextDirection;
    int m_foodGx;
    int m_foodGy;
    float m_moveTimer;
    int m_score;
    int m_highScore;
    bool m_gameOver;
    bool m_newRecordThisGame;
    bool m_paused;

    FastEngine::Entity* m_background;
    std::vector<FastEngine::Entity*> m_snakeEntities;
    FastEngine::Entity* m_foodEntity;

    static const int ID_BTN_RESTART;
    static const int ID_BTN_MENU;
    static const int ID_BTN_PLAY;
    static const int ID_BTN_QUIT;

    struct UIFontState;
    std::unique_ptr<UIFontState> m_uiFont;
};
