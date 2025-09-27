#include <QApplication>
#include <QMainWindow>
#include <QDockWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QLabel>

// Include working components
#include "SceneView2D.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "AssetsPanel.h"
#include "ConsolePanel.h"

class WorkingMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    WorkingMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("FastEngine Qt Editor - Working Version");
        setMinimumSize(1200, 800);
        
        setupUI();
        setupMenus();
        setupDockWidgets();
        setupConnections();
    }

private:
    void setupUI()
    {
        // Central widget - только 2D сцена (без проблемного 3D)
        m_sceneView2D = new SceneView2D(this);
        setCentralWidget(m_sceneView2D);
    }
    
    void setupMenus()
    {
        // File menu
        QMenu *fileMenu = menuBar()->addMenu("&File");
        fileMenu->addAction("&New Project", [this]() { 
            statusBar()->showMessage("New Project clicked", 2000); 
        });
        fileMenu->addAction("&Open Project", [this]() { 
            statusBar()->showMessage("Open Project clicked", 2000); 
        });
        fileMenu->addAction("&Save Project", [this]() { 
            statusBar()->showMessage("Save Project clicked", 2000); 
        });
        fileMenu->addSeparator();
        fileMenu->addAction("E&xit", this, &QWidget::close);
        
        // Edit menu
        QMenu *editMenu = menuBar()->addMenu("&Edit");
        editMenu->addAction("&Undo");
        editMenu->addAction("&Redo");
        editMenu->addSeparator();
        editMenu->addAction("&Cut");
        editMenu->addAction("&Copy");
        editMenu->addAction("&Paste");
        
        // View menu
        QMenu *viewMenu = menuBar()->addMenu("&View");
        viewMenu->addAction("&Hierarchy", [this]() { m_hierarchyPanel->setVisible(!m_hierarchyPanel->isVisible()); });
        viewMenu->addAction("&Inspector", [this]() { m_inspectorPanel->setVisible(!m_inspectorPanel->isVisible()); });
        viewMenu->addAction("&Assets", [this]() { m_assetsPanel->setVisible(!m_assetsPanel->isVisible()); });
        viewMenu->addAction("&Console", [this]() { m_consolePanel->setVisible(!m_consolePanel->isVisible()); });
    }
    
    void setupDockWidgets()
    {
        // Hierarchy Panel
        m_hierarchyPanel = new HierarchyPanel(this);
        addDockWidget(Qt::LeftDockWidgetArea, m_hierarchyPanel);
        
        // Inspector Panel  
        m_inspectorPanel = new InspectorPanel(this);
        addDockWidget(Qt::RightDockWidgetArea, m_inspectorPanel);
        
        // Assets Panel
        m_assetsPanel = new AssetsPanel(this);
        addDockWidget(Qt::BottomDockWidgetArea, m_assetsPanel);
        
        // Console Panel
        m_consolePanel = new ConsolePanel(this);
        addDockWidget(Qt::BottomDockWidgetArea, m_consolePanel);
        
        // Tab the bottom panels
        tabifyDockWidget(m_assetsPanel, m_consolePanel);
        m_assetsPanel->raise();
        
        // Status bar
        statusBar()->showMessage("FastEngine Qt Editor - Ready");
    }
    
    void setupConnections()
    {
        // Connect panels
        connect(m_hierarchyPanel, &HierarchyPanel::selectionChanged,
                m_inspectorPanel, &InspectorPanel::onSelectionChanged);
        
        connect(m_assetsPanel, &AssetsPanel::assetSelected,
                m_sceneView2D, &SceneView2D::onAssetSelected);
        
        // Add some test messages to console
        m_consolePanel->addMessage("FastEngine Qt Editor started successfully!", MessageType::Success);
        m_consolePanel->addMessage("All components loaded", MessageType::Info);
        m_consolePanel->addMessage("Ready for game development", MessageType::Info);
    }

private:
    SceneView2D *m_sceneView2D;
    HierarchyPanel *m_hierarchyPanel;
    InspectorPanel *m_inspectorPanel;
    AssetsPanel *m_assetsPanel;
    ConsolePanel *m_consolePanel;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("FastEngine Qt Editor");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("FastEngine");
    
    WorkingMainWindow window;
    window.show();
    
    return app.exec();
}

#include "working_editor_main.moc"
