#include "MainWindow.h"
#include "SceneView2D.h"
#include "SceneView3D.h"
#include "InspectorPanel.h"
#include "HierarchyPanel.h"
#include "AssetsPanel.h"
#include "ConsolePanel.h"
#include "ProjectManager.h"
#include "FastEngineIntegration.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QDockWidget>
#include <QAction>
#include <QActionGroup>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QProgressBar>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_sceneStack(nullptr)
    , m_sceneView2D(nullptr)
    , m_sceneView3D(nullptr)
    , m_inspectorPanel(nullptr)
    , m_hierarchyPanel(nullptr)
    , m_assetsPanel(nullptr)
    , m_consolePanel(nullptr)
    , m_inspectorDock(nullptr)
    , m_hierarchyDock(nullptr)
    , m_assetsDock(nullptr)
    , m_consoleDock(nullptr)
    , m_menuBar(nullptr)
    , m_mainToolBar(nullptr)
    , m_statusBar(nullptr)
    , m_isPlaying(false)
    , m_isPaused(false)
    , m_projectModified(false)
    , m_updateTimer(nullptr)
{
    // Инициализация менеджеров
    m_projectManager = std::make_unique<ProjectManager>();
    m_fastEngineIntegration = std::make_unique<FastEngineIntegration>();
    
    setupUI();
    createActions();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupDockWidgets();
    setupConnections();
    
    // Настройка таймера обновления
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
    m_updateTimer->start(16); // ~60 FPS
    
    // Установка режима по умолчанию
    m_2DModeAction->setChecked(true);
    onSceneModeChanged();
    
    // Загрузка последнего проекта
    // m_projectManager->loadLastProject(); // Временно отключено
    
    setWindowTitle("FastEngine Qt Editor - Untitled Project");
    resize(1400, 900);
}

MainWindow::~MainWindow()
{
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
}

void MainWindow::setupUI()
{
    // Центральный виджет с переключаемыми сценами
    m_sceneStack = new QStackedWidget(this);
    setCentralWidget(m_sceneStack);
    
    // 2D сцена
    m_sceneView2D = new SceneView2D(this);
    m_sceneStack->addWidget(m_sceneView2D);
    
    // 3D сцена
    m_sceneView3D = new SceneView3D(this);
    m_sceneStack->addWidget(m_sceneView3D);
}

void MainWindow::setupMenuBar()
{
    m_menuBar = menuBar();
    
    // File меню
    QMenu *fileMenu = m_menuBar->addMenu("&File");
    fileMenu->addAction(m_newProjectAction);
    fileMenu->addAction(m_openProjectAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_saveProjectAction);
    fileMenu->addAction(m_saveProjectAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exportProjectAction);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);
    
    // Edit меню
    QMenu *editMenu = m_menuBar->addMenu("&Edit");
    editMenu->addAction(m_undoAction);
    editMenu->addAction(m_redoAction);
    editMenu->addSeparator();
    editMenu->addAction(m_cutAction);
    editMenu->addAction(m_copyAction);
    editMenu->addAction(m_pasteAction);
    editMenu->addSeparator();
    editMenu->addAction(m_deleteAction);
    
    // View меню
    QMenu *viewMenu = m_menuBar->addMenu("&View");
    QMenu *sceneModeMenu = viewMenu->addMenu("&Scene Mode");
    sceneModeMenu->addAction(m_2DModeAction);
    sceneModeMenu->addAction(m_3DModeAction);
    viewMenu->addSeparator();
    viewMenu->addAction(m_showInspectorAction);
    viewMenu->addAction(m_showHierarchyAction);
    viewMenu->addAction(m_showAssetsAction);
    viewMenu->addAction(m_showConsoleAction);
    viewMenu->addAction(m_showGamePreviewAction);
    
    // Scene меню
    QMenu *sceneMenu = m_menuBar->addMenu("&Scene");
    sceneMenu->addAction(m_playAction);
    sceneMenu->addAction(m_pauseAction);
    sceneMenu->addAction(m_stopAction);
    
    // Help меню
    QMenu *helpMenu = m_menuBar->addMenu("&Help");
    helpMenu->addAction("&About", [this]() {
        QMessageBox::about(this, "About FastEngine Qt Editor",
                          "FastEngine Qt Editor v1.0.0\n"
                          "A modern game editor with 2D and 3D scene support.\n"
                          "Built with Qt6 and FastEngine.");
    });
}

