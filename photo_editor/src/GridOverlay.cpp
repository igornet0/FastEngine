#include "GridOverlay.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>
#include <cmath>
#include <QJsonDocument>
#include <QJsonArray>

GridOverlay::GridOverlay(QWidget *parent)
    : QWidget(parent)
    , m_gridVisible(false)
    , m_gridSize(20)
    , m_gridColor(Qt::gray)
    , m_gridOpacity(0.5f)
    , m_gridStyle(GridStyle::Lines)
    , m_snapToGrid(false)
    , m_guidesVisible(true)
    , m_guideColor(Qt::red)
    , m_snapToGuides(false)
    , m_rulersVisible(true)
    , m_rulerUnit(RulerUnit::Pixels)
    , m_rulerColor(Qt::lightGray)
    , m_draggingGuide(false)
    , m_draggingHorizontal(false)
    , m_dragOffset(0)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setMouseTracking(true);
}

GridOverlay::~GridOverlay()
{
}

void GridOverlay::setGridVisible(bool visible)
{
    if (m_gridVisible != visible) {
        m_gridVisible = visible;
        update();
        emit gridChanged();
    }
}

void GridOverlay::setGridSize(int size)
{
    if (m_gridSize != size && size > 0) {
        m_gridSize = size;
        update();
        emit gridChanged();
    }
}

void GridOverlay::setGridColor(const QColor &color)
{
    if (m_gridColor != color) {
        m_gridColor = color;
        update();
        emit gridChanged();
    }
}

void GridOverlay::setGridOpacity(float opacity)
{
    if (m_gridOpacity != opacity) {
        m_gridOpacity = qBound(0.0f, opacity, 1.0f);
        update();
        emit gridChanged();
    }
}

void GridOverlay::setGridStyle(GridStyle style)
{
    if (m_gridStyle != style) {
        m_gridStyle = style;
        update();
        emit gridChanged();
    }
}

void GridOverlay::setSnapToGrid(bool enabled)
{
    if (m_snapToGrid != enabled) {
        m_snapToGrid = enabled;
        emit gridChanged();
    }
}

QPoint GridOverlay::snapToGrid(const QPoint &point) const
{
    if (!m_snapToGrid || m_gridSize <= 0) {
        return point;
    }

    int x = ((point.x() + m_gridSize / 2) / m_gridSize) * m_gridSize;
    int y = ((point.y() + m_gridSize / 2) / m_gridSize) * m_gridSize;
    
    return QPoint(x, y);
}

QRect GridOverlay::snapToGrid(const QRect &rect) const
{
    if (!m_snapToGrid || m_gridSize <= 0) {
        return rect;
    }

    QPoint topLeft = snapToGrid(rect.topLeft());
    QPoint bottomRight = snapToGrid(rect.bottomRight());
    
    return QRect(topLeft, bottomRight);
}

void GridOverlay::addHorizontalGuide(int y)
{
    if (!m_horizontalGuides.contains(y)) {
        m_horizontalGuides.append(y);
        std::sort(m_horizontalGuides.begin(), m_horizontalGuides.end());
        update();
        emit guidesChanged();
    }
}

void GridOverlay::addVerticalGuide(int x)
{
    if (!m_verticalGuides.contains(x)) {
        m_verticalGuides.append(x);
        std::sort(m_verticalGuides.begin(), m_verticalGuides.end());
        update();
        emit guidesChanged();
    }
}

void GridOverlay::removeHorizontalGuide(int y)
{
    if (m_horizontalGuides.removeOne(y)) {
        update();
        emit guidesChanged();
    }
}

void GridOverlay::removeVerticalGuide(int x)
{
    if (m_verticalGuides.removeOne(x)) {
        update();
        emit guidesChanged();
    }
}

void GridOverlay::clearGuides()
{
    if (!m_horizontalGuides.isEmpty() || !m_verticalGuides.isEmpty()) {
        m_horizontalGuides.clear();
        m_verticalGuides.clear();
        update();
        emit guidesChanged();
    }
}

void GridOverlay::clearHorizontalGuides()
{
    if (!m_horizontalGuides.isEmpty()) {
        m_horizontalGuides.clear();
        update();
        emit guidesChanged();
    }
}

