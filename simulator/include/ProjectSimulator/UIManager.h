#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace ProjectSimulator {

// Forward declarations
class Project;
class SimulationEngine;

/**
 * UI элементы
 */
enum class UIElement {
    MainMenu,
    ProjectList,
    ProjectDetails,
    SimulationView,
    TestResults,
    Settings
};

/**
 * События UI
 */
struct UIEvent {
    UIElement element;
    std::string action;
    std::string data;
};

/**
 * Менеджер пользовательского интерфейса
 * Управляет отображением и взаимодействием с UI
 */
class UIManager {
public:
    UIManager();
    ~UIManager();

    /**
     * Инициализация UI менеджера
     * @param windowWidth ширина окна
     * @param windowHeight высота окна
     * @return true если инициализация успешна
     */
    bool Initialize(int windowWidth, int windowHeight);

    /**
     * Обновление UI
     * @param deltaTime время с последнего обновления
     */
    void Update(float deltaTime);

    /**
     * Отрисовка UI
     */
    void Render();

    /**
     * Обработка событий
     * @param event событие для обработки
     */
    void HandleEvent(const UIEvent& event);

    /**
     * Установка callback для событий UI
     * @param callback функция обратного вызова
     */
    void SetEventCallback(std::function<void(const UIEvent&)> callback);

    /**
     * Показать главное меню
     */
    void ShowMainMenu();

    /**
     * Показать список проектов
     * @param projects список проектов
     */
    void ShowProjectList(const std::vector<std::shared_ptr<Project>>& projects);

    /**
     * Показать детали проекта
     * @param project проект для отображения
     */
    void ShowProjectDetails(std::shared_ptr<Project> project);

    /**
     * Показать окно симуляции
     * @param project активный проект
     */
    void ShowSimulationView(std::shared_ptr<Project> project);

    /**
     * Показать результаты тестов
     * @param results результаты тестирования
     */
    void ShowTestResults(const std::string& results);

    /**
     * Показать настройки
     */
    void ShowSettings();

    /**
     * Обновить информацию о симуляции
     * @param fps текущий FPS
     * @param frameTime время кадра
     * @param state состояние симуляции
     */
    void UpdateSimulationInfo(float fps, float frameTime, const std::string& state);

    /**
     * Добавить сообщение в лог
     * @param message сообщение
     * @param level уровень (info, warning, error)
     */
    void AddLogMessage(const std::string& message, const std::string& level = "info");

    /**
     * Очистить лог
     */
    void ClearLog();

    /**
     * Получить текущий активный элемент
     */
    UIElement GetActiveElement() const { return m_activeElement; }

    /**
     * Установить активный элемент
     * @param element элемент для активации
     */
    void SetActiveElement(UIElement element) { m_activeElement = element; }

    /**
     * Проверить, инициализирован ли UI
     */
    bool IsInitialized() const { return m_initialized; }

private:
    UIElement m_activeElement;
    std::function<void(const UIEvent&)> m_eventCallback;
    std::vector<std::string> m_logMessages;
    std::string m_simulationState;
    float m_fps;
    float m_frameTime;
    
    bool m_initialized;
    bool m_showLog;
    bool m_showSettings;

    /**
     * Отрисовка главного меню
     */
    void RenderMainMenu();

    /**
     * Отрисовка списка проектов
     */
    void RenderProjectList();

    /**
     * Отрисовка деталей проекта
     */
    void RenderProjectDetails();

    /**
     * Отрисовка окна симуляции
     */
    void RenderSimulationView();

    /**
     * Отрисовка результатов тестов
     */
    void RenderTestResults();

    /**
     * Отрисовка настроек
     */
    void RenderSettings();

    /**
     * Отрисовка лога
     */
    void RenderLog();

    /**
     * Отрисовка панели информации
     */
    void RenderInfoPanel();

    /**
     * Обработка ввода
     */
    void HandleInput();
};

} // namespace ProjectSimulator

