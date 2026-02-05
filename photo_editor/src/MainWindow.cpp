#include "MainWindow.h"
#include "ImageDocument.h"
#include "CanvasWidget.h"
#include "LayerPanel.h"
#include "Layer.h"
#include "BrushTool.h"
#include "EraserTool.h"
#include "SelectionTool.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QDockWidget>
#include <QAction>
#include <QActionGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QColorDialog>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QListWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <QScrollBar>
#include <QFrame>
#include <QGridLayout>
#include <QFormLayout>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QScreen>
#include <QScreen>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_document(nullptr)
    , m_canvas(nullptr)
    , m_layerPanel(nullptr)
    , m_currentLayerIndex(-1)
{
    setupUI();
    setupMenus();
    setupToolbars();
    setupDockWidgets();
    setupConnections();
    
    // Create new document
    newFile();
    
    // Center window on screen
    centerWindow();
}

MainWindow::~MainWindow()
{
    delete m_document;
}

void MainWindow::setupUI()
{
    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Create canvas
    m_canvas = new CanvasWidget(this);
    mainLayout->addWidget(m_canvas);
    
    // Create document
    m_document = new ImageDocument(this);
    m_canvas->setDocument(m_document);
    
    // Create tools
    m_brushTool = new BrushTool(this);
    m_eraserTool = new EraserTool(this);
    m_selectionTool = new SelectionTool(this);
    
    // Set default tool
    m_canvas->setTool(m_brushTool);
    
    // Create status bar
    setupStatusBar();
}

