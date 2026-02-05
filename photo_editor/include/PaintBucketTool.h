#ifndef PAINTBUCKETTOOL_H
#define PAINTBUCKETTOOL_H

#include "Tool.h"
#include <QColor>
#include <QStack>

class PaintBucketTool : public Tool
{
    Q_OBJECT
public:
    explicit PaintBucketTool(QObject *parent = nullptr);
    ~PaintBucketTool();

    ToolType getType() const override;
    QString getName() const override;
    QString getDescription() const override;

    void begin(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void update(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void end(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void cancel() override;

    // Paint bucket specific properties
    void setFillColor(const QColor &color);
    QColor getFillColor() const;
    
    void setTolerance(int tolerance);
    int getTolerance() const;
    
    void setContiguous(bool enabled);
    bool isContiguous() const;
    
    void setAntiAlias(bool enabled);
    bool isAntiAlias() const;
    
    void setFillMode(FillMode mode);
    FillMode getFillMode() const;

    QVariant getProperty(const QString &name) const override;
    void setProperty(const QString &name, const QVariant &value) override;
    
    void drawPreview(QPainter &painter, const QPoint &point) const override;

signals:
    void areaFilled(const QRect &area, const QColor &color);
    void fillPreviewChanged(const QPoint &position);

private:
    enum FillMode {
        ForegroundColor,      // Fill with foreground color
        BackgroundColor,      // Fill with background color
        Pattern,             // Fill with pattern
        ContentAware         // Content-aware fill
    };
    
    QColor m_fillColor;
    int m_tolerance;
    bool m_contiguous;
    bool m_antiAlias;
    FillMode m_fillMode;
    
    void floodFill(QImage &image, const QPoint &startPoint, const QColor &targetColor, const QColor &fillColor);
    bool colorsMatch(const QColor &color1, const QColor &color2, int tolerance) const;
};

#endif // PAINTBUCKETTOOL_H
