#include "SceneView2D.h"
#include "FastEngineIntegration.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QScrollBar>
#include <QTransform>
#include <QPointF>
#include <QVector2D>
#include <QMatrix4x4>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <cmath>

SceneView2D::SceneView2D(QWidget *parent)
    : QGraphicsView(parent)
    , m_scene(nullptr)
    , m_gridItem(nullptr)
    , m_selectionBox(nullptr)
    , m_gridVisible(true)
    , m_gridSize(32.0f)
    , m_snapToGrid(true)
    , m_dragging(false)
    , m_cameraDragging(false)
    , m_cameraPosition(0, 0)
    , m_cameraZoom(1.0f)
    , m_draggedItem(nullptr)
    , m_updateTimer(nullptr)
    , m_fastEngineIntegration(std::make_unique<FastEngineIntegration>())
{
    setupScene();
    setupConnections();
    
    // Настройка стилей
    m_gridPen = QPen(QColor(100, 100, 100, 100), 1);
    m_selectionPen = QPen(QColor(255, 255, 0), 2);
    m_selectionBrush = QBrush(QColor(255, 255, 0, 50));
    
    // Настройка вида
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::RubberBandDrag);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    
    // Настройка камеры
    resetCamera();
    
    // Создание тестовых объектов
    createTestEntities();
    
    // Таймер обновления
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &SceneView2D::updateScene);
    m_updateTimer->start(16); // ~60 FPS
}

SceneView2D::~SceneView2D()
{
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
}

void SceneView2D::setupScene()
{
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-2000, -2000, 4000, 4000);
    setScene(m_scene);
    
    // Создание сетки
    createGrid();
}

void SceneView2D::setupConnections()
{
    // Соединения для обработки событий
    connect(m_scene, &QGraphicsScene::selectionChanged, this, [this]() {
        QStringList selected;
        for (auto item : m_scene->selectedItems()) {
            if (auto entity = dynamic_cast<SceneEntity2D*>(item)) {
                selected.append(entity->getName());
            }
        }
        m_selectedEntities = selected;
        emit selectionChanged(selected);
    });
}

void SceneView2D::setGridVisible(bool visible)
{
    m_gridVisible = visible;
    if (m_gridItem) {
        m_gridItem->setVisible(visible);
    }
    update();
}

void SceneView2D::setGridSize(float size)
{
    m_gridSize = size;
    updateGrid();
}

void SceneView2D::setSnapToGrid(bool enabled)
{
    m_snapToGrid = enabled;
}

void SceneView2D::resetCamera()
{
    m_cameraPosition = QPointF(0, 0);
    m_cameraZoom = 1.0f;
    updateCamera();
}

void SceneView2D::setCameraPosition(const QPointF &position)
{
    m_cameraPosition = position;
    updateCamera();
}

void SceneView2D::setCameraZoom(float zoom)
{
    m_cameraZoom = qBound(0.1f, zoom, 5.0f);
    updateCamera();
}

void SceneView2D::addEntity(const QString &name, const QPointF &position, const QSizeF &size)
{
    auto entity = new SceneEntity2D(name, size);
    entity->setPos(position);
    m_scene->addItem(entity);
    
    emit entityCreated(name, position);
}

void SceneView2D::removeEntity(const QString &name)
{
    for (auto item : m_scene->items()) {
        if (auto entity = dynamic_cast<SceneEntity2D*>(item)) {
            if (entity->getName() == name) {
                m_scene->removeItem(entity);
                delete entity;
                emit entityDeleted(name);
                break;
            }
        }
    }
}

void SceneView2D::selectEntity(const QString &name)
{
    clearSelection();
    
    for (auto item : m_scene->items()) {
        if (auto entity = dynamic_cast<SceneEntity2D*>(item)) {
            if (entity->getName() == name) {
                entity->setSelected(true);
                m_scene->clearSelection();
                m_scene->setSelectionArea(QPainterPath());
                break;
            }
        }
    }
}

void SceneView2D::clearSelection()
{
    m_scene->clearSelection();
    m_selectedEntities.clear();
    emit selectionChanged(QStringList());
}