void MainWindow::setupToolBar()
{
    m_mainToolBar = addToolBar("Main Toolbar");
    m_mainToolBar->setMovable(false);
    
    // Файловые операции
    m_mainToolBar->addAction(m_newProjectAction);
    m_mainToolBar->addAction(m_openProjectAction);
    m_mainToolBar->addAction(m_saveProjectAction);
    m_mainToolBar->addSeparator();
    
    // Редактирование
    m_mainToolBar->addAction(m_undoAction);
    m_mainToolBar->addAction(m_redoAction);
    m_mainToolBar->addSeparator();
    
    // Режимы сцены
    m_mainToolBar->addAction(m_2DModeAction);
    m_mainToolBar->addAction(m_3DModeAction);
    m_mainToolBar->addSeparator();
    
    // Воспроизведение
    m_mainToolBar->addAction(m_playAction);
    m_mainToolBar->addAction(m_pauseAction);
    m_mainToolBar->addAction(m_stopAction);
}

void MainWindow::setupStatusBar()
{
    m_statusBar = statusBar();
    
    // Информация о проекте
    QLabel *projectLabel = new QLabel("Project: Untitled");
    m_statusBar->addWidget(projectLabel);
    
    // Режим сцены
    QLabel *modeLabel = new QLabel("Mode: 2D");
    m_statusBar->addWidget(modeLabel);
    
    // Статус воспроизведения
    QLabel *playStatusLabel = new QLabel("Stopped");
    m_statusBar->addWidget(playStatusLabel);
    
    // Прогресс-бар для операций
    QProgressBar *progressBar = new QProgressBar();
    progressBar->setVisible(false);
    m_statusBar->addPermanentWidget(progressBar);
}

void MainWindow::setupDockWidgets()
{
    // Inspector Panel
    m_inspectorPanel = new InspectorPanel(this);
    m_inspectorDock = new QDockWidget("Inspector", this);
    m_inspectorDock->setWidget(m_inspectorPanel);
    m_inspectorDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_inspectorDock);
    
    // Hierarchy Panel
    m_hierarchyPanel = new HierarchyPanel(this);
    m_hierarchyDock = new QDockWidget("Hierarchy", this);
    m_hierarchyDock->setWidget(m_hierarchyPanel);
    m_hierarchyDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, m_hierarchyDock);
    
    // Assets Panel
    m_assetsPanel = new AssetsPanel(this);
    m_assetsDock = new QDockWidget("Assets", this);
    m_assetsDock->setWidget(m_assetsPanel);
    m_assetsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, m_assetsDock);
    
    // Console Panel
    m_consolePanel = new ConsolePanel(this);
    m_consoleDock = new QDockWidget("Console", this);
    m_consoleDock->setWidget(m_consolePanel);
    m_consoleDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, m_consoleDock);
}

void MainWindow::setupConnections()
{
    // Соединения между панелями
    connect(m_hierarchyPanel, &HierarchyPanel::selectionChanged,
            m_inspectorPanel, &InspectorPanel::onSelectionChanged);
    connect(m_hierarchyPanel, &HierarchyPanel::selectionChanged,
            this, &MainWindow::onSelectionChanged);
    
    connect(m_sceneView2D, &SceneView2D::selectionChanged,
            m_hierarchyPanel, &HierarchyPanel::onSelectionChanged);
    connect(m_sceneView3D, &SceneView3D::selectionChanged,
            m_hierarchyPanel, &HierarchyPanel::onSelectionChanged);
    
    connect(m_assetsPanel, &AssetsPanel::assetSelected,
            m_sceneView2D, &SceneView2D::onAssetSelected);
    connect(m_assetsPanel, &AssetsPanel::assetSelected,
            m_sceneView3D, &SceneView3D::onAssetSelected);
    
    // Соединения с менеджером проектов
    connect(m_projectManager.get(), &ProjectManager::projectChanged,
            this, &MainWindow::onProjectChanged);
}

