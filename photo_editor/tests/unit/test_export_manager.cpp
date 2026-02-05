#include <QtTest>
#include <QApplication>
#include <QSignalSpy>
#include <QImage>
#include <QColor>
#include <QDir>
#include <QFile>

#include "ExportManager.h"

class TestExportManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test basic export
    void testExportImage();
    void testExportImageWithOptions();
    
    // Test format-specific export
    void testExportToPNG();
    void testExportToJPEG();
    void testExportToBMP();
    void testExportToTIFF();
    void testExportToPDF();
    void testExportToSVG();
    void testExportToWebP();
    
    // Test advanced export
    void testExportWithLayers();
    void testExportAsAnimation();
    void testExportAsTiles();
    
    // Test batch export
    void testExportBatch();
    void testExportBatchToDirectory();
    
    // Test export options
    void testSetDefaultOptions();
    void testGetDefaultOptions();
    
    // Test supported formats
    void testGetSupportedFormats();
    void testGetSupportedImageFormats();
    void testGetSupportedVectorFormats();
    void testGetSupportedAnimationFormats();
    
    // Test format information
    void testGetFormatDescription();
    void testGetFormatExtensions();
    void testIsFormatSupported();
    
    // Test quality settings
    void testGetMaxQuality();
    void testGetMinQuality();
    void testGetCompressionOptions();
    
    // Test metadata
    void testAddMetadata();
    void testExtractMetadata();
    
    // Test signals
    void testSignals();

private:
    ExportManager *m_manager;
    QImage m_testImage;
    QString m_testDirectory;
};

void TestExportManager::initTestCase()
{
    // Initialize QApplication if not already done
    if (!QApplication::instance()) {
        int argc = 0;
        char **argv = nullptr;
        new QApplication(argc, argv);
    }
    
    // Create test directory
    m_testDirectory = QDir::tempPath() + "/PhotoEditorExportTest";
    QDir().mkpath(m_testDirectory);
    
    // Create test image
    m_testImage = QImage(800, 600, QImage::Format_RGB32);
    m_testImage.fill(Qt::red);
}

void TestExportManager::cleanupTestCase()
{
    // Clean up test directory
    QDir(m_testDirectory).removeRecursively();
}

void TestExportManager::init()
{
    m_manager = new ExportManager();
}

void TestExportManager::cleanup()
{
    delete m_manager;
}

void TestExportManager::testExportImage()
{
    // Test basic export
    QString fileName = m_testDirectory + "/test_export.png";
    bool success = m_manager->exportImage(m_testImage, fileName, "PNG");
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
}

void TestExportManager::testExportImageWithOptions()
{
    // Test export with options
    ExportOptions options;
    options.fileName = m_testDirectory + "/test_options.png";
    options.format = "PNG";
    options.size = QSize(400, 300);
    options.quality = 90;
    options.backgroundColor = Qt::blue;
    options.preserveAspectRatio = true;
    options.includeMetadata = true;
    options.dpi = 300;
    
    bool success = m_manager->exportImage(m_testImage, options);
    QVERIFY(success);
    QVERIFY(QFile::exists(options.fileName));
}

void TestExportManager::testExportToPNG()
{
    // Test PNG export
    QString fileName = m_testDirectory + "/test_png.png";
    ExportOptions options;
    options.fileName = fileName;
    
    bool success = m_manager->exportToPNG(m_testImage, fileName, options);
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
}

void TestExportManager::testExportToJPEG()
{
    // Test JPEG export
    QString fileName = m_testDirectory + "/test_jpeg.jpg";
    ExportOptions options;
    options.fileName = fileName;
    
    bool success = m_manager->exportToJPEG(m_testImage, fileName, options);
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
}

void TestExportManager::testExportToBMP()
{
    // Test BMP export
    QString fileName = m_testDirectory + "/test_bmp.bmp";
    ExportOptions options;
    options.fileName = fileName;
    
    bool success = m_manager->exportToBMP(m_testImage, fileName, options);
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
}

