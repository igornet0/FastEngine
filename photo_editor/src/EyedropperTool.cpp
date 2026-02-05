#include "EyedropperTool.h"
#include "ImageDocument.h"
#include "CanvasWidget.h"
#include "Logger.h"
#include <QPainter>
#include <QDebug>

EyedropperTool::EyedropperTool(QObject *parent)
    : Tool(Tool::EyedropperTool, parent)
    , m_sampleSize(ThreeByThree)
    , m_sampleFromAllLayers(false)
    , m_showColorInfo(true)
{
    m_name = "Eyedropper Tool";
    m_description = "Sample colors from the image";
    m_cursor = QCursor(Qt::CrossCursor);
}

EyedropperTool::~EyedropperTool()
{
}

Tool::ToolType EyedropperTool::getType() const
{
    return Tool::EyedropperTool;
}

QString EyedropperTool::getName() const
{
    return m_name;
}

QString EyedropperTool::getDescription() const
{
    return m_description;
}

void EyedropperTool::begin(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    if (!m_document) return;
    
    QImage image = m_document->getImage();
    if (image.isNull()) return;
    
    // Sample color from image
    QColor sampledColor = sampleColor(image, pos);
    m_lastPickedColor = sampledColor;
    m_lastPickPosition = pos;
    
    // Set foreground or background color based on button
    if (button == Qt::LeftButton) {
        emit foregroundColorChanged(sampledColor);
    } else if (button == Qt::RightButton) {
        emit backgroundColorChanged(sampledColor);
    }
    
    emit colorPicked(sampledColor, pos);
    
    LOG_TOOL_USAGE("Eyedropper", "PICK_COLOR", 
        QString("Color: %1, Position: (%2,%3)")
        .arg(sampledColor.name())
        .arg(pos.x()).arg(pos.y()));
}

void EyedropperTool::update(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    if (!m_document) return;
    
    QImage image = m_document->getImage();
    if (image.isNull()) return;
    
    // Sample color for preview
    QColor sampledColor = sampleColor(image, pos);
    m_lastPickedColor = sampledColor;
    m_lastPickPosition = pos;
    
    emit colorPicked(sampledColor, pos);
}

void EyedropperTool::end(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    // Eyedropper is typically a click-and-pick tool
    LOG_TOOL_USAGE("Eyedropper", "END_PICK", "");
}

void EyedropperTool::cancel()
{
    LOG_TOOL_USAGE("Eyedropper", "CANCEL_PICK", "");
}

QColor EyedropperTool::sampleColor(const QImage &image, const QPoint &pos) const
{
    if (image.isNull() || !image.rect().contains(pos)) {
        return QColor();
    }
    
    int sampleSize = static_cast<int>(m_sampleSize);
    int halfSize = sampleSize / 2;
    
    QRect sampleRect(pos.x() - halfSize, pos.y() - halfSize, sampleSize, sampleSize);
    sampleRect = sampleRect.intersected(image.rect());
    
    if (sampleRect.isEmpty()) {
        return image.pixelColor(pos);
    }
    
    // Calculate average color
    int totalRed = 0, totalGreen = 0, totalBlue = 0, totalAlpha = 0;
    int pixelCount = 0;
    
    for (int y = sampleRect.top(); y <= sampleRect.bottom(); ++y) {
        for (int x = sampleRect.left(); x <= sampleRect.right(); ++x) {
            QColor pixelColor = image.pixelColor(x, y);
            totalRed += pixelColor.red();
            totalGreen += pixelColor.green();
            totalBlue += pixelColor.blue();
            totalAlpha += pixelColor.alpha();
            pixelCount++;
        }
    }
    
    if (pixelCount == 0) {
        return QColor();
    }
    
    return QColor(
        totalRed / pixelCount,
        totalGreen / pixelCount,
        totalBlue / pixelCount,
        totalAlpha / pixelCount
    );
}

void EyedropperTool::setSampleSize(SampleSize size)
{
    m_sampleSize = size;
    LOG_TOOL_PROPERTY("Eyedropper", "sample_size", QString::number(size));
}

EyedropperTool::SampleSize EyedropperTool::getSampleSize() const
{
    return m_sampleSize;
}

void EyedropperTool::setSampleFromAllLayers(bool enabled)
{
    m_sampleFromAllLayers = enabled;
    LOG_TOOL_PROPERTY("Eyedropper", "sample_from_all_layers", enabled ? "true" : "false");
}

bool EyedropperTool::isSampleFromAllLayers() const
{
    return m_sampleFromAllLayers;
}

void EyedropperTool::setShowColorInfo(bool enabled)
{
    m_showColorInfo = enabled;
    LOG_TOOL_PROPERTY("Eyedropper", "show_color_info", enabled ? "true" : "false");
}

bool EyedropperTool::isShowColorInfo() const
{
    return m_showColorInfo;
}

QVariant EyedropperTool::getProperty(const QString &name) const
{
    if (name == "sample_size") return static_cast<int>(m_sampleSize);
    if (name == "sample_from_all_layers") return m_sampleFromAllLayers;
    if (name == "show_color_info") return m_showColorInfo;
    return QVariant();
}

void EyedropperTool::setProperty(const QString &name, const QVariant &value)
{
    if (name == "sample_size") {
        setSampleSize(static_cast<SampleSize>(value.toInt()));
    } else if (name == "sample_from_all_layers") {
        setSampleFromAllLayers(value.toBool());
    } else if (name == "show_color_info") {
        setShowColorInfo(value.toBool());
    }
}

void EyedropperTool::drawPreview(QPainter &painter, const QPoint &point) const
{
    if (!m_showColorInfo || !m_lastPickedColor.isValid()) return;
    
    // Draw color preview
    QRect colorRect(point.x() - 20, point.y() - 20, 40, 40);
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(QBrush(m_lastPickedColor));
    painter.drawRect(colorRect);
    
    // Draw color info text
    painter.setPen(QPen(Qt::white));
    QString colorText = QString("RGB: %1,%2,%3")
        .arg(m_lastPickedColor.red())
        .arg(m_lastPickedColor.green())
        .arg(m_lastPickedColor.blue());
    
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    painter.drawText(point.x() + 25, point.y() - 10, colorText);
}
