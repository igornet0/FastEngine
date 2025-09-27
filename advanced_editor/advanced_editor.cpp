#include <SDL2/SDL.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <json/json.h>
#include <cmath>

class AdvancedGameEditor {
public:
    struct Entity {
        float x, y;
        float scaleX, scaleY;
        float rotation;
        float size;
        float r, g, b;
        std::string name;
        std::string type;
        bool visible;
        bool selected;
    };

    struct Panel {
        float x, y, width, height;
        bool visible;
        std::string title;
    };

    AdvancedGameEditor() 
        : m_window(nullptr)
        , m_glContext(nullptr)
        , m_running(true)
        , m_showGrid(true)
        , m_gridSize(32.0f)
        , m_snapToGrid(true)
        , m_selectedEntity(-1)
        , m_cameraX(0), m_cameraY(0)
        , m_zoom(1.0f)
        , m_dragging(false)
        , m_dragStartX(0), m_dragStartY(0)
        , m_showInspector(true)
        , m_showHierarchy(true)
        , m_showAssets(true)
        , m_showConsole(true)
        , m_showGamePreview(false)
        , m_playMode(false)
        , m_consoleScroll(0)
        , m_assetScroll(0)
        , m_hierarchyScroll(0) {
        
        // Инициализация панелей
        m_scenePanel = {0, 0, 800, 600, true, "Scene"};
        m_inspectorPanel = {800, 0, 200, 300, true, "Inspector"};
        m_hierarchyPanel = {800, 300, 200, 200, true, "Hierarchy"};
        m_assetsPanel = {0, 600, 400, 200, true, "Assets"};
        m_consolePanel = {400, 600, 400, 200, true, "Console"};
        m_gamePreviewPanel = {1000, 0, 200, 150, false, "Game Preview"};
    }

    bool Initialize() {
        // Инициализация SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            return false;
        }

        // Создание окна
        m_window = SDL_CreateWindow("Advanced Game Editor", 
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   1200, 800, 
                                   SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (!m_window) {
            std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
            return false;
        }

        // Создание OpenGL контекста
        m_glContext = SDL_GL_CreateContext(m_window);
        if (!m_glContext) {
            std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
            return false;
        }

        // Настройка OpenGL
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 1200, 800, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Загружаем существующий проект если есть
        LoadProject("advanced_game.json");
        
        // Добавляем тестовые объекты
        if (m_entities.empty()) {
            CreateTestEntities();
        }
        
        std::cout << "=== Advanced Game Editor ===" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  WASD - Move camera" << std::endl;
        std::cout << "  Mouse - Select objects" << std::endl;
        std::cout << "  Right Click - Create object" << std::endl;
        std::cout << "  Space - Toggle grid" << std::endl;
        std::cout << "  G - Toggle snap to grid" << std::endl;
        std::cout << "  S - Save project" << std::endl;
        std::cout << "  L - Load project" << std::endl;
        std::cout << "  N - New project" << std::endl;
        std::cout << "  P - Toggle play mode" << std::endl;
        std::cout << "  F1-F5 - Toggle panels" << std::endl;
        std::cout << "  ESC - Exit" << std::endl;
        std::cout << "=========================" << std::endl;
        std::cout << "Editor initialized successfully!" << std::endl;
        
        return true;
    }

    void Run() {
        SDL_Event event;
        Uint32 lastTime = SDL_GetTicks();
        
        while (m_running) {
            Uint32 currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - lastTime) / 1000.0f;
            lastTime = currentTime;
            
            // Обработка событий
            while (SDL_PollEvent(&event)) {
                HandleEvent(event);
            }
            
            // Обновление
            Update(deltaTime);
            
            // Отрисовка
            Render();
            
            // Показ результата
            SDL_GL_SwapWindow(m_window);
            
            // Ограничение FPS
            SDL_Delay(16); // ~60 FPS
        }
    }

    void Shutdown() {
        if (m_glContext) {
            SDL_GL_DeleteContext(m_glContext);
        }
        if (m_window) {
            SDL_DestroyWindow(m_window);
        }
        SDL_Quit();
    }

