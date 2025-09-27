#include <SDL2/SDL.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <iostream>
#include <vector>
#include <fstream>
#include <json/json.h>

class SimpleGameEditor {
public:
    SimpleGameEditor() 
        : m_window(nullptr)
        , m_glContext(nullptr)
        , m_running(true)
        , m_showGrid(true)
        , m_gridSize(32.0f)
        , m_snapToGrid(true)
        , m_selectedEntity(-1) {
    }

    bool Initialize() {
        // Инициализация SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            return false;
        }

        // Создание окна
        m_window = SDL_CreateWindow("Simple Game Editor", 
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   1200, 800, 
                                   SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 1200, 800, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Загружаем существующий проект если есть
        LoadProject("my_game.json");
        
        std::cout << "=== Simple Game Editor ===" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  WASD - Move camera" << std::endl;
        std::cout << "  Mouse - Select objects" << std::endl;
        std::cout << "  Space - Toggle grid" << std::endl;
        std::cout << "  G - Toggle snap to grid" << std::endl;
        std::cout << "  S - Save project" << std::endl;
        std::cout << "  L - Load project" << std::endl;
        std::cout << "  N - New project" << std::endl;
        std::cout << "  ESC - Exit" << std::endl;
        std::cout << "=========================" << std::endl;
        std::cout << "Editor initialized successfully!" << std::endl;
        std::cout << "Window should be visible now. Press ESC to exit." << std::endl;
        
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
    
    struct Entity {
        float x, y;
        float size;
        float r, g, b;
    };
    
    std::vector<Entity> m_entities;
    std::string m_projectName;
    float m_cameraX, m_cameraY;

    void HandleEvent(const SDL_Event& event) {
        switch (event.type) {
            case SDL_QUIT:
                m_running = false;
                break;
                
            case SDL_KEYDOWN:
                HandleKeyDown(event.key.keysym.sym);
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    SelectEntityAt(event.button.x, event.button.y);
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    CreateEntityAt(event.button.x, event.button.y);
                }
                break;
        }
    }

    void HandleKeyDown(SDL_Keycode key) {
        switch (key) {
            case SDLK_ESCAPE:
                m_running = false;
                break;
            case SDLK_SPACE:
                m_showGrid = !m_showGrid;
                std::cout << "Grid: " << (m_showGrid ? "ON" : "OFF") << std::endl;
                break;
            case SDLK_g:
                m_snapToGrid = !m_snapToGrid;
                std::cout << "Snap to grid: " << (m_snapToGrid ? "ON" : "OFF") << std::endl;
                break;
            case SDLK_s:
                SaveProject();
                break;
            case SDLK_l:
                LoadProject("my_game.json");
                break;
            case SDLK_n:
                NewProject();
                break;
        }
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
        
        // Применяем трансформацию камеры
        glPushMatrix();
        glTranslatef(-m_cameraX, -m_cameraY, 0);
        
        // Отрисовка сетки
        if (m_showGrid) {
            RenderGrid();
        }
        
        // Отрисовка объектов
        RenderEntities();
        
        glPopMatrix();
    }

    void RenderGrid() {
        glColor3f(0.5f, 0.5f, 0.5f);
        glBegin(GL_LINES);
        
        // Вертикальные линии
        for (float x = 0; x < 2000; x += m_gridSize) {
            glVertex2f(x, 0);
            glVertex2f(x, 2000);
        }
        
        // Горизонтальные линии
        for (float y = 0; y < 2000; y += m_gridSize) {
            glVertex2f(0, y);
            glVertex2f(2000, y);
        }
        
        glEnd();
    }

