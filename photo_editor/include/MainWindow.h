#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QDockWidget>
#include <QAction>
#include <QActionGroup>

class ImageDocument;
class CanvasWidget;
class LayerPanel;
class QScrollArea;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
    void exportImage();
    void quit();
    
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void zoomActual();
    
    void showLayerPanel();
    void showHistoryPanel();
    void showPropertiesPanel();
    
    void about();

private:
    void setupUI();
    void setupMenus();
    void setupToolbars();
    void setupDockWidgets();
    void setupConnections();
    
    void updateActions();
    void updateWindowTitle();
    void centerWindow();
    void setupStatusBar();
    
    void onLayerSelected(int index);
    void onLayerVisibilityToggled(int index, bool visible);
    void onLayerOpacityChanged(int index, float opacity);
    void onLayerMoved(int fromIndex, int toIndex);
    
    ImageDocument *m_document;
    CanvasWidget *m_canvas;
    LayerPanel *m_layerPanel;
    
    // Tools
    class BrushTool *m_brushTool;
    class EraserTool *m_eraserTool;
    class SelectionTool *m_selectionTool;
    
    // Current state
    int m_currentLayerIndex;
    
    // Menus
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_viewMenu;
    QMenu *m_imageMenu;
    QMenu *m_layerMenu;
    QMenu *m_filterMenu;
    QMenu *m_helpMenu;
    
    // Toolbars
    QToolBar *m_mainToolBar;
    QToolBar *m_toolsToolBar;
    
    // Actions
    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_exportAction;
    QAction *m_quitAction;
    
    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    
    QAction *m_zoomInAction;
    QAction *m_zoomOutAction;
    QAction *m_zoomFitAction;
    QAction *m_zoomActualAction;
    
    QAction *m_showLayerPanelAction;
    QAction *m_showHistoryPanelAction;
    QAction *m_showPropertiesPanelAction;
    
    QAction *m_aboutAction;
    
    // Tool actions
    QActionGroup *m_toolGroup;
    QAction *m_moveToolAction;
    QAction *m_brushToolAction;
    QAction *m_eraserToolAction;
    QAction *m_selectionToolAction;
    QAction *m_fillToolAction;
    QAction *m_textToolAction;
    
    // Dock widgets
    QDockWidget *m_layerDock;
    QDockWidget *m_historyDock;
    QDockWidget *m_propertiesDock;
};

#endif // MAINWINDOW_H
