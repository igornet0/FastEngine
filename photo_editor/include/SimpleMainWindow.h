#ifndef SIMPLEMAINWINDOW_H
#define SIMPLEMAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
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

class SimpleMainWindow : public QMainWindow
{
public:
    explicit SimpleMainWindow(QWidget *parent = nullptr);
    ~SimpleMainWindow();

private:
    void setupUI();
    void setupMenus();
    void setupToolbars();
    void setupStatusBar();
    void updateImage();
    void openImage();
    void saveImage();
    void newImage();
    
    QImage m_originalImage;
    QImage m_currentImage;
    QLabel *m_imageLabel;
    QScrollArea *m_scrollArea;
    
    QSlider *m_brightnessSlider;
    QSlider *m_contrastSlider;
    QSlider *m_saturationSlider;
    
    int m_brightness;
    float m_contrast;
    float m_saturation;
};

#endif // SIMPLEMAINWINDOW_H