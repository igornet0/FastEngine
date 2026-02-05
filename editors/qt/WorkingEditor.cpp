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
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPushButton>
#include <QTextEdit>
#include <QTreeWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QGroupBox>
#include <QFormLayout>
#include <QScrollArea>
#include <QTabWidget>
#include <QMessageBox>
#include <QTime>

// Простая версия SceneView2D без OpenGL
class SimpleSceneView2D : public QGraphicsView
{
    Q_OBJECT

public:
    SimpleSceneView2D(QWidget *parent = nullptr) : QGraphicsView(parent)
    {
        m_scene = new QGraphicsScene(this);
        setScene(m_scene);
        
        // Настройка сцены
        m_scene->setSceneRect(-2000, -2000, 4000, 4000);
        
        // Настройка view
        setRenderHint(QPainter::Antialiasing);
        setDragMode(QGraphicsView::RubberBandDrag);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        
        // Добавляем тестовые объекты
        addTestObjects();
    }
    
    void addTestObjects()
    {
        // Добавляем несколько тестовых прямоугольников
        for (int i = 0; i < 5; ++i) {
            QGraphicsRectItem *rect = m_scene->addRect(i * 100, 0, 80, 60);
            rect->setFlag(QGraphicsItem::ItemIsMovable, true);
            rect->setFlag(QGraphicsItem::ItemIsSelectable, true);
            rect->setBrush(QColor(100 + i * 30, 150 + i * 20, 200 - i * 10));
        }
    }
    
    void onAssetSelected(const QString &assetPath)
    {
        // Простая обработка выбора ресурса
        QGraphicsRectItem *rect = m_scene->addRect(0, 0, 50, 50);
        rect->setFlag(QGraphicsItem::ItemIsMovable, true);
        rect->setFlag(QGraphicsItem::ItemIsSelectable, true);
        rect->setBrush(QColor(255, 100, 100));
    }

private:
    QGraphicsScene *m_scene;
};

// Простая версия HierarchyPanel
class SimpleHierarchyPanel : public QDockWidget
{
    Q_OBJECT

public:
    SimpleHierarchyPanel(QWidget *parent = nullptr) : QDockWidget("Hierarchy", parent)
    {
        QWidget *widget = new QWidget();
        setWidget(widget);
        
        QVBoxLayout *layout = new QVBoxLayout(widget);
        
        // Кнопки управления
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *addBtn = new QPushButton("+");
        QPushButton *removeBtn = new QPushButton("-");
        QPushButton *duplicateBtn = new QPushButton("Dup");
        
        buttonLayout->addWidget(addBtn);
        buttonLayout->addWidget(removeBtn);
        buttonLayout->addWidget(duplicateBtn);
        layout->addLayout(buttonLayout);
        
        // Tree widget
        m_treeWidget = new QTreeWidget();
        m_treeWidget->setHeaderLabel("Objects");
        m_treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
        m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        layout->addWidget(m_treeWidget);
        
        // Добавляем тестовые объекты
        addTestObjects();
        
        // Подключаем сигналы
        connect(addBtn, &QPushButton::clicked, this, &SimpleHierarchyPanel::addObject);
        connect(removeBtn, &QPushButton::clicked, this, &SimpleHierarchyPanel::removeObject);
        connect(duplicateBtn, &QPushButton::clicked, this, &SimpleHierarchyPanel::duplicateObject);
    }
    
    void addTestObjects()
    {
        QTreeWidgetItem *root = new QTreeWidgetItem(m_treeWidget);
        root->setText(0, "Scene");
        root->setExpanded(true);
        
        for (int i = 0; i < 3; ++i) {
            QTreeWidgetItem *item = new QTreeWidgetItem(root);
            item->setText(0, QString("Object %1").arg(i + 1));
        }
    }
    
    void addObject()
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_treeWidget->topLevelItem(0));
        item->setText(0, QString("New Object %1").arg(m_treeWidget->topLevelItem(0)->childCount()));
        emit objectAdded(item->text(0));
    }
    
    void removeObject()
    {
        QTreeWidgetItem *current = m_treeWidget->currentItem();
        if (current && current->parent()) {
            emit objectRemoved(current->text(0));
            delete current;
        }
    }
    
    void duplicateObject()
    {
        QTreeWidgetItem *current = m_treeWidget->currentItem();
        if (current && current->parent()) {
            QTreeWidgetItem *item = new QTreeWidgetItem(current->parent());
            item->setText(0, current->text(0) + " Copy");
            emit objectDuplicated(item->text(0));
        }
    }

