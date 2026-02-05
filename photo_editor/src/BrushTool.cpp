#include "BrushTool.h"
#include "ImageDocument.h"
#include "CanvasWidget.h"
#include "Layer.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

BrushTool::BrushTool(QObject *parent)
    : Tool(Tool::BrushTool, parent)
    , m_brushSize(10)
    , m_brushColor(Qt::black)
    , m_brushOpacity(1.0f)
    , m_brushHardness(1.0f)
    , m_brushSpacing(0.25f)
    , m_brushFlow(1.0f)
    , m_pressureSensitive(false)
    , m_scattering(false)
    , m_scatterAmount(0.0f)
    , m_rotationEnabled(false)
    , m_isDrawing(false)
    , m_lastPos(0, 0)
{
    m_name = "Brush Tool";
    m_description = "Paint with brush";
    m_cursor = Qt::CrossCursor;
}

BrushTool::~BrushTool()
{
}

Tool::ToolType BrushTool::getType() const
{
    return Tool::BrushTool;
}

QString BrushTool::getName() const
{
    return m_name;
}

QString BrushTool::getDescription() const
{
    return m_description;
}

void BrushTool::begin(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(button);
    Q_UNUSED(modifiers);
    
    if (!m_document) {
        return;
    }
    
    m_isDrawing = true;
    m_lastPos = pos;
    
    // TODO: Implement brush stroke beginning
    emit toolUsed();
}

void BrushTool::update(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(button);
    Q_UNUSED(modifiers);
    
    if (!m_isDrawing || !m_document) {
        return;
    }
    
    // TODO: Implement brush stroke update
    m_lastPos = pos;
    emit toolUsed();
}

void BrushTool::end(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(pos);
    Q_UNUSED(button);
    Q_UNUSED(modifiers);
    
    m_isDrawing = false;
    
    // TODO: Implement brush stroke ending
    emit toolUsed();
}

void BrushTool::cancel()
{
    m_isDrawing = false;
}

void BrushTool::drawPreview(QPainter &painter, const QPoint &point) const
{
    Q_UNUSED(painter);
    Q_UNUSED(point);
    // TODO: Implement brush preview
}

// Brush properties implementations
void BrushTool::setBrushSize(int size)
{
    m_brushSize = qBound(1, size, 1000);
    emit brushChanged();
}

void BrushTool::setBrushColor(const QColor &color)
{
    m_brushColor = color;
    emit brushChanged();
}

void BrushTool::setBrushOpacity(float opacity)
{
    m_brushOpacity = qBound(0.0f, opacity, 1.0f);
    emit brushChanged();
}

void BrushTool::setBrushHardness(float hardness)
{
    m_brushHardness = qBound(0.0f, hardness, 1.0f);
    emit brushChanged();
}

void BrushTool::setBrushSpacing(float spacing)
{
    m_brushSpacing = qBound(0.1f, spacing, 5.0f);
    emit brushChanged();
}

void BrushTool::setBrushFlow(float flow)
{
    m_brushFlow = qBound(0.0f, flow, 1.0f);
    emit brushChanged();
}

void BrushTool::setPressureSensitive(bool enabled)
{
    m_pressureSensitive = enabled;
    emit brushChanged();
}

void BrushTool::setScattering(bool enabled)
{
    m_scattering = enabled;
    emit brushChanged();
}

void BrushTool::setScatterAmount(float amount)
{
    m_scatterAmount = qBound(0.0f, amount, 1.0f);
    emit brushChanged();
}

void BrushTool::setRotationEnabled(bool enabled)
{
    m_rotationEnabled = enabled;
    emit brushChanged();
}

int BrushTool::getBrushSize() const
{
    return m_brushSize;
}

QColor BrushTool::getBrushColor() const
{
    return m_brushColor;
}

float BrushTool::getBrushOpacity() const
{
    return m_brushOpacity;
}

float BrushTool::getBrushHardness() const
{
    return m_brushHardness;
}

float BrushTool::getBrushSpacing() const
{
    return m_brushSpacing;
}

float BrushTool::getBrushFlow() const
{
    return m_brushFlow;
}

bool BrushTool::isPressureSensitive() const
{
    return m_pressureSensitive;
}

bool BrushTool::isScattering() const
{
    return m_scattering;
}

float BrushTool::getScatterAmount() const
{
    return m_scatterAmount;
}

bool BrushTool::isRotationEnabled() const
{
    return m_rotationEnabled;
}

QVariant BrushTool::getProperty(const QString &name) const
{
    if (name == "brushSize") {
        return m_brushSize;
    } else if (name == "brushColor") {
        return m_brushColor;
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

void BrushTool::setProperty(const QString &name, const QVariant &value)
{
    if (name == "brushSize") {
        setBrushSize(value.toInt());
    } else if (name == "brushColor") {
        setBrushColor(value.value<QColor>());
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