#include "ModernPhotoEditor.h"
#include "ToolManager.h"
#include "CanvasWidget.h"
#include "LogPanel.h"
#include "Logger.h"
#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QClipboard>
#include <QMimeData>
#include <QStandardPaths>
#include <QDir>

ModernPhotoEditor::ModernPhotoEditor(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_canvasWidget(nullptr)
    , m_scrollArea(nullptr)
    , m_imageLabel(nullptr)
    , m_mainSplitter(nullptr)
    , m_toolManager(nullptr)
    , m_toolsDock(nullptr)
    , m_layersDock(nullptr)
    , m_propertiesDock(nullptr)
    , m_historyDock(nullptr)
    , m_colorsDock(nullptr)
    , m_logDock(nullptr)
    , m_currentHistoryIndex(-1)
    , m_brushColor(Qt::black)
    , m_backgroundColor(Qt::white)
    , m_currentTool(0)
    , m_brushSize(10)
    , m_brushOpacity(100)
    , m_brushHardness(50)
    , m_zoomFactor(1.0f)
    , m_brightness(0)
    , m_contrast(100)
    , m_saturation(100)
    , m_hue(0)
{
    createModernStyle();
    
    // Initialize logging
    LOG_INFO("APP", "Modern Photo Editor starting...");
    
    // Create tool manager
    m_toolManager = new ToolManager(this);
    
    setupUI();
    setupMenus();
    setupToolbars();
    setupDockWidgets();
    setupStatusBar();
    setupConnections();
    
    // Create a default white image after all UI is set up
    // Delay this to avoid accessing uninitialized UI elements
    QTimer::singleShot(0, this, [this]() {
        newImage();
    });
}

ModernPhotoEditor::~ModernPhotoEditor()
{
}

void ModernPhotoEditor::createModernStyle()
{
    // Set modern dark theme
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    
    qApp->setPalette(darkPalette);
    
    // Set modern style
    qApp->setStyle(QStyleFactory::create("Fusion"));
    
    // Apply custom stylesheet
    setStyleSheet(R"(
        QMainWindow {
            background-color: #2b2b2b;
        }
        QDockWidget {
            background-color: #3c3c3c;
            border: 1px solid #555555;
            titlebar-close-icon: url(close.png);
            titlebar-normal-icon: url(normal.png);
        }
        QDockWidget::title {
            background-color: #404040;
            padding: 5px;
            font-weight: bold;
        }
        QGroupBox {
            font-weight: bold;
            border: 2px solid #555555;
            border-radius: 5px;
            margin-top: 1ex;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        QPushButton {
            background-color: #404040;
            border: 1px solid #555555;
            border-radius: 3px;
            padding: 5px;
            min-width: 60px;
        }
        QPushButton:hover {
            background-color: #505050;
        }
        QPushButton:pressed {
            background-color: #353535;
        }
        QSlider::groove:horizontal {
            border: 1px solid #555555;
            height: 8px;
            background: #404040;
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: #606060;
            border: 1px solid #555555;
            width: 18px;
            margin: -2px 0;
            border-radius: 9px;
        }
        QSlider::handle:horizontal:hover {
            background: #707070;
        }
        QListWidget {
            background-color: #3c3c3c;
            border: 1px solid #555555;
            border-radius: 3px;
        }
        QListWidget::item {
            padding: 5px;
            border-bottom: 1px solid #555555;
        }
        QListWidget::item:selected {
            background-color: #404040;
        }
        QSpinBox {
            background-color: #404040;
            border: 1px solid #555555;
            border-radius: 3px;
            padding: 2px;
        }
        QTabWidget::pane {
            border: 1px solid #555555;
            background-color: #3c3c3c;
        }
        QTabBar::tab {
            background-color: #404040;
            border: 1px solid #555555;
            padding: 5px 10px;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background-color: #505050;
        }
        QScrollArea {
            background-color: #2b2b2b;
            border: 1px solid #555555;
        }
        QLabel {
            color: white;
        }
    )");
}

void ModernPhotoEditor::setupUI()
{
    // Create central widget
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    // Create main layout
    QHBoxLayout *mainLayout = new QHBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Create canvas widget
    m_canvasWidget = new CanvasWidget(this);
    m_canvasWidget->setToolManager(m_toolManager);
    
    // Set canvas widget in tool manager for updates
    m_toolManager->setCanvasWidget(m_canvasWidget);
    
    // Canvas widget is ready for tool operations
    
    mainLayout->addWidget(m_canvasWidget);
}

void ModernPhotoEditor::setupMenus()
{
    // File Menu
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    QAction *newAction = fileMenu->addAction("&New");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &ModernPhotoEditor::newImage);
    
    QAction *openAction = fileMenu->addAction("&Open");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &ModernPhotoEditor::openImage);
    
    QAction *saveAction = fileMenu->addAction("&Save");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &ModernPhotoEditor::saveImage);
    
    QAction *saveAsAction = fileMenu->addAction("Save &As");
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &ModernPhotoEditor::saveAsImage);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    // Edit Menu
    QMenu *editMenu = menuBar()->addMenu("&Edit");
    
    QAction *undoAction = editMenu->addAction("&Undo");
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered, this, &ModernPhotoEditor::undoAction);
    
    QAction *redoAction = editMenu->addAction("&Redo");
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, &QAction::triggered, this, &ModernPhotoEditor::redoAction);
    
    editMenu->addSeparator();
    
    QAction *cutAction = editMenu->addAction("Cu&t");
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, this, &ModernPhotoEditor::cutImage);
    
    QAction *copyAction = editMenu->addAction("&Copy");
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &ModernPhotoEditor::copyImage);
    
    QAction *pasteAction = editMenu->addAction("&Paste");
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, this, &ModernPhotoEditor::pasteImage);
    
    editMenu->addSeparator();
    
    QAction *selectAllAction = editMenu->addAction("Select &All");
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAction, &QAction::triggered, this, &ModernPhotoEditor::selectAll);
    
    QAction *deselectAction = editMenu->addAction("&Deselect All");
    connect(deselectAction, &QAction::triggered, this, &ModernPhotoEditor::deselectAll);
    
    // View Menu
    QMenu *viewMenu = menuBar()->addMenu("&View");
    
    QAction *zoomInAction = viewMenu->addAction("Zoom &In");
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAction, &QAction::triggered, this, &ModernPhotoEditor::zoomIn);
    
    QAction *zoomOutAction = viewMenu->addAction("Zoom &Out");
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAction, &QAction::triggered, this, &ModernPhotoEditor::zoomOut);
    
    QAction *fitToWindowAction = viewMenu->addAction("&Fit to Window");
    connect(fitToWindowAction, &QAction::triggered, this, &ModernPhotoEditor::fitToWindow);
    
    QAction *actualSizeAction = viewMenu->addAction("&Actual Size");
    connect(actualSizeAction, &QAction::triggered, this, &ModernPhotoEditor::actualSize);
    
    // Image Menu
    QMenu *imageMenu = menuBar()->addMenu("&Image");
    
    QAction *rotateLeftAction = imageMenu->addAction("Rotate &Left");
    connect(rotateLeftAction, &QAction::triggered, this, &ModernPhotoEditor::rotateLeft);
    
    QAction *rotateRightAction = imageMenu->addAction("Rotate &Right");
    connect(rotateRightAction, &QAction::triggered, this, &ModernPhotoEditor::rotateRight);
    
    QAction *flipHAction = imageMenu->addAction("Flip &Horizontal");
    connect(flipHAction, &QAction::triggered, this, &ModernPhotoEditor::flipHorizontal);
    
    QAction *flipVAction = imageMenu->addAction("Flip &Vertical");
    connect(flipVAction, &QAction::triggered, this, &ModernPhotoEditor::flipVertical);
    
    imageMenu->addSeparator();
    
    QAction *cropAction = imageMenu->addAction("&Crop");
    connect(cropAction, &QAction::triggered, this, &ModernPhotoEditor::cropImage);
    
    QAction *resizeAction = imageMenu->addAction("&Resize");
    connect(resizeAction, &QAction::triggered, this, &ModernPhotoEditor::resizeImage);
    
    // Help Menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    
    QAction *aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, this, &ModernPhotoEditor::showAbout);
}