signals:
    void objectAdded(const QString &name);
    void objectRemoved(const QString &name);
    void objectDuplicated(const QString &name);

private:
    QTreeWidget *m_treeWidget;
};

// Простая версия InspectorPanel
class SimpleInspectorPanel : public QDockWidget
{
    Q_OBJECT

public:
    SimpleInspectorPanel(QWidget *parent = nullptr) : QDockWidget("Inspector", parent)
    {
        QWidget *widget = new QWidget();
        setWidget(widget);
        
        QVBoxLayout *layout = new QVBoxLayout(widget);
        
        // Tab widget
        QTabWidget *tabWidget = new QTabWidget();
        layout->addWidget(tabWidget);
        
        // Transform tab
        QWidget *transformTab = new QWidget();
        QFormLayout *transformLayout = new QFormLayout(transformTab);
        
        transformLayout->addRow("Position X:", new QDoubleSpinBox());
        transformLayout->addRow("Position Y:", new QDoubleSpinBox());
        transformLayout->addRow("Position Z:", new QDoubleSpinBox());
        transformLayout->addRow("Rotation X:", new QDoubleSpinBox());
        transformLayout->addRow("Rotation Y:", new QDoubleSpinBox());
        transformLayout->addRow("Rotation Z:", new QDoubleSpinBox());
        transformLayout->addRow("Scale X:", new QDoubleSpinBox());
        transformLayout->addRow("Scale Y:", new QDoubleSpinBox());
        transformLayout->addRow("Scale Z:", new QDoubleSpinBox());
        
        tabWidget->addTab(transformTab, "Transform");
        
        // Render tab
        QWidget *renderTab = new QWidget();
        QFormLayout *renderLayout = new QFormLayout(renderTab);
        
        renderLayout->addRow("Visible:", new QCheckBox());
        renderLayout->addRow("Color:", new QPushButton("Choose Color"));
        renderLayout->addRow("Texture:", new QPushButton("Load Texture"));
        renderLayout->addRow("Opacity:", new QSlider(Qt::Horizontal));
        
        tabWidget->addTab(renderTab, "Render");
        
        // Physics tab
        QWidget *physicsTab = new QWidget();
        QFormLayout *physicsLayout = new QFormLayout(physicsTab);
        
        physicsLayout->addRow("Mass:", new QDoubleSpinBox());
        physicsLayout->addRow("Friction:", new QDoubleSpinBox());
        physicsLayout->addRow("Bounce:", new QDoubleSpinBox());
        physicsLayout->addRow("Static:", new QCheckBox());
        
        tabWidget->addTab(physicsTab, "Physics");
    }
};

// Простая версия AssetsPanel
class SimpleAssetsPanel : public QDockWidget
{
    Q_OBJECT

public:
    SimpleAssetsPanel(QWidget *parent = nullptr) : QDockWidget("Assets", parent)
    {
        QWidget *widget = new QWidget();
        setWidget(widget);
        
        QVBoxLayout *layout = new QVBoxLayout(widget);
        
        // Search bar
        QLineEdit *searchEdit = new QLineEdit();
        searchEdit->setPlaceholderText("Search assets...");
        layout->addWidget(searchEdit);
        
        // Tab widget for different views
        QTabWidget *tabWidget = new QTabWidget();
        layout->addWidget(tabWidget);
        
        // List view
        QListWidget *listWidget = new QListWidget();
        listWidget->setViewMode(QListWidget::IconMode);
        listWidget->setIconSize(QSize(64, 64));
        tabWidget->addTab(listWidget, "Icons");
        
        // Tree view
        QTreeWidget *treeWidget = new QTreeWidget();
        treeWidget->setHeaderLabel("Assets");
        tabWidget->addTab(treeWidget, "Tree");
        
        // Add test assets
        addTestAssets(listWidget, treeWidget);
    }
    
    void addTestAssets(QListWidget *listWidget, QTreeWidget *treeWidget)
    {
        // Add to list
        for (int i = 0; i < 5; ++i) {
            QListWidgetItem *item = new QListWidgetItem(QString("Asset %1").arg(i + 1));
            listWidget->addItem(item);
        }
        
        // Add to tree
        QTreeWidgetItem *textures = new QTreeWidgetItem(treeWidget);
        textures->setText(0, "Textures");
        textures->setExpanded(true);
        
        for (int i = 0; i < 3; ++i) {
            QTreeWidgetItem *item = new QTreeWidgetItem(textures);
            item->setText(0, QString("texture_%1.png").arg(i + 1));
        }
        
        QTreeWidgetItem *models = new QTreeWidgetItem(treeWidget);
        models->setText(0, "Models");
        models->setExpanded(true);
        
        for (int i = 0; i < 2; ++i) {
            QTreeWidgetItem *item = new QTreeWidgetItem(models);
            item->setText(0, QString("model_%1.obj").arg(i + 1));
        }
    }
};

