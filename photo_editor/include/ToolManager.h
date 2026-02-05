#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include <QObject>
#include <QPoint>
#include <QColor>
#include <QPainter>
#include <QCursor>
#include <QMouseEvent>
#include <QImage>
#include <QPen>
#include <QBrush>

class ToolManager : public QObject
{
public:
    enum ToolType {
        MoveTool = 0,
        MarqueeTool = 1,
        LassoTool = 2,
        MagicWandTool = 3,
        SelectTool = 4,
        BrushTool = 5,
        PencilTool = 6,
        CloneStampTool = 7,
        HealingBrushTool = 8,
        EraserTool = 9,
        GradientTool = 10,
        PaintBucketTool = 11,
        CropTool = 12,
        EyedropperTool = 13,
        HandTool = 14,
        ZoomTool = 15,
        TextTool = 16,
        PenTool = 17,
        ShapeTool = 18
    };

    explicit ToolManager(QObject *parent = nullptr);
    ~ToolManager();

    // Tool management
    void setCurrentTool(ToolType tool);
    ToolType getCurrentTool() const { return m_currentTool; }
    QCursor getCurrentCursor() const;
    
    // Tool properties
    void setBrushSize(int size);
    int getBrushSize() const { return m_brushSize; }
    
    void setBrushColor(const QColor &color);
    QColor getBrushColor() const { return m_brushColor; }
    
    void setBrushOpacity(int opacity);
    int getBrushOpacity() const { return m_brushOpacity; }
    
    void setBrushHardness(int hardness);
    int getBrushHardness() const { return m_brushHardness; }
    
    // Drawing operations
    void beginDrawing(const QPoint &point, Qt::MouseButton button);
    void updateDrawing(const QPoint &point, Qt::MouseButton button);
    void endDrawing(const QPoint &point, Qt::MouseButton button);
    
    // Image operations
    void setImage(QImage *image);
    QImage* getImage() const { return m_image; }
    
    // Canvas widget connection
    void setCanvasWidget(QWidget *canvasWidget) { m_canvasWidget = canvasWidget; }
    
    // Tool-specific operations
    void drawBrushStroke(const QPoint &from, const QPoint &to);
    void drawEraserStroke(const QPoint &from, const QPoint &to);
    void drawSelectionRect(const QPoint &start, const QPoint &end);
    void drawCropRect(const QPoint &start, const QPoint &end);
    
    // Preview
    void drawToolPreview(QPainter &painter, const QPoint &point);
    
    // Selection and crop info
    bool hasSelection() const { return m_hasSelection; }
    QRect getSelectionRect() const { return m_selectionRect; }
    bool isCropping() const { return m_isCropping; }
    QRect getCropRect() const { return m_cropRect; }
    bool isSelecting() const { return m_isSelecting; }
    
    // Selection management
    void clearSelection();

private:
    QString getToolName(ToolType tool) const;
    void createCustomCursors();
    QCursor createBrushCursor(int size);
    QCursor createEraserCursor(int size);
    QCursor createCropCursor();
    
    ToolType m_currentTool;
    QImage *m_image;
    QWidget *m_canvasWidget;
    
    // Brush properties
    int m_brushSize;
    QColor m_brushColor;
    int m_brushOpacity;
    int m_brushHardness;
    
    // Drawing state
    bool m_isDrawing;
    QPoint m_lastPoint;
    QPoint m_startPoint;
    Qt::MouseButton m_currentButton;
    
    // Custom cursors
    QCursor m_selectCursor;
    QCursor m_brushCursor;
    QCursor m_eraserCursor;
    QCursor m_cropCursor;
    QCursor m_textCursor;
    QCursor m_shapeCursor;
    
    // Selection
    QRect m_selectionRect;
    bool m_hasSelection;
    bool m_isSelecting;
    
    // Crop
    QRect m_cropRect;
    bool m_isCropping;
};

#endif // TOOLMANAGER_H