void ModernPhotoEditor::setupToolbars()
{
    // Main Toolbar
    QToolBar *mainToolBar = addToolBar("Main");
    mainToolBar->setMovable(false);
    
    QAction *newAction = mainToolBar->addAction("New");
    connect(newAction, &QAction::triggered, this, &ModernPhotoEditor::newImage);
    
    QAction *openAction = mainToolBar->addAction("Open");
    connect(openAction, &QAction::triggered, this, &ModernPhotoEditor::openImage);
    
    QAction *saveAction = mainToolBar->addAction("Save");
    connect(saveAction, &QAction::triggered, this, &ModernPhotoEditor::saveImage);
    
    mainToolBar->addSeparator();
    
    QAction *undoAction = mainToolBar->addAction("Undo");
    connect(undoAction, &QAction::triggered, this, &ModernPhotoEditor::undoAction);
    
    QAction *redoAction = mainToolBar->addAction("Redo");
    connect(redoAction, &QAction::triggered, this, &ModernPhotoEditor::redoAction);
    
    mainToolBar->addSeparator();
    
    QAction *zoomInAction = mainToolBar->addAction("Zoom In");
    connect(zoomInAction, &QAction::triggered, this, &ModernPhotoEditor::zoomIn);
    
    QAction *zoomOutAction = mainToolBar->addAction("Zoom Out");
    connect(zoomOutAction, &QAction::triggered, this, &ModernPhotoEditor::zoomOut);
    
    QAction *fitToWindowAction = mainToolBar->addAction("Fit to Window");
    connect(fitToWindowAction, &QAction::triggered, this, &ModernPhotoEditor::fitToWindow);
}