void MainWindow::createActions()
{
    // File actions
    m_newProjectAction = new QAction("&New Project", this);
    m_newProjectAction->setShortcut(QKeySequence::New);
    m_newProjectAction->setStatusTip("Create a new project");
    connect(m_newProjectAction, &QAction::triggered, this, &MainWindow::newProject);
    
    m_openProjectAction = new QAction("&Open Project...", this);
    m_openProjectAction->setShortcut(QKeySequence::Open);
    m_openProjectAction->setStatusTip("Open an existing project");
    connect(m_openProjectAction, &QAction::triggered, this, &MainWindow::openProject);
    
    m_saveProjectAction = new QAction("&Save Project", this);
    m_saveProjectAction->setShortcut(QKeySequence::Save);
    m_saveProjectAction->setStatusTip("Save the current project");
    connect(m_saveProjectAction, &QAction::triggered, this, &MainWindow::saveProject);
    
    m_saveProjectAsAction = new QAction("Save Project &As...", this);
    m_saveProjectAsAction->setShortcut(QKeySequence::SaveAs);
    m_saveProjectAsAction->setStatusTip("Save the current project with a new name");
    connect(m_saveProjectAsAction, &QAction::triggered, this, &MainWindow::saveProjectAs);
    
    m_exportProjectAction = new QAction("&Export Project...", this);
    m_exportProjectAction->setStatusTip("Export project for target platform");
    connect(m_exportProjectAction, &QAction::triggered, this, &MainWindow::exportProject);
    
    // Edit actions
    m_undoAction = new QAction("&Undo", this);
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_undoAction->setStatusTip("Undo the last action");
    connect(m_undoAction, &QAction::triggered, this, &MainWindow::undo);
    
    m_redoAction = new QAction("&Redo", this);
    m_redoAction->setShortcut(QKeySequence::Redo);
    m_redoAction->setStatusTip("Redo the last undone action");
    connect(m_redoAction, &QAction::triggered, this, &MainWindow::redo);
    
    m_cutAction = new QAction("Cu&t", this);
    m_cutAction->setShortcut(QKeySequence::Cut);
    m_cutAction->setStatusTip("Cut selected objects");
    connect(m_cutAction, &QAction::triggered, this, &MainWindow::cut);
    
    m_copyAction = new QAction("&Copy", this);
    m_copyAction->setShortcut(QKeySequence::Copy);
    m_copyAction->setStatusTip("Copy selected objects");
    connect(m_copyAction, &QAction::triggered, this, &MainWindow::copy);
    
    m_pasteAction = new QAction("&Paste", this);
    m_pasteAction->setShortcut(QKeySequence::Paste);
    m_pasteAction->setStatusTip("Paste copied objects");
    connect(m_pasteAction, &QAction::triggered, this, &MainWindow::paste);
    
    m_deleteAction = new QAction("&Delete", this);
    m_deleteAction->setShortcut(QKeySequence::Delete);
    m_deleteAction->setStatusTip("Delete selected objects");
    connect(m_deleteAction, &QAction::triggered, this, &MainWindow::deleteSelected);
    
    // Scene mode actions
    m_sceneModeGroup = new QActionGroup(this);
    
    m_2DModeAction = new QAction("&2D Mode", this);
    m_2DModeAction->setCheckable(true);
    m_2DModeAction->setStatusTip("Switch to 2D scene editing");
    m_sceneModeGroup->addAction(m_2DModeAction);
    connect(m_2DModeAction, &QAction::triggered, this, &MainWindow::toggle2DMode);
    
    m_3DModeAction = new QAction("&3D Mode", this);
    m_3DModeAction->setCheckable(true);
    m_3DModeAction->setStatusTip("Switch to 3D scene editing");
    m_sceneModeGroup->addAction(m_3DModeAction);
    connect(m_3DModeAction, &QAction::triggered, this, &MainWindow::toggle3DMode);
    
    // Playback actions
    m_playAction = new QAction("&Play", this);
    m_playAction->setShortcut(QKeySequence("F5"));
    m_playAction->setStatusTip("Play the scene");
    connect(m_playAction, &QAction::triggered, this, &MainWindow::playScene);
    
    m_pauseAction = new QAction("&Pause", this);
    m_pauseAction->setShortcut(QKeySequence("F6"));
    m_pauseAction->setStatusTip("Pause the scene");
    connect(m_pauseAction, &QAction::triggered, this, &MainWindow::pauseScene);
    
    m_stopAction = new QAction("&Stop", this);
    m_stopAction->setShortcut(QKeySequence("F7"));
    m_stopAction->setStatusTip("Stop the scene");
    connect(m_stopAction, &QAction::triggered, this, &MainWindow::stopScene);
    
    // View actions
    m_showInspectorAction = new QAction("&Inspector", this);
    m_showInspectorAction->setCheckable(true);
    m_showInspectorAction->setChecked(true);
    m_showInspectorAction->setStatusTip("Show/hide Inspector panel");
    connect(m_showInspectorAction, &QAction::triggered, this, &MainWindow::showInspector);
    
    m_showHierarchyAction = new QAction("&Hierarchy", this);
    m_showHierarchyAction->setCheckable(true);
    m_showHierarchyAction->setChecked(true);
    m_showHierarchyAction->setStatusTip("Show/hide Hierarchy panel");
    connect(m_showHierarchyAction, &QAction::triggered, this, &MainWindow::showHierarchy);
    
    m_showAssetsAction = new QAction("&Assets", this);
    m_showAssetsAction->setCheckable(true);
    m_showAssetsAction->setChecked(true);
    m_showAssetsAction->setStatusTip("Show/hide Assets panel");
    connect(m_showAssetsAction, &QAction::triggered, this, &MainWindow::showAssets);
    
    m_showConsoleAction = new QAction("&Console", this);
    m_showConsoleAction->setCheckable(true);
    m_showConsoleAction->setChecked(true);
    m_showConsoleAction->setStatusTip("Show/hide Console panel");
    connect(m_showConsoleAction, &QAction::triggered, this, &MainWindow::showConsole);
    
    m_showGamePreviewAction = new QAction("&Game Preview", this);
    m_showGamePreviewAction->setCheckable(true);
    m_showGamePreviewAction->setStatusTip("Show/hide Game Preview panel");
    connect(m_showGamePreviewAction, &QAction::triggered, this, &MainWindow::showGamePreview);
}

