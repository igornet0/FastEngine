#include <QtTest>
#include <QApplication>
#include <QSignalSpy>
#include <QImage>
#include <QColor>
#include <QClipboard>
#include <QMimeData>
#include <QJsonObject>

#include "ClipboardHistory.h"

class TestClipboardHistory : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test history management
    void testAddItem();
    void testAddImage();
    void testAddText();
    void testRemoveItem();
    void testClearHistory();
    
    // Test item access
    void testGetItem();
    void testGetImage();
    void testGetText();
    void testGetItemCount();
    
    // Test history operations
    void testSetMaxItems();
    void testGetMaxItems();
    void testSetEnabled();
    void testIsEnabled();
    
    // Test clipboard monitoring
    void testStartMonitoring();
    void testStopMonitoring();
    void testIsMonitoring();
    
    // Test search and filter
    void testSearchItems();
    void testGetItemsByType();
    void testGetRecentItems();
    
    // Test export/import
    void testExportHistory();
    void testImportHistory();
    
    // Test serialization
    void testToJson();
    void testFromJson();
    
    // Test signals
    void testSignals();

private:
    ClipboardHistory *m_history;
    QClipboard *m_clipboard;
    QImage m_testImage;
    QString m_testText;
};

void TestClipboardHistory::initTestCase()
{
    // Initialize QApplication if not already done
    if (!QApplication::instance()) {
        int argc = 0;
        char **argv = nullptr;
        new QApplication(argc, argv);
    }
    
    m_clipboard = QApplication::clipboard();
    
    // Create test image
    m_testImage = QImage(100, 100, QImage::Format_RGB32);
    m_testImage.fill(Qt::red);
    
    // Create test text
    m_testText = "Test clipboard text";
}

void TestClipboardHistory::cleanupTestCase()
{
    // Cleanup handled by Qt
}

void TestClipboardHistory::init()
{
    m_history = new ClipboardHistory();
}

void TestClipboardHistory::cleanup()
{
    delete m_history;
}

void TestClipboardHistory::testAddItem()
{
    // Test adding image item
    ClipboardItem imageItem(m_testImage, "Test image");
    m_history->addItem(imageItem);
    QCOMPARE(m_history->getItemCount(), 1);
    
    // Test adding text item
    ClipboardItem textItem(m_testText, "Test text");
    m_history->addItem(textItem);
    QCOMPARE(m_history->getItemCount(), 2);
    
    // Test adding duplicate item (should not add)
    m_history->addItem(imageItem);
    QCOMPARE(m_history->getItemCount(), 2); // Should remain unchanged
}

void TestClipboardHistory::testAddImage()
{
    // Test adding image
    m_history->addImage(m_testImage, "Test image");
    QCOMPARE(m_history->getItemCount(), 1);
    
    // Test adding null image (should not add)
    QImage nullImage;
    m_history->addImage(nullImage, "Null image");
    QCOMPARE(m_history->getItemCount(), 1); // Should remain unchanged
}

void TestClipboardHistory::testAddText()
{
    // Test adding text
    m_history->addText(m_testText, "Test text");
    QCOMPARE(m_history->getItemCount(), 1);
    
    // Test adding empty text (should not add)
    m_history->addText("", "Empty text");
    QCOMPARE(m_history->getItemCount(), 1); // Should remain unchanged
}

void TestClipboardHistory::testRemoveItem()
{
    // Add items
    m_history->addImage(m_testImage, "Test image");
    m_history->addText(m_testText, "Test text");
    QCOMPARE(m_history->getItemCount(), 2);
    
    // Remove first item
    m_history->removeItem(0);
    QCOMPARE(m_history->getItemCount(), 1);
    
    // Remove invalid index
    m_history->removeItem(10);
    QCOMPARE(m_history->getItemCount(), 1); // Should remain unchanged
}

void TestClipboardHistory::testClearHistory()
{
    // Add items
    m_history->addImage(m_testImage, "Test image");
    m_history->addText(m_testText, "Test text");
    QCOMPARE(m_history->getItemCount(), 2);
    
    // Clear history
    m_history->clearHistory();
    QCOMPARE(m_history->getItemCount(), 0);
}

void TestClipboardHistory::testGetItem()
{
    // Add item
    ClipboardItem testItem(m_testImage, "Test item");
    m_history->addItem(testItem);
    
    // Get item
    ClipboardItem retrievedItem = m_history->getItem(0);
    QCOMPARE(retrievedItem.type, "image");
    QCOMPARE(retrievedItem.description, "Test item");
    
    // Get invalid index
    ClipboardItem invalidItem = m_history->getItem(10);
    QVERIFY(invalidItem.type.isEmpty());
}

void TestClipboardHistory::testGetImage()
{
    // Add image item
    m_history->addImage(m_testImage, "Test image");
    
    // Get image
    QImage retrievedImage = m_history->getImage(0);
    QCOMPARE(retrievedImage.size(), m_testImage.size());
    
    // Get invalid index
    QImage invalidImage = m_history->getImage(10);
    QVERIFY(invalidImage.isNull());
}

