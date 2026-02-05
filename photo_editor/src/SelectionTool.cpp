#include "SelectionTool.h"
#include "ImageDocument.h"
#include "CanvasWidget.h"
#include "Layer.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

SelectionTool::SelectionTool(QObject *parent)
    : Tool(Tool::SelectionTool, parent)
    , m_selectionMode(RectangleSelection)
    , m_selectionRect(0, 0, 0, 0)
    , m_isSelecting(false)
    , m_startPos(0, 0)
{
    m_name = "Selection Tool";
    m_description = "Select areas of the image";
    m_cursor = Qt::CrossCursor;
}

SelectionTool::~SelectionTool()
{
}

Tool::ToolType SelectionTool::getType() const
{
    return Tool::SelectionTool;
}

QString SelectionTool::getName() const
{
    return m_name;
}

QString SelectionTool::getDescription() const
{
    return m_description;
}

void SelectionTool::begin(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(button);
    Q_UNUSED(modifiers);
    
    if (!m_document) {
        return;
    }
    
    m_isSelecting = true;
    m_startPos = pos;
    m_selectionRect = QRect(pos, pos);
    
    // TODO: Implement selection beginning
    emit toolUsed();
}

void SelectionTool::update(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(button);
    Q_UNUSED(modifiers);
    
    if (!m_isSelecting || !m_document) {
        return;
    }
    
    // Update selection rectangle
    m_selectionRect = QRect(m_startPos, pos).normalized();
    
    // TODO: Implement selection update
    emit toolUsed();
}

void SelectionTool::end(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(pos);
    Q_UNUSED(button);
    Q_UNUSED(modifiers);
    
    m_isSelecting = false;
    
    // TODO: Implement selection ending
    emit toolUsed();
}

void SelectionTool::cancel()
{
    m_isSelecting = false;
    m_selectionRect = QRect(0, 0, 0, 0);
}

void SelectionTool::drawPreview(QPainter &painter, const QPoint &point) const
{
    Q_UNUSED(painter);
    Q_UNUSED(point);
    // TODO: Implement selection preview
}

void SelectionTool::setSelectionMode(SelectionMode mode)
{
    m_selectionMode = mode;
    emit selectionModeChanged(mode);
}

SelectionTool::SelectionMode SelectionTool::getSelectionMode() const
{
    return m_selectionMode;
}

QRect SelectionTool::getSelectionRect() const
{
    return m_selectionRect;
}

QList<QPoint> SelectionTool::getSelectionPoints() const
{
    return m_selectionPoints;
}

bool SelectionTool::hasSelection() const
{
    return !m_selectionRect.isEmpty();
}

void SelectionTool::clearSelection()
{
    m_selectionRect = QRect(0, 0, 0, 0);
    m_selectionPoints.clear();
    emit selectionChanged();
}

QVariant SelectionTool::getProperty(const QString &name) const
{
    if (name == "selectionMode") {
        return static_cast<int>(m_selectionMode);
    } else if (name == "hasSelection") {
        return hasSelection();
    }
    
    return Tool::getProperty(name);
}

void SelectionTool::setProperty(const QString &name, const QVariant &value)
{
    if (name == "selectionMode") {
        setSelectionMode(static_cast<SelectionMode>(value.toInt()));
    } else {
        Tool::setProperty(name, value);
    }
}