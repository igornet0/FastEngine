#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QPoint>
#include <QColor>
#include <QPainter>
#include <QCursor>

class ImageDocument;
class CanvasWidget;

class Tool : public QObject
{
    Q_OBJECT

public:
    enum ToolType {
        MoveTool,
        MarqueeTool,
        LassoTool,
        MagicWandTool,
        SelectionTool,
        BrushTool,
        PencilTool,
        CloneStampTool,
        HealingBrushTool,
        EraserTool,
        GradientTool,
        PaintBucketTool,
        CropTool,
        EyedropperTool,
        HandTool,
        ZoomTool,
        TextTool,
        PenTool,
        ShapeTool
    };

    explicit Tool(ToolType type, QObject *parent = nullptr);
    virtual ~Tool();

    // Tool properties
    virtual ToolType getType() const { return m_type; }
    virtual QString getName() const { return m_name; }
    virtual QString getDescription() const { return m_description; }
    QCursor getCursor() const { return m_cursor; }
    
    // Tool state
    bool isActive() const { return m_active; }
    void setActive(bool active);
    
    // Tool operations
    virtual void begin(const QPoint &point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) = 0;
    virtual void update(const QPoint &point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) = 0;
    virtual void end(const QPoint &point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) = 0;
    
    virtual void cancel();
    
    // Tool settings
    virtual void setBrushSize(int size);
    virtual void setBrushColor(const QColor &color);
    virtual void setBrushOpacity(float opacity);
    virtual void setBrushHardness(float hardness);
    
    // Tool-specific properties
    virtual QVariant getProperty(const QString &name) const;
    virtual void setProperty(const QString &name, const QVariant &value);
    
    // Drawing
    virtual void drawPreview(QPainter &painter, const QPoint &point) const;

signals:
    void toolChanged();
    void toolUsed();

protected:
    void setDocument(ImageDocument *document);
    void setCanvas(CanvasWidget *canvas);
    
    ImageDocument* getDocument() const { return m_document; }
    CanvasWidget* getCanvas() const { return m_canvas; }
    
    ToolType m_type;
    QString m_name;
    QString m_description;
    QCursor m_cursor;
    bool m_active;
    
    // Tool settings
    int m_brushSize;
    QColor m_brushColor;
    float m_brushOpacity;
    float m_brushHardness;
    
    // References
    ImageDocument *m_document;
    CanvasWidget *m_canvas;
    
    // Tool state
    bool m_drawing;
    QPoint m_lastPoint;
    QPoint m_startPoint;
};

// Forward declarations for tool classes
class BrushTool;
class EraserTool;
class SelectionTool;

#endif // TOOL_H