void MainWindow::updateActions()
{
    // Обновление состояния действий в зависимости от текущего состояния
    bool hasSelection = m_hierarchyPanel->hasSelection();
    bool isPlaying = m_isPlaying;
    
    m_cutAction->setEnabled(hasSelection && !isPlaying);
    m_copyAction->setEnabled(hasSelection && !isPlaying);
    m_pasteAction->setEnabled(!isPlaying);
    m_deleteAction->setEnabled(hasSelection && !isPlaying);
    
    m_playAction->setEnabled(!isPlaying);
    m_pauseAction->setEnabled(isPlaying && !m_isPaused);
    m_stopAction->setEnabled(isPlaying);
}

// Реализация слотов
void MainWindow::newProject()
{
    if (!saveProjectIfNeeded()) {
        return;
    }
    
    m_projectManager->newProject("New Project");
    m_projectModified = false;
    setWindowTitle("FastEngine Qt Editor - New Project");
}

void MainWindow::openProject()
{
    if (!saveProjectIfNeeded()) {
        return;
    }
    
    QString filePath = QFileDialog::getOpenFileName(this,
        "Open Project", "", "FastEngine Projects (*.feproj)");
    
    if (!filePath.isEmpty()) {
        loadProject(filePath);
    }
}

void MainWindow::saveProject()
{
    if (m_currentProjectPath.isEmpty()) {
        saveProjectAs();
    } else {
        saveProjectToFile(m_currentProjectPath);
    }
}

void MainWindow::saveProjectAs()
{
    QString filePath = QFileDialog::getSaveFileName(this,
        "Save Project As", "", "FastEngine Projects (*.feproj)");
    
    if (!filePath.isEmpty()) {
        saveProjectToFile(filePath);
    }
}

void MainWindow::exportProject()
{
    // TODO: Реализовать экспорт проекта
    QMessageBox::information(this, "Export Project", "Export functionality will be implemented soon.");
}

void MainWindow::undo()
{
    // TODO: Реализовать отмену действий
    m_consolePanel->addMessage("Undo not implemented yet", MessageType::Warning);
}

void MainWindow::redo()
{
    // TODO: Реализовать повтор действий
    m_consolePanel->addMessage("Redo not implemented yet", MessageType::Warning);
}

void MainWindow::cut()
{
    // TODO: Реализовать вырезание объектов
    m_consolePanel->addMessage("Cut not implemented yet", MessageType::Warning);
}

void MainWindow::copy()
{
    // TODO: Реализовать копирование объектов
    m_consolePanel->addMessage("Copy not implemented yet", MessageType::Warning);
}

void MainWindow::paste()
{
    // TODO: Реализовать вставку объектов
    m_consolePanel->addMessage("Paste not implemented yet", MessageType::Warning);
}

void MainWindow::deleteSelected()
{
    // TODO: Реализовать удаление выбранных объектов
    m_consolePanel->addMessage("Delete not implemented yet", MessageType::Warning);
}