void MainWindow::setupMenus()
{
    // File menu
    m_fileMenu = menuBar()->addMenu("&File");
    
    m_newAction = new QAction("&New", this);
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setIcon(QIcon(":/icons/new.png"));
    m_newAction->setToolTip("Create a new document");
    m_fileMenu->addAction(m_newAction);
    
    m_openAction = new QAction("&Open", this);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setIcon(QIcon(":/icons/open.png"));
    m_openAction->setToolTip("Open an existing document");
    m_fileMenu->addAction(m_openAction);
    
    m_fileMenu->addSeparator();
    
    m_saveAction = new QAction("&Save", this);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setIcon(QIcon(":/icons/save.png"));
    m_saveAction->setToolTip("Save the current document");
    m_fileMenu->addAction(m_saveAction);
    
    m_saveAsAction = new QAction("Save &As...", this);
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    m_saveAsAction->setToolTip("Save the current document with a new name");
    m_fileMenu->addAction(m_saveAsAction);
    
    m_fileMenu->addSeparator();
    
    m_exportAction = new QAction("&Export...", this);
    m_exportAction->setShortcut(Qt::CTRL | Qt::Key_E);
    m_exportAction->setIcon(QIcon(":/icons/export.png"));
    m_exportAction->setToolTip("Export the current document");
    m_fileMenu->addAction(m_exportAction);
    
    m_fileMenu->addSeparator();
    
    m_quitAction = new QAction("&Quit", this);
    m_quitAction->setShortcut(QKeySequence::Quit);
    m_quitAction->setToolTip("Quit the application");
    m_fileMenu->addAction(m_quitAction);
    
    // Edit menu
    m_editMenu = menuBar()->addMenu("&Edit");
    
    m_undoAction = new QAction("&Undo", this);
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_undoAction->setIcon(QIcon(":/icons/undo.png"));
    m_undoAction->setToolTip("Undo the last action");
    m_editMenu->addAction(m_undoAction);
    
    m_redoAction = new QAction("&Redo", this);
    m_redoAction->setShortcut(QKeySequence::Redo);
    m_redoAction->setIcon(QIcon(":/icons/redo.png"));
    m_redoAction->setToolTip("Redo the last undone action");
    m_editMenu->addAction(m_redoAction);
    
    m_editMenu->addSeparator();
    
    m_cutAction = new QAction("&Cut", this);
    m_cutAction->setShortcut(QKeySequence::Cut);
    m_cutAction->setIcon(QIcon(":/icons/cut.png"));
    m_cutAction->setToolTip("Cut the selection");
    m_editMenu->addAction(m_cutAction);
    
    m_copyAction = new QAction("&Copy", this);
    m_copyAction->setShortcut(QKeySequence::Copy);
    m_copyAction->setIcon(QIcon(":/icons/copy.png"));
    m_copyAction->setToolTip("Copy the selection");
    m_editMenu->addAction(m_copyAction);
    
    m_pasteAction = new QAction("&Paste", this);
    m_pasteAction->setShortcut(QKeySequence::Paste);
    m_pasteAction->setIcon(QIcon(":/icons/paste.png"));
    m_pasteAction->setToolTip("Paste the clipboard");
    m_editMenu->addAction(m_pasteAction);
    
    // View menu
    m_viewMenu = menuBar()->addMenu("&View");
    
    m_zoomInAction = new QAction("Zoom &In", this);
    m_zoomInAction->setShortcut(Qt::CTRL | Qt::Key_Plus);
    m_zoomInAction->setIcon(QIcon(":/icons/zoom_in.png"));
    m_zoomInAction->setToolTip("Zoom in");
    m_viewMenu->addAction(m_zoomInAction);
    
    m_zoomOutAction = new QAction("Zoom &Out", this);
    m_zoomOutAction->setShortcut(Qt::CTRL | Qt::Key_Minus);
    m_zoomOutAction->setIcon(QIcon(":/icons/zoom_out.png"));
    m_zoomOutAction->setToolTip("Zoom out");
    m_viewMenu->addAction(m_zoomOutAction);
    
    m_zoomFitAction = new QAction("Zoom &Fit", this);
    m_zoomFitAction->setShortcut(Qt::CTRL | Qt::Key_0);
    m_zoomFitAction->setIcon(QIcon(":/icons/zoom_fit.png"));
    m_zoomFitAction->setToolTip("Fit image to window");
    m_viewMenu->addAction(m_zoomFitAction);
    
    m_zoomActualAction = new QAction("Zoom &Actual Size", this);
    m_zoomActualAction->setShortcut(Qt::CTRL | Qt::Key_1);
    m_zoomActualAction->setIcon(QIcon(":/icons/zoom_actual.png"));
    m_zoomActualAction->setToolTip("Show image at actual size");
    m_viewMenu->addAction(m_zoomActualAction);
    
    m_viewMenu->addSeparator();
    
    m_showLayerPanelAction = new QAction("&Layers", this);
    m_showLayerPanelAction->setToolTip("Show/hide layers panel");
    m_showLayerPanelAction->setCheckable(true);
    m_showLayerPanelAction->setChecked(true);
    m_viewMenu->addAction(m_showLayerPanelAction);
    
    m_showHistoryPanelAction = new QAction("&History", this);
    m_showHistoryPanelAction->setToolTip("Show/hide history panel");
    m_showHistoryPanelAction->setCheckable(true);
    m_showHistoryPanelAction->setChecked(true);
    m_viewMenu->addAction(m_showHistoryPanelAction);
    
    m_showPropertiesPanelAction = new QAction("&Properties", this);
    m_showPropertiesPanelAction->setToolTip("Show/hide properties panel");
    m_showPropertiesPanelAction->setCheckable(true);
    m_showPropertiesPanelAction->setChecked(true);
    m_viewMenu->addAction(m_showPropertiesPanelAction);
    
    // Image menu
    m_imageMenu = menuBar()->addMenu("&Image");
    
    // Layer menu
    m_layerMenu = menuBar()->addMenu("&Layer");
    
    // Filter menu
    m_filterMenu = menuBar()->addMenu("&Filter");
    
    // Help menu
    m_helpMenu = menuBar()->addMenu("&Help");
    
    m_aboutAction = new QAction("&About", this);
    m_aboutAction->setToolTip("About Photo Editor");
    m_helpMenu->addAction(m_aboutAction);
}

