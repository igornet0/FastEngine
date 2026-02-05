#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QScrollBar>
#include <QTransform>
#include <QPointF>
#include <QVector2D>
#include <QMatrix4x4>
#include <memory>
#include <vector>

class FastEngineIntegration;

class SceneView2D : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SceneView2D(QWidget *parent = nullptr);
    ~SceneView2D();

    // Управление сценой
    void setGridVisible(bool visible);
    void setGridSize(float size);
    void setSnapToGrid(bool enabled);
    
    // Управление камерой
    void resetCamera();
    void setCameraPosition(const QPointF &position);
    void setCameraZoom(float zoom);
    
    // Управление объектами
    void addEntity(const QString &name, const QPointF &position, const QSizeF &size);
    void removeEntity(const QString &name);
    void selectEntity(const QString &name);
    void clearSelection();
    
    // Получение информации
    QStringList getSelectedEntities() const;
    QPointF getCameraPosition() const;
    float getCameraZoom() const;
    bool isGridVisible() const;
    float getGridSize() const;
    bool isSnapToGrid() const;

signals:
    void selectionChanged(const QStringList &selectedEntities);
    void entityCreated(const QString &name, const QPointF &position);
    void entityMoved(const QString &name, const QPointF &position);
    void entityDeleted(const QString &name);
    void assetSelected(const QString &assetPath);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

public slots:
    void onAssetSelected(const QString &assetPath);

private slots:
    void updateScene();
    void onEntityMoved();

private:
    void setupScene();
    void setupConnections();
    void updateGrid();
    void updateCamera();
    
    QPointF screenToWorld(const QPoint &screenPos) const;
    QPoint worldToScreen(const QPointF &worldPos) const;
    QPointF snapToGrid(const QPointF &position) const;
    
    void createGrid();
    void createTestEntities();
    
    // Сцена и объекты
    QGraphicsScene *m_scene;
    QGraphicsItem *m_gridItem;
    QGraphicsItem *m_selectionBox;
    
    // Состояние
    bool m_gridVisible;
    float m_gridSize;
    bool m_snapToGrid;
    bool m_dragging;
    bool m_cameraDragging;
    QPoint m_lastMousePos;
    QPointF m_cameraPosition;
    float m_cameraZoom;
    
    // Выбранные объекты
    QStringList m_selectedEntities;
    QGraphicsItem *m_draggedItem;
    QPointF m_dragStartPos;
    
    // Таймер обновления
    QTimer *m_updateTimer;
    
    // Интеграция с FastEngine
    std::unique_ptr<FastEngineIntegration> m_fastEngineIntegration;
    
    // Стили
    QPen m_gridPen;
    QPen m_selectionPen;
    QBrush m_selectionBrush;
};

// Класс для объектов сцены
class SceneEntity2D : public QGraphicsItem
{
public:
    SceneEntity2D(const QString &name, const QSizeF &size, QGraphicsItem *parent = nullptr);
    
    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    // Свойства
    QString getName() const { return m_name; }
    void setName(const QString &name) { m_name = name; }
    
    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }
    
    void setColor(const QColor &color) { m_color = color; update(); }
    QColor getColor() const { return m_color; }
    
    void setTexture(const QPixmap &texture) { m_texture = texture; update(); }
    QPixmap getTexture() const { return m_texture; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString m_name;
    QSizeF m_size;
    QColor m_color;
    QPixmap m_texture;
    bool m_selected;
    bool m_dragging;
    QPointF m_dragStartPos;
};
