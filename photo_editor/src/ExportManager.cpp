#include "ExportManager.h"
#include <QImageWriter>
#include <QImageReader>
#include <QPrinter>
#include <QSvgGenerator>
#include <QPainter>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QBuffer>
#include <QIODevice>

ExportManager::ExportManager(QObject *parent)
    : QObject(parent)
{
    initializeFormatInfo();
}

ExportManager::~ExportManager()
{
}

bool ExportManager::exportImage(const QImage &image, const ExportOptions &options)
{
    if (image.isNull()) {
        emit exportFailed(options.fileName, "Invalid image");
        return false;
    }
    
    emit exportStarted(options.fileName);
    
    bool success = false;
    
    if (options.format.toUpper() == "PNG") {
        success = exportToPNG(image, options.fileName, options);
    } else if (options.format.toUpper() == "JPEG" || options.format.toUpper() == "JPG") {
        success = exportToJPEG(image, options.fileName, options);
    } else if (options.format.toUpper() == "BMP") {
        success = exportToBMP(image, options.fileName, options);
    } else if (options.format.toUpper() == "TIFF") {
        success = exportToTIFF(image, options.fileName, options);
    } else if (options.format.toUpper() == "PDF") {
        success = exportToPDF(image, options.fileName, options);
    } else if (options.format.toUpper() == "SVG") {
        success = exportToSVG(image, options.fileName, options);
    } else if (options.format.toUpper() == "WEBP") {
        success = exportToWebP(image, options.fileName, options);
    } else {
        // Try generic export
        success = saveImageWithOptions(image, options.fileName, options);
    }
    
    if (success) {
        emit exportCompleted(options.fileName, true);
    } else {
        emit exportFailed(options.fileName, "Export failed");
    }
    
    return success;
}

bool ExportManager::exportImage(const QImage &image, const QString &fileName, const QString &format)
{
    ExportOptions options = m_defaultOptions;
    options.fileName = fileName;
    options.format = format;
    return exportImage(image, options);
}

bool ExportManager::exportToPNG(const QImage &image, const QString &fileName, const ExportOptions &options)
{
    QImage exportImage = prepareImageForExport(image, options);
    
    QImageWriter writer(fileName, "PNG");
    writer.setQuality(options.quality);
    writer.setCompression(0); // PNG uses lossless compression
    
    if (options.includeMetadata) {
        // Add metadata if needed
        writer.setText("Software", "Photo Editor");
        writer.setText("Description", "Exported from Photo Editor");
    }
    
    bool success = writer.write(exportImage);
    if (!success) {
        qDebug() << "PNG export failed:" << writer.errorString();
    }
    
    return success;
}

bool ExportManager::exportToJPEG(const QImage &image, const QString &fileName, const ExportOptions &options)
{
    QImage exportImage = prepareImageForExport(image, options);
    
    // Convert to RGB if necessary (JPEG doesn't support alpha)
    if (exportImage.hasAlphaChannel()) {
        QImage rgbImage(exportImage.size(), QImage::Format_RGB32);
        rgbImage.fill(options.backgroundColor);
        QPainter painter(&rgbImage);
        painter.drawImage(0, 0, exportImage);
        exportImage = rgbImage;
    }
    
    QImageWriter writer(fileName, "JPEG");
    writer.setQuality(options.quality);
    
    if (options.includeMetadata) {
        writer.setText("Software", "Photo Editor");
        writer.setText("Description", "Exported from Photo Editor");
    }
    
    bool success = writer.write(exportImage);
    if (!success) {
        qDebug() << "JPEG export failed:" << writer.errorString();
    }
    
    return success;
}

bool ExportManager::exportToBMP(const QImage &image, const QString &fileName, const ExportOptions &options)
{
    QImage exportImage = prepareImageForExport(image, options);
    
    QImageWriter writer(fileName, "BMP");
    writer.setQuality(options.quality);
    
    bool success = writer.write(exportImage);
    if (!success) {
        qDebug() << "BMP export failed:" << writer.errorString();
    }
    
    return success;
}