void MainWindow::setupToolbars()
{
    // Main toolbar
    m_mainToolBar = addToolBar("Main Toolbar");
    m_mainToolBar->setObjectName("mainToolBar");
    m_mainToolBar->addAction(m_newAction);
    m_mainToolBar->addAction(m_openAction);
    m_mainToolBar->addAction(m_saveAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_undoAction);
    m_mainToolBar->addAction(m_redoAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_zoomInAction);
    m_mainToolBar->addAction(m_zoomOutAction);
    m_mainToolBar->addAction(m_zoomFitAction);
    m_mainToolBar->addAction(m_zoomActualAction);
    
    // Tools toolbar
    m_toolsToolBar = addToolBar("Tools");
    m_toolsToolBar->setObjectName("toolsToolBar");
    m_toolsToolBar->setOrientation(Qt::Vertical);
    m_toolsToolBar->setMovable(false);
    m_toolsToolBar->setFloatable(false);
    
    // Create tool group
    m_toolGroup = new QActionGroup(this);
    
    m_moveToolAction = new QAction("Move Tool", this);
    m_moveToolAction->setToolTip("Move and transform layers");
    m_moveToolAction->setCheckable(true);
    m_moveToolAction->setIcon(QIcon(":/icons/move_tool.png"));
    m_toolGroup->addAction(m_moveToolAction);
    m_toolsToolBar->addAction(m_moveToolAction);
    
    m_brushToolAction = new QAction("Brush Tool", this);
    m_brushToolAction->setToolTip("Paint with brush");
    m_brushToolAction->setCheckable(true);
    m_brushToolAction->setChecked(true);
    m_brushToolAction->setIcon(QIcon(":/icons/brush_tool.png"));
    m_toolGroup->addAction(m_brushToolAction);
    m_toolsToolBar->addAction(m_brushToolAction);
    
    m_eraserToolAction = new QAction("Eraser Tool", this);
    m_eraserToolAction->setToolTip("Erase pixels");
    m_eraserToolAction->setCheckable(true);
    m_eraserToolAction->setIcon(QIcon(":/icons/eraser_tool.png"));
    m_toolGroup->addAction(m_eraserToolAction);
    m_toolsToolBar->addAction(m_eraserToolAction);
    
    m_selectionToolAction = new QAction("Selection Tool", this);
    m_selectionToolAction->setToolTip("Select areas");
    m_selectionToolAction->setCheckable(true);
    m_selectionToolAction->setIcon(QIcon(":/icons/selection_tool.png"));
    m_toolGroup->addAction(m_selectionToolAction);
    m_toolsToolBar->addAction(m_selectionToolAction);
    
    m_fillToolAction = new QAction("Fill Tool", this);
    m_fillToolAction->setToolTip("Fill areas with color");
    m_fillToolAction->setCheckable(true);
    m_fillToolAction->setIcon(QIcon(":/icons/fill_tool.png"));
    m_toolGroup->addAction(m_fillToolAction);
    m_toolsToolBar->addAction(m_fillToolAction);
    
    m_textToolAction = new QAction("Text Tool", this);
    m_textToolAction->setToolTip("Add text");
    m_textToolAction->setCheckable(true);
    m_textToolAction->setIcon(QIcon(":/icons/text_tool.png"));
    m_toolGroup->addAction(m_textToolAction);
    m_toolsToolBar->addAction(m_textToolAction);
}

