#pragma once

#include "ProjectSimulator.h"
#include "SceneEditor.h"
#include "Inspector.h"
#include "Hierarchy.h"
#include "AssetBrowser.h"

namespace ProjectSimulator {

/**
 * Расширенный симулятор с редактором сцен
 * Интегрирует FastEngine с визуальным редактором
 */
class SceneEditorSimulator : public ProjectSimulator {
public:
    SceneEditorSimulator();
    ~SceneEditorSimulator();

    /**
     * Инициализация расширенного симулятора
     * @param windowWidth ширина окна
     * @param windowHeight высота окна
     * @return true если инициализация успешна
     */
    bool Initialize(int windowWidth = 1024, int windowHeight = 768) override;

    /**
     * Запуск главного цикла с редактором сцен
     */
    void Run() override;

    /**
     * Остановка симулятора
     */
    void Shutdown() override;

    /**
     * Получение редактора сцен
     */
    SceneEditor* GetSceneEditor() const { return m_sceneEditor; }

    /**
     * Получение инспектора
     */
    Inspector* GetInspector() const { return m_inspector; }

    /**
     * Получение иерархии
     */
    Hierarchy* GetHierarchy() const { return m_hierarchy; }

    /**
     * Получение браузера ассетов
     */
    AssetBrowser* GetAssetBrowser() const { return m_assetBrowser; }

    /**
     * Создание нового проекта
     * @param projectName имя проекта
     * @param projectPath путь к проекту
     * @return true если проект создан
     */
    bool CreateNewProject(const std::string& projectName, const std::string& projectPath);

    /**
     * Открытие проекта
     * @param projectPath путь к проекту
     * @return true если проект открыт
     */
    bool OpenProject(const std::string& projectPath);

    /**
     * Сохранение проекта
     * @return true если проект сохранен
     */
    bool SaveProject();

    /**
     * Сохранение сцены
     * @param sceneName имя сцены
     * @return true если сцена сохранена
     */
    bool SaveScene(const std::string& sceneName);

    /**
     * Загрузка сцены
     * @param sceneName имя сцены
     * @return true если сцена загружена
     */
    bool LoadScene(const std::string& sceneName);

    /**
     * Создание нового объекта в сцене
     * @param objectType тип объекта
     * @param position позиция
     * @return созданная сущность
     */
    FastEngine::Entity* CreateSceneObject(const std::string& objectType, const glm::vec3& position = glm::vec3(0.0f));

    /**
     * Удаление объекта из сцены
     * @param entity сущность для удаления
     */
    void DeleteSceneObject(FastEngine::Entity* entity);

    /**
     * Выбор объекта в сцене
     * @param entity сущность для выбора
     */
    void SelectSceneObject(FastEngine::Entity* entity);

    /**
     * Получение выбранного объекта
     */
    FastEngine::Entity* GetSelectedObject() const;

    /**
     * Установка режима редактирования
     * @param enabled включен ли режим редактирования
     */
    void SetEditMode(bool enabled) { m_editMode = enabled; }

    /**
     * Получение режима редактирования
     */
    bool IsEditMode() const { return m_editMode; }

    /**
     * Установка режима симуляции
     * @param enabled включен ли режим симуляции
     */
    void SetSimulationMode(bool enabled) { m_simulationMode = enabled; }

    /**
     * Получение режима симуляции
     */
    bool IsSimulationMode() const { return m_simulationMode; }

    /**
     * Переключение между режимами
     */
    void ToggleMode();

    /**
     * Получение состояния редактора
     */
    bool IsEditorInitialized() const { return m_editorInitialized; }

private:
    // Компоненты редактора
    SceneEditor* m_sceneEditor;
    Inspector* m_inspector;
    Hierarchy* m_hierarchy;
    AssetBrowser* m_assetBrowser;
    
    // Состояние
    bool m_editorInitialized;
    bool m_editMode;
    bool m_simulationMode;
    
    // Текущий проект
    std::string m_currentProjectPath;
    std::string m_currentSceneName;
    
    // Вспомогательные методы
    bool InitializeEditor();
    void UpdateEditor(float deltaTime);
    void RenderEditor();
    void HandleEditorInput();
    
    void OnObjectSelected(FastEngine::Entity* entity);
    void OnAssetSelected(const AssetInfo& assetInfo);
    
    void SetupEditorCallbacks();
    void CleanupEditor();
};

} // namespace ProjectSimulator