void GridOverlay::clearVerticalGuides()
{
    if (!m_verticalGuides.isEmpty()) {
        m_verticalGuides.clear();
        update();
        emit guidesChanged();
    }
}

void GridOverlay::setGuidesVisible(bool visible)
{
    if (m_guidesVisible != visible) {
        m_guidesVisible = visible;
        update();
        emit guidesChanged();
    }
}

void GridOverlay::setGuideColor(const QColor &color)
{
    if (m_guideColor != color) {
        m_guideColor = color;
        update();
        emit guidesChanged();
    }
}

void GridOverlay::setSnapToGuides(bool enabled)
{
    if (m_snapToGuides != enabled) {
        m_snapToGuides = enabled;
        emit guidesChanged();
    }
}

QPoint GridOverlay::snapToGuides(const QPoint &point) const
{
    if (!m_snapToGuides) {
        return point;
    }

    int x = point.x();
    int y = point.y();
    int snapDistance = 5; // pixels

    // Snap to vertical guides
    for (int guideX : m_verticalGuides) {
        if (qAbs(x - guideX) <= snapDistance) {
            x = guideX;
            break;
        }
    }

    // Snap to horizontal guides
    for (int guideY : m_horizontalGuides) {
        if (qAbs(y - guideY) <= snapDistance) {
            y = guideY;
            break;
        }
    }

    return QPoint(x, y);
}

QRect GridOverlay::snapToGuides(const QRect &rect) const
{
    if (!m_snapToGuides) {
        return rect;
    }

    QPoint topLeft = snapToGuides(rect.topLeft());
    QPoint bottomRight = snapToGuides(rect.bottomRight());
    
    return QRect(topLeft, bottomRight);
}

void GridOverlay::setRulersVisible(bool visible)
{
    if (m_rulersVisible != visible) {
        m_rulersVisible = visible;
        update();
        emit rulersChanged();
    }
}

void GridOverlay::setRulerUnit(RulerUnit unit)
{
    if (m_rulerUnit != unit) {
        m_rulerUnit = unit;
        update();
        emit rulersChanged();
    }
}

void GridOverlay::setRulerColor(const QColor &color)
{
    if (m_rulerColor != color) {
        m_rulerColor = color;
        update();
        emit rulersChanged();
    }
}

QJsonObject GridOverlay::toJson() const
{
    QJsonObject json;
    
    // Grid properties
    json["gridVisible"] = m_gridVisible;
    json["gridSize"] = m_gridSize;
    json["gridColor"] = m_gridColor.name();
    json["gridOpacity"] = m_gridOpacity;
    json["gridStyle"] = static_cast<int>(m_gridStyle);
    json["snapToGrid"] = m_snapToGrid;
    
    // Guides properties
    json["guidesVisible"] = m_guidesVisible;
    json["guideColor"] = m_guideColor.name();
    json["snapToGuides"] = m_snapToGuides;
    
    QJsonArray hGuides;
    for (int y : m_horizontalGuides) {
        hGuides.append(y);
    }
    json["horizontalGuides"] = hGuides;
    
    QJsonArray vGuides;
    for (int x : m_verticalGuides) {
        vGuides.append(x);
    }
    json["verticalGuides"] = vGuides;
    
    // Rulers properties
    json["rulersVisible"] = m_rulersVisible;
    json["rulerUnit"] = static_cast<int>(m_rulerUnit);
    json["rulerColor"] = m_rulerColor.name();
    
    return json;
}