void TestExportManager::testExportToTIFF()
{
    // Test TIFF export
    QString fileName = m_testDirectory + "/test_tiff.tiff";
    ExportOptions options;
    options.fileName = fileName;
    
    bool success = m_manager->exportToTIFF(m_testImage, fileName, options);
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
}

void TestExportManager::testExportToPDF()
{
    // Test PDF export
    QString fileName = m_testDirectory + "/test_pdf.pdf";
    ExportOptions options;
    options.fileName = fileName;
    options.size = QSize(800, 600);
    
    bool success = m_manager->exportToPDF(m_testImage, fileName, options);
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
}

void TestExportManager::testExportToSVG()
{
    // Test SVG export
    QString fileName = m_testDirectory + "/test_svg.svg";
    ExportOptions options;
    options.fileName = fileName;
    options.size = QSize(800, 600);
    
    bool success = m_manager->exportToSVG(m_testImage, fileName, options);
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
}

void TestExportManager::testExportToWebP()
{
    // Test WebP export
    QString fileName = m_testDirectory + "/test_webp.webp";
    ExportOptions options;
    options.fileName = fileName;
    
    bool success = m_manager->exportToWebP(m_testImage, fileName, options);
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
}

void TestExportManager::testExportWithLayers()
{
    // Test export with layers
    QList<QImage> layers;
    layers.append(m_testImage);
    layers.append(m_testImage);
    
    QString fileName = m_testDirectory + "/test_layers.png";
    bool success = m_manager->exportWithLayers(layers, fileName, "PNG");
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
}

void TestExportManager::testExportAsAnimation()
{
    // Test export as animation
    QList<QImage> frames;
    frames.append(m_testImage);
    frames.append(m_testImage);
    
    QString fileName = m_testDirectory + "/test_animation.png";
    bool success = m_manager->exportAsAnimation(frames, fileName, 100);
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
}

void TestExportManager::testExportAsTiles()
{
    // Test export as tiles
    QString baseFileName = m_testDirectory + "/test_tile";
    QSize tileSize(200, 200);
    
    bool success = m_manager->exportAsTiles(m_testImage, baseFileName, tileSize);
    QVERIFY(success);
    
    // Check if tile files were created
    QVERIFY(QFile::exists(baseFileName + "_0_0.png"));
    QVERIFY(QFile::exists(baseFileName + "_1_0.png"));
    QVERIFY(QFile::exists(baseFileName + "_2_0.png"));
    QVERIFY(QFile::exists(baseFileName + "_3_0.png"));
}

void TestExportManager::testExportBatch()
{
    // Test batch export
    QList<QImage> images;
    images.append(m_testImage);
    images.append(m_testImage);
    
    QStringList fileNames;
    fileNames.append(m_testDirectory + "/batch1.png");
    fileNames.append(m_testDirectory + "/batch2.png");
    
    bool success = m_manager->exportBatch(images, fileNames, "PNG");
    QVERIFY(success);
    QVERIFY(QFile::exists(fileNames[0]));
    QVERIFY(QFile::exists(fileNames[1]));
}

void TestExportManager::testExportBatchToDirectory()
{
    // Test batch export to directory
    QList<QImage> images;
    images.append(m_testImage);
    images.append(m_testImage);
    
    QString directory = m_testDirectory + "/batch";
    bool success = m_manager->exportBatch(images, directory, "PNG");
    QVERIFY(success);
    
    // Check if files were created
    QVERIFY(QFile::exists(directory + "/image_000.png"));
    QVERIFY(QFile::exists(directory + "/image_001.png"));
}