void ModernPhotoEditor::setupDockWidgets()
{
    // Tools Dock
    m_toolsDock = new QDockWidget("Tools", this);
    m_toolsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    QWidget *toolsWidget = new QWidget();
    QVBoxLayout *toolsLayout = new QVBoxLayout(toolsWidget);
    
    // Tool Selection Group
    m_toolsGroup = new QGroupBox("Tools", this);
    QVBoxLayout *toolGroupLayout = new QVBoxLayout(m_toolsGroup);
    
    m_toolGroup = new QActionGroup(this);
    
    m_selectTool = new QAction("Select", this);
    m_selectTool->setCheckable(true);
    m_selectTool->setChecked(true);
    m_toolGroup->addAction(m_selectTool);
    
    m_brushTool = new QAction("Brush", this);
    m_brushTool->setCheckable(true);
    m_toolGroup->addAction(m_brushTool);
    
    m_eraserTool = new QAction("Eraser", this);
    m_eraserTool->setCheckable(true);
    m_toolGroup->addAction(m_eraserTool);
    
    m_cropTool = new QAction("Crop", this);
    m_cropTool->setCheckable(true);
    m_toolGroup->addAction(m_cropTool);
    
    m_textTool = new QAction("Text", this);
    m_textTool->setCheckable(true);
    m_toolGroup->addAction(m_textTool);
    
    m_shapeTool = new QAction("Shape", this);
    m_shapeTool->setCheckable(true);
    m_toolGroup->addAction(m_shapeTool);
    
    // Add tool buttons
    QPushButton *selectBtn = new QPushButton("Select");
    selectBtn->setCheckable(true);
    selectBtn->setChecked(true);
    connect(selectBtn, &QPushButton::clicked, [this]() { 
        m_toolManager->setCurrentTool(ToolManager::SelectTool);
    });
    
    QPushButton *brushBtn = new QPushButton("Brush");
    brushBtn->setCheckable(true);
    connect(brushBtn, &QPushButton::clicked, [this]() { 
        m_toolManager->setCurrentTool(ToolManager::BrushTool);
    });
    
    QPushButton *eraserBtn = new QPushButton("Eraser");
    eraserBtn->setCheckable(true);
    connect(eraserBtn, &QPushButton::clicked, [this]() { 
        m_toolManager->setCurrentTool(ToolManager::EraserTool);
    });
    
    QPushButton *cropBtn = new QPushButton("Crop");
    cropBtn->setCheckable(true);
    connect(cropBtn, &QPushButton::clicked, [this]() { 
        m_toolManager->setCurrentTool(ToolManager::CropTool);
    });
    
    QPushButton *textBtn = new QPushButton("Text");
    textBtn->setCheckable(true);
    connect(textBtn, &QPushButton::clicked, [this]() { 
        m_toolManager->setCurrentTool(ToolManager::TextTool);
    });
    
    QPushButton *shapeBtn = new QPushButton("Shape");
    shapeBtn->setCheckable(true);
    connect(shapeBtn, &QPushButton::clicked, [this]() { 
        m_toolManager->setCurrentTool(ToolManager::ShapeTool);
    });
    
    toolGroupLayout->addWidget(selectBtn);
    toolGroupLayout->addWidget(brushBtn);
    toolGroupLayout->addWidget(eraserBtn);
    toolGroupLayout->addWidget(cropBtn);
    toolGroupLayout->addWidget(textBtn);
    toolGroupLayout->addWidget(shapeBtn);
    
    // Brush Properties Group
    m_brushGroup = new QGroupBox("Brush Properties", this);
    QFormLayout *brushLayout = new QFormLayout(m_brushGroup);
    
    m_brushSizeSlider = new QSlider(Qt::Horizontal);
    m_brushSizeSlider->setRange(1, 100);
    m_brushSizeSlider->setValue(m_brushSize);
    
    m_brushSizeSpinBox = new QSpinBox();
    m_brushSizeSpinBox->setRange(1, 100);
    m_brushSizeSpinBox->setValue(m_brushSize);
    
    m_brushOpacitySlider = new QSlider(Qt::Horizontal);
    m_brushOpacitySlider->setRange(1, 100);
    m_brushOpacitySlider->setValue(m_brushOpacity);
    
    m_brushOpacitySpinBox = new QSpinBox();
    m_brushOpacitySpinBox->setRange(1, 100);
    m_brushOpacitySpinBox->setValue(m_brushOpacity);
    
    m_brushHardnessSlider = new QSlider(Qt::Horizontal);
    m_brushHardnessSlider->setRange(0, 100);
    m_brushHardnessSlider->setValue(m_brushHardness);
    
    m_brushHardnessSpinBox = new QSpinBox();
    m_brushHardnessSpinBox->setRange(0, 100);
    m_brushHardnessSpinBox->setValue(m_brushHardness);
    
    m_brushColorButton = new QPushButton();
    m_brushColorButton->setFixedSize(30, 30);
    m_brushColorButton->setStyleSheet(QString("background-color: %1; border: 1px solid #555555;").arg(m_brushColor.name()));
    
    brushLayout->addRow("Size:", m_brushSizeSlider);
    brushLayout->addRow("", m_brushSizeSpinBox);
    brushLayout->addRow("Opacity:", m_brushOpacitySlider);
    brushLayout->addRow("", m_brushOpacitySpinBox);
    brushLayout->addRow("Hardness:", m_brushHardnessSlider);
    brushLayout->addRow("", m_brushHardnessSpinBox);
    brushLayout->addRow("Color:", m_brushColorButton);
    
    toolsLayout->addWidget(m_toolsGroup);
    toolsLayout->addWidget(m_brushGroup);
    toolsLayout->addStretch();
    
    m_toolsDock->setWidget(toolsWidget);
    addDockWidget(Qt::LeftDockWidgetArea, m_toolsDock);
    
    // Layers Dock
    m_layersDock = new QDockWidget("Layers", this);
    m_layersDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    QWidget *layersWidget = new QWidget();
    QVBoxLayout *layersLayout = new QVBoxLayout(layersWidget);
    
    m_layersList = new QListWidget();
    m_layersList->addItem("Background");
    
    m_addLayerButton = new QPushButton("Add Layer");
    m_deleteLayerButton = new QPushButton("Delete Layer");
    m_duplicateLayerButton = new QPushButton("Duplicate Layer");
    m_mergeButton = new QPushButton("Merge Down");
    
    m_layerOpacitySlider = new QSlider(Qt::Horizontal);
    m_layerOpacitySlider->setRange(0, 100);
    m_layerOpacitySlider->setValue(100);
    
    m_layerVisibilityCheckBox = new QCheckBox("Visible");
    m_layerVisibilityCheckBox->setChecked(true);
    
    layersLayout->addWidget(m_layersList);
    layersLayout->addWidget(m_addLayerButton);
    layersLayout->addWidget(m_deleteLayerButton);
    layersLayout->addWidget(m_duplicateLayerButton);
    layersLayout->addWidget(m_mergeButton);
    layersLayout->addWidget(new QLabel("Opacity:"));
    layersLayout->addWidget(m_layerOpacitySlider);
    layersLayout->addWidget(m_layerVisibilityCheckBox);
    
    m_layersDock->setWidget(layersWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_layersDock);
    
    // Properties Dock
    m_propertiesDock = new QDockWidget("Properties", this);
    m_propertiesDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    QWidget *propertiesWidget = new QWidget();
    QVBoxLayout *propertiesLayout = new QVBoxLayout(propertiesWidget);
    
    m_propertiesTabs = new QTabWidget();
    
    // Adjustments Tab
    QWidget *adjustmentsTab = new QWidget();
    QFormLayout *adjustmentsLayout = new QFormLayout(adjustmentsTab);
    
    m_adjustmentsGroup = new QGroupBox("Image Adjustments");
    QFormLayout *adjustmentsGroupLayout = new QFormLayout(m_adjustmentsGroup);
    
    m_brightnessSlider = new QSlider(Qt::Horizontal);
    m_brightnessSlider->setRange(-100, 100);
    m_brightnessSlider->setValue(m_brightness);
    
    m_brightnessSpinBox = new QSpinBox();
    m_brightnessSpinBox->setRange(-100, 100);
    m_brightnessSpinBox->setValue(m_brightness);
    
    m_contrastSlider = new QSlider(Qt::Horizontal);
    m_contrastSlider->setRange(0, 200);
    m_contrastSlider->setValue(m_contrast);
    
    m_contrastSpinBox = new QSpinBox();
    m_contrastSpinBox->setRange(0, 200);
    m_contrastSpinBox->setValue(m_contrast);
    
    m_saturationSlider = new QSlider(Qt::Horizontal);
    m_saturationSlider->setRange(0, 200);
    m_saturationSlider->setValue(m_saturation);
    
    m_saturationSpinBox = new QSpinBox();
    m_saturationSpinBox->setRange(0, 200);
    m_saturationSpinBox->setValue(m_saturation);
    
    m_hueSlider = new QSlider(Qt::Horizontal);
    m_hueSlider->setRange(-180, 180);
    m_hueSlider->setValue(m_hue);
    
    m_hueSpinBox = new QSpinBox();
    m_hueSpinBox->setRange(-180, 180);
    m_hueSpinBox->setValue(m_hue);
    
    m_resetButton = new QPushButton("Reset All");
    
    adjustmentsGroupLayout->addRow("Brightness:", m_brightnessSlider);
    adjustmentsGroupLayout->addRow("", m_brightnessSpinBox);
    adjustmentsGroupLayout->addRow("Contrast:", m_contrastSlider);
    adjustmentsGroupLayout->addRow("", m_contrastSpinBox);
    adjustmentsGroupLayout->addRow("Saturation:", m_saturationSlider);
    adjustmentsGroupLayout->addRow("", m_saturationSpinBox);
    adjustmentsGroupLayout->addRow("Hue:", m_hueSlider);
    adjustmentsGroupLayout->addRow("", m_hueSpinBox);
    adjustmentsGroupLayout->addRow("", m_resetButton);
    
    adjustmentsLayout->addWidget(m_adjustmentsGroup);
    
    m_propertiesTabs->addTab(adjustmentsTab, "Adjustments");
    
    propertiesLayout->addWidget(m_propertiesTabs);
    
    m_propertiesDock->setWidget(propertiesWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_propertiesDock);
    
    // History Dock
    m_historyDock = new QDockWidget("History", this);
    m_historyDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    QWidget *historyWidget = new QWidget();
    QVBoxLayout *historyLayout = new QVBoxLayout(historyWidget);
    
    m_historyList = new QListWidget();
    m_historyList->addItem("Initial State");
    
    m_undoButton = new QPushButton("Undo");
    m_redoButton = new QPushButton("Redo");
    m_clearHistoryButton = new QPushButton("Clear History");
    
    historyLayout->addWidget(m_historyList);
    historyLayout->addWidget(m_undoButton);
    historyLayout->addWidget(m_redoButton);
    historyLayout->addWidget(m_clearHistoryButton);
    
    m_historyDock->setWidget(historyWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_historyDock);
    
    // Colors Dock
    m_colorsDock = new QDockWidget("Colors", this);
    m_colorsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    QWidget *colorsWidget = new QWidget();
    QVBoxLayout *colorsLayout = new QVBoxLayout(colorsWidget);
    
    m_colorsGroup = new QGroupBox("Color Selection");
    QFormLayout *colorsGroupLayout = new QFormLayout(m_colorsGroup);
    
    m_foregroundColorButton = new QPushButton();
    m_foregroundColorButton->setFixedSize(40, 40);
    m_foregroundColorButton->setStyleSheet(QString("background-color: %1; border: 2px solid #555555;").arg(m_brushColor.name()));
    
    m_backgroundColorButton = new QPushButton();
    m_backgroundColorButton->setFixedSize(40, 40);
    m_backgroundColorButton->setStyleSheet(QString("background-color: %1; border: 2px solid #555555;").arg(m_backgroundColor.name()));
    
    m_swapColorsButton = new QPushButton("Swap");
    m_swapColorsButton->setFixedSize(40, 20);
    
    m_colorPreviewLabel = new QLabel("Foreground / Background");
    m_colorPreviewLabel->setAlignment(Qt::AlignCenter);
    
    colorsGroupLayout->addRow("Foreground:", m_foregroundColorButton);
    colorsGroupLayout->addRow("Background:", m_backgroundColorButton);
    colorsGroupLayout->addRow("", m_swapColorsButton);
    colorsGroupLayout->addRow("", m_colorPreviewLabel);
    
    colorsLayout->addWidget(m_colorsGroup);
    
    m_colorsDock->setWidget(colorsWidget);
    addDockWidget(Qt::LeftDockWidgetArea, m_colorsDock);
    
    // Log Panel
    m_logDock = new QDockWidget("Logs", this);
    m_logDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, m_logDock);
    
    LogPanel *logPanel = new LogPanel();
    m_logDock->setWidget(logPanel);
}