void MainWindow::setupDockWidgets()
{
    // Layer panel
    m_layerDock = new QDockWidget("Layers", this);
    m_layerDock->setObjectName("layerDock");
    m_layerPanel = new LayerPanel(this);
    m_layerDock->setWidget(m_layerPanel);
    addDockWidget(Qt::RightDockWidgetArea, m_layerDock);
    
    // History panel
    m_historyDock = new QDockWidget("History", this);
    m_historyDock->setObjectName("historyDock");
    QListWidget *historyList = new QListWidget(this);
    m_historyDock->setWidget(historyList);
    addDockWidget(Qt::RightDockWidgetArea, m_historyDock);
    
    // Properties panel
    m_propertiesDock = new QDockWidget("Properties", this);
    m_propertiesDock->setObjectName("propertiesDock");
    QWidget *propertiesWidget = new QWidget(this);
    QVBoxLayout *propertiesLayout = new QVBoxLayout(propertiesWidget);
    
    // Brush properties
    QGroupBox *brushGroup = new QGroupBox("Brush", this);
    QFormLayout *brushLayout = new QFormLayout(brushGroup);
    
    QSlider *sizeSlider = new QSlider(Qt::Horizontal, this);
    sizeSlider->setRange(1, 100);
    sizeSlider->setValue(10);
    brushLayout->addRow("Size:", sizeSlider);
    
    QSlider *opacitySlider = new QSlider(Qt::Horizontal, this);
    opacitySlider->setRange(0, 100);
    opacitySlider->setValue(100);
    brushLayout->addRow("Opacity:", opacitySlider);
    
    QSlider *hardnessSlider = new QSlider(Qt::Horizontal, this);
    hardnessSlider->setRange(0, 100);
    hardnessSlider->setValue(100);
    brushLayout->addRow("Hardness:", hardnessSlider);
    
    QPushButton *colorButton = new QPushButton("Color", this);
    colorButton->setStyleSheet("background-color: black;");
    brushLayout->addRow("Color:", colorButton);
    
    propertiesLayout->addWidget(brushGroup);
    propertiesLayout->addStretch();
    
    m_propertiesDock->setWidget(propertiesWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_propertiesDock);
}

void MainWindow::setupConnections()
{
    // File actions
    connect(m_newAction, &QAction::triggered, this, &MainWindow::newFile);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveAsFile);
    connect(m_exportAction, &QAction::triggered, this, &MainWindow::exportImage);
    connect(m_quitAction, &QAction::triggered, this, &MainWindow::quit);
    
    // Edit actions
    connect(m_undoAction, &QAction::triggered, this, &MainWindow::undo);
    connect(m_redoAction, &QAction::triggered, this, &MainWindow::redo);
    connect(m_cutAction, &QAction::triggered, this, &MainWindow::cut);
    connect(m_copyAction, &QAction::triggered, this, &MainWindow::copy);
    connect(m_pasteAction, &QAction::triggered, this, &MainWindow::paste);
    
    // View actions
    connect(m_zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);
    connect(m_zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);
    connect(m_zoomFitAction, &QAction::triggered, this, &MainWindow::zoomFit);
    connect(m_zoomActualAction, &QAction::triggered, this, &MainWindow::zoomActual);
    
    // Panel actions
    connect(m_showLayerPanelAction, &QAction::toggled, m_layerDock, &QDockWidget::setVisible);
    connect(m_showHistoryPanelAction, &QAction::toggled, m_historyDock, &QDockWidget::setVisible);
    connect(m_showPropertiesPanelAction, &QAction::toggled, m_propertiesDock, &QDockWidget::setVisible);
    
    // Tool actions
    connect(m_brushToolAction, &QAction::triggered, [this]() { m_canvas->setTool(m_brushTool); });
    connect(m_eraserToolAction, &QAction::triggered, [this]() { m_canvas->setTool(m_eraserTool); });
    connect(m_selectionToolAction, &QAction::triggered, [this]() { m_canvas->setTool(m_selectionTool); });
    
    // Help actions
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::about);
    
    // Document connections
    connect(m_document, &ImageDocument::documentChanged, this, &MainWindow::updateActions);
    connect(m_document, &ImageDocument::documentChanged, this, &MainWindow::updateWindowTitle);
    connect(m_document, &ImageDocument::modifiedChanged, this, &MainWindow::updateWindowTitle);
    
    // Canvas connections
    connect(m_canvas, &CanvasWidget::zoomChanged, this, &MainWindow::updateActions);
    
    // Layer panel connections
    connect(m_layerPanel, &LayerPanel::layerSelected, this, &MainWindow::onLayerSelected);
    connect(m_layerPanel, &LayerPanel::layerVisibilityToggled, this, &MainWindow::onLayerVisibilityToggled);
    connect(m_layerPanel, &LayerPanel::layerOpacityChanged, this, &MainWindow::onLayerOpacityChanged);
    connect(m_layerPanel, &LayerPanel::layerMoved, this, &MainWindow::onLayerMoved);
}

