#include "CanvasWidget.h"
#include "ToolManager.h"
#include "Logger.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <QScrollArea>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent)
    , m_toolManager(nullptr)
    , m_zoomFactor(1.0f)
    , m_offset(0, 0)
    , m_lastMousePos(0, 0)
    , m_isPanning(false)
    , m_showGrid(true)
    , m_gridSize(20)
    , m_gridColor(QColor(128, 128, 128, 100))
    , m_isCropping(false)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(400, 300);
    
    // Create default white image
    m_image = QImage(800, 600, QImage::Format_RGB32);
    m_image.fill(Qt::white);
    m_displayImage = m_image;
}

CanvasWidget::~CanvasWidget()
{
}

void CanvasWidget::setImage(const QImage &image)
{
    m_image = image;
    updateDisplayImage();
    update();
}

void CanvasWidget::setToolManager(ToolManager *toolManager)
{
    m_toolManager = toolManager;
    if (m_toolManager) {
        m_toolManager->setImage(&m_image);
    }
}

void CanvasWidget::setZoomFactor(float factor)
{
    m_zoomFactor = qBound(0.1f, factor, 10.0f);
    updateDisplayImage();
    update();
}

void CanvasWidget::fitToWindow()
{
    if (m_image.isNull()) return;
    
    QSize widgetSize = size();
    QSize imageSize = m_image.size();
    
    float scaleX = static_cast<float>(widgetSize.width()) / imageSize.width();
    float scaleY = static_cast<float>(widgetSize.height()) / imageSize.height();
    
    m_zoomFactor = qMin(scaleX, scaleY) * 0.9f; // 90% to leave some margin
    updateDisplayImage();
    update();
}

void CanvasWidget::actualSize()
{
    m_zoomFactor = 1.0f;
    updateDisplayImage();
    update();
}

void CanvasWidget::zoomIn()
{
    setZoomFactor(m_zoomFactor * 1.2f);
}

void CanvasWidget::zoomOut()
{
    setZoomFactor(m_zoomFactor / 1.2f);
}

void CanvasWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Fill background
    painter.fillRect(rect(), QColor(50, 50, 50));
    
    if (m_displayImage.isNull()) {
        return;
    }
    
    // Calculate image position
    QSize imageSize = m_displayImage.size();
    QPoint imagePos = QPoint(
        (width() - imageSize.width()) / 2 + m_offset.x(),
        (height() - imageSize.height()) / 2 + m_offset.y()
    );
    
    // Draw image
    painter.drawImage(imagePos, m_displayImage);
    
    // Draw grid if enabled
    if (m_showGrid && m_zoomFactor > 0.5f) {
        drawGrid(painter);
    }
    
    // Draw selection rectangle
    if (m_toolManager && (m_toolManager->hasSelection() || m_toolManager->isSelecting())) {
        drawSelection(painter);
    }
    
    // Draw crop rectangle
    if (m_toolManager && m_toolManager->isCropping()) {
        drawCropRect(painter);
    }
    
    // Draw tool preview
    if (m_toolManager) {
        QPoint mousePos = mapFromGlobal(QCursor::pos());
        if (rect().contains(mousePos)) {
            QPoint imagePoint = widgetToImage(mousePos);
            m_toolManager->drawToolPreview(painter, imageToWidget(imagePoint));
        }
    }
}