void GridOverlay::fromJson(const QJsonObject &json)
{
    // Grid properties
    m_gridVisible = json["gridVisible"].toBool();
    m_gridSize = json["gridSize"].toInt();
    m_gridColor = QColor(json["gridColor"].toString());
    m_gridOpacity = json["gridOpacity"].toDouble();
    m_gridStyle = static_cast<GridStyle>(json["gridStyle"].toInt());
    m_snapToGrid = json["snapToGrid"].toBool();
    
    // Guides properties
    m_guidesVisible = json["guidesVisible"].toBool();
    m_guideColor = QColor(json["guideColor"].toString());
    m_snapToGuides = json["snapToGuides"].toBool();
    
    m_horizontalGuides.clear();
    QJsonArray hGuides = json["horizontalGuides"].toArray();
    for (const QJsonValue &value : hGuides) {
        m_horizontalGuides.append(value.toInt());
    }
    
    m_verticalGuides.clear();
    QJsonArray vGuides = json["verticalGuides"].toArray();
    for (const QJsonValue &value : vGuides) {
        m_verticalGuides.append(value.toInt());
    }
    
    // Rulers properties
    m_rulersVisible = json["rulersVisible"].toBool();
    m_rulerUnit = static_cast<RulerUnit>(json["rulerUnit"].toInt());
    m_rulerColor = QColor(json["rulerColor"].toString());
    
    update();
}

void GridOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw rulers
    if (m_rulersVisible) {
        drawRulers(painter);
    }
    
    // Draw grid
    if (m_gridVisible) {
        drawGrid(painter);
    }
    
    // Draw guides
    if (m_guidesVisible) {
        drawGuides(painter);
    }
}

void GridOverlay::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint pos = event->pos();
        
        // Check if clicking on a guide
        int snapDistance = 5;
        
        // Check horizontal guides
        for (int y : m_horizontalGuides) {
            if (qAbs(pos.y() - y) <= snapDistance) {
                m_draggingGuide = true;
                m_draggingHorizontal = true;
                m_dragOffset = pos.y() - y;
                setCursor(Qt::SizeVerCursor);
                return;
            }
        }
        
        // Check vertical guides
        for (int x : m_verticalGuides) {
            if (qAbs(pos.x() - x) <= snapDistance) {
                m_draggingGuide = true;
                m_draggingHorizontal = false;
                m_dragOffset = pos.x() - x;
                setCursor(Qt::SizeHorCursor);
                return;
            }
        }
        
        // Create new guide
        if (event->modifiers() & Qt::ControlModifier) {
            if (event->modifiers() & Qt::ShiftModifier) {
                // Add horizontal guide
                addHorizontalGuide(pos.y());
            } else {
                // Add vertical guide
                addVerticalGuide(pos.x());
            }
        }
    }
}

void GridOverlay::mouseMoveEvent(QMouseEvent *event)
{
    if (m_draggingGuide) {
        QPoint pos = event->pos();
        
        if (m_draggingHorizontal) {
            int newY = pos.y() - m_dragOffset;
            // Update guide position (would need to track which guide is being dragged)
            update();
        } else {
            int newX = pos.x() - m_dragOffset;
            // Update guide position (would need to track which guide is being dragged)
            update();
        }
    } else {
        // Check if hovering over a guide
        QPoint pos = event->pos();
        int snapDistance = 5;
        
        bool hoveringGuide = false;
        
        // Check horizontal guides
        for (int y : m_horizontalGuides) {
            if (qAbs(pos.y() - y) <= snapDistance) {
                setCursor(Qt::SizeVerCursor);
                hoveringGuide = true;
                break;
            }
        }
        
        // Check vertical guides
        if (!hoveringGuide) {
            for (int x : m_verticalGuides) {
                if (qAbs(pos.x() - x) <= snapDistance) {
                    setCursor(Qt::SizeHorCursor);
                    hoveringGuide = true;
                    break;
                }
            }
        }
        
        if (!hoveringGuide) {
            setCursor(Qt::ArrowCursor);
        }
    }
}

void GridOverlay::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    
    if (m_draggingGuide) {
        m_draggingGuide = false;
        setCursor(Qt::ArrowCursor);
    }
}

