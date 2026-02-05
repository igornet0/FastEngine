#include "ImageDocument.h"
#include <QImageReader>
#include <QImageWriter>
#include <QFileInfo>
#include <QUndoStack>

ImageDocument::ImageDocument(QObject *parent)
    : QObject(parent)
    , m_modified(false)
    , m_undoStack(new QUndoStack(this))
{
}

ImageDocument::~ImageDocument()
{
}

void ImageDocument::setImage(const QImage &image)
{
    m_image = image;
    emit imageChanged();
    setModified(true);
}

bool ImageDocument::openDocument(const QString &filePath)
{
    QImageReader reader(filePath);
    QImage image = reader.read();
    
    if (image.isNull()) {
        return false;
    }
    
    m_image = image;
    m_filePath = filePath;
    m_modified = false;
    
    emit imageChanged();
    emit modifiedChanged(false);
    
    return true;
}

bool ImageDocument::saveDocument(const QString &filePath)
{
    QImageWriter writer(filePath);
    bool success = writer.write(m_image);
    
    if (success) {
        m_filePath = filePath;
        m_modified = false;
        emit modifiedChanged(false);
    }
    
    return success;
}

void ImageDocument::setModified(bool modified)
{
    if (m_modified != modified) {
        m_modified = modified;
        emit modifiedChanged(modified);
    }
}

bool ImageDocument::newDocument(int width, int height, const QColor &backgroundColor)
{
    QImage newImage(width, height, QImage::Format_ARGB32);
    newImage.fill(backgroundColor);
    
    m_image = newImage;
    m_modified = false;
    m_filePath.clear();
    
    emit imageChanged();
    emit modifiedChanged(false);
    
    return true;
}

bool ImageDocument::exportImage(const QString &fileName, const QString &format)
{
    QImageWriter writer(fileName);
    writer.setFormat(format.toUtf8());
    return writer.write(m_image);
}

QUndoStack* ImageDocument::getHistoryManager()
{
    return m_undoStack;
}

int ImageDocument::addLayer(Layer *layer)
{
    Q_UNUSED(layer);
    // TODO: Implement layer addition
    return 0;
}

void ImageDocument::removeLayer(int index)
{
    Q_UNUSED(index);
    // TODO: Implement layer removal
}

Layer* ImageDocument::getLayer(int index) const
{
    Q_UNUSED(index);
    // TODO: Implement layer retrieval
    return nullptr;
}

int ImageDocument::getLayerCount() const
{
    // TODO: Implement layer count
    return 0;
}

void ImageDocument::setActiveLayer(int index)
{
    Q_UNUSED(index);
    // TODO: Implement active layer setting
}

void ImageDocument::moveLayer(int fromIndex, int toIndex)
{
    Q_UNUSED(fromIndex);
    Q_UNUSED(toIndex);
    // TODO: Implement layer moving
}