void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_toolManager) return;
    
    QPoint imagePoint = widgetToImage(event->pos());
    QString buttonName = (event->button() == Qt::LeftButton) ? "Left" : 
                       (event->button() == Qt::RightButton) ? "Right" : 
                       (event->button() == Qt::MiddleButton) ? "Middle" : "Unknown";
    
    LOG_UI_EVENT("MOUSE_PRESS", 
        QString("Widget: (%1,%2) | Image: (%3,%4) | Button: %5 | Modifiers: %6")
        .arg(event->pos().x()).arg(event->pos().y())
        .arg(imagePoint.x()).arg(imagePoint.y())
        .arg(buttonName)
        .arg(event->modifiers() & Qt::ControlModifier ? "Ctrl+" : "")
        + (event->modifiers() & Qt::ShiftModifier ? "Shift+" : "")
        + (event->modifiers() & Qt::AltModifier ? "Alt+" : ""));
    
    if (event->button() == Qt::MiddleButton || 
        (event->button() == Qt::LeftButton && event->modifiers() & Qt::ShiftModifier)) {
        // Start panning
        m_isPanning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        LOG_UI_EVENT("PANNING", "Started panning mode");
    } else {
        // Tool operation
        m_toolManager->beginDrawing(imagePoint, event->button());
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_toolManager) return;
    
    QPoint imagePoint = widgetToImage(event->pos());
    
    if (m_isPanning) {
        // Panning
        QPoint delta = event->pos() - m_lastMousePos;
        m_offset += delta;
        m_lastMousePos = event->pos();
        update();
    } else {
        // Tool operation
        Qt::MouseButton button = Qt::NoButton;
        if (event->buttons() & Qt::LeftButton) button = Qt::LeftButton;
        else if (event->buttons() & Qt::RightButton) button = Qt::RightButton;
        else if (event->buttons() & Qt::MiddleButton) button = Qt::MiddleButton;
        
        m_toolManager->updateDrawing(imagePoint, button);
    }
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_toolManager) return;
    
    QPoint imagePoint = widgetToImage(event->pos());
    QString buttonName = (event->button() == Qt::LeftButton) ? "Left" : 
                       (event->button() == Qt::RightButton) ? "Right" : 
                       (event->button() == Qt::MiddleButton) ? "Middle" : "Unknown";
    
    LOG_UI_EVENT("MOUSE_RELEASE", 
        QString("Widget: (%1,%2) | Image: (%3,%4) | Button: %5")
        .arg(event->pos().x()).arg(event->pos().y())
        .arg(imagePoint.x()).arg(imagePoint.y())
        .arg(buttonName));
    
    if (m_isPanning) {
        // End panning
        m_isPanning = false;
        updateCursor();
        LOG_UI_EVENT("PANNING", "Ended panning mode");
    } else {
        // Tool operation
        m_toolManager->endDrawing(imagePoint, event->button());
    }
}

void CanvasWidget::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        // Zoom
        float delta = event->angleDelta().y() > 0 ? 1.2f : 1.0f / 1.2f;
        float oldZoom = m_zoomFactor;
        setZoomFactor(m_zoomFactor * delta);
        
        LOG_UI_EVENT("ZOOM", 
            QString("Zoom: %1% -> %2% | Delta: %3 | Position: (%4,%5)")
            .arg(static_cast<int>(oldZoom * 100))
            .arg(static_cast<int>(m_zoomFactor * 100))
            .arg(event->angleDelta().y() > 0 ? "In" : "Out")
            .arg(event->position().x()).arg(event->position().y()));
    } else {
        // Scroll
        LOG_UI_EVENT("SCROLL", 
            QString("Scroll: %1 | Position: (%2,%3)")
            .arg(event->angleDelta().y())
            .arg(event->position().x()).arg(event->position().y()));
        QWidget::wheelEvent(event);
    }
}

void CanvasWidget::keyPressEvent(QKeyEvent *event)
{
    QString keyName = QKeySequence(event->key()).toString();
    QString modifiers = "";
    if (event->modifiers() & Qt::ControlModifier) modifiers += "Ctrl+";
    if (event->modifiers() & Qt::ShiftModifier) modifiers += "Shift+";
    if (event->modifiers() & Qt::AltModifier) modifiers += "Alt+";
    
    LOG_UI_EVENT("KEY_PRESS", 
        QString("Key: %1 | Modifiers: %2")
        .arg(keyName).arg(modifiers));
    
    switch (event->key()) {
        case Qt::Key_Plus:
        case Qt::Key_Equal:
            zoomIn();
            break;
        case Qt::Key_Minus:
            zoomOut();
            break;
        case Qt::Key_0:
            actualSize();
            break;
        case Qt::Key_F:
            fitToWindow();
            break;
        case Qt::Key_Escape:
            if (m_toolManager) {
                m_toolManager->setCurrentTool(ToolManager::SelectTool);
                LOG_UI_EVENT("KEYBOARD_SHORTCUT", "Escape -> Select Tool");
            }
            break;
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}

void CanvasWidget::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    updateCursor();
}

void CanvasWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::ArrowCursor);
}

void CanvasWidget::updateCursor()
{
    if (m_toolManager) {
        setCursor(m_toolManager->getCurrentCursor());
    }
}

QPoint CanvasWidget::imageToWidget(const QPoint &imagePoint) const
{
    QSize imageSize = m_displayImage.size();
    QPoint imagePos = QPoint(
        (width() - imageSize.width()) / 2 + m_offset.x(),
        (height() - imageSize.height()) / 2 + m_offset.y()
    );
    
    return QPoint(
        imagePos.x() + imagePoint.x(),
        imagePos.y() + imagePoint.y()
    );
}

QPoint CanvasWidget::widgetToImage(const QPoint &widgetPoint) const
{
    QSize imageSize = m_displayImage.size();
    QPoint imagePos = QPoint(
        (width() - imageSize.width()) / 2 + m_offset.x(),
        (height() - imageSize.height()) / 2 + m_offset.y()
    );
    
    return QPoint(
        widgetPoint.x() - imagePos.x(),
        widgetPoint.y() - imagePos.y()
    );
}

