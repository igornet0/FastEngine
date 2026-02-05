#include "LayerManager.h"
#include "Layer.h"
#include <QDebug>

LayerManager::LayerManager(QObject *parent)
    : QObject(parent)
    , m_currentLayerIndex(-1)
{
}

LayerManager::~LayerManager()
{
    qDeleteAll(m_layers);
}

void LayerManager::addLayer(const QString &name, const QImage &image)
{
    Layer *layer = new Layer(name, image, this);
    m_layers.append(layer);
    int index = m_layers.size() - 1;
    
    if (m_currentLayerIndex == -1) {
        m_currentLayerIndex = 0;
    }
    
    emit layerAdded(index);
}

void LayerManager::removeLayer(int index)
{
    if (index >= 0 && index < m_layers.size()) {
        Layer *layer = m_layers.takeAt(index);
        delete layer;
        
        // Adjust current layer index
        if (m_currentLayerIndex >= index) {
            m_currentLayerIndex--;
            if (m_currentLayerIndex < 0 && !m_layers.isEmpty()) {
                m_currentLayerIndex = 0;
            }
        }
        
        emit layerRemoved(index);
        
        if (m_currentLayerIndex != -1) {
            emit currentLayerChanged(m_currentLayerIndex);
        }
    }
}

void LayerManager::duplicateLayer(int index)
{
    if (index >= 0 && index < m_layers.size()) {
        Layer *originalLayer = m_layers[index];
        Layer *duplicatedLayer = new Layer(originalLayer->name() + " Copy", originalLayer->image(), this);
        m_layers.insert(index + 1, duplicatedLayer);
        emit layerAdded(index + 1);
    }
}

void LayerManager::moveLayer(int fromIndex, int toIndex)
{
    if (fromIndex >= 0 && fromIndex < m_layers.size() && 
        toIndex >= 0 && toIndex < m_layers.size()) {
        m_layers.move(fromIndex, toIndex);
        
        // Update current layer index
        if (m_currentLayerIndex == fromIndex) {
            m_currentLayerIndex = toIndex;
        } else if (fromIndex < toIndex) {
            if (m_currentLayerIndex > fromIndex && m_currentLayerIndex <= toIndex) {
                m_currentLayerIndex--;
            }
        } else {
            if (m_currentLayerIndex >= toIndex && m_currentLayerIndex < fromIndex) {
                m_currentLayerIndex++;
            }
        }
    }
}

void LayerManager::clearLayers()
{
    qDeleteAll(m_layers);
    m_layers.clear();
    m_currentLayerIndex = -1;
}

Layer* LayerManager::layer(int index) const
{
    if (index >= 0 && index < m_layers.size()) {
        return m_layers[index];
    }
    return nullptr;
}

Layer* LayerManager::currentLayer() const
{
    return layer(m_currentLayerIndex);
}

int LayerManager::currentLayerIndex() const
{
    return m_currentLayerIndex;
}

int LayerManager::layerCount() const
{
    return m_layers.size();
}

void LayerManager::setCurrentLayer(int index)
{
    if (index >= 0 && index < m_layers.size() && index != m_currentLayerIndex) {
        m_currentLayerIndex = index;
        emit currentLayerChanged(index);
    }
}

void LayerManager::setLayerVisible(int index, bool visible)
{
    Layer *layer = this->layer(index);
    if (layer) {
        layer->setVisible(visible);
        emit layerVisibilityChanged(index, visible);
    }
}

void LayerManager::setLayerOpacity(int index, float opacity)
{
    Layer *layer = this->layer(index);
    if (layer) {
        layer->setOpacity(opacity);
        emit layerOpacityChanged(index, opacity);
    }
}

void LayerManager::setLayerName(int index, const QString &name)
{
    Layer *layer = this->layer(index);
    if (layer) {
        layer->setName(name);
        emit layerNameChanged(index, name);
    }
}

QImage LayerManager::compositeImage() const
{
    if (m_layers.isEmpty()) {
        return QImage();
    }
    
    QImage result;
    for (int i = 0; i < m_layers.size(); ++i) {
        Layer *layer = m_layers[i];
        if (layer && layer->visible()) {
            if (result.isNull()) {
                result = layer->image();
            } else {
                result = layer->composite(result);
            }
        }
    }
    
    return result;
}

void LayerManager::resizeLayers(const QSize &size)
{
    for (Layer *layer : m_layers) {
        if (layer) {
            layer->resize(size);
        }
    }
}