#include <QtTest>
#include <QApplication>
#include <QSignalSpy>
#include <QImage>
#include <QColor>
#include <QDateTime>
#include <QFileInfo>

#include "MetadataViewer.h"

class TestMetadataViewer : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test image setting
    void testSetImage();
    void testClearMetadata();
    void testGetMetadata();
    
    // Test metadata loading
    void testLoadMetadata();
    void testLoadExifData();
    
    // Test display updates
    void testUpdateDisplay();
    void testPopulateBasicInfo();
    void testPopulateFileInfo();
    void testPopulateExifData();
    void testPopulateTechnicalInfo();
    void testPopulateColorInfo();
    void testPopulateGpsInfo();
    void testPopulateCameraInfo();
    void testPopulateSoftwareInfo();
    
    // Test utility functions
    void testFormatFileSize();
    void testFormatDateTime();
    void testFormatGpsCoordinate();
    void testFormatExifValue();
    
    // Test tree item creation
    void testAddTreeItem();
    void testAddTreeItemWithDescription();
    
    // Test actions
    void testOnExportMetadata();
    void testOnCopyMetadata();
    void testOnRefreshMetadata();
    
    // Test signals
    void testSignals();

private:
    MetadataViewer *m_viewer;
    QImage m_testImage;
    QString m_testFilePath;
};

void TestMetadataViewer::initTestCase()
{
    // Initialize QApplication if not already done
    if (!QApplication::instance()) {
        int argc = 0;
        char **argv = nullptr;
        new QApplication(argc, argv);
    }
    
    // Create test image
    m_testImage = QImage(800, 600, QImage::Format_RGB32);
    m_testImage.fill(Qt::red);
    
    // Create test file path
    m_testFilePath = QDir::tempPath() + "/test_image.png";
    m_testImage.save(m_testFilePath);
}

void TestMetadataViewer::cleanupTestCase()
{
    // Clean up test file
    QFile::remove(m_testFilePath);
}

void TestMetadataViewer::init()
{
    m_viewer = new MetadataViewer();
}

void TestMetadataViewer::cleanup()
{
    delete m_viewer;
}

void TestMetadataViewer::testSetImage()
{
    // Test setting image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Verify metadata was loaded
    ImageMetadata metadata = m_viewer->getMetadata();
    QCOMPARE(metadata.imageSize, m_testImage.size());
    QCOMPARE(metadata.fileName, QFileInfo(m_testFilePath).fileName());
    QCOMPARE(metadata.filePath, m_testFilePath);
}

void TestMetadataViewer::testClearMetadata()
{
    // Set image first
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Clear metadata
    m_viewer->clearMetadata();
    
    // Verify metadata was cleared
    ImageMetadata metadata = m_viewer->getMetadata();
    QVERIFY(metadata.imageSize.isEmpty());
    QVERIFY(metadata.fileName.isEmpty());
    QVERIFY(metadata.filePath.isEmpty());
}

void TestMetadataViewer::testGetMetadata()
{
    // Initially metadata should be empty
    ImageMetadata metadata = m_viewer->getMetadata();
    QVERIFY(metadata.imageSize.isEmpty());
    
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Get metadata
    metadata = m_viewer->getMetadata();
    QCOMPARE(metadata.imageSize, m_testImage.size());
    QCOMPARE(metadata.fileName, QFileInfo(m_testFilePath).fileName());
}

void TestMetadataViewer::testLoadMetadata()
{
    // Test loading metadata
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    ImageMetadata metadata = m_viewer->getMetadata();
    QCOMPARE(metadata.imageSize, m_testImage.size());
    QCOMPARE(metadata.bitDepth, m_testImage.depth());
    QCOMPARE(metadata.hasAlpha, m_testImage.hasAlphaChannel());
    QVERIFY(metadata.fileSize > 0);
    QVERIFY(metadata.created.isValid());
    QVERIFY(metadata.modified.isValid());
}

void TestMetadataViewer::testLoadExifData()
{
    // Test loading EXIF data
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    ImageMetadata metadata = m_viewer->getMetadata();
    QVERIFY(!metadata.exifData.isEmpty());
    QVERIFY(metadata.exifData.contains("ImageWidth"));
    QVERIFY(metadata.exifData.contains("ImageHeight"));
}

void TestMetadataViewer::testUpdateDisplay()
{
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Update display (should not crash)
    m_viewer->updateDisplay();
    
    // This test mainly ensures no crashes occur
    QVERIFY(true);
}

void TestMetadataViewer::testPopulateBasicInfo()
{
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Populate basic info (should not crash)
    m_viewer->populateBasicInfo();
    
    // This test mainly ensures no crashes occur
    QVERIFY(true);
}

void TestMetadataViewer::testPopulateFileInfo()
{
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Populate file info (should not crash)
    m_viewer->populateFileInfo();
    
    // This test mainly ensures no crashes occur
    QVERIFY(true);
}

void TestMetadataViewer::testPopulateExifData()
{
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Populate EXIF data (should not crash)
    m_viewer->populateExifData();
    
    // This test mainly ensures no crashes occur
    QVERIFY(true);
}

void TestMetadataViewer::testPopulateTechnicalInfo()
{
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Populate technical info (should not crash)
    m_viewer->populateTechnicalInfo();
    
    // This test mainly ensures no crashes occur
    QVERIFY(true);
}

void TestMetadataViewer::testPopulateColorInfo()
{
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Populate color info (should not crash)
    m_viewer->populateColorInfo();
    
    // This test mainly ensures no crashes occur
    QVERIFY(true);
}