QStringList SceneView2D::getSelectedEntities() const
{
    return m_selectedEntities;
}

QPointF SceneView2D::getCameraPosition() const
{
    return m_cameraPosition;
}

float SceneView2D::getCameraZoom() const
{
    return m_cameraZoom;
}

bool SceneView2D::isGridVisible() const
{
    return m_gridVisible;
}

float SceneView2D::getGridSize() const
{
    return m_gridSize;
}

bool SceneView2D::isSnapToGrid() const
{
    return m_snapToGrid;
}

void SceneView2D::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem *item = itemAt(event->pos());
        
        if (item && dynamic_cast<SceneEntity2D*>(item)) {
            // Начало перетаскивания объекта
            m_dragging = true;
            m_draggedItem = item;
            m_dragStartPos = item->pos();
        } else {
            // Начало перетаскивания камеры
            m_cameraDragging = true;
            m_lastMousePos = event->pos();
        }
    } else if (event->button() == Qt::RightButton) {
        // Создание нового объекта
        QPointF worldPos = screenToWorld(event->pos());
        if (m_snapToGrid) {
            worldPos = snapToGrid(worldPos);
        }
        
        QString name = QString("Entity_%1").arg(m_scene->items().size());
        addEntity(name, worldPos, QSizeF(32, 32));
    }
    
    QGraphicsView::mousePressEvent(event);
}

void SceneView2D::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && m_draggedItem) {
        // Перетаскивание объекта
        QPointF worldPos = screenToWorld(event->pos());
        if (m_snapToGrid) {
            worldPos = snapToGrid(worldPos);
        }
        
        m_draggedItem->setPos(worldPos);
        
        if (auto entity = dynamic_cast<SceneEntity2D*>(m_draggedItem)) {
            emit entityMoved(entity->getName(), worldPos);
        }
    } else if (m_cameraDragging) {
        // Перетаскивание камеры
        QPoint delta = event->pos() - m_lastMousePos;
        m_cameraPosition -= QPointF(delta) / m_cameraZoom;
        m_lastMousePos = event->pos();
        updateCamera();
    }
    
    QGraphicsView::mouseMoveEvent(event);
}

void SceneView2D::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        m_cameraDragging = false;
        m_draggedItem = nullptr;
    }
    
    QGraphicsView::mouseReleaseEvent(event);
}

void SceneView2D::wheelEvent(QWheelEvent *event)
{
    // Масштабирование
    float scaleFactor = 1.15f;
    if (event->angleDelta().y() < 0) {
        scaleFactor = 1.0f / scaleFactor;
    }
    
    setCameraZoom(m_cameraZoom * scaleFactor);
}

void SceneView2D::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Delete:
            // Удаление выбранных объектов
            for (const QString &name : m_selectedEntities) {
                removeEntity(name);
            }
            break;
            
        case Qt::Key_Escape:
            clearSelection();
            break;
            
        case Qt::Key_G:
            setGridVisible(!m_gridVisible);
            break;
            
        case Qt::Key_S:
            setSnapToGrid(!m_snapToGrid);
            break;
            
        case Qt::Key_0:
            resetCamera();
            break;
            
        default:
            QGraphicsView::keyPressEvent(event);
            break;
    }
}

void SceneView2D::drawBackground(QPainter *painter, const QRectF &rect)
{
    // Очистка фона
    painter->fillRect(rect, QColor(30, 30, 30));
    
    // Отрисовка сетки
    if (m_gridVisible) {
        painter->setPen(m_gridPen);
        
        QRectF sceneRect = m_scene->sceneRect();
        float gridSize = m_gridSize;
        
        // Вертикальные линии
        for (float x = sceneRect.left(); x <= sceneRect.right(); x += gridSize) {
            painter->drawLine(QPointF(x, sceneRect.top()), QPointF(x, sceneRect.bottom()));
        }
        
        // Горизонтальные линии
        for (float y = sceneRect.top(); y <= sceneRect.bottom(); y += gridSize) {
            painter->drawLine(QPointF(sceneRect.left(), y), QPointF(sceneRect.right(), y));
        }
    }
}

