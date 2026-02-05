#ifndef SELECTIONTOOL_H
#define SELECTIONTOOL_H

#include "Tool.h"
#include <QList>
#include <QRect>

class SelectionTool : public Tool
{
    Q_OBJECT
public:
    enum SelectionMode {
        RectangleSelection,
        EllipseSelection,
        LassoSelection,
        MagicWandSelection
    };

    explicit SelectionTool(QObject *parent = nullptr);
    ~SelectionTool();

    ToolType getType() const override;
    QString getName() const override;
    QString getDescription() const override;

    void begin(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void update(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void end(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) override;
    void cancel() override;

    // Selection properties
    void setSelectionMode(SelectionMode mode);
    SelectionMode getSelectionMode() const;
    QRect getSelectionRect() const;
    QList<QPoint> getSelectionPoints() const;
    bool hasSelection() const;
    void clearSelection();

    QVariant getProperty(const QString &name) const override;
    void setProperty(const QString &name, const QVariant &value) override;
    
    void drawPreview(QPainter &painter, const QPoint &point) const override;

signals:
    void selectionChanged();
    void selectionModeChanged(SelectionMode mode);

private:
    SelectionMode m_selectionMode;
    QRect m_selectionRect;
    QList<QPoint> m_selectionPoints;
    bool m_isSelecting = false;
    QPoint m_startPos;
};

#endif // SELECTIONTOOL_H