void TestClipboardHistory::testGetText()
{
    // Add text item
    m_history->addText(m_testText, "Test text");
    
    // Get text
    QString retrievedText = m_history->getText(0);
    QCOMPARE(retrievedText, m_testText);
    
    // Get invalid index
    QString invalidText = m_history->getText(10);
    QVERIFY(invalidText.isEmpty());
}

void TestClipboardHistory::testGetItemCount()
{
    QCOMPARE(m_history->getItemCount(), 0);
    
    m_history->addImage(m_testImage, "Test image");
    QCOMPARE(m_history->getItemCount(), 1);
    
    m_history->addText(m_testText, "Test text");
    QCOMPARE(m_history->getItemCount(), 2);
}

void TestClipboardHistory::testSetMaxItems()
{
    QCOMPARE(m_history->getMaxItems(), 50);
    
    m_history->setMaxItems(10);
    QCOMPARE(m_history->getMaxItems(), 10);
    
    // Test invalid count
    m_history->setMaxItems(0);
    QCOMPARE(m_history->getMaxItems(), 10); // Should remain unchanged
    
    m_history->setMaxItems(-5);
    QCOMPARE(m_history->getMaxItems(), 10); // Should remain unchanged
}

void TestClipboardHistory::testGetMaxItems()
{
    QCOMPARE(m_history->getMaxItems(), 50);
    
    m_history->setMaxItems(25);
    QCOMPARE(m_history->getMaxItems(), 25);
}

void TestClipboardHistory::testSetEnabled()
{
    QVERIFY(m_history->isEnabled());
    
    m_history->setEnabled(false);
    QVERIFY(!m_history->isEnabled());
    
    m_history->setEnabled(true);
    QVERIFY(m_history->isEnabled());
}

void TestClipboardHistory::testIsEnabled()
{
    QVERIFY(m_history->isEnabled());
    
    m_history->setEnabled(false);
    QVERIFY(!m_history->isEnabled());
}

void TestClipboardHistory::testStartMonitoring()
{
    QVERIFY(!m_history->isMonitoring());
    
    m_history->startMonitoring();
    QVERIFY(m_history->isMonitoring());
    
    // Start monitoring again (should not change state)
    m_history->startMonitoring();
    QVERIFY(m_history->isMonitoring());
}

void TestClipboardHistory::testStopMonitoring()
{
    m_history->startMonitoring();
    QVERIFY(m_history->isMonitoring());
    
    m_history->stopMonitoring();
    QVERIFY(!m_history->isMonitoring());
    
    // Stop monitoring again (should not change state)
    m_history->stopMonitoring();
    QVERIFY(!m_history->isMonitoring());
}

void TestClipboardHistory::testIsMonitoring()
{
    QVERIFY(!m_history->isMonitoring());
    
    m_history->startMonitoring();
    QVERIFY(m_history->isMonitoring());
    
    m_history->stopMonitoring();
    QVERIFY(!m_history->isMonitoring());
}

void TestClipboardHistory::testSearchItems()
{
    // Add items
    m_history->addImage(m_testImage, "Red image");
    m_history->addText("Blue text", "Blue description");
    m_history->addText("Green text", "Green description");
    
    // Search for "Red"
    QList<int> redResults = m_history->searchItems("Red");
    QCOMPARE(redResults.size(), 1);
    QCOMPARE(redResults[0], 0);
    
    // Search for "text"
    QList<int> textResults = m_history->searchItems("text");
    QCOMPARE(textResults.size(), 2);
    QVERIFY(textResults.contains(1));
    QVERIFY(textResults.contains(2));
    
    // Search for "Blue"
    QList<int> blueResults = m_history->searchItems("Blue");
    QCOMPARE(blueResults.size(), 1);
    QCOMPARE(blueResults[0], 1);
    
    // Search for empty query
    QList<int> emptyResults = m_history->searchItems("");
    QCOMPARE(emptyResults.size(), 3);
    
    // Search for non-existent
    QList<int> noResults = m_history->searchItems("NonExistent");
    QVERIFY(noResults.isEmpty());
}

void TestClipboardHistory::testGetItemsByType()
{
    // Add items
    m_history->addImage(m_testImage, "Test image");
    m_history->addText("Test text", "Test description");
    m_history->addImage(m_testImage, "Another image");
    
    // Get image items
    QList<int> imageItems = m_history->getItemsByType("image");
    QCOMPARE(imageItems.size(), 2);
    QVERIFY(imageItems.contains(0));
    QVERIFY(imageItems.contains(2));
    
    // Get text items
    QList<int> textItems = m_history->getItemsByType("text");
    QCOMPARE(textItems.size(), 1);
    QCOMPARE(textItems[0], 1);
    
    // Get mixed items
    QList<int> mixedItems = m_history->getItemsByType("mixed");
    QVERIFY(mixedItems.isEmpty());
}