void MainWindow::toggle2DMode()
{
    m_sceneStack->setCurrentWidget(m_sceneView2D);
    onSceneModeChanged();
}

void MainWindow::toggle3DMode()
{
    m_sceneStack->setCurrentWidget(m_sceneView3D);
    onSceneModeChanged();
}

void MainWindow::playScene()
{
    m_isPlaying = true;
    m_isPaused = false;
    m_playAction->setEnabled(false);
    m_pauseAction->setEnabled(true);
    m_stopAction->setEnabled(true);
    
    m_consolePanel->addMessage("Scene started playing", MessageType::Info);
}

void MainWindow::pauseScene()
{
    m_isPaused = !m_isPaused;
    m_playAction->setEnabled(m_isPaused);
    m_pauseAction->setText(m_isPaused ? "Resume" : "Pause");
    
    m_consolePanel->addMessage(m_isPaused ? "Scene paused" : "Scene resumed", 
                              MessageType::Info);
}

void MainWindow::stopScene()
{
    m_isPlaying = false;
    m_isPaused = false;
    m_playAction->setEnabled(true);
    m_pauseAction->setEnabled(false);
    m_pauseAction->setText("Pause");
    m_stopAction->setEnabled(false);
    
    m_consolePanel->addMessage("Scene stopped", MessageType::Info);
}

void MainWindow::showInspector()
{
    m_inspectorDock->setVisible(m_showInspectorAction->isChecked());
}

void MainWindow::showHierarchy()
{
    m_hierarchyDock->setVisible(m_showHierarchyAction->isChecked());
}

void MainWindow::showAssets()
{
    m_assetsDock->setVisible(m_showAssetsAction->isChecked());
}

void MainWindow::showConsole()
{
    m_consoleDock->setVisible(m_showConsoleAction->isChecked());
}

void MainWindow::showGamePreview()
{
    // TODO: Реализовать панель предпросмотра игры
    m_consolePanel->addMessage("Game Preview not implemented yet", MessageType::Warning);
}

void MainWindow::onSceneModeChanged()
{
    bool is2D = m_2DModeAction->isChecked();
    QString mode = is2D ? "2D" : "3D";
    
    // Обновление статус-бара
    QLabel *modeLabel = qobject_cast<QLabel*>(m_statusBar->children().at(1));
    if (modeLabel) {
        modeLabel->setText("Mode: " + mode);
    }
    
    m_consolePanel->addMessage("Switched to " + mode + " mode", MessageType::Info);
}

void MainWindow::onSelectionChanged()
{
    updateActions();
}

void MainWindow::onProjectChanged()
{
    m_projectModified = true;
    setWindowTitle("FastEngine Qt Editor - " + m_projectManager->getProjectName() + " *");
}

void MainWindow::updateStatusBar()
{
    // Обновление статус-бара
    QLabel *playStatusLabel = qobject_cast<QLabel*>(m_statusBar->children().at(2));
    if (playStatusLabel) {
        if (m_isPlaying) {
            playStatusLabel->setText(m_isPaused ? "Paused" : "Playing");
        } else {
            playStatusLabel->setText("Stopped");
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (saveProjectIfNeeded()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::saveProjectIfNeeded()
{
    if (m_projectModified) {
        QMessageBox::StandardButton ret = QMessageBox::question(this,
            "Save Project", "The project has been modified. Do you want to save it?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (ret == QMessageBox::Save) {
            saveProject();
            return true;
        } else if (ret == QMessageBox::Discard) {
            return true;
        } else {
            return false;
        }
    }
    return true;
}

void MainWindow::loadProject(const QString &filePath)
{
    if (m_projectManager->loadProject(filePath)) {
        m_currentProjectPath = filePath;
        m_projectModified = false;
        setWindowTitle("FastEngine Qt Editor - " + m_projectManager->getProjectName());
        m_consolePanel->addMessage("Project loaded: " + filePath, MessageType::Info);
    } else {
        QMessageBox::warning(this, "Load Project", "Failed to load project: " + filePath);
    }
}

void MainWindow::saveProjectToFile(const QString &filePath)
{
    if (m_projectManager->saveProject(filePath)) {
        m_currentProjectPath = filePath;
        m_projectModified = false;
        setWindowTitle("FastEngine Qt Editor - " + m_projectManager->getProjectName());
        m_consolePanel->addMessage("Project saved: " + filePath, MessageType::Info);
    } else {
        QMessageBox::warning(this, "Save Project", "Failed to save project: " + filePath);
    }
}