void GridOverlay::drawGrid(QPainter &painter)
{
    painter.save();
    
    QColor gridColor = m_gridColor;
    gridColor.setAlphaF(m_gridOpacity);
    painter.setPen(QPen(gridColor, 1));
    
    QRect rect = this->rect();
    
    if (m_gridStyle == GridStyle::Lines) {
        // Draw vertical lines
        for (int x = 0; x < rect.width(); x += m_gridSize) {
            painter.drawLine(x, 0, x, rect.height());
        }
        
        // Draw horizontal lines
        for (int y = 0; y < rect.height(); y += m_gridSize) {
            painter.drawLine(0, y, rect.width(), y);
        }
    } else if (m_gridStyle == GridStyle::Dots) {
        painter.setBrush(QBrush(gridColor));
        painter.setPen(Qt::NoPen);
        
        for (int x = 0; x < rect.width(); x += m_gridSize) {
            for (int y = 0; y < rect.height(); y += m_gridSize) {
                painter.drawEllipse(x - 1, y - 1, 2, 2);
            }
        }
    } else if (m_gridStyle == GridStyle::Crosses) {
        painter.setPen(QPen(gridColor, 1));
        
        for (int x = 0; x < rect.width(); x += m_gridSize) {
            for (int y = 0; y < rect.height(); y += m_gridSize) {
                painter.drawLine(x - 2, y, x + 2, y);
                painter.drawLine(x, y - 2, x, y + 2);
            }
        }
    }
    
    painter.restore();
}

void GridOverlay::drawGuides(QPainter &painter)
{
    painter.save();
    
    painter.setPen(QPen(m_guideColor, 1, Qt::DashLine));
    
    // Draw horizontal guides
    for (int y : m_horizontalGuides) {
        painter.drawLine(0, y, width(), y);
    }
    
    // Draw vertical guides
    for (int x : m_verticalGuides) {
        painter.drawLine(x, 0, x, height());
    }
    
    painter.restore();
}

void GridOverlay::drawRulers(QPainter &painter)
{
    painter.save();
    
    painter.setPen(QPen(m_rulerColor, 1));
    painter.setBrush(QBrush(m_rulerColor));
    
    QRect rect = this->rect();
    int rulerHeight = getRulerHeight();
    int rulerWidth = getRulerWidth();
    
    // Draw horizontal ruler
    if (m_rulersVisible) {
        drawHorizontalRuler(painter);
    }
    
    // Draw vertical ruler
    if (m_rulersVisible) {
        drawVerticalRuler(painter);
    }
    
    painter.restore();
}

void GridOverlay::drawHorizontalRuler(QPainter &painter)
{
    QRect rect = this->rect();
    int rulerHeight = getRulerHeight();
    
    // Draw ruler background
    painter.fillRect(0, 0, rect.width(), rulerHeight, m_rulerColor);
    
    // Draw ruler marks and labels
    int step = m_gridSize;
    if (step < 10) step = 10;
    
    painter.setPen(QPen(Qt::black, 1));
    painter.setFont(QFont("Arial", 8));
    
    for (int x = 0; x < rect.width(); x += step) {
        // Draw tick mark
        painter.drawLine(x, rulerHeight - 5, x, rulerHeight);
        
        // Draw label
        QString label = formatRulerValue(x);
        QRect textRect(x - 20, 0, 40, rulerHeight - 5);
        painter.drawText(textRect, Qt::AlignCenter, label);
    }
}

void GridOverlay::drawVerticalRuler(QPainter &painter)
{
    QRect rect = this->rect();
    int rulerWidth = getRulerWidth();
    
    // Draw ruler background
    painter.fillRect(0, 0, rulerWidth, rect.height(), m_rulerColor);
    
    // Draw ruler marks and labels
    int step = m_gridSize;
    if (step < 10) step = 10;
    
    painter.setPen(QPen(Qt::black, 1));
    painter.setFont(QFont("Arial", 8));
    
    for (int y = 0; y < rect.height(); y += step) {
        // Draw tick mark
        painter.drawLine(rulerWidth - 5, y, rulerWidth, y);
        
        // Draw label
        QString label = formatRulerValue(y);
        QRect textRect(0, y - 10, rulerWidth - 5, 20);
        painter.drawText(textRect, Qt::AlignCenter, label);
    }
}

QString GridOverlay::formatRulerValue(int value) const
{
    switch (m_rulerUnit) {
        case RulerUnit::Pixels:
            return QString::number(value);
        case RulerUnit::Inches:
            return QString::number(value / 96.0, 'f', 2) + "\"";
        case RulerUnit::Centimeters:
            return QString::number(value / 37.8, 'f', 2) + "cm";
        case RulerUnit::Millimeters:
            return QString::number(value / 3.78, 'f', 1) + "mm";
        default:
            return QString::number(value);
    }
}
