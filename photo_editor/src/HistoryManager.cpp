#include "HistoryManager.h"
#include "ImageDocument.h"
#include "Layer.h"

#include <QDebug>

// HistoryCommand implementation
HistoryCommand::HistoryCommand(const QString &description, QObject *parent)
    : QObject(parent)
    , m_description(description)
    , m_executed(false)
{
}

// HistoryManager implementation
HistoryManager::HistoryManager(QObject *parent)
    : QObject(parent)
    , m_batching(false)
    , m_maxHistorySize(50)
    , m_batchTimer(new QTimer(this))
{
    m_batchTimer->setSingleShot(true);
    m_batchTimer->setInterval(1000); // 1 second
    connect(m_batchTimer, &QTimer::timeout, this, &HistoryManager::processBatch);
}

HistoryManager::~HistoryManager()
{
    clear();
}

void HistoryManager::addCommand(HistoryCommand *command)
{
    if (!command) {
        return;
    }
    
    if (m_batching) {
        m_currentBatch.append(command);
        m_batchTimer->start();
    } else {
        executeCommand(command);
    }
}

void HistoryManager::executeCommand(HistoryCommand *command)
{
    if (!command) {
        return;
    }
    
    // Clear redo stack
    clearRedo();
    
    // Execute command
    command->execute();
    command->setExecuted(true);
    
    // Add to undo stack
    m_undoStack.push(command);
    
    // Trim history if needed
    trimHistory();
    
    emit historyChanged();
    emit canUndoChanged(true);
    emit canRedoChanged(false);
}

bool HistoryManager::canUndo() const
{
    return !m_undoStack.isEmpty();
}

bool HistoryManager::canRedo() const
{
    return !m_redoStack.isEmpty();
}

void HistoryManager::undo()
{
    if (!canUndo()) {
        return;
    }
    
    HistoryCommand *command = m_undoStack.pop();
    command->undo();
    command->setExecuted(false);
    
    m_redoStack.push(command);
    
    emit historyChanged();
    emit canUndoChanged(canUndo());
    emit canRedoChanged(true);
}

void HistoryManager::redo()
{
    if (!canRedo()) {
        return;
    }
    
    HistoryCommand *command = m_redoStack.pop();
    command->execute();
    command->setExecuted(true);
    
    m_undoStack.push(command);
    
    emit historyChanged();
    emit canUndoChanged(true);
    emit canRedoChanged(canRedo());
}

void HistoryManager::clear()
{
    // Clear undo stack
    while (!m_undoStack.isEmpty()) {
        delete m_undoStack.pop();
    }
    
    // Clear redo stack
    while (!m_redoStack.isEmpty()) {
        delete m_redoStack.pop();
    }
    
    // Clear current batch
    for (HistoryCommand *command : m_currentBatch) {
        delete command;
    }
    m_currentBatch.clear();
    
    emit historyChanged();
    emit canUndoChanged(false);
    emit canRedoChanged(false);
}

void HistoryManager::clearRedo()
{
    while (!m_redoStack.isEmpty()) {
        delete m_redoStack.pop();
    }
    emit canRedoChanged(false);
}

QString HistoryManager::getUndoText() const
{
    if (m_undoStack.isEmpty()) {
        return QString();
    }
    return m_undoStack.top()->getDescription();
}

QString HistoryManager::getRedoText() const
{
    if (m_redoStack.isEmpty()) {
        return QString();
    }
    return m_redoStack.top()->getDescription();
}

void HistoryManager::setMaxHistorySize(int size)
{
    m_maxHistorySize = qMax(1, size);
    trimHistory();
}

void HistoryManager::beginBatch(const QString &description)
{
    if (m_batching) {
        endBatch();
    }
    
    m_batching = true;
    m_currentBatch.clear();
    m_batchDescription = description;
}

void HistoryManager::endBatch()
{
    if (!m_batching) {
        return;
    }
    
    m_batching = false;
    processBatch();
}

void HistoryManager::onCommandExecuted()
{
    // Command was executed
}

void HistoryManager::onCommandUndone()
{
    // Command was undone
}

void HistoryManager::trimHistory()
{
    while (m_undoStack.size() > m_maxHistorySize) {
        delete m_undoStack.takeFirst();
    }
}

void HistoryManager::processBatch()
{
    if (m_currentBatch.isEmpty()) {
        return;
    }
    
    // Create batch command
    // TODO: Implement batch command
    Q_UNUSED(m_batchDescription);
    // batchCommand->setCommands(m_currentBatch);
    
    // Execute batch command
    // executeCommand(batchCommand);
    
    // Clear current batch
    m_currentBatch.clear();
}

// PaintCommand implementation
PaintCommand::PaintCommand(ImageDocument *document, int layerIndex, const QPoint &point, 
                           const QColor &color, int size, QObject *parent)
    : HistoryCommand("Paint", parent)
    , m_document(document)
    , m_layerIndex(layerIndex)
    , m_point(point)
    , m_color(color)
    , m_size(size)
{
    if (m_document && m_layerIndex >= 0) {
        Layer *layer = m_document->getLayer(m_layerIndex);
        if (layer && layer->getType() == Layer::RasterLayer) {
            RasterLayer *rasterLayer = static_cast<RasterLayer*>(layer);
            m_originalImage = rasterLayer->getImage();
        }
    }
}

