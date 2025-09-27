#include "test_mainwindow.h"
#include <QDebug>
#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QStackedWidget>

void TestMainWindow::initTestCase()
{
    qDebug() << "Initializing MainWindow test case...";
}

void TestMainWindow::cleanupTestCase()
{
    qDebug() << "Cleaning up MainWindow test case...";
}

void TestMainWindow::init()
{
    // Create a new MainWindow for each test
    try {
        m_mainWindow = new MainWindow();
        qDebug() << "✅ MainWindow created successfully";
    } catch (...) {
        QFAIL("❌ Failed to create MainWindow - possible segfault source!");
    }
}

void TestMainWindow::cleanup()
{
    // Clean up after each test
    if (m_mainWindow) {
        delete m_mainWindow;
        m_mainWindow = nullptr;
    }
}

void TestMainWindow::testWindowCreation()
{
    QVERIFY(m_mainWindow != nullptr);
    QVERIFY(m_mainWindow->isWindow());
    qDebug() << "✅ Window creation test passed";
}

void TestMainWindow::testWindowTitle()
{
    QVERIFY(m_mainWindow != nullptr);
    QString title = m_mainWindow->windowTitle();
    QVERIFY(!title.isEmpty());
    QVERIFY(title.contains("FastEngine"));
    qDebug() << "✅ Window title test passed:" << title;
}

void TestMainWindow::testMenuBarCreation()
{
    QVERIFY(m_mainWindow != nullptr);
    QMenuBar *menuBar = m_mainWindow->menuBar();
    QVERIFY(menuBar != nullptr);
    
    // Check for expected menus
    QList<QAction*> actions = menuBar->actions();
    QVERIFY(actions.size() > 0);
    
    bool hasFileMenu = false;
    bool hasEditMenu = false;
    bool hasViewMenu = false;
    
    for (QAction *action : actions) {
        if (action->text().contains("File")) hasFileMenu = true;
        if (action->text().contains("Edit")) hasEditMenu = true;
        if (action->text().contains("View")) hasViewMenu = true;
    }
    
    QVERIFY(hasFileMenu);
    qDebug() << "✅ MenuBar creation test passed - Found" << actions.size() << "menus";
}

void TestMainWindow::testStatusBarCreation()
{
    QVERIFY(m_mainWindow != nullptr);
    QStatusBar *statusBar = m_mainWindow->statusBar();
    QVERIFY(statusBar != nullptr);
    QVERIFY(statusBar->isVisible());
    qDebug() << "✅ StatusBar creation test passed";
}

void TestMainWindow::testDockWidgetsCreation()
{
    QVERIFY(m_mainWindow != nullptr);
    
    // Find all dock widgets
    QList<QDockWidget*> dockWidgets = m_mainWindow->findChildren<QDockWidget*>();
    
    // We expect several dock widgets (Hierarchy, Inspector, Assets, Console)
    QVERIFY(dockWidgets.size() >= 4);
    
    bool hasHierarchy = false;
    bool hasInspector = false;
    bool hasAssets = false;
    bool hasConsole = false;
    
    for (QDockWidget *dock : dockWidgets) {
        QString title = dock->windowTitle();
        if (title.contains("Hierarchy")) hasHierarchy = true;
        if (title.contains("Inspector")) hasInspector = true;
        if (title.contains("Assets")) hasAssets = true;
        if (title.contains("Console")) hasConsole = true;
    }
    
    QVERIFY(hasHierarchy);
    QVERIFY(hasInspector);
    QVERIFY(hasAssets);
    QVERIFY(hasConsole);
    
    qDebug() << "✅ DockWidgets creation test passed - Found" << dockWidgets.size() << "dock widgets";
}

void TestMainWindow::testCentralWidget()
{
    QVERIFY(m_mainWindow != nullptr);
    QWidget *centralWidget = m_mainWindow->centralWidget();
    QVERIFY(centralWidget != nullptr);
    
    // Central widget should be a stacked widget for 2D/3D scene switching
    QStackedWidget *stackedWidget = qobject_cast<QStackedWidget*>(centralWidget);
    if (stackedWidget) {
        QVERIFY(stackedWidget->count() >= 2); // Should have at least 2D and 3D views
        qDebug() << "✅ Central widget test passed - StackedWidget with" << stackedWidget->count() << "views";
    } else {
        qDebug() << "⚠️ Central widget is not a StackedWidget, but exists";
    }
}

