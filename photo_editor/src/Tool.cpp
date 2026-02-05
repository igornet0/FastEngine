#include "Tool.h"
#include "ImageDocument.h"
#include "CanvasWidget.h"
#include <QDebug>

Tool::Tool(ToolType type, QObject *parent)
    : QObject(parent)
    , m_type(type)
    , m_active(false)
    , m_brushSize(10)
    , m_brushColor(Qt::black)
    , m_brushOpacity(1.0f)
    , m_brushHardness(0.5f)
    , m_document(nullptr)
    , m_canvas(nullptr)
    , m_drawing(false)
{
}

Tool::~Tool()
{
}

void Tool::setActive(bool active)
{
    m_active = active;
    emit toolChanged();
}

void Tool::cancel()
{
    // Default implementation - override in derived classes
}

void Tool::setBrushSize(int size)
{
    Q_UNUSED(size)
    // Default implementation - override in derived classes
}

void Tool::setBrushColor(const QColor &color)
{
    Q_UNUSED(color)
    // Default implementation - override in derived classes
}

void Tool::setBrushOpacity(float opacity)
{
    Q_UNUSED(opacity)
    // Default implementation - override in derived classes
}

void Tool::setBrushHardness(float hardness)
{
    Q_UNUSED(hardness)
    // Default implementation - override in derived classes
}

QVariant Tool::getProperty(const QString &name) const
{
    Q_UNUSED(name)
    return QVariant();
}

void Tool::setProperty(const QString &name, const QVariant &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)
    // Default implementation - override in derived classes
}

void Tool::drawPreview(QPainter &painter, const QPoint &point) const
{
    Q_UNUSED(painter)
    Q_UNUSED(point)
    // Default implementation - override in derived classes
}

void Tool::setDocument(ImageDocument *document)
{
    m_document = document;
}

void Tool::setCanvas(CanvasWidget *canvas)
{
    m_canvas = canvas;
}