void ModernPhotoEditor::setupStatusBar()
{
    statusBar()->showMessage("Ready - Modern Photo Editor v1.0");
    
    // Add zoom info
    QLabel *zoomLabel = new QLabel("100%");
    statusBar()->addPermanentWidget(zoomLabel);
}

void ModernPhotoEditor::setupConnections()
{
    // Tool manager is ready for operations
    
    // Brush controls
    connect(m_brushSizeSlider, &QSlider::valueChanged, m_brushSizeSpinBox, &QSpinBox::setValue);
    connect(m_brushSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), m_brushSizeSlider, &QSlider::setValue);
    connect(m_brushSizeSlider, &QSlider::valueChanged, [this](int value) { 
        m_brushSize = value; 
        m_toolManager->setBrushSize(value);
    });
    
    connect(m_brushOpacitySlider, &QSlider::valueChanged, m_brushOpacitySpinBox, &QSpinBox::setValue);
    connect(m_brushOpacitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), m_brushOpacitySlider, &QSlider::setValue);
    connect(m_brushOpacitySlider, &QSlider::valueChanged, [this](int value) { 
        m_brushOpacity = value; 
        m_toolManager->setBrushOpacity(value);
    });
    
    connect(m_brushHardnessSlider, &QSlider::valueChanged, m_brushHardnessSpinBox, &QSpinBox::setValue);
    connect(m_brushHardnessSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), m_brushHardnessSlider, &QSlider::setValue);
    connect(m_brushHardnessSlider, &QSlider::valueChanged, [this](int value) { 
        m_brushHardness = value; 
        m_toolManager->setBrushHardness(value);
    });
    
    connect(m_brushColorButton, &QPushButton::clicked, this, &ModernPhotoEditor::brushColorChanged);
    
    // Adjustment controls
    connect(m_brightnessSlider, &QSlider::valueChanged, m_brightnessSpinBox, &QSpinBox::setValue);
    connect(m_brightnessSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), m_brightnessSlider, &QSlider::setValue);
    connect(m_brightnessSlider, &QSlider::valueChanged, this, &ModernPhotoEditor::adjustBrightness);
    
    connect(m_contrastSlider, &QSlider::valueChanged, m_contrastSpinBox, &QSpinBox::setValue);
    connect(m_contrastSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), m_contrastSlider, &QSlider::setValue);
    connect(m_contrastSlider, &QSlider::valueChanged, this, &ModernPhotoEditor::adjustContrast);
    
    connect(m_saturationSlider, &QSlider::valueChanged, m_saturationSpinBox, &QSpinBox::setValue);
    connect(m_saturationSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), m_saturationSlider, &QSlider::setValue);
    connect(m_saturationSlider, &QSlider::valueChanged, this, &ModernPhotoEditor::adjustSaturation);
    
    connect(m_hueSlider, &QSlider::valueChanged, m_hueSpinBox, &QSpinBox::setValue);
    connect(m_hueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), m_hueSlider, &QSlider::setValue);
    connect(m_hueSlider, &QSlider::valueChanged, this, &ModernPhotoEditor::adjustHue);
    
    connect(m_resetButton, &QPushButton::clicked, this, &ModernPhotoEditor::resetAdjustments);
    
    // Layer controls
    connect(m_addLayerButton, &QPushButton::clicked, this, &ModernPhotoEditor::addNewLayer);
    connect(m_deleteLayerButton, &QPushButton::clicked, this, &ModernPhotoEditor::deleteLayer);
    connect(m_duplicateLayerButton, &QPushButton::clicked, this, &ModernPhotoEditor::duplicateLayer);
    connect(m_mergeButton, &QPushButton::clicked, this, &ModernPhotoEditor::mergeLayers);
    connect(m_layerVisibilityCheckBox, &QCheckBox::toggled, this, &ModernPhotoEditor::layerVisibilityToggled);
    connect(m_layerOpacitySlider, &QSlider::valueChanged, this, &ModernPhotoEditor::layerOpacityChanged);
    
    // History controls
    connect(m_undoButton, &QPushButton::clicked, this, &ModernPhotoEditor::undoAction);
    connect(m_redoButton, &QPushButton::clicked, this, &ModernPhotoEditor::redoAction);
    connect(m_clearHistoryButton, &QPushButton::clicked, [this]() {
        m_historyList->clear();
        m_imageHistory.clear();
        m_currentHistoryIndex = -1;
        m_historyList->addItem("Initial State");
    });
    
    // Color controls
    connect(m_foregroundColorButton, &QPushButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(m_brushColor, this, "Select Foreground Color");
        if (color.isValid()) {
            m_brushColor = color;
            m_foregroundColorButton->setStyleSheet(QString("background-color: %1; border: 2px solid #555555;").arg(color.name()));
            m_brushColorButton->setStyleSheet(QString("background-color: %1; border: 1px solid #555555;").arg(color.name()));
        }
    });
    
    connect(m_backgroundColorButton, &QPushButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(m_backgroundColor, this, "Select Background Color");
        if (color.isValid()) {
            m_backgroundColor = color;
            m_backgroundColorButton->setStyleSheet(QString("background-color: %1; border: 2px solid #555555;").arg(color.name()));
        }
    });
    
    connect(m_swapColorsButton, &QPushButton::clicked, [this]() {
        QColor temp = m_brushColor;
        m_brushColor = m_backgroundColor;
        m_backgroundColor = temp;
        
        m_foregroundColorButton->setStyleSheet(QString("background-color: %1; border: 2px solid #555555;").arg(m_brushColor.name()));
        m_backgroundColorButton->setStyleSheet(QString("background-color: %1; border: 2px solid #555555;").arg(m_backgroundColor.name()));
        m_brushColorButton->setStyleSheet(QString("background-color: %1; border: 1px solid #555555;").arg(m_brushColor.name()));
    });
}

