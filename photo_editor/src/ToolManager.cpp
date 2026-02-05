#include "ToolManager.h"
#include "Logger.h"
#include <QApplication>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QBitmap>
#include <QPolygon>
#include <QWidget>

ToolManager::ToolManager(QObject *parent)
    : QObject(parent)
    , m_currentTool(SelectTool)
    , m_image(nullptr)
    , m_canvasWidget(nullptr)
    , m_brushSize(10)
    , m_brushColor(Qt::black)
    , m_brushOpacity(100)
    , m_brushHardness(50)
    , m_isDrawing(false)
    , m_lastPoint(0, 0)
    , m_startPoint(0, 0)
    , m_currentButton(Qt::NoButton)
    , m_hasSelection(false)
    , m_isSelecting(false)
    , m_isCropping(false)
{
    createCustomCursors();
}

ToolManager::~ToolManager()
{
}

void ToolManager::setCurrentTool(ToolType tool)
{
    if (m_currentTool != tool) {
        QString toolName = getToolName(tool);
        QString oldToolName = getToolName(m_currentTool);
        
        // Clear selection when switching away from Select tool
        if (m_currentTool == SelectTool && tool != SelectTool) {
            clearSelection();
        }
        
        m_currentTool = tool;
        
        LOG_TOOL_USAGE(toolName, "SELECTED", QString("Changed from %1").arg(oldToolName));
    }
}

QCursor ToolManager::getCurrentCursor() const
{
    switch (m_currentTool) {
        case SelectTool:
            return m_selectCursor;
        case BrushTool:
            return m_brushCursor;
        case EraserTool:
            return m_eraserCursor;
        case CropTool:
            return m_cropCursor;
        case TextTool:
            return m_textCursor;
        case ShapeTool:
            return m_shapeCursor;
        default:
            return Qt::ArrowCursor;
    }
}

void ToolManager::setBrushSize(int size)
{
    m_brushSize = qBound(1, size, 100);
    createCustomCursors(); // Recreate cursors with new size
    
    QString toolName = getToolName(m_currentTool);
    LOG_TOOL_PROPERTY(toolName, "brush_size", QString::number(m_brushSize));
}

void ToolManager::setBrushColor(const QColor &color)
{
    m_brushColor = color;
    QString toolName = getToolName(m_currentTool);
    LOG_TOOL_PROPERTY(toolName, "brush_color", color.name());
}

void ToolManager::setBrushOpacity(int opacity)
{
    m_brushOpacity = qBound(1, opacity, 100);
    QString toolName = getToolName(m_currentTool);
    LOG_TOOL_PROPERTY(toolName, "brush_opacity", QString::number(m_brushOpacity));
}

void ToolManager::setBrushHardness(int hardness)
{
    m_brushHardness = qBound(0, hardness, 100);
    QString toolName = getToolName(m_currentTool);
    LOG_TOOL_PROPERTY(toolName, "brush_hardness", QString::number(m_brushHardness));
}

void ToolManager::beginDrawing(const QPoint &point, Qt::MouseButton button)
{
    if (!m_image) return;
    
    m_isDrawing = true;
    m_lastPoint = point;
    m_startPoint = point;
    m_currentButton = button;
    
    QString toolName = getToolName(m_currentTool);
    LOG_TOOL_USAGE(toolName, "BEGIN_DRAWING", QString("Point: (%1, %2)").arg(point.x()).arg(point.y()));
    
    switch (m_currentTool) {
        case BrushTool:
        case EraserTool:
            // Start drawing stroke
            break;
        case SelectTool:
            // Clear previous selection when starting new one
            m_hasSelection = false;
            m_isSelecting = true;
            m_selectionRect = QRect(point, QSize(0, 0));
            
            // Force canvas update to clear previous selection
            if (m_canvasWidget) {
                m_canvasWidget->update();
            }
            break;
        case CropTool:
            m_cropRect = QRect(point, QSize(0, 0));
            m_isCropping = true;
            break;
        default:
            break;
    }
}