QRect CanvasWidget::imageToWidget(const QRect &imageRect) const
{
    return QRect(
        imageToWidget(imageRect.topLeft()),
        imageToWidget(imageRect.bottomRight())
    );
}

QRect CanvasWidget::widgetToImage(const QRect &widgetRect) const
{
    return QRect(
        widgetToImage(widgetRect.topLeft()),
        widgetToImage(widgetRect.bottomRight())
    );
}

void CanvasWidget::drawGrid(QPainter &painter)
{
    QSize imageSize = m_displayImage.size();
    QPoint imagePos = QPoint(
        (width() - imageSize.width()) / 2 + m_offset.x(),
        (height() - imageSize.height()) / 2 + m_offset.y()
    );
    
    painter.setPen(QPen(m_gridColor, 1));
    
    int scaledGridSize = static_cast<int>(m_gridSize * m_zoomFactor);
    
    // Draw vertical lines
    for (int x = imagePos.x(); x < imagePos.x() + imageSize.width(); x += scaledGridSize) {
        painter.drawLine(x, imagePos.y(), x, imagePos.y() + imageSize.height());
    }
    
    // Draw horizontal lines
    for (int y = imagePos.y(); y < imagePos.y() + imageSize.height(); y += scaledGridSize) {
        painter.drawLine(imagePos.x(), y, imagePos.x() + imageSize.width(), y);
    }
}

void CanvasWidget::drawSelection(QPainter &painter)
{
    if (!m_toolManager || (!m_toolManager->hasSelection() && !m_toolManager->isSelecting())) return;
    
    QRect selectionRect = m_toolManager->getSelectionRect();
    QRect widgetRect = imageToWidget(selectionRect);
    
    // Draw selection rectangle
    QPen pen(QColor(0, 120, 255), 1, Qt::DashLine);
    painter.setPen(pen);
    painter.setBrush(QColor(0, 120, 255, 30));
    painter.drawRect(widgetRect);
    
    // Draw corner handles only for completed selections
    if (m_toolManager->hasSelection()) {
        int handleSize = 6;
        QRect handles[] = {
            QRect(widgetRect.topLeft() - QPoint(handleSize/2, handleSize/2), QSize(handleSize, handleSize)),
            QRect(widgetRect.topRight() - QPoint(handleSize/2, handleSize/2), QSize(handleSize, handleSize)),
            QRect(widgetRect.bottomLeft() - QPoint(handleSize/2, handleSize/2), QSize(handleSize, handleSize)),
            QRect(widgetRect.bottomRight() - QPoint(handleSize/2, handleSize/2), QSize(handleSize, handleSize))
        };
        
        painter.setPen(QPen(QColor(0, 120, 255), 2));
        painter.setBrush(QColor(255, 255, 255));
        
        for (const QRect &handle : handles) {
            painter.drawRect(handle);
        }
    }
}

void CanvasWidget::drawCropRect(QPainter &painter)
{
    if (!m_toolManager || !m_toolManager->isCropping()) return;
    
    QRect cropRect = m_toolManager->getCropRect();
    QRect widgetRect = imageToWidget(cropRect);
    
    // Draw crop rectangle
    QPen pen(QColor(255, 0, 0), 2);
    painter.setPen(pen);
    painter.setBrush(QColor(255, 0, 0, 50));
    painter.drawRect(widgetRect);
    
    // Draw corner handles
    int handleSize = 8;
    QRect handles[] = {
        QRect(widgetRect.topLeft() - QPoint(handleSize/2, handleSize/2), QSize(handleSize, handleSize)),
        QRect(widgetRect.topRight() - QPoint(handleSize/2, handleSize/2), QSize(handleSize, handleSize)),
        QRect(widgetRect.bottomLeft() - QPoint(handleSize/2, handleSize/2), QSize(handleSize, handleSize)),
        QRect(widgetRect.bottomRight() - QPoint(handleSize/2, handleSize/2), QSize(handleSize, handleSize))
    };
    
    painter.setPen(QPen(QColor(255, 0, 0), 2));
    painter.setBrush(QColor(255, 255, 255));
    
    for (const QRect &handle : handles) {
        painter.drawRect(handle);
    }
}

void CanvasWidget::updateDisplayImage()
{
    if (m_image.isNull()) return;
    
    if (qAbs(m_zoomFactor - 1.0f) < 0.01f) {
        m_displayImage = m_image;
    } else {
        QSize newSize = m_image.size() * m_zoomFactor;
        m_displayImage = m_image.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
}