void PaintCommand::execute()
{
    if (!m_document || m_layerIndex < 0) {
        return;
    }
    
    Layer *layer = m_document->getLayer(m_layerIndex);
    if (layer && layer->getType() == Layer::RasterLayer) {
        RasterLayer *rasterLayer = static_cast<RasterLayer*>(layer);
        rasterLayer->applyBrush(m_point, m_color, m_size, 1.0f);
    }
}

void PaintCommand::undo()
{
    if (!m_document || m_layerIndex < 0) {
        return;
    }
    
    Layer *layer = m_document->getLayer(m_layerIndex);
    if (layer && layer->getType() == Layer::RasterLayer) {
        RasterLayer *rasterLayer = static_cast<RasterLayer*>(layer);
        rasterLayer->setImage(m_originalImage);
    }
}

bool PaintCommand::canMergeWith(const HistoryCommand *other) const
{
    const PaintCommand *otherPaint = qobject_cast<const PaintCommand*>(other);
    if (!otherPaint) {
        return false;
    }
    
    return (m_document == otherPaint->m_document && 
            m_layerIndex == otherPaint->m_layerIndex &&
            m_color == otherPaint->m_color &&
            m_size == otherPaint->m_size);
}

void PaintCommand::mergeWith(const HistoryCommand *other)
{
    const PaintCommand *otherPaint = qobject_cast<const PaintCommand*>(other);
    if (!otherPaint) {
        return;
    }
    
    // Update point to the latest one
    m_point = otherPaint->m_point;
}

// LayerAddCommand implementation
LayerAddCommand::LayerAddCommand(ImageDocument *document, int index, QObject *parent)
    : HistoryCommand("Add Layer", parent)
    , m_document(document)
    , m_index(index)
{
}

void LayerAddCommand::execute()
{
    if (!m_document) {
        return;
    }
    
    // Layer is already added, just update index
    m_index = m_document->getLayerCount() - 1;
}

void LayerAddCommand::undo()
{
    if (!m_document || m_index < 0) {
        return;
    }
    
    m_document->removeLayer(m_index);
}

// LayerRemoveCommand implementation
LayerRemoveCommand::LayerRemoveCommand(ImageDocument *document, int index, QObject *parent)
    : HistoryCommand("Remove Layer", parent)
    , m_document(document)
    , m_index(index)
{
    if (m_document && index >= 0) {
        Layer *layer = m_document->getLayer(index);
        if (layer) {
            m_layerImage = layer->getImage();
            m_layerName = layer->getName();
        }
    }
}

void LayerRemoveCommand::execute()
{
    if (!m_document || m_index < 0) {
        return;
    }
    
    m_document->removeLayer(m_index);
}

void LayerRemoveCommand::undo()
{
    if (!m_document) {
        return;
    }
    
    RasterLayer *layer = new RasterLayer(m_layerName, m_document);
    layer->setImage(m_layerImage);
    m_document->addLayer(layer);
}

// FilterCommand implementation
FilterCommand::FilterCommand(ImageDocument *document, int layerIndex, const QString &filterName, 
                            const QVariantMap &parameters, QObject *parent)
    : HistoryCommand("Apply Filter: " + filterName, parent)
    , m_document(document)
    , m_layerIndex(layerIndex)
    , m_filterName(filterName)
    , m_parameters(parameters)
{
    if (m_document && m_layerIndex >= 0) {
        Layer *layer = m_document->getLayer(m_layerIndex);
        if (layer && layer->getType() == Layer::RasterLayer) {
            RasterLayer *rasterLayer = static_cast<RasterLayer*>(layer);
            m_originalImage = rasterLayer->getImage();
        }
    }
}

void FilterCommand::execute()
{
    if (!m_document || m_layerIndex < 0) {
        return;
    }
    
    Layer *layer = m_document->getLayer(m_layerIndex);
    if (layer && layer->getType() == Layer::RasterLayer) {
        RasterLayer *rasterLayer = static_cast<RasterLayer*>(layer);
        // TODO: Apply filter
        // QImage filteredImage = applyFilter(m_originalImage, m_filterName, m_parameters);
        // rasterLayer->setImage(filteredImage);
    }
}

void FilterCommand::undo()
{
    if (!m_document || m_layerIndex < 0) {
        return;
    }
    
    Layer *layer = m_document->getLayer(m_layerIndex);
    if (layer && layer->getType() == Layer::RasterLayer) {
        RasterLayer *rasterLayer = static_cast<RasterLayer*>(layer);
        rasterLayer->setImage(m_originalImage);
    }
}

// BatchCommand implementation
class BatchCommand : public HistoryCommand
{
public:
    BatchCommand(const QString &description, QObject *parent)
        : HistoryCommand(description, parent)
    {
    }
    
    void setCommands(const QList<HistoryCommand*> &commands)
    {
        m_commands = commands;
    }
    
    void execute() override
    {
        for (HistoryCommand *command : m_commands) {
            command->execute();
        }
    }
    
    void undo() override
    {
        for (int i = m_commands.size() - 1; i >= 0; --i) {
            m_commands[i]->undo();
        }
    }
    
private:
    QList<HistoryCommand*> m_commands;
};