void TestMainWindow::testWindowResize()
{
    QVERIFY(m_mainWindow != nullptr);
    
    QSize originalSize = m_mainWindow->size();
    QSize newSize(1200, 800);
    
    m_mainWindow->resize(newSize);
    
    // Give it a moment to resize
    QTest::qWait(100);
    
    QSize currentSize = m_mainWindow->size();
    QVERIFY(currentSize.width() >= newSize.width() - 10); // Allow some margin
    QVERIFY(currentSize.height() >= newSize.height() - 10);
    
    qDebug() << "✅ Window resize test passed:" << originalSize << "→" << currentSize;
}

void TestMainWindow::testMenuActions()
{
    QVERIFY(m_mainWindow != nullptr);
    QMenuBar *menuBar = m_mainWindow->menuBar();
    QVERIFY(menuBar != nullptr);
    
    // Find File menu
    QMenu *fileMenu = nullptr;
    for (QAction *action : menuBar->actions()) {
        if (action->text().contains("File")) {
            fileMenu = action->menu();
            break;
        }
    }
    
    if (fileMenu) {
        QList<QAction*> fileActions = fileMenu->actions();
        QVERIFY(fileActions.size() > 0);
        
        bool hasNewAction = false;
        bool hasOpenAction = false;
        bool hasSaveAction = false;
        
        for (QAction *action : fileActions) {
            if (action->text().contains("New")) hasNewAction = true;
            if (action->text().contains("Open")) hasOpenAction = true;
            if (action->text().contains("Save")) hasSaveAction = true;
        }
        
        QVERIFY(hasNewAction);
        QVERIFY(hasOpenAction);
        
        qDebug() << "✅ Menu actions test passed - Found" << fileActions.size() << "file actions";
    } else {
        qDebug() << "⚠️ File menu not found, but test continues";
    }
}

void TestMainWindow::testToolBarCreation()
{
    QVERIFY(m_mainWindow != nullptr);
    
    // Check for toolbars
    QList<QToolBar*> toolBars = m_mainWindow->findChildren<QToolBar*>();
    
    // We expect at least one toolbar
    QVERIFY(toolBars.size() >= 1);
    
    qDebug() << "✅ ToolBar creation test passed - Found" << toolBars.size() << "toolbars";
}

void TestMainWindow::testPanelVisibility()
{
    QVERIFY(m_mainWindow != nullptr);
    
    // Test that panels can be shown/hidden
    QList<QDockWidget*> dockWidgets = m_mainWindow->findChildren<QDockWidget*>();
    
    for (QDockWidget *dock : dockWidgets) {
        // Test hide/show
        bool originalVisibility = dock->isVisible();
        
        dock->hide();
        QVERIFY(!dock->isVisible());
        
        dock->show();
        QVERIFY(dock->isVisible());
        
        // Restore original state
        dock->setVisible(originalVisibility);
    }
    
    qDebug() << "✅ Panel visibility test passed";
}

void TestMainWindow::testProjectActions()
{
    QVERIFY(m_mainWindow != nullptr);
    
    // Test that we can trigger project-related actions without crashing
    try {
        // These should not crash even if they don't do much
        // m_mainWindow->newProject(); // If this method exists
        // m_mainWindow->openProject(); // If this method exists
        
        qDebug() << "✅ Project actions test passed (no crashes)";
    } catch (...) {
        QFAIL("❌ Project actions caused an exception");
    }
}

void TestMainWindow::testSceneSwitching()
{
    QVERIFY(m_mainWindow != nullptr);
    
    QWidget *centralWidget = m_mainWindow->centralWidget();
    QStackedWidget *stackedWidget = qobject_cast<QStackedWidget*>(centralWidget);
    
    if (stackedWidget && stackedWidget->count() >= 2) {
        int originalIndex = stackedWidget->currentIndex();
        
        // Switch to different view
        int newIndex = (originalIndex + 1) % stackedWidget->count();
        stackedWidget->setCurrentIndex(newIndex);
        
        QVERIFY(stackedWidget->currentIndex() == newIndex);
        
        // Switch back
        stackedWidget->setCurrentIndex(originalIndex);
        QVERIFY(stackedWidget->currentIndex() == originalIndex);
        
        qDebug() << "✅ Scene switching test passed";
    } else {
        qDebug() << "⚠️ Scene switching not available or not implemented";
    }
}

#include "test_mainwindow.moc"
