#ifndef EYEDROPPERTOOL_H
#define EYEDROPPERTOOL_H

#include "Tool.h"
#include <QColor>

class EyedropperTool : public Tool
{
    Q_OBJECT
public:
    explicit EyedropperTool(QObject *parent = nullptr);
    ~EyedropperTool();

    ToolType getType() const override;
    QString getName() const override;
    QString getDescription() const override;

    void begin(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void update(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void end(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void cancel() override;

    // Eyedropper specific properties
    enum SampleSize {
        PointSample = 1,        // Single pixel
        ThreeByThree = 3,       // 3x3 average
        FiveByFive = 5,         // 5x5 average
        ElevenByEleven = 11,    // 11x11 average
        ThirtyOneByThirtyOne = 31, // 31x31 average
        FiftyOneByFiftyOne = 51  // 51x51 average
    };
    
    void setSampleSize(SampleSize size);
    SampleSize getSampleSize() const;
    
    void setSampleFromAllLayers(bool enabled);
    bool isSampleFromAllLayers() const;
    
    void setShowColorInfo(bool enabled);
    bool isShowColorInfo() const;

    QVariant getProperty(const QString &name) const override;
    void setProperty(const QString &name, const QVariant &value) override;
    
    void drawPreview(QPainter &painter, const QPoint &point) const override;

signals:
    void colorPicked(const QColor &color, const QPoint &position);
    void foregroundColorChanged(const QColor &color);
    void backgroundColorChanged(const QColor &color);

private:
    SampleSize m_sampleSize;
    bool m_sampleFromAllLayers;
    bool m_showColorInfo;
    
    QColor m_lastPickedColor;
    QPoint m_lastPickPosition;
    
    QColor sampleColor(const QImage &image, const QPoint &pos) const;
};

#endif // EYEDROPPERTOOL_H