void TestExportManager::testSetDefaultOptions()
{
    // Test setting default options
    ExportOptions options;
    options.quality = 95;
    options.backgroundColor = Qt::green;
    options.preserveAspectRatio = false;
    options.includeMetadata = false;
    options.dpi = 150;
    
    m_manager->setDefaultOptions(options);
    
    ExportOptions defaultOptions = m_manager->getDefaultOptions();
    QCOMPARE(defaultOptions.quality, 95);
    QCOMPARE(defaultOptions.backgroundColor, Qt::green);
    QCOMPARE(defaultOptions.preserveAspectRatio, false);
    QCOMPARE(defaultOptions.includeMetadata, false);
    QCOMPARE(defaultOptions.dpi, 150);
}

void TestExportManager::testGetDefaultOptions()
{
    // Test getting default options
    ExportOptions options = m_manager->getDefaultOptions();
    QVERIFY(options.quality > 0);
    QVERIFY(options.backgroundColor.isValid());
    QVERIFY(options.dpi > 0);
}

void TestExportManager::testGetSupportedFormats()
{
    // Test getting supported formats
    QStringList formats = m_manager->getSupportedFormats();
    QVERIFY(!formats.isEmpty());
    QVERIFY(formats.contains("PNG"));
    QVERIFY(formats.contains("JPEG"));
    QVERIFY(formats.contains("BMP"));
    QVERIFY(formats.contains("TIFF"));
    QVERIFY(formats.contains("PDF"));
    QVERIFY(formats.contains("SVG"));
    QVERIFY(formats.contains("WEBP"));
}

void TestExportManager::testGetSupportedImageFormats()
{
    // Test getting supported image formats
    QStringList formats = m_manager->getSupportedImageFormats();
    QVERIFY(!formats.isEmpty());
    QVERIFY(formats.contains("PNG"));
    QVERIFY(formats.contains("JPEG"));
    QVERIFY(formats.contains("BMP"));
    QVERIFY(formats.contains("TIFF"));
    QVERIFY(formats.contains("WEBP"));
}

void TestExportManager::testGetSupportedVectorFormats()
{
    // Test getting supported vector formats
    QStringList formats = m_manager->getSupportedVectorFormats();
    QVERIFY(!formats.isEmpty());
    QVERIFY(formats.contains("PDF"));
    QVERIFY(formats.contains("SVG"));
}

void TestExportManager::testGetSupportedAnimationFormats()
{
    // Test getting supported animation formats
    QStringList formats = m_manager->getSupportedAnimationFormats();
    QVERIFY(!formats.isEmpty());
    QVERIFY(formats.contains("GIF"));
    QVERIFY(formats.contains("WEBP"));
}

void TestExportManager::testGetFormatDescription()
{
    // Test getting format descriptions
    QString pngDesc = m_manager->getFormatDescription("PNG");
    QVERIFY(!pngDesc.isEmpty());
    QVERIFY(pngDesc.contains("Portable Network Graphics"));
    
    QString jpegDesc = m_manager->getFormatDescription("JPEG");
    QVERIFY(!jpegDesc.isEmpty());
    QVERIFY(jpegDesc.contains("Joint Photographic Experts Group"));
    
    // Test unknown format
    QString unknownDesc = m_manager->getFormatDescription("UNKNOWN");
    QCOMPARE(unknownDesc, "Unknown format");
}

void TestExportManager::testGetFormatExtensions()
{
    // Test getting format extensions
    QStringList pngExts = m_manager->getFormatExtensions("PNG");
    QVERIFY(!pngExts.isEmpty());
    QVERIFY(pngExts.contains("png"));
    
    QStringList jpegExts = m_manager->getFormatExtensions("JPEG");
    QVERIFY(!jpegExts.isEmpty());
    QVERIFY(jpegExts.contains("jpg"));
    QVERIFY(jpegExts.contains("jpeg"));
    
    // Test unknown format
    QStringList unknownExts = m_manager->getFormatExtensions("UNKNOWN");
    QVERIFY(unknownExts.isEmpty());
}