// Простая версия ConsolePanel
class SimpleConsolePanel : public QDockWidget
{
    Q_OBJECT

public:
    SimpleConsolePanel(QWidget *parent = nullptr) : QDockWidget("Console", parent)
    {
        QWidget *widget = new QWidget();
        setWidget(widget);
        
        QVBoxLayout *layout = new QVBoxLayout(widget);
        
        // Controls
        QHBoxLayout *controlLayout = new QHBoxLayout();
        
        QComboBox *levelCombo = new QComboBox();
        levelCombo->addItems({"All", "Info", "Warning", "Error", "Debug"});
        controlLayout->addWidget(levelCombo);
        
        QCheckBox *autoScrollCheck = new QCheckBox("Auto-scroll");
        autoScrollCheck->setChecked(true);
        controlLayout->addWidget(autoScrollCheck);
        
        QPushButton *clearBtn = new QPushButton("Clear");
        controlLayout->addWidget(clearBtn);
        
        QPushButton *saveBtn = new QPushButton("Save");
        controlLayout->addWidget(saveBtn);
        
        controlLayout->addStretch();
        layout->addLayout(controlLayout);
        
        // Console text
        m_textEdit = new QTextEdit();
        m_textEdit->setReadOnly(true);
        m_textEdit->setMaximumHeight(200);
        layout->addWidget(m_textEdit);
        
        // Add initial messages
        addMessage("FastEngine Qt Editor started", "Info");
        addMessage("All systems operational", "Info");
        addMessage("Ready for development", "Success");
        
        // Connect signals
        connect(clearBtn, &QPushButton::clicked, m_textEdit, &QTextEdit::clear);
    }
    
    void addMessage(const QString &message, const QString &type = "Info")
    {
        QString timestamp = QTime::currentTime().toString("hh:mm:ss");
        QString color = type == "Error" ? "red" : 
                       type == "Warning" ? "orange" : 
                       type == "Success" ? "green" : "black";
        
        QString formatted = QString("<span style='color: %1'>[%2] %3: %4</span>")
                          .arg(color).arg(timestamp).arg(type).arg(message);
        
        m_textEdit->append(formatted);
    }

private:
    QTextEdit *m_textEdit;
};

// Главное окно редактора
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
        // Central widget - только 2D сцена
        m_sceneView2D = new SimpleSceneView2D(this);
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
        
        // Help menu
        QMenu *helpMenu = menuBar()->addMenu("&Help");
        helpMenu->addAction("&About", [this]() {
            QMessageBox::about(this, "About", "FastEngine Qt Editor v1.0\nA working game editor built with Qt6");
        });
    }
    
    void setupDockWidgets()
    {
        // Hierarchy Panel
        m_hierarchyPanel = new SimpleHierarchyPanel(this);
        addDockWidget(Qt::LeftDockWidgetArea, m_hierarchyPanel);
        
        // Inspector Panel  
        m_inspectorPanel = new SimpleInspectorPanel(this);
        addDockWidget(Qt::RightDockWidgetArea, m_inspectorPanel);
        
        // Assets Panel
        m_assetsPanel = new SimpleAssetsPanel(this);
        addDockWidget(Qt::BottomDockWidgetArea, m_assetsPanel);
        
        // Console Panel
        m_consolePanel = new SimpleConsolePanel(this);
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
        connect(m_hierarchyPanel, &SimpleHierarchyPanel::objectAdded,
                [this](const QString &name) {
                    m_consolePanel->addMessage(QString("Added object: %1").arg(name), "Info");
                });
        
        connect(m_hierarchyPanel, &SimpleHierarchyPanel::objectRemoved,
                [this](const QString &name) {
                    m_consolePanel->addMessage(QString("Removed object: %1").arg(name), "Warning");
                });
        
        connect(m_hierarchyPanel, &SimpleHierarchyPanel::objectDuplicated,
                [this](const QString &name) {
                    m_consolePanel->addMessage(QString("Duplicated object: %1").arg(name), "Info");
                });
    }

private:
    SimpleSceneView2D *m_sceneView2D;
    SimpleHierarchyPanel *m_hierarchyPanel;
    SimpleInspectorPanel *m_inspectorPanel;
    SimpleAssetsPanel *m_assetsPanel;
    SimpleConsolePanel *m_consolePanel;
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

#include "WorkingEditor.moc"