// Implementation of slot methods
void ModernPhotoEditor::openImage()
{
    LOG_INFO("FILE", "Opening image dialog...");
    
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Image", "", "Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.tiff)");
    
    if (!fileName.isEmpty()) {
        LOG_INFO("FILE", QString("Selected file: %1").arg(fileName));
        
        QImage image(fileName);
        if (!image.isNull()) {
            m_originalImage = image;
            m_currentImage = image;
            
            // Set image in canvas and tool manager - check if they exist
            if (m_canvasWidget) {
                m_canvasWidget->setImage(image);
            }
            if (m_toolManager) {
                m_toolManager->setImage(&m_currentImage);
            }
            
            LOG_IMAGE_OP("OPEN", QString("Size: %1x%2").arg(image.width()).arg(image.height()));
            statusBar()->showMessage(QString("Opened: %1").arg(fileName));
            
            // Add to history - check if history list exists
            m_imageHistory.append(m_currentImage);
            m_currentHistoryIndex = m_imageHistory.size() - 1;
            if (m_historyList) {
                m_historyList->addItem(QString("Open: %1").arg(QFileInfo(fileName).baseName()));
            }
        } else {
            LOG_ERROR("FILE", QString("Failed to load image: %1").arg(fileName));
            QMessageBox::warning(this, "Error", "Could not load image!");
        }
    } else {
        LOG_INFO("FILE", "Image open dialog cancelled");
    }
}