void MainWindow::setupStatusBar()
{
    // Create status bar
    QStatusBar *statusBar = this->statusBar();
    
    // Zoom label
    QLabel *zoomLabel = new QLabel("100%", this);
    statusBar->addPermanentWidget(zoomLabel);
    
    // Image size label
    QLabel *sizeLabel = new QLabel("0 x 0", this);
    statusBar->addPermanentWidget(sizeLabel);
    
    // Progress bar
    QProgressBar *progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
    statusBar->addPermanentWidget(progressBar);
    
    // Connect signals
    // TODO: Implement zoom change connection
    Q_UNUSED(zoomLabel);
    
    connect(m_document, &ImageDocument::imageSizeChanged, [sizeLabel](const QSize &size) {
        sizeLabel->setText(QString::number(size.width()) + " x " + QString::number(size.height()));
    });
}

void MainWindow::centerWindow()
{
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    QRect windowGeometry = frameGeometry();
    
    int x = (screenGeometry.width() - windowGeometry.width()) / 2;
    int y = (screenGeometry.height() - windowGeometry.height()) / 2;
    
    move(x, y);
}

void MainWindow::newFile()
{
    // Create new document dialog
    QDialog dialog(this);
    dialog.setWindowTitle("New Document");
    dialog.setModal(true);
    
    QFormLayout *layout = new QFormLayout(&dialog);
    
    QSpinBox *widthSpinBox = new QSpinBox(&dialog);
    widthSpinBox->setRange(1, 10000);
    widthSpinBox->setValue(800);
    layout->addRow("Width:", widthSpinBox);
    
    QSpinBox *heightSpinBox = new QSpinBox(&dialog);
    heightSpinBox->setRange(1, 10000);
    heightSpinBox->setValue(600);
    layout->addRow("Height:", heightSpinBox);
    
    QComboBox *colorCombo = new QComboBox(&dialog);
    colorCombo->addItem("White", QVariant(QColor(Qt::white)));
    colorCombo->addItem("Black", QVariant(QColor(Qt::black)));
    colorCombo->addItem("Transparent", QColor(0, 0, 0, 0));
    layout->addRow("Background:", colorCombo);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addRow(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        int width = widthSpinBox->value();
        int height = heightSpinBox->value();
        QColor backgroundColor = colorCombo->currentData().value<QColor>();
        
        if (m_document->newDocument(width, height, backgroundColor)) {
            m_canvas->resetView();
            updateActions();
            updateWindowTitle();
        }
    }
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", 
        "Image Files (*.png *.jpg *.jpeg *.bmp *.tiff *.gif);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        if (m_document->openDocument(fileName)) {
            m_canvas->resetView();
            updateActions();
            updateWindowTitle();
        } else {
            QMessageBox::warning(this, "Error", "Failed to open file: " + fileName);
        }
    }
}

void MainWindow::saveFile()
{
    if (m_document->getFilePath().isEmpty()) {
        saveAsFile();
    } else {
        if (!m_document->saveDocument(m_document->getFilePath())) {
            QMessageBox::warning(this, "Error", "Failed to save file: " + m_document->getFilePath());
        }
    }
}

void MainWindow::saveAsFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save As", "", 
        "Photo Editor Files (*.qphoto);;PNG Files (*.png);;JPEG Files (*.jpg);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        if (m_document->saveDocument(fileName)) {
            updateWindowTitle();
        } else {
            QMessageBox::warning(this, "Error", "Failed to save file: " + fileName);
        }
    }
}