void SceneView2D::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect)
    
    // Отрисовка дополнительных элементов поверх сцены
    // (например, UI элементы, которые не должны масштабироваться)
}

void SceneView2D::updateScene()
{
    // Обновление сцены (вызывается по таймеру)
    // Здесь можно добавить анимации, обновления состояния и т.д.
}

void SceneView2D::onEntityMoved()
{
    // Обработка перемещения объекта
}

QPointF SceneView2D::screenToWorld(const QPoint &screenPos) const
{
    QPointF scenePos = mapToScene(screenPos);
    return scenePos;
}

QPoint SceneView2D::worldToScreen(const QPointF &worldPos) const
{
    QPoint screenPos = mapFromScene(worldPos);
    return screenPos;
}

QPointF SceneView2D::snapToGrid(const QPointF &position) const
{
    if (!m_snapToGrid) {
        return position;
    }
    
    float gridSize = m_gridSize;
    float x = std::round(position.x() / gridSize) * gridSize;
    float y = std::round(position.y() / gridSize) * gridSize;
    
    return QPointF(x, y);
}

void SceneView2D::createGrid()
{
    // Создание сетки как графического элемента
    // (альтернатива отрисовке в drawBackground)
}

void SceneView2D::createTestEntities()
{
    // Создание тестовых объектов
    addEntity("Player", QPointF(100, 100), QSizeF(32, 32));
    addEntity("Enemy", QPointF(200, 150), QSizeF(24, 24));
    addEntity("Ground", QPointF(300, 250), QSizeF(64, 16));
}

void SceneView2D::updateGrid()
{
    // Обновление сетки при изменении размера
    update();
}

void SceneView2D::updateCamera()
{
    // Обновление позиции камеры
    QTransform transform;
    transform.translate(m_cameraPosition.x(), m_cameraPosition.y());
    transform.scale(m_cameraZoom, m_cameraZoom);
    
    setTransform(transform);
}

// Реализация SceneEntity2D
SceneEntity2D::SceneEntity2D(const QString &name, const QSizeF &size, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_name(name)
    , m_size(size)
    , m_color(Qt::blue)
    , m_selected(false)
    , m_dragging(false)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

QRectF SceneEntity2D::boundingRect() const
{
    return QRectF(-m_size.width()/2, -m_size.height()/2, m_size.width(), m_size.height());
}

void SceneEntity2D::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    QRectF rect = boundingRect();
    
    // Отрисовка фона
    painter->fillRect(rect, m_color);
    
    // Отрисовка текстуры если есть
    if (!m_texture.isNull()) {
        painter->drawPixmap(rect.toRect(), m_texture);
    }
    
    // Отрисовка рамки выделения
    if (m_selected) {
        painter->setPen(QPen(Qt::yellow, 2));
        painter->setBrush(QBrush(Qt::yellow, Qt::NoBrush));
        painter->drawRect(rect);
    }
    
    // Отрисовка имени
    painter->setPen(Qt::white);
    painter->drawText(rect, Qt::AlignCenter, m_name);
}

void SceneEntity2D::setSelected(bool selected)
{
    m_selected = selected;
    update();
}

void SceneEntity2D::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartPos = pos();
    }
    
    QGraphicsItem::mousePressEvent(event);
}

void SceneEntity2D::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_dragging) {
        // Обновление позиции
        QPointF newPos = m_dragStartPos + (event->scenePos() - event->buttonDownScenePos(Qt::LeftButton));
        setPos(newPos);
    }
    
    QGraphicsItem::mouseMoveEvent(event);
}

void SceneEntity2D::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
    
    QGraphicsItem::mouseReleaseEvent(event);
}

void SceneEntity2D::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    
    // Двойной клик для редактирования свойств
    qDebug() << "Double clicked entity:" << m_name;
}

void SceneView2D::onAssetSelected(const QString &assetPath)
{
    // Обработка выбора ассета
    Q_UNUSED(assetPath);
    // Здесь можно добавить логику для загрузки ассета в сцену
}
