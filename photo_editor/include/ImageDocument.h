#ifndef IMAGEDOCUMENT_H
#define IMAGEDOCUMENT_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QSize>
#include <QColor>

class ImageDocument : public QObject
{
    Q_OBJECT

public:
    explicit ImageDocument(QObject *parent = nullptr);
    ~ImageDocument();

    // Document properties
    void setImage(const QImage &image);
    QImage getImage() const { return m_image; }
    QSize getImageSize() const { return m_image.size(); }
    
    // File operations
    bool openDocument(const QString &filePath);
    bool saveDocument(const QString &filePath);
    
    // Document state
    void setModified(bool modified);
    bool isModified() const { return m_modified; }
    
    QString getFilePath() const { return m_filePath; }
    void setFilePath(const QString &filePath) { m_filePath = filePath; }
    
    // New document creation
    bool newDocument(int width, int height, const QColor &backgroundColor = Qt::white);
    
    // Export functionality
    bool exportImage(const QString &fileName, const QString &format = "PNG");
    
    // History management
    class QUndoStack* getHistoryManager();
    
    // Layer management
    int addLayer(class Layer *layer);
    void removeLayer(int index);
    class Layer* getLayer(int index) const;
    int getLayerCount() const;
    void setActiveLayer(int index);
    void moveLayer(int fromIndex, int toIndex);

signals:
    void imageChanged();
    void modifiedChanged(bool modified);
    void documentChanged();
    void imageSizeChanged(const QSize &size);

private:
    QImage m_image;
    QString m_filePath;
    bool m_modified;
    class QUndoStack *m_undoStack;
};

#endif // IMAGEDOCUMENT_H