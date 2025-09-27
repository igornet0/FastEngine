#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QDockWidget>
#include <QAction>
#include <QActionGroup>
#include <QTimer>
#include <memory>

class SceneView2D;
class SceneView3D;
class InspectorPanel;
class HierarchyPanel;
class AssetsPanel;
class ConsolePanel;
class ProjectManager;
class FastEngineIntegration;

QT_BEGIN_NAMESPACE
class QStackedWidget;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectAs();
    void exportProject();
    
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void deleteSelected();
    
    void toggle2DMode();
    void toggle3DMode();
    void playScene();
    void pauseScene();
    void stopScene();
    
    void showInspector();
    void showHierarchy();
    void showAssets();
    void showConsole();
    void showGamePreview();
    
    void onSceneModeChanged();
    void onSelectionChanged();
    void onProjectChanged();
    
    void updateStatusBar();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupDockWidgets();
    void setupConnections();
    
    void createActions();
    void updateActions();
    
    bool saveProjectIfNeeded();
    void loadProject(const QString &filePath);
    void saveProjectToFile(const QString &filePath);
    
    // UI компоненты
    QStackedWidget *m_sceneStack;
    SceneView2D *m_sceneView2D;
    SceneView3D *m_sceneView3D;
    
    // Панели
    InspectorPanel *m_inspectorPanel;
    HierarchyPanel *m_hierarchyPanel;
    AssetsPanel *m_assetsPanel;
    ConsolePanel *m_consolePanel;
    
    // Dock виджеты
    QDockWidget *m_inspectorDock;
    QDockWidget *m_hierarchyDock;
    QDockWidget *m_assetsDock;
    QDockWidget *m_consoleDock;
    
    // Меню и панели инструментов
    QMenuBar *m_menuBar;
    QToolBar *m_mainToolBar;
    QStatusBar *m_statusBar;
    
    // Действия
    QAction *m_newProjectAction;
    QAction *m_openProjectAction;
    QAction *m_saveProjectAction;
    QAction *m_saveProjectAsAction;
    QAction *m_exportProjectAction;
    
    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_deleteAction;
    
    QActionGroup *m_sceneModeGroup;
    QAction *m_2DModeAction;
    QAction *m_3DModeAction;
    
    QAction *m_playAction;
    QAction *m_pauseAction;
    QAction *m_stopAction;
    
    QAction *m_showInspectorAction;
    QAction *m_showHierarchyAction;
    QAction *m_showAssetsAction;
    QAction *m_showConsoleAction;
    QAction *m_showGamePreviewAction;
    
    // Менеджеры
    std::unique_ptr<ProjectManager> m_projectManager;
    std::unique_ptr<FastEngineIntegration> m_fastEngineIntegration;
    
    // Состояние
    bool m_isPlaying;
    bool m_isPaused;
    bool m_projectModified;
    QString m_currentProjectPath;
    
    // Таймер обновления
    QTimer *m_updateTimer;
};

