#include "SimpleMainWindow.h"
#include <QApplication>
#include <QDebug>

SimpleMainWindow::SimpleMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_imageLabel(nullptr)
    , m_scrollArea(nullptr)
    , m_brightnessSlider(nullptr)
    , m_contrastSlider(nullptr)
    , m_saturationSlider(nullptr)
    , m_brightness(0)
    , m_contrast(1.0f)
    , m_saturation(1.0f)
{
    setupUI();
    setupMenus();
    setupToolbars();
    setupStatusBar();
    
    // Create a default white image
    newImage();
}

SimpleMainWindow::~SimpleMainWindow()
{
}

void SimpleMainWindow::setupUI()
{
    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Create layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Create image display area
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setMinimumSize(400, 300);
    m_imageLabel->setStyleSheet("border: 1px solid gray; background-color: white;");
    
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidget(m_imageLabel);
    m_scrollArea->setWidgetResizable(true);
    
    mainLayout->addWidget(m_scrollArea);
    
    // Create adjustment controls
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    
    // Brightness control
    QLabel *brightnessLabel = new QLabel("Brightness:", this);
    m_brightnessSlider = new QSlider(Qt::Horizontal, this);
    m_brightnessSlider->setRange(-100, 100);
    m_brightnessSlider->setValue(0);
    
    controlsLayout->addWidget(brightnessLabel);
    controlsLayout->addWidget(m_brightnessSlider);
    
    // Contrast control
    QLabel *contrastLabel = new QLabel("Contrast:", this);
    m_contrastSlider = new QSlider(Qt::Horizontal, this);
    m_contrastSlider->setRange(0, 200);
    m_contrastSlider->setValue(100);
    
    controlsLayout->addWidget(contrastLabel);
    controlsLayout->addWidget(m_contrastSlider);
    
    // Saturation control
    QLabel *saturationLabel = new QLabel("Saturation:", this);
    m_saturationSlider = new QSlider(Qt::Horizontal, this);
    m_saturationSlider->setRange(0, 200);
    m_saturationSlider->setValue(100);
    
    controlsLayout->addWidget(saturationLabel);
    controlsLayout->addWidget(m_saturationSlider);
    
    // Reset button
    QPushButton *resetButton = new QPushButton("Reset", this);
    
    controlsLayout->addWidget(resetButton);
    
    mainLayout->addLayout(controlsLayout);
    
    // Connect signals using lambda functions
    connect(m_brightnessSlider, &QSlider::valueChanged, [this](int value) {
        m_brightness = value;
        updateImage();
    });
    
    connect(m_contrastSlider, &QSlider::valueChanged, [this](int value) {
        m_contrast = value / 100.0f;
        updateImage();
    });
    
    connect(m_saturationSlider, &QSlider::valueChanged, [this](int value) {
        m_saturation = value / 100.0f;
        updateImage();
    });
    
    connect(resetButton, &QPushButton::clicked, [this]() {
        m_brightnessSlider->setValue(0);
        m_contrastSlider->setValue(100);
        m_saturationSlider->setValue(100);
        
        m_brightness = 0;
        m_contrast = 1.0f;
        m_saturation = 1.0f;
        
        m_currentImage = m_originalImage;
        updateImage();
    });
}

void SimpleMainWindow::setupMenus()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    QAction *newAction = fileMenu->addAction("&New");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, [this]() {
        newImage();
    });
    
    QAction *openAction = fileMenu->addAction("&Open");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, [this]() {
        openImage();
    });
    
    QAction *saveAction = fileMenu->addAction("&Save");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, [this]() {
        saveImage();
    });
    
    fileMenu->addSeparator();
    
    QAction *exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
}

void SimpleMainWindow::setupToolbars()
{
    QToolBar *toolBar = addToolBar("Main");
    
    QAction *newAction = toolBar->addAction("New");
    connect(newAction, &QAction::triggered, [this]() {
        newImage();
    });
    
    QAction *openAction = toolBar->addAction("Open");
    connect(openAction, &QAction::triggered, [this]() {
        openImage();
    });
    
    QAction *saveAction = toolBar->addAction("Save");
    connect(saveAction, &QAction::triggered, [this]() {
        saveImage();
    });
}

void SimpleMainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ready");
}

void SimpleMainWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Image", "", "Image Files (*.png *.jpg *.jpeg *.bmp *.gif)");
    
    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if (!image.isNull()) {
            m_originalImage = image;
            m_currentImage = image;
            updateImage();
            statusBar()->showMessage(QString("Opened: %1").arg(fileName));
        } else {
            QMessageBox::warning(this, "Error", "Could not load image!");
        }
    }
}

void SimpleMainWindow::saveImage()
{
    if (m_currentImage.isNull()) {
        QMessageBox::warning(this, "Error", "No image to save!");
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Image", "", "PNG Files (*.png);;JPEG Files (*.jpg)");
    
    if (!fileName.isEmpty()) {
        if (m_currentImage.save(fileName)) {
            statusBar()->showMessage(QString("Saved: %1").arg(fileName));
        } else {
            QMessageBox::warning(this, "Error", "Could not save image!");
        }
    }
}

void SimpleMainWindow::newImage()
{
    m_originalImage = QImage(800, 600, QImage::Format_RGB32);
    m_originalImage.fill(Qt::white);
    m_currentImage = m_originalImage;
    
    // Reset sliders
    m_brightnessSlider->setValue(0);
    m_contrastSlider->setValue(100);
    m_saturationSlider->setValue(100);
    
    updateImage();
    statusBar()->showMessage("New image created");
}

void SimpleMainWindow::updateImage()
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
    if (m_contrast != 1.0f) {
        for (int y = 0; y < m_currentImage.height(); ++y) {
            for (int x = 0; x < m_currentImage.width(); ++x) {
                QColor color = m_currentImage.pixelColor(x, y);
                int r = qBound(0, static_cast<int>((color.red() - 128) * m_contrast + 128), 255);
                int g = qBound(0, static_cast<int>((color.green() - 128) * m_contrast + 128), 255);
                int b = qBound(0, static_cast<int>((color.blue() - 128) * m_contrast + 128), 255);
                m_currentImage.setPixelColor(x, y, QColor(r, g, b, color.alpha()));
            }
        }
    }
    
    // Apply saturation
    if (m_saturation != 1.0f) {
        for (int y = 0; y < m_currentImage.height(); ++y) {
            for (int x = 0; x < m_currentImage.width(); ++x) {
                QColor color = m_currentImage.pixelColor(x, y);
                QColor hsv = color.toHsv();
                int s = qBound(0, static_cast<int>(hsv.saturation() * m_saturation), 255);
                hsv.setHsv(hsv.hue(), s, hsv.value(), color.alpha());
                m_currentImage.setPixelColor(x, y, hsv.toRgb());
            }
        }
    }
    
    // Display the image
    QPixmap pixmap = QPixmap::fromImage(m_currentImage);
    m_imageLabel->setPixmap(pixmap);
}