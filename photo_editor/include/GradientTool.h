#ifndef GRADIENTTOOL_H
#define GRADIENTTOOL_H

#include "Tool.h"
#include <QLinearGradient>
#include <QRadialGradient>
#include <QConicalGradient>

class GradientTool : public Tool
{
    Q_OBJECT
public:
    explicit GradientTool(QObject *parent = nullptr);
    ~GradientTool();

    ToolType getType() const override;
    QString getName() const override;
    QString getDescription() const override;

    void begin(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void update(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void end(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void cancel() override;

    // Gradient specific properties
    void setGradientType(GradientType type);
    GradientType getGradientType() const;
    
    void setGradientColors(const QList<QColor> &colors);
    QList<QColor> getGradientColors() const;
    
    void setGradientStops(const QList<qreal> &stops);
    QList<qreal> getGradientStops() const;
    
    void setOpacity(float opacity);
    float getOpacity() const;
    
    void setBlendMode(BlendMode mode);
    BlendMode getBlendMode() const;

    QVariant getProperty(const QString &name) const override;
    void setProperty(const QString &name, const QVariant &value) override;
    
    void drawPreview(QPainter &painter, const QPoint &point) const override;

signals:
    void gradientApplied(const QRect &area, GradientType type);
    void gradientPreviewChanged(const QPoint &start, const QPoint &end);

private:
    enum GradientType {
        LinearGradient,         // Linear gradient
        RadialGradient,        // Radial gradient
        ConicalGradient,       // Conical gradient
        ReflectedGradient,     // Reflected gradient
        DiamondGradient        // Diamond gradient
    };
    
    enum BlendMode {
        Normal,
        Multiply,
        Screen,
        Overlay,
        SoftLight,
        HardLight,
        ColorDodge,
        ColorBurn,
        Darken,
        Lighten,
        Difference,
        Exclusion
    };
    
    GradientType m_gradientType;
    QList<QColor> m_gradientColors;
    QList<qreal> m_gradientStops;
    float m_opacity;
    BlendMode m_blendMode;
    
    bool m_isDrawing = false;
    QPoint m_startPos;
    QPoint m_endPos;
    
    QGradient* createGradient(const QPoint &start, const QPoint &end) const;
};

#endif // GRADIENTTOOL_H