bool ExportManager::exportToTIFF(const QImage &image, const QString &fileName, const ExportOptions &options)
{
    QImage exportImage = prepareImageForExport(image, options);
    
    QImageWriter writer(fileName, "TIFF");
    writer.setQuality(options.quality);
    
    if (options.compression == "LZW") {
        writer.setCompression(1);
    } else if (options.compression == "JPEG") {
        writer.setCompression(7);
    } else {
        writer.setCompression(0); // No compression
    }
    
    if (options.includeMetadata) {
        writer.setText("Software", "Photo Editor");
        writer.setText("Description", "Exported from Photo Editor");
    }
    
    bool success = writer.write(exportImage);
    if (!success) {
        qDebug() << "TIFF export failed:" << writer.errorString();
    }
    
    return success;
}

bool ExportManager::exportToPDF(const QImage &image, const QString &fileName, const ExportOptions &options)
{
    QPrinter printer(QPrinter::HighResolution);
    setupPrinter(&printer, options);
    
    QPainter painter(&printer);
    
    // Calculate image size to fit page while preserving aspect ratio
    QSizeF pageSizeF = printer.pageRect(QPrinter::DevicePixel).size();
    QSize pageSize = pageSizeF.toSize();
    QSize imageSize = image.size();
    
    if (options.preserveAspectRatio) {
        double scaleX = static_cast<double>(pageSize.width()) / imageSize.width();
        double scaleY = static_cast<double>(pageSize.height()) / imageSize.height();
        double scale = qMin(scaleX, scaleY);
        
        imageSize = QSize(
            static_cast<int>(imageSize.width() * scale),
            static_cast<int>(imageSize.height() * scale)
        );
    } else {
        imageSize = pageSize;
    }
    
    // Center image on page
    int x = (pageSize.width() - imageSize.width()) / 2;
    int y = (pageSize.height() - imageSize.height()) / 2;
    
    // Draw background if needed
    if (options.backgroundColor != Qt::transparent) {
        painter.fillRect(QRect(0, 0, pageSize.width(), pageSize.height()), options.backgroundColor);
    }
    
    // Draw image
    painter.drawImage(QRect(x, y, imageSize.width(), imageSize.height()), image);
    
    bool success = painter.end();
    
    if (!success) {
        qWarning() << "Failed to export to PDF:" << fileName;
    }
    
    return success;
}

bool ExportManager::exportToSVG(const QImage &image, const QString &fileName, const ExportOptions &options)
{
    QSvgGenerator generator;
    setupSvgGenerator(&generator, options);
    
    QPainter painter(&generator);
    
    // Draw background if needed
    if (options.backgroundColor != Qt::transparent) {
        painter.fillRect(image.rect(), options.backgroundColor);
    }
    
    // Draw image
    painter.drawImage(0, 0, image);
    
    bool success = painter.end();
    if (!success) {
        qDebug() << "SVG export failed";
    }
    
    return success;
}

bool ExportManager::exportToWebP(const QImage &image, const QString &fileName, const ExportOptions &options)
{
    QImage exportImage = prepareImageForExport(image, options);
    
    QImageWriter writer(fileName, "WEBP");
    writer.setQuality(options.quality);
    
    if (options.compression == "Lossless") {
        writer.setCompression(0);
    } else {
        writer.setCompression(1); // Lossy compression
    }
    
    if (options.includeMetadata) {
        writer.setText("Software", "Photo Editor");
        writer.setText("Description", "Exported from Photo Editor");
    }
    
    bool success = writer.write(exportImage);
    if (!success) {
        qDebug() << "WebP export failed:" << writer.errorString();
    }
    
    return success;
}

bool ExportManager::exportWithLayers(const QList<QImage> &layers, const QString &fileName, const QString &format)
{
    if (layers.isEmpty()) {
        return false;
    }
    
    // For now, just export the top layer
    // In a full implementation, you might want to create a multi-layer format
    return exportImage(layers.last(), fileName, format);
}

bool ExportManager::exportAsAnimation(const QList<QImage> &frames, const QString &fileName, int duration)
{
    if (frames.isEmpty()) {
        return false;
    }
    
    // For GIF animation, we would need additional libraries
    // For now, just export the first frame
    return exportImage(frames.first(), fileName, "PNG");
}

