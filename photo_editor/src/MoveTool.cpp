#include "MoveTool.h"
#include "ImageDocument.h"
#include "CanvasWidget.h"
#include "Logger.h"
#include <QPainter>
#include <QDebug>

MoveTool::MoveTool(QObject *parent)
    : Tool(Tool::MoveTool, parent)
    , m_moveMode(MoveLayer)
    , m_autoSelectLayer(true)
    , m_showTransformControls(true)
    , m_isMoving(false)
    , m_selectedLayerIndex(-1)
{
    m_name = "Move Tool";
    m_description = "Move and transform layers or selections";
    m_cursor = QCursor(Qt::OpenHandCursor);
}

MoveTool::~MoveTool()
{
}

Tool::ToolType MoveTool::getType() const
{
    return Tool::MoveTool;
}

QString MoveTool::getName() const
{
    return m_name;
}

QString MoveTool::getDescription() const
{
    return m_description;
}

void MoveTool::begin(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    if (button != Qt::LeftButton) return;
    
    m_isMoving = true;
    m_startPos = pos;
    m_lastPos = pos;
    m_totalOffset = QPoint(0, 0);
    
    // Auto-select layer if enabled
    if (m_autoSelectLayer && m_document) {
        // Find layer at position (simplified - would need proper layer hit testing)
        m_selectedLayerIndex = 0; // Default to first layer
    }
    
    LOG_TOOL_USAGE("Move", "BEGIN_MOVE", QString("Position: (%1,%2)").arg(pos.x()).arg(pos.y()));
}

void MoveTool::update(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    if (!m_isMoving || button != Qt::LeftButton) return;
    
    QPoint offset = pos - m_lastPos;
    m_totalOffset += offset;
    m_lastPos = pos;
    
    // Update cursor to closed hand while moving
    m_cursor = QCursor(Qt::ClosedHandCursor);
    
    // Emit move signal
    if (m_moveMode == MoveLayer) {
        emit layerMoved(m_selectedLayerIndex, offset);
    } else if (m_moveMode == MoveSelection) {
        emit selectionMoved(offset);
    }
    
    LOG_TOOL_USAGE("Move", "UPDATE_MOVE", QString("Offset: (%1,%2)").arg(offset.x()).arg(offset.y()));
}

void MoveTool::end(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    if (!m_isMoving || button != Qt::LeftButton) return;
    
    m_isMoving = false;
    
    // Reset cursor to open hand
    m_cursor = QCursor(Qt::OpenHandCursor);
    
    LOG_TOOL_USAGE("Move", "END_MOVE", QString("Total offset: (%1,%2)").arg(m_totalOffset.x()).arg(m_totalOffset.y()));
}

void MoveTool::cancel()
{
    m_isMoving = false;
    m_cursor = QCursor(Qt::OpenHandCursor);
    
    LOG_TOOL_USAGE("Move", "CANCEL_MOVE", "");
}

void MoveTool::setMoveMode(MoveMode mode)
{
    m_moveMode = mode;
    LOG_TOOL_PROPERTY("Move", "move_mode", QString::number(mode));
}

MoveTool::MoveMode MoveTool::getMoveMode() const
{
    return m_moveMode;
}

void MoveTool::setAutoSelectLayer(bool enabled)
{
    m_autoSelectLayer = enabled;
    LOG_TOOL_PROPERTY("Move", "auto_select_layer", enabled ? "true" : "false");
}

bool MoveTool::isAutoSelectLayer() const
{
    return m_autoSelectLayer;
}

void MoveTool::setShowTransformControls(bool enabled)
{
    m_showTransformControls = enabled;
    LOG_TOOL_PROPERTY("Move", "show_transform_controls", enabled ? "true" : "false");
}

bool MoveTool::isShowTransformControls() const
{
    return m_showTransformControls;
}

QVariant MoveTool::getProperty(const QString &name) const
{
    if (name == "move_mode") return static_cast<int>(m_moveMode);
    if (name == "auto_select_layer") return m_autoSelectLayer;
    if (name == "show_transform_controls") return m_showTransformControls;
    return QVariant();
}

void MoveTool::setProperty(const QString &name, const QVariant &value)
{
    if (name == "move_mode") {
        setMoveMode(static_cast<MoveMode>(value.toInt()));
    } else if (name == "auto_select_layer") {
        setAutoSelectLayer(value.toBool());
    } else if (name == "show_transform_controls") {
        setShowTransformControls(value.toBool());
    }
}

void MoveTool::drawPreview(QPainter &painter, const QPoint &point) const
{
    if (!m_isMoving) return;
    
    // Draw move preview
    painter.setPen(QPen(Qt::blue, 2, Qt::DashLine));
    painter.setBrush(QBrush(QColor(0, 0, 255, 50)));
    
    // Draw bounding box of moved area
    QRect previewRect = QRect(m_startPos, point).normalized();
    painter.drawRect(previewRect);
    
    // Draw arrow showing direction
    QPoint center = previewRect.center();
    painter.setPen(QPen(Qt::red, 3));
    painter.drawLine(center, center + m_totalOffset);
}
