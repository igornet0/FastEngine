#include "Layer.h"
#include <QDebug>

Layer::Layer(QObject *parent)
    : QObject(parent)
    , m_name("Layer")
    , m_visible(true)
    , m_locked(false)
    , m_opacity(1.0f)
    , m_index(0)
{
}

Layer::Layer(const QString &name, const QImage &image, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_image(image)
    , m_visible(true)
    , m_locked(false)
    , m_opacity(1.0f)
    , m_index(0)
{
}

Layer::~Layer()
{
}

void Layer::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged(name);
    }
}

void Layer::setImage(const QImage &image)
{
    m_image = image;
    emit imageChanged();
}

void Layer::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;
        emit visibilityChanged(visible);
    }
}

void Layer::setLocked(bool locked)
{
    m_locked = locked;
}

void Layer::setOpacity(float opacity)
{
    if (qAbs(m_opacity - opacity) > 0.01f) {
        m_opacity = qBound(0.0f, opacity, 1.0f);
        emit opacityChanged(m_opacity);
    }
}

void Layer::setIndex(int index)
{
    m_index = index;
}

void Layer::clear()
{
    if (!m_image.isNull()) {
        m_image.fill(Qt::transparent);
        emit imageChanged();
    }
}

void Layer::resize(const QSize &size)
{
    if (m_image.size() != size) {
        m_image = m_image.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        emit imageChanged();
    }
}

QImage Layer::composite(const QImage &background) const
{
    if (!m_visible || m_opacity <= 0.0f) {
        return background;
    }

    QImage result = background;
    QPainter painter(&result);
    
    if (m_opacity < 1.0f) {
        painter.setOpacity(m_opacity);
    }
    
    painter.drawImage(0, 0, m_image);
    painter.end();
    
    return result;
}