bool ExportManager::exportAsTiles(const QImage &image, const QString &baseFileName, const QSize &tileSize)
{
    if (image.isNull() || tileSize.isEmpty()) {
        return false;
    }
    
    QFileInfo fileInfo(baseFileName);
    QString baseName = fileInfo.baseName();
    QString directory = fileInfo.absolutePath();
    QString extension = fileInfo.suffix();
    
    int tilesX = (image.width() + tileSize.width() - 1) / tileSize.width();
    int tilesY = (image.height() + tileSize.height() - 1) / tileSize.height();
    
    bool allSuccess = true;
    
    for (int y = 0; y < tilesY; ++y) {
        for (int x = 0; x < tilesX; ++x) {
            QRect tileRect(
                x * tileSize.width(),
                y * tileSize.height(),
                qMin(tileSize.width(), image.width() - x * tileSize.width()),
                qMin(tileSize.height(), image.height() - y * tileSize.height())
            );
            
            QImage tile = image.copy(tileRect);
            QString tileFileName = QString("%1/%2_%3_%4.%5")
                .arg(directory)
                .arg(baseName)
                .arg(x)
                .arg(y)
                .arg(extension);
            
            if (!exportImage(tile, tileFileName, "PNG")) {
                allSuccess = false;
            }
        }
    }
    
    return allSuccess;
}

