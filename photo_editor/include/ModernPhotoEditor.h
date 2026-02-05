#ifndef MODERNPHOTOEDITOR_H
#define MODERNPHOTOEDITOR_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QActionGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QColorDialog>
#include <QImage>
#include <QPainter>
#include <QDockWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QGroupBox>
#include <QFormLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QSplitter>
#include <QFrame>
#include <QScrollBar>
#include <QProgressBar>
#include <QTimer>
#include <QStyleFactory>
#include <QPalette>
#include <QApplication>
#include "ToolManager.h"
#include "CanvasWidget.h"
#include "LogPanel.h"
#include "HotkeyManager.h"
#include "TooltipManager.h"

class ModernPhotoEditor : public QMainWindow
{
public:
    explicit ModernPhotoEditor(QWidget *parent = nullptr);
    ~ModernPhotoEditor();

private:
    void setupUI();
    void setupMenus();
    void setupToolbars();
    void setupDockWidgets();
    void setupStatusBar();
    void setupConnections();
    void updateImage();
    void updateLayersList();
    void updateHistoryList();
    void applyImageAdjustments();
    void createModernStyle();
    
    // Image operations
    void openImage();
    void saveImage();
    void saveAsImage();
    void newImage();
    void undoAction();
    void redoAction();
    void copyImage();
    void pasteImage();
    void cutImage();
    void selectAll();
    void deselectAll();
    void zoomIn();
    void zoomOut();
    void fitToWindow();
    void actualSize();
    void rotateLeft();
    void rotateRight();
    void flipHorizontal();
    void flipVertical();
    void cropImage();
    void resizeImage();
    void adjustBrightness(int value);
    void adjustContrast(int value);
    void adjustSaturation(int value);
    void adjustHue(int value);
    void resetAdjustments();
    void toolChanged();
    void brushSizeChanged(int size);
    void brushOpacityChanged(int opacity);
    void brushHardnessChanged(int hardness);
    void brushColorChanged();
    void layerVisibilityToggled();
    void layerOpacityChanged(int opacity);
    void addNewLayer();
    void deleteLayer();
    void duplicateLayer();
    void mergeLayers();
    void showAbout();
    
    // UI Components
    QWidget *m_centralWidget;
    CanvasWidget *m_canvasWidget;
    QScrollArea *m_scrollArea;
    QLabel *m_imageLabel;
    QSplitter *m_mainSplitter;
    
    // Tool Manager
    ToolManager *m_toolManager;
    
    // Hotkey and Tooltip Managers
    HotkeyManager *m_hotkeyManager;
    TooltipManager *m_tooltipManager;
    
    // Dock Widgets
    QDockWidget *m_toolsDock;
    QDockWidget *m_layersDock;
    QDockWidget *m_propertiesDock;
    QDockWidget *m_historyDock;
    QDockWidget *m_colorsDock;
    QDockWidget *m_logDock;
    
    // Tools Panel
    QGroupBox *m_toolsGroup;
    QActionGroup *m_toolGroup;
    QAction *m_moveTool;
    QAction *m_marqueeTool;
    QAction *m_lassoTool;
    QAction *m_magicWandTool;
    QAction *m_selectTool;
    QAction *m_brushTool;
    QAction *m_pencilTool;
    QAction *m_cloneStampTool;
    QAction *m_healingBrushTool;
    QAction *m_eraserTool;
    QAction *m_gradientTool;
    QAction *m_paintBucketTool;
    QAction *m_cropTool;
    QAction *m_eyedropperTool;
    QAction *m_handTool;
    QAction *m_zoomTool;
    QAction *m_textTool;
    QAction *m_penTool;
    QAction *m_shapeTool;
    
    // Brush Properties
    QGroupBox *m_brushGroup;
    QSlider *m_brushSizeSlider;
    QSlider *m_brushOpacitySlider;
    QSlider *m_brushHardnessSlider;
    QPushButton *m_brushColorButton;
    QSpinBox *m_brushSizeSpinBox;
    QSpinBox *m_brushOpacitySpinBox;
    QSpinBox *m_brushHardnessSpinBox;
    
    // Layers Panel
    QListWidget *m_layersList;
    QPushButton *m_addLayerButton;
    QPushButton *m_deleteLayerButton;
    QPushButton *m_duplicateLayerButton;
    QPushButton *m_mergeButton;
    QSlider *m_layerOpacitySlider;
    QCheckBox *m_layerVisibilityCheckBox;
    
    // Properties Panel
    QTabWidget *m_propertiesTabs;
    QGroupBox *m_adjustmentsGroup;
    QSlider *m_brightnessSlider;
    QSlider *m_contrastSlider;
    QSlider *m_saturationSlider;
    QSlider *m_hueSlider;
    QSpinBox *m_brightnessSpinBox;
    QSpinBox *m_contrastSpinBox;
    QSpinBox *m_saturationSpinBox;
    QSpinBox *m_hueSpinBox;
    QPushButton *m_resetButton;
    
    // History Panel
    QListWidget *m_historyList;
    QPushButton *m_undoButton;
    QPushButton *m_redoButton;
    QPushButton *m_clearHistoryButton;
    
    // Colors Panel
    QGroupBox *m_colorsGroup;
    QPushButton *m_foregroundColorButton;
    QPushButton *m_backgroundColorButton;
    QPushButton *m_swapColorsButton;
    QLabel *m_colorPreviewLabel;
    
    // Image Data
    QImage m_originalImage;
    QImage m_currentImage;
    QList<QImage> m_imageHistory;
    int m_currentHistoryIndex;
    
    // Current Settings
    QColor m_brushColor;
    QColor m_backgroundColor;
    int m_currentTool;
    int m_brushSize;
    int m_brushOpacity;
    int m_brushHardness;
    float m_zoomFactor;
    
    // Adjustment Values
    int m_brightness;
    int m_contrast;
    int m_saturation;
    int m_hue;
};

#endif // MODERNPHOTOEDITOR_H