void ModernPhotoEditor::saveImage()
{
    if (m_currentImage.isNull()) {
        QMessageBox::warning(this, "Error", "No image to save!");
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Image", "", "PNG Files (*.png);;JPEG Files (*.jpg);;BMP Files (*.bmp)");
    
    if (!fileName.isEmpty()) {
        if (m_currentImage.save(fileName)) {
            statusBar()->showMessage(QString("Saved: %1").arg(fileName));
        } else {
            QMessageBox::warning(this, "Error", "Could not save image!");
        }
    }
}

void ModernPhotoEditor::saveAsImage()
{
    saveImage();
}

void ModernPhotoEditor::newImage()
{
    m_originalImage = QImage(800, 600, QImage::Format_RGB32);
    m_originalImage.fill(Qt::white);
    m_currentImage = m_originalImage;
    
    // Set image in canvas and tool manager
    if (m_canvasWidget) {
        m_canvasWidget->setImage(m_currentImage);
    }
    if (m_toolManager) {
        m_toolManager->setImage(&m_currentImage);
    }
    
    // Reset adjustments - check if sliders exist
    if (m_brightnessSlider) m_brightnessSlider->setValue(0);
    if (m_contrastSlider) m_contrastSlider->setValue(100);
    if (m_saturationSlider) m_saturationSlider->setValue(100);
    if (m_hueSlider) m_hueSlider->setValue(0);
    
    m_brightness = 0;
    m_contrast = 100;
    m_saturation = 100;
    m_hue = 0;
    
    statusBar()->showMessage("New image created");
    
    // Add to history - check if history list exists
    m_imageHistory.clear();
    m_imageHistory.append(m_currentImage);
    m_currentHistoryIndex = 0;
    if (m_historyList) {
        m_historyList->clear();
        m_historyList->addItem("New Image");
    }
}

void ModernPhotoEditor::undoAction()
{
    if (m_currentHistoryIndex > 0) {
        m_currentHistoryIndex--;
        m_currentImage = m_imageHistory[m_currentHistoryIndex];
        updateImage();
        statusBar()->showMessage("Undo");
    }
}

void ModernPhotoEditor::redoAction()
{
    if (m_currentHistoryIndex < m_imageHistory.size() - 1) {
        m_currentHistoryIndex++;
        m_currentImage = m_imageHistory[m_currentHistoryIndex];
        updateImage();
        statusBar()->showMessage("Redo");
    }
}

void ModernPhotoEditor::copyImage()
{
    if (!m_currentImage.isNull()) {
        QApplication::clipboard()->setImage(m_currentImage);
        statusBar()->showMessage("Image copied to clipboard");
    }
}

void ModernPhotoEditor::pasteImage()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    
    if (mimeData->hasImage()) {
        QImage image = qvariant_cast<QImage>(mimeData->imageData());
        if (!image.isNull()) {
            m_currentImage = image;
            updateImage();
            statusBar()->showMessage("Image pasted from clipboard");
            
            // Add to history
            m_imageHistory.append(m_currentImage);
            m_currentHistoryIndex = m_imageHistory.size() - 1;
            m_historyList->addItem("Paste");
        }
    }
}