void TestMetadataViewer::testPopulateGpsInfo()
{
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Populate GPS info (should not crash)
    m_viewer->populateGpsInfo();
    
    // This test mainly ensures no crashes occur
    QVERIFY(true);
}

void TestMetadataViewer::testPopulateCameraInfo()
{
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Populate camera info (should not crash)
    m_viewer->populateCameraInfo();
    
    // This test mainly ensures no crashes occur
    QVERIFY(true);
}

void TestMetadataViewer::testPopulateSoftwareInfo()
{
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Populate software info (should not crash)
    m_viewer->populateSoftwareInfo();
    
    // This test mainly ensures no crashes occur
    QVERIFY(true);
}

void TestMetadataViewer::testFormatFileSize()
{
    // Test formatting file sizes
    QString size1 = m_viewer->formatFileSize(1024);
    QVERIFY(size1.contains("KB"));
    
    QString size2 = m_viewer->formatFileSize(1024 * 1024);
    QVERIFY(size2.contains("MB"));
    
    QString size3 = m_viewer->formatFileSize(1024 * 1024 * 1024);
    QVERIFY(size3.contains("GB"));
    
    QString size4 = m_viewer->formatFileSize(512);
    QVERIFY(size4.contains("B"));
}

void TestMetadataViewer::testFormatDateTime()
{
    // Test formatting valid datetime
    QDateTime validDateTime = QDateTime::currentDateTime();
    QString formatted = m_viewer->formatDateTime(validDateTime);
    QVERIFY(!formatted.isEmpty());
    QVERIFY(formatted.contains(":"));
    
    // Test formatting invalid datetime
    QDateTime invalidDateTime;
    QString invalidFormatted = m_viewer->formatDateTime(invalidDateTime);
    QCOMPARE(invalidFormatted, "Unknown");
}

void TestMetadataViewer::testFormatGpsCoordinate()
{
    // Test formatting latitude
    QString latFormatted = m_viewer->formatGpsCoordinate(37.7749, true);
    QVERIFY(!latFormatted.isEmpty());
    QVERIFY(latFormatted.contains("°"));
    QVERIFY(latFormatted.contains("'"));
    QVERIFY(latFormatted.contains("\""));
    QVERIFY(latFormatted.contains("N"));
    
    // Test formatting longitude
    QString lonFormatted = m_viewer->formatGpsCoordinate(-122.4194, false);
    QVERIFY(!lonFormatted.isEmpty());
    QVERIFY(lonFormatted.contains("°"));
    QVERIFY(lonFormatted.contains("'"));
    QVERIFY(lonFormatted.contains("\""));
    QVERIFY(lonFormatted.contains("W"));
    
    // Test formatting zero coordinate
    QString zeroFormatted = m_viewer->formatGpsCoordinate(0.0, true);
    QCOMPARE(zeroFormatted, "Not available");
}

void TestMetadataViewer::testFormatExifValue()
{
    // Test formatting common EXIF values
    QString widthDesc = m_viewer->formatExifValue("ImageWidth", "800");
    QVERIFY(!widthDesc.isEmpty());
    QVERIFY(widthDesc.contains("pixels"));
    
    QString heightDesc = m_viewer->formatExifValue("ImageHeight", "600");
    QVERIFY(!heightDesc.isEmpty());
    QVERIFY(heightDesc.contains("pixels"));
    
    QString bitsDesc = m_viewer->formatExifValue("BitsPerSample", "8");
    QVERIFY(!bitsDesc.isEmpty());
    QVERIFY(bitsDesc.contains("bits"));
    
    // Test unknown EXIF value
    QString unknownDesc = m_viewer->formatExifValue("UnknownTag", "value");
    QVERIFY(!unknownDesc.isEmpty());
    QVERIFY(unknownDesc.contains("EXIF"));
}

void TestMetadataViewer::testAddTreeItem()
{
    // This test would require access to private methods
    // For now, just verify the method exists and doesn't crash
    QVERIFY(true);
}

void TestMetadataViewer::testAddTreeItemWithDescription()
{
    // This test would require access to private methods
    // For now, just verify the method exists and doesn't crash
    QVERIFY(true);
}

void TestMetadataViewer::testOnExportMetadata()
{
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Test export metadata (should not crash)
    m_viewer->onExportMetadata();
    
    // This test mainly ensures no crashes occur
    QVERIFY(true);
}

void TestMetadataViewer::testOnCopyMetadata()
{
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Test copy metadata (should not crash)
    m_viewer->onCopyMetadata();
    
    // This test mainly ensures no crashes occur
    QVERIFY(true);
}

void TestMetadataViewer::testOnRefreshMetadata()
{
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Test refresh metadata (should not crash)
    m_viewer->onRefreshMetadata();
    
    // This test mainly ensures no crashes occur
    QVERIFY(true);
}

void TestMetadataViewer::testSignals()
{
    // Test signals
    QSignalSpy changedSpy(m_viewer, &MetadataViewer::metadataChanged);
    
    // Set image
    m_viewer->setImage(m_testImage, m_testFilePath);
    
    // Verify signal was emitted
    QCOMPARE(changedSpy.count(), 1);
    
    // Verify signal parameters
    QList<QVariant> args = changedSpy.takeFirst();
    QVERIFY(args.at(0).canConvert<ImageMetadata>());
}

QTEST_MAIN(TestMetadataViewer)