void TestClipboardHistory::testGetRecentItems()
{
    // Add items
    m_history->addImage(m_testImage, "First image");
    m_history->addText("First text", "First description");
    m_history->addImage(m_testImage, "Second image");
    m_history->addText("Second text", "Second description");
    m_history->addImage(m_testImage, "Third image");
    
    // Get recent items
    QList<int> recent3 = m_history->getRecentItems(3);
    QCOMPARE(recent3.size(), 3);
    QCOMPARE(recent3[0], 0); // Most recent
    QCOMPARE(recent3[1], 1);
    QCOMPARE(recent3[2], 2);
    
    // Get recent items (more than available)
    QList<int> recent10 = m_history->getRecentItems(10);
    QCOMPARE(recent10.size(), 5);
    
    // Get recent items (0 count)
    QList<int> recent0 = m_history->getRecentItems(0);
    QVERIFY(recent0.isEmpty());
}

void TestClipboardHistory::testExportHistory()
{
    // Add items
    m_history->addImage(m_testImage, "Test image");
    m_history->addText(m_testText, "Test text");
    
    // Export history
    QString fileName = QDir::tempPath() + "/test_clipboard_history.json";
    bool success = m_history->exportHistory(fileName);
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
    
    // Clean up
    QFile::remove(fileName);
}

void TestClipboardHistory::testImportHistory()
{
    // First export some history
    m_history->addImage(m_testImage, "Test image");
    m_history->addText(m_testText, "Test text");
    
    QString fileName = QDir::tempPath() + "/test_import_clipboard_history.json";
    m_history->exportHistory(fileName);
    
    // Clear history
    m_history->clearHistory();
    QCOMPARE(m_history->getItemCount(), 0);
    
    // Import history
    bool success = m_history->importHistory(fileName);
    QVERIFY(success);
    QCOMPARE(m_history->getItemCount(), 2);
    
    // Clean up
    QFile::remove(fileName);
}

void TestClipboardHistory::testToJson()
{
    // Add items
    m_history->addImage(m_testImage, "Test image");
    m_history->addText(m_testText, "Test text");
    
    // Convert to JSON
    QJsonObject json = m_history->toJson();
    
    // Verify JSON contains expected values
    QVERIFY(json.contains("maxItems"));
    QVERIFY(json.contains("enabled"));
    QVERIFY(json.contains("monitoring"));
    QVERIFY(json.contains("items"));
    
    QCOMPARE(json["maxItems"].toInt(), 50);
    QCOMPARE(json["enabled"].toBool(), true);
    QCOMPARE(json["monitoring"].toBool(), false);
    
    QJsonArray items = json["items"].toArray();
    QCOMPARE(items.size(), 2);
}

void TestClipboardHistory::testFromJson()
{
    // Create JSON object
    QJsonObject json;
    json["maxItems"] = 25;
    json["enabled"] = false;
    json["monitoring"] = true;
    
    QJsonArray items;
    QJsonObject imageItem;
    imageItem["description"] = "Test image";
    imageItem["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    imageItem["type"] = "image";
    imageItem["size"] = m_testImage.byteCount();
    imageItem["imageSize"] = QJsonObject{
        {"width", m_testImage.width()},
        {"height", m_testImage.height()}
    };
    
    // Convert image to base64
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    m_testImage.save(&buffer, "PNG");
    imageItem["imageData"] = QString::fromLatin1(buffer.data().toBase64());
    
    items.append(imageItem);
    json["items"] = items;
    
    // Load from JSON
    m_history->fromJson(json);
    
    // Verify properties were loaded
    QCOMPARE(m_history->getMaxItems(), 25);
    QCOMPARE(m_history->isEnabled(), false);
    QCOMPARE(m_history->isMonitoring(), true);
    QCOMPARE(m_history->getItemCount(), 1);
}

void TestClipboardHistory::testSignals()
{
    // Test signals
    QSignalSpy addedSpy(m_history, &ClipboardHistory::itemAdded);
    QSignalSpy removedSpy(m_history, &ClipboardHistory::itemRemoved);
    QSignalSpy clearedSpy(m_history, &ClipboardHistory::historyCleared);
    QSignalSpy monitoringStartedSpy(m_history, &ClipboardHistory::monitoringStarted);
    QSignalSpy monitoringStoppedSpy(m_history, &ClipboardHistory::monitoringStopped);
    
    // Add item
    m_history->addImage(m_testImage, "Test image");
    QCOMPARE(addedSpy.count(), 1);
    
    // Remove item
    m_history->removeItem(0);
    QCOMPARE(removedSpy.count(), 1);
    
    // Clear history
    m_history->addImage(m_testImage, "Test image");
    m_history->clearHistory();
    QCOMPARE(clearedSpy.count(), 1);
    
    // Start monitoring
    m_history->startMonitoring();
    QCOMPARE(monitoringStartedSpy.count(), 1);
    
    // Stop monitoring
    m_history->stopMonitoring();
    QCOMPARE(monitoringStoppedSpy.count(), 1);
}

QTEST_MAIN(TestClipboardHistory)