void ToolManager::updateDrawing(const QPoint &point, Qt::MouseButton button)
{
    if (!m_isDrawing || !m_image) return;
    
    QString toolName = getToolName(m_currentTool);
    QString buttonName = (button == Qt::LeftButton) ? "Left" : 
                       (button == Qt::RightButton) ? "Right" : 
                       (button == Qt::MiddleButton) ? "Middle" : "Unknown";
    
    switch (m_currentTool) {
        case BrushTool:
            if (button == Qt::LeftButton) {
                drawBrushStroke(m_lastPoint, point);
                LOG_TOOL_USAGE(toolName, "DRAWING", 
                    QString("Stroke from (%1,%2) to (%3,%4) | Size: %5 | Color: %6 | Opacity: %7%")
                    .arg(m_lastPoint.x()).arg(m_lastPoint.y())
                    .arg(point.x()).arg(point.y())
                    .arg(m_brushSize)
                    .arg(m_brushColor.name())
                    .arg(m_brushOpacity));
            }
            break;
        case EraserTool:
            if (button == Qt::LeftButton) {
                drawEraserStroke(m_lastPoint, point);
                LOG_TOOL_USAGE(toolName, "ERASING", 
                    QString("Erase from (%1,%2) to (%3,%4) | Size: %5 | Opacity: %6%")
                    .arg(m_lastPoint.x()).arg(m_lastPoint.y())
                    .arg(point.x()).arg(point.y())
                    .arg(m_brushSize)
                    .arg(m_brushOpacity));
            }
            break;
        case SelectTool:
            m_selectionRect = QRect(m_startPoint, point).normalized();
            m_isSelecting = true; // Show selection during dragging
            LOG_TOOL_USAGE(toolName, "SELECTING", 
                QString("Selection: (%1,%2) to (%3,%4) | Size: %5x%6")
                .arg(m_startPoint.x()).arg(m_startPoint.y())
                .arg(point.x()).arg(point.y())
                .arg(m_selectionRect.width()).arg(m_selectionRect.height()));
            // Notify canvas to update
            if (m_canvasWidget) {
                m_canvasWidget->update();
            }
            break;
        case CropTool:
            m_cropRect = QRect(m_startPoint, point).normalized();
            LOG_TOOL_USAGE(toolName, "CROPPING", 
                QString("Crop area: (%1,%2) to (%3,%4) | Size: %5x%6")
                .arg(m_startPoint.x()).arg(m_startPoint.y())
                .arg(point.x()).arg(point.y())
                .arg(m_cropRect.width()).arg(m_cropRect.height()));
            break;
        case TextTool:
            LOG_TOOL_USAGE(toolName, "TEXT_PLACEMENT", 
                QString("Text position: (%1,%2) | Button: %3")
                .arg(point.x()).arg(point.y()).arg(buttonName));
            break;
        case ShapeTool:
            LOG_TOOL_USAGE(toolName, "SHAPE_CREATION", 
                QString("Shape point: (%1,%2) | Button: %3")
                .arg(point.x()).arg(point.y()).arg(buttonName));
            break;
        default:
            break;
    }
    
    m_lastPoint = point;
}