    void RenderEntities() {
        for (size_t i = 0; i < m_entities.size(); ++i) {
            const Entity& entity = m_entities[i];
            
            // Выделяем выбранный объект
            if (i == m_selectedEntity) {
                glColor3f(1.0f, 1.0f, 0.0f); // Желтый для выбранного
            } else {
                glColor3f(entity.r, entity.g, entity.b);
            }
            
            // Отрисовка квадрата
            glBegin(GL_QUADS);
            glVertex2f(entity.x - entity.size/2, entity.y - entity.size/2);
            glVertex2f(entity.x + entity.size/2, entity.y - entity.size/2);
            glVertex2f(entity.x + entity.size/2, entity.y + entity.size/2);
            glVertex2f(entity.x - entity.size/2, entity.y + entity.size/2);
            glEnd();
        }
    }

    void SelectEntityAt(int screenX, int screenY) {
        // Преобразуем экранные координаты в мировые
        float worldX = screenX + m_cameraX;
        float worldY = screenY + m_cameraY;
        
        m_selectedEntity = -1;
        for (size_t i = 0; i < m_entities.size(); ++i) {
            const Entity& entity = m_entities[i];
            float distance = sqrt((worldX - entity.x) * (worldX - entity.x) + 
                                 (worldY - entity.y) * (worldY - entity.y));
            if (distance < entity.size) {
                m_selectedEntity = i;
                std::cout << "Selected entity " << i << " at (" << entity.x << ", " << entity.y << ")" << std::endl;
                break;
            }
        }
    }

    void CreateEntityAt(int screenX, int screenY) {
        // Преобразуем экранные координаты в мировые
        float worldX = screenX + m_cameraX;
        float worldY = screenY + m_cameraY;
        
        // Привязка к сетке
        if (m_snapToGrid) {
            worldX = std::round(worldX / m_gridSize) * m_gridSize;
            worldY = std::round(worldY / m_gridSize) * m_gridSize;
        }
        
        Entity entity;
        entity.x = worldX;
        entity.y = worldY;
        entity.size = 32.0f;
        entity.r = 0.0f;
        entity.g = 1.0f;
        entity.b = 0.0f;
        
        m_entities.push_back(entity);
        m_selectedEntity = m_entities.size() - 1;
        
        std::cout << "Created entity at (" << worldX << ", " << worldY << ")" << std::endl;
    }

    void SaveProject() {
        Json::Value root;
        root["name"] = m_projectName;
        root["version"] = "1.0.0";
        
        Json::Value entities(Json::arrayValue);
        for (const auto& entity : m_entities) {
            Json::Value entityData;
            entityData["x"] = entity.x;
            entityData["y"] = entity.y;
            entityData["size"] = entity.size;
            entityData["r"] = entity.r;
            entityData["g"] = entity.g;
            entityData["b"] = entity.b;
            entities.append(entityData);
        }
        root["entities"] = entities;
        
        std::ofstream file("my_game.json");
        if (file.is_open()) {
            file << root;
            file.close();
            std::cout << "Project saved to my_game.json" << std::endl;
        } else {
            std::cerr << "Failed to save project" << std::endl;
        }
    }

    void LoadProject(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "No existing project found, starting with empty scene" << std::endl;
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
            entity.x = entityData.get("x", 0.0f).asFloat();
            entity.y = entityData.get("y", 0.0f).asFloat();
            entity.size = entityData.get("size", 32.0f).asFloat();
            entity.r = entityData.get("r", 0.0f).asFloat();
            entity.g = entityData.get("g", 1.0f).asFloat();
            entity.b = entityData.get("b", 0.0f).asFloat();
            m_entities.push_back(entity);
        }
        
        std::cout << "Loaded project: " << m_projectName << " with " << m_entities.size() << " entities" << std::endl;
    }

    void NewProject() {
        m_entities.clear();
        m_selectedEntity = -1;
        m_projectName = "New Project";
        std::cout << "New project created" << std::endl;
    }
};

int main() {
    SimpleGameEditor editor;
    
    if (!editor.Initialize()) {
        std::cerr << "Failed to initialize editor" << std::endl;
        return -1;
    }
    
    editor.Run();
    editor.Shutdown();
    
    return 0;
}
