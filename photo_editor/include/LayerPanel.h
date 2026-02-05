#ifndef LAYERPANEL_H
#define LAYERPANEL_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

class ImageDocument; // Forward declaration

class LayerPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LayerPanel(QWidget *parent = nullptr);
    ~LayerPanel();

    void setDocument(ImageDocument *document);

signals:
    void layerSelected(int index);
    void layerVisibilityChanged(int index, bool visible);
    void layerVisibilityToggled(int index, bool visible);
    void layerOpacityChanged(int index, float opacity);
    void layerMoved(int fromIndex, int toIndex);

private slots:
    void onLayerSelectionChanged();
    void onLayerVisibilityToggled();

private:
    void setupUI();
    void updateLayers();

    QVBoxLayout *m_mainLayout;
    QListWidget *m_layerList;
    QPushButton *m_addLayerButton;
    QPushButton *m_removeLayerButton;
    QPushButton *m_moveUpButton;
    QPushButton *m_moveDownButton;
    
    ImageDocument *m_document;
};

#endif // LAYERPANEL_H