private:
    SDL_Window* m_window;
    SDL_GLContext m_glContext;
    bool m_running;
    bool m_showGrid;
    float m_gridSize;
    bool m_snapToGrid;
    int m_selectedEntity;
    
    std::vector<Entity> m_entities;
    std::string m_projectName;
    float m_cameraX, m_cameraY;
    float m_zoom;
    
    // Панели
    Panel m_scenePanel;
    Panel m_inspectorPanel;
    Panel m_hierarchyPanel;
    Panel m_assetsPanel;
    Panel m_consolePanel;
    Panel m_gamePreviewPanel;
    
    // Состояние панелей
    bool m_showInspector;
    bool m_showHierarchy;
    bool m_showAssets;
    bool m_showConsole;
    bool m_showGamePreview;
    bool m_playMode;
    
    // Прокрутка
    int m_consoleScroll;
    int m_assetScroll;
    int m_hierarchyScroll;
    
    // Перетаскивание
    bool m_dragging;
    float m_dragStartX, m_dragStartY;
    
    // Консоль
    std::vector<std::string> m_consoleMessages;

    void CreateTestEntities() {
        // Создаем тестовые объекты
        Entity player;
        player.x = 100; player.y = 200;
        player.scaleX = 1.0f; player.scaleY = 1.0f;
        player.rotation = 0.0f;
        player.size = 32.0f;
        player.r = 1.0f; player.g = 1.0f; player.b = 0.0f;
        player.name = "Player";
        player.type = "Sprite";
        player.visible = true;
        player.selected = false;
        m_entities.push_back(player);
        
        Entity ground;
        ground.x = 200; ground.y = 300;
        ground.scaleX = 3.0f; ground.scaleY = 1.0f;
        ground.rotation = 0.0f;
        ground.size = 32.0f;
        ground.r = 0.0f; ground.g = 1.0f; ground.b = 0.0f;
        ground.name = "Ground";
        ground.type = "Sprite";
        ground.visible = true;
        ground.selected = false;
        m_entities.push_back(ground);
        
        Entity enemy;
        enemy.x = 300; enemy.y = 150;
        enemy.scaleX = 1.0f; enemy.scaleY = 1.0f;
        enemy.rotation = 0.0f;
        enemy.size = 24.0f;
        enemy.r = 1.0f; enemy.g = 0.0f; enemy.b = 0.0f;
        enemy.name = "Enemy";
        enemy.type = "Sprite";
        enemy.visible = true;
        enemy.selected = false;
        m_entities.push_back(enemy);
        
        AddConsoleMessage("Test entities created");
    }

    void HandleEvent(const SDL_Event& event) {
        switch (event.type) {
            case SDL_QUIT:
                m_running = false;
                break;
                
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int w = event.window.data1;
                    int h = event.window.data2;
                    glViewport(0, 0, w, h);
                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    glOrtho(0, w, h, 0, -1, 1);
                    glMatrixMode(GL_MODELVIEW);
                    
                    // Обновляем размеры панелей
                    UpdatePanelLayout(w, h);
                }
                break;
                
            case SDL_KEYDOWN:
                HandleKeyDown(event.key.keysym.sym);
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                HandleMouseDown(event.button);
                break;
                
            case SDL_MOUSEBUTTONUP:
                HandleMouseUp(event.button);
                break;
                
            case SDL_MOUSEMOTION:
                HandleMouseMotion(event.motion);
                break;
                
            case SDL_MOUSEWHEEL:
                HandleMouseWheel(event.wheel);
                break;
        }
    }

    void UpdatePanelLayout(int w, int h) {
        m_scenePanel.width = w - 200;
        m_scenePanel.height = h - 200;
        m_inspectorPanel.x = w - 200;
        m_inspectorPanel.height = h / 2;
        m_hierarchyPanel.x = w - 200;
        m_hierarchyPanel.y = h / 2;
        m_hierarchyPanel.height = h / 2;
        m_assetsPanel.width = w / 2;
        m_assetsPanel.y = h - 200;
        m_consolePanel.x = w / 2;
        m_consolePanel.y = h - 200;
        m_consolePanel.width = w / 2;
        m_gamePreviewPanel.x = w - 200;
        m_gamePreviewPanel.y = 0;
    }

    void HandleKeyDown(SDL_Keycode key) {
        switch (key) {
            case SDLK_ESCAPE:
                m_running = false;
                break;
            case SDLK_SPACE:
                m_showGrid = !m_showGrid;
                AddConsoleMessage("Grid: " + std::string(m_showGrid ? "ON" : "OFF"));
                break;
            case SDLK_g:
                m_snapToGrid = !m_snapToGrid;
                AddConsoleMessage("Snap to grid: " + std::string(m_snapToGrid ? "ON" : "OFF"));
                break;
            case SDLK_s:
                SaveProject();
                break;
            case SDLK_l:
                LoadProject("advanced_game.json");
                break;
            case SDLK_n:
                NewProject();
                break;
            case SDLK_p:
                m_playMode = !m_playMode;
                AddConsoleMessage("Play mode: " + std::string(m_playMode ? "ON" : "OFF"));
                break;
            case SDLK_F1:
                m_showInspector = !m_showInspector;
                break;
            case SDLK_F2:
                m_showHierarchy = !m_showHierarchy;
                break;
            case SDLK_F3:
                m_showAssets = !m_showAssets;
                break;
            case SDLK_F4:
                m_showConsole = !m_showConsole;
                break;
            case SDLK_F5:
                m_showGamePreview = !m_showGamePreview;
                break;
        }
    }

    void HandleMouseDown(const SDL_MouseButtonEvent& button) {
        if (button.button == SDL_BUTTON_LEFT) {
            if (IsPointInPanel(button.x, button.y, m_scenePanel)) {
                SelectEntityAt(button.x, button.y);
                m_dragging = true;
                m_dragStartX = button.x;
                m_dragStartY = button.y;
            }
        } else if (button.button == SDL_BUTTON_RIGHT) {
            if (IsPointInPanel(button.x, button.y, m_scenePanel)) {
                CreateEntityAt(button.x, button.y);
            }
        }
    }

    void HandleMouseUp(const SDL_MouseButtonEvent& button) {
        if (button.button == SDL_BUTTON_LEFT) {
            m_dragging = false;
        }
    }

    void HandleMouseMotion(const SDL_MouseMotionEvent& motion) {
        if (m_dragging) {
            if (m_selectedEntity >= 0 && m_selectedEntity < m_entities.size()) {
                // Перемещение объекта
                float deltaX = (motion.x - m_dragStartX) / m_zoom;
                float deltaY = (motion.y - m_dragStartY) / m_zoom;
                
                m_entities[m_selectedEntity].x += deltaX;
                m_entities[m_selectedEntity].y += deltaY;
                
                if (m_snapToGrid) {
                    m_entities[m_selectedEntity].x = std::round(m_entities[m_selectedEntity].x / m_gridSize) * m_gridSize;
                    m_entities[m_selectedEntity].y = std::round(m_entities[m_selectedEntity].y / m_gridSize) * m_gridSize;
                }
                
                m_dragStartX = motion.x;
                m_dragStartY = motion.y;
            } else {
                // Перемещение камеры
                m_cameraX -= (motion.x - m_dragStartX) / m_zoom;
                m_cameraY -= (motion.y - m_dragStartY) / m_zoom;
                m_dragStartX = motion.x;
                m_dragStartY = motion.y;
            }
        }
    }

    void HandleMouseWheel(const SDL_MouseWheelEvent& wheel) {
        if (IsPointInPanel(SDL_GetMouseState(nullptr, nullptr), m_scenePanel.y, m_scenePanel)) {
            float zoomFactor = 1.1f;
            if (wheel.y > 0) {
                m_zoom *= zoomFactor;
            } else if (wheel.y < 0) {
                m_zoom /= zoomFactor;
            }
            m_zoom = std::max(0.1f, std::min(5.0f, m_zoom));
        }
    }

    bool IsPointInPanel(int x, int y, const Panel& panel) {
        return x >= panel.x && x <= panel.x + panel.width &&
               y >= panel.y && y <= panel.y + panel.height;
    }

    void Update(float deltaTime) {
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        float moveSpeed = 300.0f;
        
        // WASD для движения камеры
        if (keys[SDL_SCANCODE_W]) {
            m_cameraY -= moveSpeed * deltaTime;
        }
        if (keys[SDL_SCANCODE_S]) {
            m_cameraY += moveSpeed * deltaTime;
        }
        if (keys[SDL_SCANCODE_A]) {
            m_cameraX -= moveSpeed * deltaTime;
        }
        if (keys[SDL_SCANCODE_D]) {
            m_cameraX += moveSpeed * deltaTime;
        }
        
        // Стрелки для движения выбранного объекта
        if (m_selectedEntity >= 0 && m_selectedEntity < m_entities.size()) {
            float moveSpeed = 200.0f;
            if (keys[SDL_SCANCODE_UP]) {
                m_entities[m_selectedEntity].y -= moveSpeed * deltaTime;
            }
            if (keys[SDL_SCANCODE_DOWN]) {
                m_entities[m_selectedEntity].y += moveSpeed * deltaTime;
            }
            if (keys[SDL_SCANCODE_LEFT]) {
                m_entities[m_selectedEntity].x -= moveSpeed * deltaTime;
            }
            if (keys[SDL_SCANCODE_RIGHT]) {
                m_entities[m_selectedEntity].x += moveSpeed * deltaTime;
            }
            
            // Привязка к сетке
            if (m_snapToGrid) {
                m_entities[m_selectedEntity].x = std::round(m_entities[m_selectedEntity].x / m_gridSize) * m_gridSize;
                m_entities[m_selectedEntity].y = std::round(m_entities[m_selectedEntity].y / m_gridSize) * m_gridSize;
            }
        }
    }

    void Render() {
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Отрисовка панелей
        RenderPanels();
        
        // Отрисовка сцены
        RenderScene();
        
        // Отрисовка UI поверх панелей
        RenderUI();
    }

    void RenderPanels() {
        // Фон панелей
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(1200, 0);
        glVertex2f(1200, 800);
        glVertex2f(0, 800);
        glEnd();
        
        // Scene Panel
        RenderPanel(m_scenePanel, 0.25f, 0.25f, 0.25f);
        
        // Inspector Panel
        if (m_showInspector) {
            RenderPanel(m_inspectorPanel, 0.3f, 0.3f, 0.3f);
            RenderInspector();
        }
        
        // Hierarchy Panel
        if (m_showHierarchy) {
            RenderPanel(m_hierarchyPanel, 0.3f, 0.3f, 0.3f);
            RenderHierarchy();
        }
        
        // Assets Panel
        if (m_showAssets) {
            RenderPanel(m_assetsPanel, 0.3f, 0.3f, 0.3f);
            RenderAssets();
        }
        
        // Console Panel
        if (m_showConsole) {
            RenderPanel(m_consolePanel, 0.3f, 0.3f, 0.3f);
            RenderConsole();
        }
        
        // Game Preview Panel
        if (m_showGamePreview) {
            RenderPanel(m_gamePreviewPanel, 0.3f, 0.3f, 0.3f);
            RenderGamePreview();
        }
    }

    void RenderPanel(const Panel& panel, float r, float g, float b) {
        glColor3f(r, g, b);
        glBegin(GL_QUADS);
        glVertex2f(panel.x, panel.y);
        glVertex2f(panel.x + panel.width, panel.y);
        glVertex2f(panel.x + panel.width, panel.y + panel.height);
        glVertex2f(panel.x, panel.y + panel.height);
        glEnd();
        
        // Заголовок панели
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_LINES);
        glVertex2f(panel.x, panel.y + 20);
        glVertex2f(panel.x + panel.width, panel.y + 20);
        glEnd();
    }

    void RenderScene() {
        // Применяем трансформацию камеры
        glPushMatrix();
        glTranslatef(-m_cameraX, -m_cameraY, 0);
        glScalef(m_zoom, m_zoom, 1.0f);
        
        // Отрисовка сетки
        if (m_showGrid) {
            RenderGrid();
        }
        
        // Отрисовка объектов
        RenderEntities();
        
        glPopMatrix();
    }

    void RenderGrid() {
        glColor3f(0.4f, 0.4f, 0.4f);
        glBegin(GL_LINES);
        
        // Вертикальные линии
        for (float x = -1000; x < 2000; x += m_gridSize) {
            glVertex2f(x, -1000);
            glVertex2f(x, 2000);
        }
        
        // Горизонтальные линии
        for (float y = -1000; y < 2000; y += m_gridSize) {
            glVertex2f(-1000, y);
            glVertex2f(2000, y);
        }
        
        glEnd();
    }

    void RenderEntities() {
        for (size_t i = 0; i < m_entities.size(); ++i) {
            const Entity& entity = m_entities[i];
            if (!entity.visible) continue;
            
            // Выделяем выбранный объект
            if (i == m_selectedEntity) {
                glColor3f(1.0f, 1.0f, 0.0f); // Желтый для выбранного
            } else {
                glColor3f(entity.r, entity.g, entity.b);
            }
            
            // Отрисовка квадрата
            glPushMatrix();
            glTranslatef(entity.x, entity.y, 0);
            glRotatef(entity.rotation, 0, 0, 1);
            glScalef(entity.scaleX, entity.scaleY, 1.0f);
            
            glBegin(GL_QUADS);
            glVertex2f(-entity.size/2, -entity.size/2);
            glVertex2f(entity.size/2, -entity.size/2);
            glVertex2f(entity.size/2, entity.size/2);
            glVertex2f(-entity.size/2, entity.size/2);
            glEnd();
            
            glPopMatrix();
        }
    }

    void RenderInspector() {
        if (m_selectedEntity >= 0 && m_selectedEntity < m_entities.size()) {
            const Entity& entity = m_entities[m_selectedEntity];
            
            // Заголовок
            DrawText(m_inspectorPanel.x + 10, m_inspectorPanel.y + 30, "Properties");
            
            // Transform
            DrawText(m_inspectorPanel.x + 10, m_inspectorPanel.y + 50, "Transform:");
            DrawText(m_inspectorPanel.x + 20, m_inspectorPanel.y + 70, "Position: " + 
                    std::to_string((int)entity.x) + ", " + std::to_string((int)entity.y));
            DrawText(m_inspectorPanel.x + 20, m_inspectorPanel.y + 90, "Scale: " + 
                    std::to_string(entity.scaleX) + ", " + std::to_string(entity.scaleY));
            DrawText(m_inspectorPanel.x + 20, m_inspectorPanel.y + 110, "Rotation: " + 
                    std::to_string((int)entity.rotation) + "°");
            
            // Sprite Renderer
            DrawText(m_inspectorPanel.x + 10, m_inspectorPanel.y + 140, "Sprite Renderer:");
            DrawText(m_inspectorPanel.x + 20, m_inspectorPanel.y + 160, "Type: " + entity.type);
            DrawText(m_inspectorPanel.x + 20, m_inspectorPanel.y + 180, "Size: " + std::to_string((int)entity.size));
            DrawText(m_inspectorPanel.x + 20, m_inspectorPanel.y + 200, "Color: RGB(" + 
                    std::to_string((int)(entity.r * 255)) + ", " + 
                    std::to_string((int)(entity.g * 255)) + ", " + 
                    std::to_string((int)(entity.b * 255)) + ")");
        } else {
            DrawText(m_inspectorPanel.x + 10, m_inspectorPanel.y + 30, "No object selected");
        }
    }

    void RenderHierarchy() {
        DrawText(m_hierarchyPanel.x + 10, m_hierarchyPanel.y + 30, "Scene");
        
        int y = 50;
        for (size_t i = 0; i < m_entities.size(); ++i) {
            const Entity& entity = m_entities[i];
            
            // Выделяем выбранный объект
            if (i == m_selectedEntity) {
                glColor3f(1.0f, 1.0f, 0.0f);
            } else {
                glColor3f(0.8f, 0.8f, 0.8f);
            }
            
            DrawText(m_hierarchyPanel.x + 20, m_hierarchyPanel.y + y, entity.name);
            y += 20;
        }
    }

    void RenderAssets() {
        DrawText(m_assetsPanel.x + 10, m_assetsPanel.y + 30, "Assets");
        
        // Спрайты
        DrawText(m_assetsPanel.x + 10, m_assetsPanel.y + 50, "Sprites:");
        DrawText(m_assetsPanel.x + 20, m_assetsPanel.y + 70, "• Player");
        DrawText(m_assetsPanel.x + 20, m_assetsPanel.y + 90, "• Enemy");
        DrawText(m_assetsPanel.x + 20, m_assetsPanel.y + 110, "• Ground");
        
        // Графика
        DrawText(m_assetsPanel.x + 10, m_assetsPanel.y + 140, "Graphics:");
        DrawText(m_assetsPanel.x + 20, m_assetsPanel.y + 160, "• Background");
        DrawText(m_assetsPanel.x + 20, m_assetsPanel.y + 180, "• UI Elements");
    }

    void RenderConsole() {
        DrawText(m_consolePanel.x + 10, m_consolePanel.y + 30, "Console");
        
        int y = 50;
        for (size_t i = m_consoleScroll; i < m_consoleMessages.size() && y < m_consolePanel.y + m_consolePanel.height - 20; ++i) {
            DrawText(m_consolePanel.x + 10, m_consolePanel.y + y, m_consoleMessages[i]);
            y += 15;
        }
    }

    void RenderGamePreview() {
        DrawText(m_gamePreviewPanel.x + 10, m_gamePreviewPanel.y + 30, "Game Preview");
        
        if (m_playMode) {
            DrawText(m_gamePreviewPanel.x + 10, m_gamePreviewPanel.y + 50, "▶ PLAYING");
        } else {
            DrawText(m_gamePreviewPanel.x + 10, m_gamePreviewPanel.y + 50, "⏸ PAUSED");
        }
    }

    void RenderUI() {
        // Заголовки панелей
        glColor3f(0.9f, 0.9f, 0.9f);
        DrawText(m_scenePanel.x + 10, m_scenePanel.y + 5, "Scene");
        if (m_showInspector) DrawText(m_inspectorPanel.x + 10, m_inspectorPanel.y + 5, "Inspector");
        if (m_showHierarchy) DrawText(m_hierarchyPanel.x + 10, m_hierarchyPanel.y + 5, "Hierarchy");
        if (m_showAssets) DrawText(m_assetsPanel.x + 10, m_assetsPanel.y + 5, "Assets");
        if (m_showConsole) DrawText(m_consolePanel.x + 10, m_consolePanel.y + 5, "Console");
        if (m_showGamePreview) DrawText(m_gamePreviewPanel.x + 10, m_gamePreviewPanel.y + 5, "Game Preview");
    }

    void DrawText(float x, float y, const std::string& text) {
        // Простая отрисовка текста (в реальной реализации нужен шрифт)
        glRasterPos2f(x, y);
        for (char c : text) {
            // В реальной реализации здесь будет отрисовка символов
        }
    }

    void SelectEntityAt(int screenX, int screenY) {
        // Преобразуем экранные координаты в мировые
        float worldX = (screenX - m_scenePanel.x) / m_zoom + m_cameraX;
        float worldY = (screenY - m_scenePanel.y) / m_zoom + m_cameraY;
        
        m_selectedEntity = -1;
        for (size_t i = 0; i < m_entities.size(); ++i) {
            const Entity& entity = m_entities[i];
            float distance = sqrt((worldX - entity.x) * (worldX - entity.x) + 
                                 (worldY - entity.y) * (worldY - entity.y));
            if (distance < entity.size) {
                m_selectedEntity = i;
                AddConsoleMessage("Selected: " + entity.name);
                break;
            }
        }
    }

    void CreateEntityAt(int screenX, int screenY) {
        // Преобразуем экранные координаты в мировые
        float worldX = (screenX - m_scenePanel.x) / m_zoom + m_cameraX;
        float worldY = (screenY - m_scenePanel.y) / m_zoom + m_cameraY;
        
        // Привязка к сетке
        if (m_snapToGrid) {
            worldX = std::round(worldX / m_gridSize) * m_gridSize;
            worldY = std::round(worldY / m_gridSize) * m_gridSize;
        }
        
        Entity entity;
        entity.x = worldX;
        entity.y = worldY;
        entity.scaleX = 1.0f;
        entity.scaleY = 1.0f;
        entity.rotation = 0.0f;
        entity.size = 32.0f;
        entity.r = 0.5f;
        entity.g = 0.5f;
        entity.b = 1.0f;
        entity.name = "Object_" + std::to_string(m_entities.size() + 1);
        entity.type = "Sprite";
        entity.visible = true;
        entity.selected = false;
        
        m_entities.push_back(entity);
        m_selectedEntity = m_entities.size() - 1;
        
        AddConsoleMessage("Created: " + entity.name);
    }

    void AddConsoleMessage(const std::string& message) {
        m_consoleMessages.push_back(message);
        if (m_consoleMessages.size() > 50) {
            m_consoleMessages.erase(m_consoleMessages.begin());
        }
    }

    void SaveProject() {
        Json::Value root;
        root["name"] = m_projectName;
        root["version"] = "1.0.0";
        
        Json::Value entities(Json::arrayValue);
        for (const auto& entity : m_entities) {
            Json::Value entityData;
            entityData["name"] = entity.name;
            entityData["type"] = entity.type;
            entityData["x"] = entity.x;
            entityData["y"] = entity.y;
            entityData["scaleX"] = entity.scaleX;
            entityData["scaleY"] = entity.scaleY;
            entityData["rotation"] = entity.rotation;
            entityData["size"] = entity.size;
            entityData["r"] = entity.r;
            entityData["g"] = entity.g;
            entityData["b"] = entity.b;
            entityData["visible"] = entity.visible;
            entities.append(entityData);
        }
        root["entities"] = entities;
        
        std::ofstream file("advanced_game.json");
        if (file.is_open()) {
            file << root;
            file.close();
            AddConsoleMessage("Project saved to advanced_game.json");
        } else {
            AddConsoleMessage("Failed to save project");
        }
    }

    void LoadProject(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            AddConsoleMessage("No existing project found");
            return;
        }
        
        Json::Value root;
        file >> root;
        file.close();
        
        m_projectName = root.get("name", "Untitled").asString();
        m_entities.clear();
        
        const Json::Value& entities = root["entities"];
        for (const auto& entityData : entities) {
            Entity entity;
            entity.name = entityData.get("name", "Object").asString();
            entity.type = entityData.get("type", "Sprite").asString();
            entity.x = entityData.get("x", 0.0f).asFloat();
            entity.y = entityData.get("y", 0.0f).asFloat();
            entity.scaleX = entityData.get("scaleX", 1.0f).asFloat();
            entity.scaleY = entityData.get("scaleY", 1.0f).asFloat();
            entity.rotation = entityData.get("rotation", 0.0f).asFloat();
            entity.size = entityData.get("size", 32.0f).asFloat();
            entity.r = entityData.get("r", 1.0f).asFloat();
            entity.g = entityData.get("g", 1.0f).asFloat();
            entity.b = entityData.get("b", 1.0f).asFloat();
            entity.visible = entityData.get("visible", true).asBool();
            entity.selected = false;
            m_entities.push_back(entity);
        }
        
        AddConsoleMessage("Loaded project: " + m_projectName + " with " + std::to_string(m_entities.size()) + " entities");
    }

    void NewProject() {
        m_entities.clear();
        m_selectedEntity = -1;
        m_projectName = "New Project";
        m_consoleMessages.clear();
        AddConsoleMessage("New project created");
    }
};

int main() {
    AdvancedGameEditor editor;
    
    if (!editor.Initialize()) {
        std::cerr << "Failed to initialize editor" << std::endl;
        return -1;
    }
    
    editor.Run();
    editor.Shutdown();
    
    return 0;
}