void MainWindow::exportImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export Image", "", 
        "PNG Files (*.png);;JPEG Files (*.jpg);;BMP Files (*.bmp);;TIFF Files (*.tiff);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        QString format = QFileInfo(fileName).suffix().toUpper();
        if (m_document->exportImage(fileName, format)) {
            QMessageBox::information(this, "Success", "Image exported successfully!");
        } else {
            QMessageBox::warning(this, "Error", "Failed to export image: " + fileName);
        }
    }
}

void MainWindow::quit()
{
    close();
}

void MainWindow::undo()
{
    // TODO: Implement undo functionality
    Q_UNUSED(m_document);
    m_canvas->update();
    updateActions();
}

void MainWindow::redo()
{
    // TODO: Implement redo functionality
    Q_UNUSED(m_document);
    m_canvas->update();
    updateActions();
}

void MainWindow::cut()
{
    // TODO: Implement cut functionality
}

void MainWindow::copy()
{
    // TODO: Implement copy functionality
}

void MainWindow::paste()
{
    // TODO: Implement paste functionality
}

void MainWindow::zoomIn()
{
    m_canvas->zoomIn();
}

void MainWindow::zoomOut()
{
    m_canvas->zoomOut();
}

void MainWindow::zoomFit()
{
    m_canvas->zoomFit();
}

void MainWindow::zoomActual()
{
    m_canvas->zoomActual();
}

void MainWindow::showLayerPanel()
{
    m_layerDock->setVisible(!m_layerDock->isVisible());
}

void MainWindow::showHistoryPanel()
{
    m_historyDock->setVisible(!m_historyDock->isVisible());
}

void MainWindow::showPropertiesPanel()
{
    m_propertiesDock->setVisible(!m_propertiesDock->isVisible());
}

void MainWindow::about()
{
    QMessageBox::about(this, "About Photo Editor", 
        "Photo Editor v1.0.0\n\n"
        "A powerful image editing application built with Qt.\n\n"
        "Features:\n"
        "• Multi-layer support\n"
        "• Advanced brush tools\n"
        "• Professional filters\n"
        "• Undo/Redo system\n"
        "• Multiple export formats\n\n"
        "Built with Qt " + QString(QT_VERSION_STR));
}

void MainWindow::updateActions()
{
    // Update undo/redo actions
    m_undoAction->setEnabled(true); // TODO: Implement proper undo/redo state
    m_redoAction->setEnabled(true); // TODO: Implement proper undo/redo state
    
    // Update save action
    m_saveAction->setEnabled(!m_document->getFilePath().isEmpty());
    
    // Update zoom actions
    m_zoomInAction->setEnabled(m_canvas->getZoom() < 10.0f);
    m_zoomOutAction->setEnabled(m_canvas->getZoom() > 0.1f);
}

void MainWindow::updateWindowTitle()
{
    QString title = "Photo Editor";
    
    if (!m_document->getFilePath().isEmpty()) {
        title += " - " + QFileInfo(m_document->getFilePath()).fileName();
    } else {
        title += " - Untitled";
    }
    
    if (m_document->isModified()) {
        title += " *";
    }
    
    setWindowTitle(title);
}

void MainWindow::onLayerSelected(int index)
{
    m_currentLayerIndex = index;
    m_document->setActiveLayer(index);
}

void MainWindow::onLayerVisibilityToggled(int index, bool visible)
{
    if (Layer *layer = m_document->getLayer(index)) {
        layer->setVisible(visible);
    }
}

void MainWindow::onLayerOpacityChanged(int index, float opacity)
{
    if (Layer *layer = m_document->getLayer(index)) {
        layer->setOpacity(opacity);
    }
}

void MainWindow::onLayerMoved(int fromIndex, int toIndex)
{
    m_document->moveLayer(fromIndex, toIndex);
}
