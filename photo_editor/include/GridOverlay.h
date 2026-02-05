#ifndef GRIDOVERLAY_H
#define GRIDOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QPoint>
#include <QSize>
#include <QList>
#include <QJsonObject>

enum class GridStyle {
    Lines,
    Dots,
    Crosses
};

enum class RulerUnit {
    Pixels,
    Inches,
    Centimeters,
    Millimeters
};

class GridOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit GridOverlay(QWidget *parent = nullptr);
    ~GridOverlay();

    // Grid properties
    void setGridVisible(bool visible);
    bool isGridVisible() const { return m_gridVisible; }

    void setGridSize(int size);
    int getGridSize() const { return m_gridSize; }

    void setGridColor(const QColor &color);
    QColor getGridColor() const { return m_gridColor; }

    void setGridOpacity(float opacity);
    float getGridOpacity() const { return m_gridOpacity; }

    void setGridStyle(GridStyle style);
    GridStyle getGridStyle() const { return m_gridStyle; }

    // Snap to grid
    void setSnapToGrid(bool enabled);
    bool isSnapToGrid() const { return m_snapToGrid; }

    QPoint snapToGrid(const QPoint &point) const;
    QRect snapToGrid(const QRect &rect) const;

    // Guides
    void addHorizontalGuide(int y);
    void addVerticalGuide(int x);
    void removeHorizontalGuide(int y);
    void removeVerticalGuide(int x);
    void clearGuides();
    void clearHorizontalGuides();
    void clearVerticalGuides();

    QList<int> getHorizontalGuides() const { return m_horizontalGuides; }
    QList<int> getVerticalGuides() const { return m_verticalGuides; }

    void setGuidesVisible(bool visible);
    bool areGuidesVisible() const { return m_guidesVisible; }

    void setGuideColor(const QColor &color);
    QColor getGuideColor() const { return m_guideColor; }

    // Snap to guides
    void setSnapToGuides(bool enabled);
    bool isSnapToGuides() const { return m_snapToGuides; }

    QPoint snapToGuides(const QPoint &point) const;
    QRect snapToGuides(const QRect &rect) const;

    // Rulers
    void setRulersVisible(bool visible);
    bool areRulersVisible() const { return m_rulersVisible; }

    void setRulerUnit(RulerUnit unit);
    RulerUnit getRulerUnit() const { return m_rulerUnit; }

    void setRulerColor(const QColor &color);
    QColor getRulerColor() const { return m_rulerColor; }

    // Serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

signals:
    void gridChanged();
    void guidesChanged();
    void rulersChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void drawGrid(QPainter &painter);
    void drawGuides(QPainter &painter);
    void drawRulers(QPainter &painter);
    void drawHorizontalRuler(QPainter &painter);
    void drawVerticalRuler(QPainter &painter);
    
    QString formatRulerValue(int value) const;
    int getRulerHeight() const { return 20; }
    int getRulerWidth() const { return 20; }

    // Grid properties
    bool m_gridVisible;
    int m_gridSize;
    QColor m_gridColor;
    float m_gridOpacity;
    GridStyle m_gridStyle;
    bool m_snapToGrid;

    // Guides properties
    QList<int> m_horizontalGuides;
    QList<int> m_verticalGuides;
    bool m_guidesVisible;
    QColor m_guideColor;
    bool m_snapToGuides;

    // Rulers properties
    bool m_rulersVisible;
    RulerUnit m_rulerUnit;
    QColor m_rulerColor;

    // Interaction
    bool m_draggingGuide;
    bool m_draggingHorizontal;
    int m_dragOffset;
};

#endif // GRIDOVERLAY_H