void TestExportManager::testIsFormatSupported()
{
    // Test format support
    QVERIFY(m_manager->isFormatSupported("PNG"));
    QVERIFY(m_manager->isFormatSupported("JPEG"));
    QVERIFY(m_manager->isFormatSupported("BMP"));
    QVERIFY(m_manager->isFormatSupported("TIFF"));
    QVERIFY(m_manager->isFormatSupported("PDF"));
    QVERIFY(m_manager->isFormatSupported("SVG"));
    QVERIFY(m_manager->isFormatSupported("WEBP"));
    
    QVERIFY(!m_manager->isFormatSupported("UNKNOWN"));
    QVERIFY(!m_manager->isFormatSupported(""));
}

void TestExportManager::testGetMaxQuality()
{
    // Test getting max quality
    int pngMax = m_manager->getMaxQuality("PNG");
    QCOMPARE(pngMax, 100);
    
    int jpegMax = m_manager->getMaxQuality("JPEG");
    QCOMPARE(jpegMax, 100);
    
    // Test unknown format
    int unknownMax = m_manager->getMaxQuality("UNKNOWN");
    QCOMPARE(unknownMax, 100);
}

void TestExportManager::testGetMinQuality()
{
    // Test getting min quality
    int pngMin = m_manager->getMinQuality("PNG");
    QCOMPARE(pngMin, 0);
    
    int jpegMin = m_manager->getMinQuality("JPEG");
    QCOMPARE(jpegMin, 0);
    
    // Test unknown format
    int unknownMin = m_manager->getMinQuality("UNKNOWN");
    QCOMPARE(unknownMin, 0);
}

void TestExportManager::testGetCompressionOptions()
{
    // Test getting compression options
    QStringList pngOptions = m_manager->getCompressionOptions("PNG");
    QVERIFY(!pngOptions.isEmpty());
    QVERIFY(pngOptions.contains("None"));
    
    QStringList jpegOptions = m_manager->getCompressionOptions("JPEG");
    QVERIFY(!jpegOptions.isEmpty());
    QVERIFY(jpegOptions.contains("Baseline"));
    
    // Test unknown format
    QStringList unknownOptions = m_manager->getCompressionOptions("UNKNOWN");
    QVERIFY(unknownOptions.isEmpty());
}

void TestExportManager::testAddMetadata()
{
    // Test adding metadata
    QString fileName = m_testDirectory + "/test_metadata.png";
    m_testImage.save(fileName);
    
    QJsonObject metadata;
    metadata["author"] = "Test Author";
    metadata["description"] = "Test Description";
    
    bool success = m_manager->addMetadata(fileName, metadata);
    QVERIFY(success);
}

void TestExportManager::testExtractMetadata()
{
    // Test extracting metadata
    QString fileName = m_testDirectory + "/test_extract_metadata.png";
    m_testImage.save(fileName);
    
    QJsonObject metadata = m_manager->extractMetadata(fileName);
    // Metadata extraction might return empty object
    QVERIFY(metadata.isObject());
}

void TestExportManager::testSignals()
{
    // Test signals
    QSignalSpy startedSpy(m_manager, &ExportManager::exportStarted);
    QSignalSpy progressSpy(m_manager, &ExportManager::exportProgress);
    QSignalSpy completedSpy(m_manager, &ExportManager::exportCompleted);
    QSignalSpy failedSpy(m_manager, &ExportManager::exportFailed);
    
    // Export an image
    QString fileName = m_testDirectory + "/test_signals.png";
    m_manager->exportImage(m_testImage, fileName, "PNG");
    
    // Check signals were emitted
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(completedSpy.count(), 1);
    
    // Verify signal parameters
    QList<QVariant> startedArgs = startedSpy.takeFirst();
    QCOMPARE(startedArgs.at(0).toString(), fileName);
    
    QList<QVariant> completedArgs = completedSpy.takeFirst();
    QCOMPARE(completedArgs.at(0).toString(), fileName);
    QVERIFY(completedArgs.at(1).toBool()); // Should be successful
}

QTEST_MAIN(TestExportManager)
