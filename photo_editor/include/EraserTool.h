#ifndef ERASERTOOL_H
#define ERASERTOOL_H

#include "Tool.h"

class EraserTool : public Tool
{
    Q_OBJECT
public:
    explicit EraserTool(QObject *parent = nullptr);
    ~EraserTool();

    ToolType getType() const override;
    QString getName() const override;
    QString getDescription() const override;

    void begin(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void update(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void end(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void cancel() override;

    // Brush properties
    void setBrushSize(int size) override;
    int getBrushSize() const;
    void setBrushOpacity(float opacity) override;
    float getBrushOpacity() const;
    void setBrushHardness(float hardness) override;
    float getBrushHardness() const;
    void setBrushSpacing(float spacing);
    float getBrushSpacing() const;
    void setBrushFlow(float flow);
    float getBrushFlow() const;
    void setPressureSensitive(bool enabled);
    bool isPressureSensitive() const;
    void setScattering(bool enabled);
    bool isScattering() const;
    void setScatterAmount(float amount);
    float getScatterAmount() const;
    void setRotationEnabled(bool enabled);
    bool isRotationEnabled() const;

    QVariant getProperty(const QString &name) const override;
    void setProperty(const QString &name, const QVariant &value) override;
    
    void drawPreview(QPainter &painter, const QPoint &point) const override;

signals:
    void brushChanged();

private:
    int m_brushSize;
    float m_brushOpacity;
    float m_brushHardness;
    float m_brushSpacing;
    float m_brushFlow;
    bool m_pressureSensitive;
    bool m_scattering;
    float m_scatterAmount;
    bool m_rotationEnabled;
    bool m_isErasing = false;
    QPoint m_lastPos;
};

#endif // ERASERTOOL_H