void ModernPhotoEditor::cutImage()
{
    copyImage();
    m_currentImage.fill(Qt::transparent);
    updateImage();
    statusBar()->showMessage("Image cut to clipboard");
}

void ModernPhotoEditor::selectAll()
{
    statusBar()->showMessage("Select All");
}

void ModernPhotoEditor::deselectAll()
{
    statusBar()->showMessage("Deselect All");
}

void ModernPhotoEditor::zoomIn()
{
    m_zoomFactor *= 1.2f;
    updateImage();
    statusBar()->showMessage(QString("Zoom: %1%").arg(static_cast<int>(m_zoomFactor * 100)));
}

void ModernPhotoEditor::zoomOut()
{
    m_zoomFactor /= 1.2f;
    updateImage();
    statusBar()->showMessage(QString("Zoom: %1%").arg(static_cast<int>(m_zoomFactor * 100)));
}

void ModernPhotoEditor::fitToWindow()
{
    m_zoomFactor = 1.0f;
    updateImage();
    statusBar()->showMessage("Fit to Window");
}

void ModernPhotoEditor::actualSize()
{
    m_zoomFactor = 1.0f;
    updateImage();
    statusBar()->showMessage("Actual Size");
}

void ModernPhotoEditor::rotateLeft()
{
    QTransform transform;
    transform.rotate(-90);
    m_currentImage = m_currentImage.transformed(transform);
    updateImage();
    statusBar()->showMessage("Rotated Left");
}

void ModernPhotoEditor::rotateRight()
{
    QTransform transform;
    transform.rotate(90);
    m_currentImage = m_currentImage.transformed(transform);
    updateImage();
    statusBar()->showMessage("Rotated Right");
}

void ModernPhotoEditor::flipHorizontal()
{
    m_currentImage = m_currentImage.flipped(Qt::Horizontal);
    updateImage();
    statusBar()->showMessage("Flipped Horizontal");
}

void ModernPhotoEditor::flipVertical()
{
    m_currentImage = m_currentImage.flipped(Qt::Vertical);
    updateImage();
    statusBar()->showMessage("Flipped Vertical");
}

void ModernPhotoEditor::cropImage()
{
    statusBar()->showMessage("Crop Tool Selected");
}

void ModernPhotoEditor::resizeImage()
{
    statusBar()->showMessage("Resize Tool Selected");
}

void ModernPhotoEditor::adjustBrightness(int value)
{
    int oldValue = m_brightness;
    m_brightness = value;
    applyImageAdjustments();
    
    LOG_IMAGE_OP("BRIGHTNESS_ADJUST", 
        QString("From: %1 to: %2")
        .arg(oldValue).arg(value));
}

void ModernPhotoEditor::adjustContrast(int value)
{
    int oldValue = m_contrast;
    m_contrast = value;
    applyImageAdjustments();
    
    LOG_IMAGE_OP("CONTRAST_ADJUST", 
        QString("From: %1 to: %2")
        .arg(oldValue).arg(value));
}

void ModernPhotoEditor::adjustSaturation(int value)
{
    int oldValue = m_saturation;
    m_saturation = value;
    applyImageAdjustments();
    
    LOG_IMAGE_OP("SATURATION_ADJUST", 
        QString("From: %1 to: %2")
        .arg(oldValue).arg(value));
}

void ModernPhotoEditor::adjustHue(int value)
{
    int oldValue = m_hue;
    m_hue = value;
    applyImageAdjustments();
    
    LOG_IMAGE_OP("HUE_ADJUST", 
        QString("From: %1 to: %2")
        .arg(oldValue).arg(value));
}

void ModernPhotoEditor::resetAdjustments()
{
    m_brightnessSlider->setValue(0);
    m_contrastSlider->setValue(100);
    m_saturationSlider->setValue(100);
    m_hueSlider->setValue(0);
    
    m_brightness = 0;
    m_contrast = 100;
    m_saturation = 100;
    m_hue = 0;
    
    m_currentImage = m_originalImage;
    updateImage();
    statusBar()->showMessage("Adjustments Reset");
}

void ModernPhotoEditor::toolChanged()
{
    statusBar()->showMessage("Tool Changed");
}

void ModernPhotoEditor::brushSizeChanged(int size)
{
    m_brushSize = size;
}

void ModernPhotoEditor::brushOpacityChanged(int opacity)
{
    m_brushOpacity = opacity;
}

void ModernPhotoEditor::brushHardnessChanged(int hardness)
{
    m_brushHardness = hardness;
}

void ModernPhotoEditor::brushColorChanged()
{
    QColor color = QColorDialog::getColor(m_brushColor, this, "Select Brush Color");
    if (color.isValid()) {
        QColor oldColor = m_brushColor;
        m_brushColor = color;
        m_brushColorButton->setStyleSheet(QString("background-color: %1; border: 1px solid #555555;").arg(color.name()));
        m_foregroundColorButton->setStyleSheet(QString("background-color: %1; border: 2px solid #555555;").arg(color.name()));
        m_toolManager->setBrushColor(color);
        
        LOG_TOOL_PROPERTY("Brush", "color_change", 
            QString("From: %1 to: %2")
            .arg(oldColor.name()).arg(color.name()));
    } else {
        LOG_INFO("UI", "Color dialog cancelled");
    }
}

