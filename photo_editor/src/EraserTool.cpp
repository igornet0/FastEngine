#include "EraserTool.h"
#include "ImageDocument.h"
#include "CanvasWidget.h"
#include "Layer.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

EraserTool::EraserTool(QObject *parent)
    : Tool(Tool::EraserTool, parent)
    , m_brushSize(10)
    , m_brushOpacity(1.0f)
    , m_brushHardness(1.0f)
    , m_brushSpacing(0.25f)
    , m_brushFlow(1.0f)
    , m_pressureSensitive(false)
    , m_scattering(false)
    , m_scatterAmount(0.0f)
    , m_rotationEnabled(false)
    , m_isErasing(false)
    , m_lastPos(0, 0)
{
    m_name = "Eraser Tool";
    m_description = "Erase pixels";
    m_cursor = Qt::CrossCursor;
}

EraserTool::~EraserTool()
{
}

Tool::ToolType EraserTool::getType() const
{
    return Tool::EraserTool;
}

QString EraserTool::getName() const
{
    return m_name;
}

QString EraserTool::getDescription() const
{
    return m_description;
}

void EraserTool::begin(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(button);
    Q_UNUSED(modifiers);
    
    if (!m_document) {
        return;
    }
    
    m_isErasing = true;
    m_lastPos = pos;
    
    // TODO: Implement eraser stroke beginning
    emit toolUsed();
}

void EraserTool::update(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(button);
    Q_UNUSED(modifiers);
    
    if (!m_isErasing || !m_document) {
        return;
    }
    
    // TODO: Implement eraser stroke update
    m_lastPos = pos;
    emit toolUsed();
}

void EraserTool::end(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(pos);
    Q_UNUSED(button);
    Q_UNUSED(modifiers);
    
    m_isErasing = false;
    
    // TODO: Implement eraser stroke ending
    emit toolUsed();
}

void EraserTool::cancel()
{
    m_isErasing = false;
}

void EraserTool::drawPreview(QPainter &painter, const QPoint &point) const
{
    Q_UNUSED(painter);
    Q_UNUSED(point);
    // TODO: Implement eraser preview
}

// Brush properties implementations
void EraserTool::setBrushSize(int size)
{
    m_brushSize = qBound(1, size, 1000);
    emit brushChanged();
}

int EraserTool::getBrushSize() const
{
    return m_brushSize;
}

void EraserTool::setBrushOpacity(float opacity)
{
    m_brushOpacity = qBound(0.0f, opacity, 1.0f);
    emit brushChanged();
}

float EraserTool::getBrushOpacity() const
{
    return m_brushOpacity;
}

void EraserTool::setBrushHardness(float hardness)
{
    m_brushHardness = qBound(0.0f, hardness, 1.0f);
    emit brushChanged();
}

float EraserTool::getBrushHardness() const
{
    return m_brushHardness;
}

void EraserTool::setBrushSpacing(float spacing)
{
    m_brushSpacing = qBound(0.1f, spacing, 5.0f);
    emit brushChanged();
}

float EraserTool::getBrushSpacing() const
{
    return m_brushSpacing;
}

void EraserTool::setBrushFlow(float flow)
{
    m_brushFlow = qBound(0.0f, flow, 1.0f);
    emit brushChanged();
}

float EraserTool::getBrushFlow() const
{
    return m_brushFlow;
}

void EraserTool::setPressureSensitive(bool enabled)
{
    m_pressureSensitive = enabled;
    emit brushChanged();
}

bool EraserTool::isPressureSensitive() const
{
    return m_pressureSensitive;
}

void EraserTool::setScattering(bool enabled)
{
    m_scattering = enabled;
    emit brushChanged();
}

bool EraserTool::isScattering() const
{
    return m_scattering;
}

void EraserTool::setScatterAmount(float amount)
{
    m_scatterAmount = qBound(0.0f, amount, 1.0f);
    emit brushChanged();
}

float EraserTool::getScatterAmount() const
{
    return m_scatterAmount;
}

void EraserTool::setRotationEnabled(bool enabled)
{
    m_rotationEnabled = enabled;
    emit brushChanged();
}

bool EraserTool::isRotationEnabled() const
{
    return m_rotationEnabled;
}

int EraserTool::getBrushSize() const
{
    return m_brushSize;
}

float EraserTool::getBrushOpacity() const
{
    return m_brushOpacity;
}

float EraserTool::getBrushHardness() const
{
    return m_brushHardness;
}

float EraserTool::getBrushSpacing() const
{
    return m_brushSpacing;
}

float EraserTool::getBrushFlow() const
{
    return m_brushFlow;
}

bool EraserTool::isPressureSensitive() const
{
    return m_pressureSensitive;
}

bool EraserTool::isScattering() const
{
    return m_scattering;
}

float EraserTool::getScatterAmount() const
{
    return m_scatterAmount;
}

bool EraserTool::isRotationEnabled() const
{
    return m_rotationEnabled;
}

QVariant EraserTool::getProperty(const QString &name) const
{
    if (name == "brushSize") {
        return m_brushSize;
    } else if (name == "brushOpacity") {
        return m_brushOpacity;
    } else if (name == "brushHardness") {
        return m_brushHardness;
    } else if (name == "brushSpacing") {
        return m_brushSpacing;
    } else if (name == "brushFlow") {
        return m_brushFlow;
    } else if (name == "pressureSensitive") {
        return m_pressureSensitive;
    } else if (name == "scattering") {
        return m_scattering;
    } else if (name == "scatterAmount") {
        return m_scatterAmount;
    } else if (name == "rotationEnabled") {
        return m_rotationEnabled;
    }
    
    return Tool::getProperty(name);
}

void EraserTool::setProperty(const QString &name, const QVariant &value)
{
    if (name == "brushSize") {
        setBrushSize(value.toInt());
    } else if (name == "brushOpacity") {
        setBrushOpacity(value.toFloat());
    } else if (name == "brushHardness") {
        setBrushHardness(value.toFloat());
    } else if (name == "brushSpacing") {
        setBrushSpacing(value.toFloat());
    } else if (name == "brushFlow") {
        setBrushFlow(value.toFloat());
    } else if (name == "pressureSensitive") {
        setPressureSensitive(value.toBool());
    } else if (name == "scattering") {
        setScattering(value.toBool());
    } else if (name == "scatterAmount") {
        setScatterAmount(value.toFloat());
    } else if (name == "rotationEnabled") {
        setRotationEnabled(value.toBool());
    } else {
        Tool::setProperty(name, value);
    }
}