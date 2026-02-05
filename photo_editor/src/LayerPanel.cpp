#include "LayerPanel.h"
#include "ImageDocument.h"

LayerPanel::LayerPanel(QWidget *parent)
    : QWidget(parent)
    , m_document(nullptr)
{
    setupUI();
}

LayerPanel::~LayerPanel()
{
}

void LayerPanel::setDocument(ImageDocument *document)
{
    m_document = document;
    updateLayers();
}

void LayerPanel::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    m_layerList = new QListWidget();
    m_mainLayout->addWidget(m_layerList);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addLayerButton = new QPushButton("Add");
    m_removeLayerButton = new QPushButton("Remove");
    m_moveUpButton = new QPushButton("Up");
    m_moveDownButton = new QPushButton("Down");
    
    buttonLayout->addWidget(m_addLayerButton);
    buttonLayout->addWidget(m_removeLayerButton);
    buttonLayout->addWidget(m_moveUpButton);
    buttonLayout->addWidget(m_moveDownButton);
    
    m_mainLayout->addLayout(buttonLayout);
    
    connect(m_layerList, &QListWidget::currentRowChanged, this, &LayerPanel::onLayerSelectionChanged);
}

void LayerPanel::updateLayers()
{
    m_layerList->clear();
    
    if (m_document) {
        // Add a single layer for now
        m_layerList->addItem("Background");
    }
}

void LayerPanel::onLayerSelectionChanged()
{
    int currentRow = m_layerList->currentRow();
    if (currentRow >= 0) {
        emit layerSelected(currentRow);
    }
}

void LayerPanel::onLayerVisibilityToggled()
{
    // Implementation for layer visibility toggle
}