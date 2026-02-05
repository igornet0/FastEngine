#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

#include <QObject>
#include <QList>
#include <QImage>
#include <QString>
#include "Layer.h"

class LayerManager : public QObject
{
    Q_OBJECT

public:
    explicit LayerManager(QObject *parent = nullptr);
    ~LayerManager();

    // Layer management
    void addLayer(const QString &name, const QImage &image);
    void removeLayer(int index);
    void duplicateLayer(int index);
    void moveLayer(int fromIndex, int toIndex);
    void clearLayers();

    // Layer access
    Layer* layer(int index) const;
    Layer* currentLayer() const;
    int currentLayerIndex() const;
    int layerCount() const;

    // Current layer operations
    void setCurrentLayer(int index);
    void setLayerVisible(int index, bool visible);
    void setLayerOpacity(int index, float opacity);
    void setLayerName(int index, const QString &name);

    // Image operations
    QImage compositeImage() const;
    void resizeLayers(const QSize &size);

signals:
    void layerAdded(int index);
    void layerRemoved(int index);
    void currentLayerChanged(int index);
    void layerVisibilityChanged(int index, bool visible);
    void layerOpacityChanged(int index, float opacity);
    void layerNameChanged(int index, const QString &name);

private:
    QList<Layer*> m_layers;
    int m_currentLayerIndex;
};

#endif // LAYERMANAGER_H