void ToolManager::endDrawing(const QPoint &point, Qt::MouseButton button)
{
    if (!m_isDrawing) return;
    
    QString toolName = getToolName(m_currentTool);
    QString buttonName = (button == Qt::LeftButton) ? "Left" : 
                       (button == Qt::RightButton) ? "Right" : 
                       (button == Qt::MiddleButton) ? "Middle" : "Unknown";
    
    switch (m_currentTool) {
        case SelectTool:
            m_selectionRect = QRect(m_startPoint, point).normalized();
            m_hasSelection = !m_selectionRect.isEmpty();
            m_isSelecting = false; // End selection process
            LOG_TOOL_USAGE(toolName, "SELECTION_COMPLETE", 
                QString("Final selection: (%1,%2) to (%3,%4) | Size: %5x%6 | Valid: %7")
                .arg(m_startPoint.x()).arg(m_startPoint.y())
                .arg(point.x()).arg(point.y())
                .arg(m_selectionRect.width()).arg(m_selectionRect.height())
                .arg(m_hasSelection ? "Yes" : "No"));
            // Notify canvas to update
            if (m_canvasWidget) {
                m_canvasWidget->update();
            }
            break;
        case CropTool:
            m_cropRect = QRect(m_startPoint, point).normalized();
            m_isCropping = false;
            LOG_TOOL_USAGE(toolName, "CROP_COMPLETE", 
                QString("Final crop: (%1,%2) to (%3,%4) | Size: %5x%6")
                .arg(m_startPoint.x()).arg(m_startPoint.y())
                .arg(point.x()).arg(point.y())
                .arg(m_cropRect.width()).arg(m_cropRect.height()));
            break;
        case BrushTool:
            LOG_TOOL_USAGE(toolName, "DRAWING_COMPLETE", 
                QString("Brush stroke finished at (%1,%2) | Button: %3 | Total distance: %4px")
                .arg(point.x()).arg(point.y())
                .arg(buttonName)
                .arg(QLineF(m_startPoint, point).length()));
            break;
        case EraserTool:
            LOG_TOOL_USAGE(toolName, "ERASING_COMPLETE", 
                QString("Erase stroke finished at (%1,%2) | Button: %3 | Total distance: %4px")
                .arg(point.x()).arg(point.y())
                .arg(buttonName)
                .arg(QLineF(m_startPoint, point).length()));
            break;
        case TextTool:
            LOG_TOOL_USAGE(toolName, "TEXT_PLACED", 
                QString("Text placed at (%1,%2) | Button: %3")
                .arg(point.x()).arg(point.y()).arg(buttonName));
            break;
        case ShapeTool:
            LOG_TOOL_USAGE(toolName, "SHAPE_CREATED", 
                QString("Shape created at (%1,%2) | Button: %3")
                .arg(point.x()).arg(point.y()).arg(buttonName));
            break;
        default:
            break;
    }
    
    m_isDrawing = false;
}

void ToolManager::setImage(QImage *image)
{
    m_image = image;
}

void ToolManager::drawBrushStroke(const QPoint &from, const QPoint &to)
{
    if (!m_image) return;
    
    QPainter painter(m_image);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Set brush properties
    QPen pen(m_brushColor);
    pen.setWidth(m_brushSize);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    
    painter.setPen(pen);
    painter.setOpacity(m_brushOpacity / 100.0);
    
    // Draw line
    painter.drawLine(from, to);
}

void ToolManager::drawEraserStroke(const QPoint &from, const QPoint &to)
{
    if (!m_image) return;
    
    QPainter painter(m_image);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Set eraser properties
    QPen pen(Qt::transparent);
    pen.setWidth(m_brushSize);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    
    painter.setPen(pen);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    
    // Draw line
    painter.drawLine(from, to);
}

void ToolManager::drawSelectionRect(const QPoint &start, const QPoint &end)
{
    Q_UNUSED(start);
    Q_UNUSED(end);
    // Selection rectangle is drawn in the canvas widget
}

void ToolManager::drawCropRect(const QPoint &start, const QPoint &end)
{
    Q_UNUSED(start);
    Q_UNUSED(end);
    // Crop rectangle is drawn in the canvas widget
}

void ToolManager::drawToolPreview(QPainter &painter, const QPoint &point)
{
    painter.setRenderHint(QPainter::Antialiasing);
    
    switch (m_currentTool) {
        case BrushTool:
        case EraserTool: {
            // Draw brush preview circle
            QPen pen(Qt::black, 1);
            painter.setPen(pen);
            painter.setBrush(Qt::transparent);
            
            int radius = m_brushSize / 2;
            painter.drawEllipse(point.x() - radius, point.y() - radius, 
                              m_brushSize, m_brushSize);
            break;
        }
        case SelectTool:
            // Selection rectangle is drawn in CanvasWidget::paintEvent()
            // No preview needed for selection tool
            break;
        case CropTool:
            if (m_isCropping) {
                // Draw crop rectangle
                QPen pen(Qt::red, 2);
                painter.setPen(pen);
                painter.setBrush(QColor(255, 0, 0, 50));
                painter.drawRect(m_cropRect);
            }
            break;
        default:
            break;
    }
}

