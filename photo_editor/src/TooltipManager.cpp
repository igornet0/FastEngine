#include "TooltipManager.h"
#include "ToolManager.h"
#include <QApplication>
#include <QDebug>

TooltipManager::TooltipManager(QObject *parent)
    : QObject(parent)
{
    setupToolMappings();
    registerToolTooltips();
}

TooltipManager::~TooltipManager()
{
}

void TooltipManager::registerTooltip(QWidget *widget, const QString &tooltip)
{
    if (!widget) return;
    
    m_tooltips[widget] = tooltip;
    widget->setToolTip(tooltip);
}

void TooltipManager::registerTooltipWithHotkey(QWidget *widget, const QString &tooltip, const QString &hotkey)
{
    if (!widget) return;
    
    QString fullTooltip = QString("%1\nHotkey: %2").arg(tooltip, hotkey);
    
    m_tooltips[widget] = tooltip;
    m_hotkeys[widget] = hotkey;
    widget->setToolTip(fullTooltip);
}

void TooltipManager::registerToolTooltips()
{
    // This will be called by the main window to register tooltips for tool buttons
}

void TooltipManager::registerMenuTooltips()
{
    // Register tooltips for menu items
}

void TooltipManager::registerButtonTooltips()
{
    // Register tooltips for buttons
}

void TooltipManager::showTooltip(QWidget *widget, const QString &text, const QString &hotkey)
{
    QString fullText = text;
    if (!hotkey.isEmpty()) {
        fullText += QString("\nHotkey: %1").arg(hotkey);
    }
    
    QToolTip::showText(widget->mapToGlobal(QPoint(0, 0)), fullText, widget);
}

QString TooltipManager::getToolTooltip(int toolType) const
{
    return m_toolTooltips.value(toolType, "Unknown Tool");
}

QString TooltipManager::getToolHotkey(int toolType) const
{
    return m_toolHotkeys.value(toolType, "");
}

void TooltipManager::setupToolMappings()
{
    // Tool tooltips and hotkeys matching Photoshop
    m_toolTooltips[ToolManager::MoveTool] = "Move Tool\nMove and transform layers or selections";
    m_toolHotkeys[ToolManager::MoveTool] = "V";
    
    m_toolTooltips[ToolManager::MarqueeTool] = "Marquee Tool\nCreate rectangular or elliptical selections";
    m_toolHotkeys[ToolManager::MarqueeTool] = "M";
    
    m_toolTooltips[ToolManager::LassoTool] = "Lasso Tool\nCreate freehand selections";
    m_toolHotkeys[ToolManager::LassoTool] = "L";
    
    m_toolTooltips[ToolManager::MagicWandTool] = "Magic Wand Tool\nSelect areas of similar color";
    m_toolHotkeys[ToolManager::MagicWandTool] = "W";
    
    m_toolTooltips[ToolManager::SelectTool] = "Selection Tool\nBasic selection tool";
    m_toolHotkeys[ToolManager::SelectTool] = "";
    
    m_toolTooltips[ToolManager::BrushTool] = "Brush Tool\nPaint with customizable brush";
    m_toolHotkeys[ToolManager::BrushTool] = "B";
    
    m_toolTooltips[ToolManager::PencilTool] = "Pencil Tool\nDraw with hard-edged brush";
    m_toolHotkeys[ToolManager::PencilTool] = "B";
    
    m_toolTooltips[ToolManager::CloneStampTool] = "Clone Stamp Tool\nClone parts of the image";
    m_toolHotkeys[ToolManager::CloneStampTool] = "S";
    
    m_toolTooltips[ToolManager::HealingBrushTool] = "Healing Brush Tool\nRemove imperfections";
    m_toolHotkeys[ToolManager::HealingBrushTool] = "J";
    
    m_toolTooltips[ToolManager::EraserTool] = "Eraser Tool\nErase pixels from the image";
    m_toolHotkeys[ToolManager::EraserTool] = "E";
    
    m_toolTooltips[ToolManager::GradientTool] = "Gradient Tool\nCreate color gradients";
    m_toolHotkeys[ToolManager::GradientTool] = "G";
    
    m_toolTooltips[ToolManager::PaintBucketTool] = "Paint Bucket Tool\nFill areas with color";
    m_toolHotkeys[ToolManager::PaintBucketTool] = "G";
    
    m_toolTooltips[ToolManager::CropTool] = "Crop Tool\nCrop and resize the image";
    m_toolHotkeys[ToolManager::CropTool] = "C";
    
    m_toolTooltips[ToolManager::EyedropperTool] = "Eyedropper Tool\nSample colors from the image";
    m_toolHotkeys[ToolManager::EyedropperTool] = "I";
    
    m_toolTooltips[ToolManager::HandTool] = "Hand Tool\nPan around the canvas";
    m_toolHotkeys[ToolManager::HandTool] = "H";
    
    m_toolTooltips[ToolManager::ZoomTool] = "Zoom Tool\nZoom in and out of the image";
    m_toolHotkeys[ToolManager::ZoomTool] = "Z";
    
    m_toolTooltips[ToolManager::TextTool] = "Text Tool\nAdd and edit text";
    m_toolHotkeys[ToolManager::TextTool] = "T";
    
    m_toolTooltips[ToolManager::PenTool] = "Pen Tool\nCreate vector paths";
    m_toolHotkeys[ToolManager::PenTool] = "P";
    
    m_toolTooltips[ToolManager::ShapeTool] = "Shape Tool\nDraw geometric shapes";
    m_toolHotkeys[ToolManager::ShapeTool] = "U";
}