void ModernPhotoEditor::layerVisibilityToggled()
{
    statusBar()->showMessage("Layer Visibility Changed");
}

void ModernPhotoEditor::layerOpacityChanged(int opacity)
{
    statusBar()->showMessage(QString("Layer Opacity: %1%").arg(opacity));
}

void ModernPhotoEditor::addNewLayer()
{
    QString layerName = QString("Layer %1").arg(m_layersList->count());
    m_layersList->addItem(layerName);
    statusBar()->showMessage(QString("Added: %1").arg(layerName));
}

void ModernPhotoEditor::deleteLayer()
{
    int currentRow = m_layersList->currentRow();
    if (currentRow >= 0 && m_layersList->count() > 1) {
        QListWidgetItem *item = m_layersList->takeItem(currentRow);
        delete item;
        statusBar()->showMessage("Layer Deleted");
    }
}

void ModernPhotoEditor::duplicateLayer()
{
    int currentRow = m_layersList->currentRow();
    if (currentRow >= 0) {
        QString layerName = QString("Copy of %1").arg(m_layersList->item(currentRow)->text());
        m_layersList->addItem(layerName);
        statusBar()->showMessage(QString("Duplicated: %1").arg(layerName));
    }
}

void ModernPhotoEditor::mergeLayers()
{
    statusBar()->showMessage("Layers Merged");
}

void ModernPhotoEditor::showAbout()
{
    QMessageBox::about(this, "About Modern Photo Editor",
        "<h3>Modern Photo Editor v1.0</h3>"
        "<p>A modern, feature-rich photo editing application built with Qt6.</p>"
        "<p><b>Features:</b></p>"
        "<ul>"
        "<li>Multiple editing tools (Brush, Eraser, Crop, Text, Shape)</li>"
        "<li>Layer support with opacity and visibility controls</li>"
        "<li>Image adjustments (Brightness, Contrast, Saturation, Hue)</li>"
        "<li>Undo/Redo functionality</li>"
        "<li>Modern dark theme interface</li>"
        "<li>Color selection and management</li>"
        "</ul>"
        "<p>Built with FastEngine framework.</p>");
}

void ModernPhotoEditor::updateImage()
{
    if (m_currentImage.isNull()) {
        return;
    }
    
    // Apply zoom
    QImage displayImage = m_currentImage;
    if (m_zoomFactor != 1.0f) {
        QSize newSize = m_currentImage.size() * m_zoomFactor;
        displayImage = m_currentImage.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    // Display the image
    QPixmap pixmap = QPixmap::fromImage(displayImage);
    m_imageLabel->setPixmap(pixmap);
}

void ModernPhotoEditor::updateLayersList()
{
    // Update layers list if needed
}

void ModernPhotoEditor::updateHistoryList()
{
    // Update history list if needed
}

void ModernPhotoEditor::applyImageAdjustments()
{
    if (m_originalImage.isNull()) {
        return;
    }
    
    m_currentImage = m_originalImage;
    
    // Apply brightness
    if (m_brightness != 0) {
        for (int y = 0; y < m_currentImage.height(); ++y) {
            for (int x = 0; x < m_currentImage.width(); ++x) {
                QColor color = m_currentImage.pixelColor(x, y);
                int r = qBound(0, color.red() + m_brightness, 255);
                int g = qBound(0, color.green() + m_brightness, 255);
                int b = qBound(0, color.blue() + m_brightness, 255);
                m_currentImage.setPixelColor(x, y, QColor(r, g, b, color.alpha()));
            }
        }
    }
    
    // Apply contrast
    if (m_contrast != 100) {
        float contrastFactor = m_contrast / 100.0f;
        for (int y = 0; y < m_currentImage.height(); ++y) {
            for (int x = 0; x < m_currentImage.width(); ++x) {
                QColor color = m_currentImage.pixelColor(x, y);
                int r = qBound(0, static_cast<int>((color.red() - 128) * contrastFactor + 128), 255);
                int g = qBound(0, static_cast<int>((color.green() - 128) * contrastFactor + 128), 255);
                int b = qBound(0, static_cast<int>((color.blue() - 128) * contrastFactor + 128), 255);
                m_currentImage.setPixelColor(x, y, QColor(r, g, b, color.alpha()));
            }
        }
    }
    
    // Apply saturation
    if (m_saturation != 100) {
        float saturationFactor = m_saturation / 100.0f;
        for (int y = 0; y < m_currentImage.height(); ++y) {
            for (int x = 0; x < m_currentImage.width(); ++x) {
                QColor color = m_currentImage.pixelColor(x, y);
                QColor hsv = color.toHsv();
                int s = qBound(0, static_cast<int>(hsv.saturation() * saturationFactor), 255);
                hsv.setHsv(hsv.hue(), s, hsv.value(), color.alpha());
                m_currentImage.setPixelColor(x, y, hsv.toRgb());
            }
        }
    }
    
    // Apply hue
    if (m_hue != 0) {
        for (int y = 0; y < m_currentImage.height(); ++y) {
            for (int x = 0; x < m_currentImage.width(); ++x) {
                QColor color = m_currentImage.pixelColor(x, y);
                QColor hsv = color.toHsv();
                int h = (hsv.hue() + m_hue) % 360;
                if (h < 0) h += 360;
                hsv.setHsv(h, hsv.saturation(), hsv.value(), color.alpha());
                m_currentImage.setPixelColor(x, y, hsv.toRgb());
            }
        }
    }
    
    updateImage();
}
