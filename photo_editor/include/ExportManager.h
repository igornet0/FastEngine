#ifndef EXPORTMANAGER_H
#define EXPORTMANAGER_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QSize>
#include <QColor>
#include <QJsonObject>

class QPainter;
class QPrinter;
class QSvgGenerator;

struct ExportOptions {
    QString format;
    QString fileName;
    QSize size;
    int quality;
    QColor backgroundColor;
    bool preserveAspectRatio;
    bool includeMetadata;
    bool includeLayers;
    QString compression;
    int dpi;
    
    ExportOptions() 
        : quality(90)
        , backgroundColor(Qt::white)
        , preserveAspectRatio(true)
        , includeMetadata(true)
        , includeLayers(false)
        , compression("None")
        , dpi(300)
    {}
};

class ExportManager : public QObject
{
    Q_OBJECT

public:
    explicit ExportManager(QObject *parent = nullptr);
    ~ExportManager();

    // Basic export
    bool exportImage(const QImage &image, const ExportOptions &options);
    bool exportImage(const QImage &image, const QString &fileName, const QString &format = "PNG");
    
    // Format-specific export
    bool exportToPNG(const QImage &image, const QString &fileName, const ExportOptions &options = ExportOptions());
    bool exportToJPEG(const QImage &image, const QString &fileName, const ExportOptions &options = ExportOptions());
    bool exportToBMP(const QImage &image, const QString &fileName, const ExportOptions &options = ExportOptions());
    bool exportToTIFF(const QImage &image, const QString &fileName, const ExportOptions &options = ExportOptions());
    bool exportToPDF(const QImage &image, const QString &fileName, const ExportOptions &options = ExportOptions());
    bool exportToSVG(const QImage &image, const QString &fileName, const ExportOptions &options = ExportOptions());
    bool exportToWebP(const QImage &image, const QString &fileName, const ExportOptions &options = ExportOptions());
    
    // Advanced export
    bool exportWithLayers(const QList<QImage> &layers, const QString &fileName, const QString &format = "PNG");
    bool exportAsAnimation(const QList<QImage> &frames, const QString &fileName, int duration = 100);
    bool exportAsTiles(const QImage &image, const QString &baseFileName, const QSize &tileSize);
    
    // Batch export
    bool exportBatch(const QList<QImage> &images, const QStringList &fileNames, const QString &format = "PNG");
    bool exportBatch(const QList<QImage> &images, const QString &directory, const QString &format = "PNG");
    
    // Export options
    void setDefaultOptions(const ExportOptions &options);
    ExportOptions getDefaultOptions() const { return m_defaultOptions; }
    
    // Supported formats
    QStringList getSupportedFormats() const;
    QStringList getSupportedImageFormats() const;
    QStringList getSupportedVectorFormats() const;
    QStringList getSupportedAnimationFormats() const;
    
    // Format information
    QString getFormatDescription(const QString &format) const;
    QStringList getFormatExtensions(const QString &format) const;
    bool isFormatSupported(const QString &format) const;
    
    // Quality settings
    int getMaxQuality(const QString &format) const;
    int getMinQuality(const QString &format) const;
    QStringList getCompressionOptions(const QString &format) const;
    
    // Metadata
    bool addMetadata(const QString &fileName, const QJsonObject &metadata);
    QJsonObject extractMetadata(const QString &fileName) const;

signals:
    void exportStarted(const QString &fileName);
    void exportProgress(int percentage);
    void exportCompleted(const QString &fileName, bool success);
    void exportFailed(const QString &fileName, const QString &error);

private:
    bool saveImageWithOptions(const QImage &image, const QString &fileName, const ExportOptions &options);
    QImage prepareImageForExport(const QImage &image, const ExportOptions &options);
    void setupPrinter(QPrinter *printer, const ExportOptions &options);
    void setupSvgGenerator(QSvgGenerator *generator, const ExportOptions &options);
    QString getFileExtension(const QString &format) const;
    bool createDirectory(const QString &filePath) const;
    
    ExportOptions m_defaultOptions;
    QHash<QString, QString> m_formatDescriptions;
    QHash<QString, QStringList> m_formatExtensions;
    QHash<QString, int> m_maxQuality;
    QHash<QString, int> m_minQuality;
    QHash<QString, QStringList> m_compressionOptions;
    
    void initializeFormatInfo();
};

#endif // EXPORTMANAGER_H







