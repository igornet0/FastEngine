#ifndef MOVETOOL_H
#define MOVETOOL_H

#include "Tool.h"
#include <QPoint>
#include <QRect>

class MoveTool : public Tool
{
    Q_OBJECT
public:
    explicit MoveTool(QObject *parent = nullptr);
    ~MoveTool();

    ToolType getType() const override;
    QString getName() const override;
    QString getDescription() const override;

    void begin(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void update(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void end(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void cancel() override;

    // Move tool specific properties
    enum MoveMode {
        MoveLayer,          // Move entire layer
        MoveSelection,      // Move selected area
        MoveObject          // Move specific object
    };
    
    void setMoveMode(MoveMode mode);
    MoveMode getMoveMode() const;
    
    void setAutoSelectLayer(bool enabled);
    bool isAutoSelectLayer() const;
    
    void setShowTransformControls(bool enabled);
    bool isShowTransformControls() const;

    QVariant getProperty(const QString &name) const override;
    void setProperty(const QString &name, const QVariant &value) override;
    
    void drawPreview(QPainter &painter, const QPoint &point) const override;

signals:
    void layerMoved(int layerIndex, const QPoint &offset);
    void selectionMoved(const QPoint &offset);

private:
    MoveMode m_moveMode;
    bool m_autoSelectLayer;
    bool m_showTransformControls;
    
    bool m_isMoving = false;
    QPoint m_startPos;
    QPoint m_lastPos;
    QPoint m_totalOffset;
    int m_selectedLayerIndex;
};

#endif // MOVETOOL_H
