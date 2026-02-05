#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <QObject>
#include <QStack>
#include <QString>
#include <QImage>
#include <QTimer>
#include <QMap>
#include <QVariant>

class ImageDocument;

class HistoryCommand : public QObject
{
    Q_OBJECT

public:
    explicit HistoryCommand(const QString &description, QObject *parent = nullptr);
    virtual ~HistoryCommand() = default;
    
    QString getDescription() const { return m_description; }
    bool isExecuted() const { return m_executed; }
    void setExecuted(bool executed) { m_executed = executed; }
    
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual bool canMergeWith(const HistoryCommand *other) const { Q_UNUSED(other); return false; }
    virtual void mergeWith(const HistoryCommand *other) { Q_UNUSED(other); }

signals:
    void commandExecuted();
    void commandUndone();

protected:
    QString m_description;
    bool m_executed;
};

class HistoryManager : public QObject
{
    Q_OBJECT

public:
    explicit HistoryManager(QObject *parent = nullptr);
    ~HistoryManager();

    // History operations
    void addCommand(HistoryCommand *command);
    void executeCommand(HistoryCommand *command);
    
    bool canUndo() const;
    bool canRedo() const;
    
    void undo();
    void redo();
    
    void clear();
    void clearRedo();
    
    // History information
    QString getUndoText() const;
    QString getRedoText() const;
    
    int getUndoCount() const { return m_undoStack.size(); }
    int getRedoCount() const { return m_redoStack.size(); }
    
    // History limits
    void setMaxHistorySize(int size);
    int getMaxHistorySize() const { return m_maxHistorySize; }
    
    // Batch operations
    void beginBatch(const QString &description);
    void endBatch();
    bool isBatching() const { return m_batching; }

signals:
    void historyChanged();
    void canUndoChanged(bool canUndo);
    void canRedoChanged(bool canRedo);

private slots:
    void onCommandExecuted();
    void onCommandUndone();

private:
    void trimHistory();
    void processBatch();
    
    QStack<HistoryCommand*> m_undoStack;
    QStack<HistoryCommand*> m_redoStack;
    
    QStack<QString> m_batchStack;
    QList<HistoryCommand*> m_currentBatch;
    bool m_batching;
    QString m_batchDescription;
    
    int m_maxHistorySize;
    QTimer *m_batchTimer;
};

// Specific command implementations
class PaintCommand : public HistoryCommand
{
    Q_OBJECT

public:
    PaintCommand(ImageDocument *document, int layerIndex, const QPoint &point, 
                 const QColor &color, int size, QObject *parent = nullptr);
    
    void execute() override;
    void undo() override;
    bool canMergeWith(const HistoryCommand *other) const override;
    void mergeWith(const HistoryCommand *other) override;

private:
    ImageDocument *m_document;
    int m_layerIndex;
    QPoint m_point;
    QColor m_color;
    int m_size;
    QImage m_originalImage;
};

class LayerAddCommand : public HistoryCommand
{
    Q_OBJECT

public:
    LayerAddCommand(ImageDocument *document, int index, QObject *parent = nullptr);
    
    void execute() override;
    void undo() override;

private:
    ImageDocument *m_document;
    int m_index;
};

class LayerRemoveCommand : public HistoryCommand
{
    Q_OBJECT

public:
    LayerRemoveCommand(ImageDocument *document, int index, QObject *parent = nullptr);
    
    void execute() override;
    void undo() override;

private:
    ImageDocument *m_document;
    int m_index;
    QImage m_layerImage;
    QString m_layerName;
};

class FilterCommand : public HistoryCommand
{
    Q_OBJECT

public:
    FilterCommand(ImageDocument *document, int layerIndex, const QString &filterName, 
                  const QVariantMap &parameters, QObject *parent = nullptr);
    
    void execute() override;
    void undo() override;

private:
    ImageDocument *m_document;
    int m_layerIndex;
    QString m_filterName;
    QVariantMap m_parameters;
    QImage m_originalImage;
};

#endif // HISTORYMANAGER_H