bool ExportManager::exportBatch(const QList<QImage> &images, const QStringList &fileNames, const QString &format)
{
    if (images.size() != fileNames.size()) {
        return false;
    }
    
    bool allSuccess = true;
    
    for (int i = 0; i < images.size(); ++i) {
        if (!exportImage(images[i], fileNames[i], format)) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

bool ExportManager::exportBatch(const QList<QImage> &images, const QString &directory, const QString &format)
{
    if (!QDir().mkpath(directory)) {
        return false;
    }
    
    QStringList fileNames;
    for (int i = 0; i < images.size(); ++i) {
        QString fileName = QString("%1/image_%2.%3")
            .arg(directory)
            .arg(i, 3, 10, QChar('0'))
            .arg(format.toLower());
        fileNames.append(fileName);
    }
    
    return exportBatch(images, fileNames, format);
}

void ExportManager::setDefaultOptions(const ExportOptions &options)
{
    m_defaultOptions = options;
}

QStringList ExportManager::getSupportedFormats() const
{
    return QStringList() << "PNG" << "JPEG" << "BMP" << "TIFF" << "PDF" << "SVG" << "WEBP";
}

QStringList ExportManager::getSupportedImageFormats() const
{
    return QStringList() << "PNG" << "JPEG" << "BMP" << "TIFF" << "WEBP";
}

QStringList ExportManager::getSupportedVectorFormats() const
{
    return QStringList() << "PDF" << "SVG";
}

QStringList ExportManager::getSupportedAnimationFormats() const
{
    return QStringList() << "GIF" << "WEBP";
}

QString ExportManager::getFormatDescription(const QString &format) const
{
    return m_formatDescriptions.value(format.toUpper(), "Unknown format");
}

QStringList ExportManager::getFormatExtensions(const QString &format) const
{
    return m_formatExtensions.value(format.toUpper(), QStringList());
}

bool ExportManager::isFormatSupported(const QString &format) const
{
    return getSupportedFormats().contains(format.toUpper());
}

int ExportManager::getMaxQuality(const QString &format) const
{
    return m_maxQuality.value(format.toUpper(), 100);
}

int ExportManager::getMinQuality(const QString &format) const
{
    return m_minQuality.value(format.toUpper(), 0);
}

QStringList ExportManager::getCompressionOptions(const QString &format) const
{
    return m_compressionOptions.value(format.toUpper(), QStringList());
}

bool ExportManager::addMetadata(const QString &fileName, const QJsonObject &metadata)
{
    // This would require additional libraries like Exiv2
    // For now, just return true
    Q_UNUSED(fileName)
    Q_UNUSED(metadata)
    return true;
}

QJsonObject ExportManager::extractMetadata(const QString &fileName) const
{
    // This would require additional libraries like Exiv2
    // For now, return empty object
    Q_UNUSED(fileName)
    return QJsonObject();
}

bool ExportManager::saveImageWithOptions(const QImage &image, const QString &fileName, const ExportOptions &options)
{
    QImage exportImage = prepareImageForExport(image, options);
    
    QImageWriter writer(fileName);
    writer.setQuality(options.quality);
    
    if (options.includeMetadata) {
        writer.setText("Software", "Photo Editor");
        writer.setText("Description", "Exported from Photo Editor");
    }
    
    return writer.write(exportImage);
}

QImage ExportManager::prepareImageForExport(const QImage &image, const ExportOptions &options)
{
    QImage result = image;
    
    // Resize if needed
    if (!options.size.isEmpty() && options.size != image.size()) {
        if (options.preserveAspectRatio) {
            result = image.scaled(options.size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
            result = image.scaled(options.size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
    }
    
    // Set DPI if needed
    if (options.dpi > 0) {
        result.setDotsPerMeterX(static_cast<int>(options.dpi * 39.3701));
        result.setDotsPerMeterY(static_cast<int>(options.dpi * 39.3701));
    }
    
    return result;
}

void ExportManager::setupPrinter(QPrinter *printer, const ExportOptions &options)
{
    printer->setOutputFileName(options.fileName);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setResolution(options.dpi);
    printer->setPageSize(QPageSize(QPageSize::A4));
    printer->setPageMargins(QMarginsF(0, 0, 0, 0), QPageLayout::Millimeter);
}

void ExportManager::setupSvgGenerator(QSvgGenerator *generator, const ExportOptions &options)
{
    generator->setFileName(options.fileName);
    generator->setSize(options.size.isEmpty() ? QSize(800, 600) : options.size);
    generator->setViewBox(QRect(0, 0, options.size.width(), options.size.height()));
    generator->setTitle("Exported from Photo Editor");
    generator->setDescription("Image exported from Photo Editor");
}

QString ExportManager::getFileExtension(const QString &format) const
{
    QStringList extensions = getFormatExtensions(format);
    return extensions.isEmpty() ? format.toLower() : extensions.first();
}

bool ExportManager::createDirectory(const QString &filePath) const
{
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.dir();
    return dir.exists() || dir.mkpath(".");
}

void ExportManager::initializeFormatInfo()
{
    // PNG
    m_formatDescriptions["PNG"] = "Portable Network Graphics - Lossless compression with transparency support";
    m_formatExtensions["PNG"] = QStringList() << "png";
    m_maxQuality["PNG"] = 100;
    m_minQuality["PNG"] = 0;
    m_compressionOptions["PNG"] = QStringList() << "None" << "Fast" << "Best";
    
    // JPEG
    m_formatDescriptions["JPEG"] = "Joint Photographic Experts Group - Lossy compression for photographs";
    m_formatExtensions["JPEG"] = QStringList() << "jpg" << "jpeg";
    m_maxQuality["JPEG"] = 100;
    m_minQuality["JPEG"] = 0;
    m_compressionOptions["JPEG"] = QStringList() << "Baseline" << "Progressive";
    
    // BMP
    m_formatDescriptions["BMP"] = "Windows Bitmap - Uncompressed bitmap format";
    m_formatExtensions["BMP"] = QStringList() << "bmp";
    m_maxQuality["BMP"] = 100;
    m_minQuality["BMP"] = 0;
    m_compressionOptions["BMP"] = QStringList() << "None";
    
    // TIFF
    m_formatDescriptions["TIFF"] = "Tagged Image File Format - Flexible format with compression options";
    m_formatExtensions["TIFF"] = QStringList() << "tiff" << "tif";
    m_maxQuality["TIFF"] = 100;
    m_minQuality["TIFF"] = 0;
    m_compressionOptions["TIFF"] = QStringList() << "None" << "LZW" << "JPEG";
    
    // PDF
    m_formatDescriptions["PDF"] = "Portable Document Format - Vector and raster graphics";
    m_formatExtensions["PDF"] = QStringList() << "pdf";
    m_maxQuality["PDF"] = 100;
    m_minQuality["PDF"] = 0;
    m_compressionOptions["PDF"] = QStringList() << "None" << "JPEG" << "LZW";
    
    // SVG
    m_formatDescriptions["SVG"] = "Scalable Vector Graphics - Vector graphics format";
    m_formatExtensions["SVG"] = QStringList() << "svg";
    m_maxQuality["SVG"] = 100;
    m_minQuality["SVG"] = 0;
    m_compressionOptions["SVG"] = QStringList() << "None";
    
    // WebP
    m_formatDescriptions["WEBP"] = "WebP - Modern image format with lossy and lossless compression";
    m_formatExtensions["WEBP"] = QStringList() << "webp";
    m_maxQuality["WEBP"] = 100;
    m_minQuality["WEBP"] = 0;
    m_compressionOptions["WEBP"] = QStringList() << "Lossy" << "Lossless";
}