void ToolManager::createCustomCursors()
{
    // Select cursor (arrow)
    m_selectCursor = Qt::ArrowCursor;
    
    // Brush cursor (custom circle)
    m_brushCursor = createBrushCursor(m_brushSize);
    
    // Eraser cursor (custom circle with eraser icon)
    m_eraserCursor = createEraserCursor(m_brushSize);
    
    // Crop cursor (crosshair)
    m_cropCursor = createCropCursor();
    
    // Text cursor (I-beam)
    m_textCursor = Qt::IBeamCursor;
    
    // Shape cursor (crosshair)
    m_shapeCursor = Qt::CrossCursor;
}

QCursor ToolManager::createBrushCursor(int size)
{
    int cursorSize = qMax(16, qMin(32, size));
    QPixmap pixmap(cursorSize, cursorSize);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw outer circle
    QPen pen(Qt::black, 1);
    painter.setPen(pen);
    painter.setBrush(Qt::transparent);
    painter.drawEllipse(1, 1, cursorSize - 2, cursorSize - 2);
    
    // Draw inner circle
    pen.setColor(Qt::white);
    painter.setPen(pen);
    painter.drawEllipse(2, 2, cursorSize - 4, cursorSize - 4);
    
    // Draw center dot
    painter.setBrush(Qt::black);
    painter.drawEllipse(cursorSize/2 - 1, cursorSize/2 - 1, 2, 2);
    
    return QCursor(pixmap, cursorSize/2, cursorSize/2);
}

QCursor ToolManager::createEraserCursor(int size)
{
    int cursorSize = qMax(16, qMin(32, size));
    QPixmap pixmap(cursorSize, cursorSize);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw eraser icon (square with diagonal lines)
    QPen pen(Qt::black, 2);
    painter.setPen(pen);
    
    int margin = 2;
    QRect rect(margin, margin, cursorSize - 2*margin, cursorSize - 2*margin);
    
    // Draw square
    painter.drawRect(rect);
    
    // Draw diagonal lines
    painter.drawLine(rect.topLeft(), rect.bottomRight());
    painter.drawLine(rect.topRight(), rect.bottomLeft());
    
    return QCursor(pixmap, cursorSize/2, cursorSize/2);
}

QCursor ToolManager::createCropCursor()
{
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw crop icon (corner brackets)
    QPen pen(Qt::black, 2);
    painter.setPen(pen);
    
    // Top-left corner
    painter.drawLine(8, 4, 8, 12);
    painter.drawLine(4, 8, 12, 8);
    
    // Top-right corner
    painter.drawLine(24, 4, 24, 12);
    painter.drawLine(20, 8, 28, 8);
    
    // Bottom-left corner
    painter.drawLine(8, 20, 8, 28);
    painter.drawLine(4, 24, 12, 24);
    
    // Bottom-right corner
    painter.drawLine(24, 20, 24, 28);
    painter.drawLine(20, 24, 28, 24);
    
    return QCursor(pixmap, 16, 16);
}

QString ToolManager::getToolName(ToolType tool) const
{
    switch (tool) {
        case SelectTool: return "Select";
        case BrushTool: return "Brush";
        case EraserTool: return "Eraser";
        case CropTool: return "Crop";
        case TextTool: return "Text";
        case ShapeTool: return "Shape";
        default: return "Unknown";
    }
}

void ToolManager::clearSelection()
{
    m_hasSelection = false;
    m_isSelecting = false;
    m_selectionRect = QRect();
    
    // Force canvas update to clear selection
    if (m_canvasWidget) {
        m_canvasWidget->update();
        m_canvasWidget->repaint(); // Force immediate repaint
    }
    
    LOG_TOOL_USAGE("Select", "CLEAR_SELECTION", "Selection cleared");
}
