#include <QtTest>
#include <QApplication>
#include <QSignalSpy>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QDateTime>

#include "AutoSaveManager.h"
#include "ImageDocument.h"
#include "DocumentManager.h"

class TestAutoSaveManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test configuration
    void testSetEnabled();
    void testSetInterval();
    void testSetMaxBackups();
    void testSetAutoSaveDirectory();
    
    // Test document management
    void testRegisterDocument();
    void testUnregisterDocument();
    void testSaveDocument();
    void testRestoreDocument();
    void testCleanupOldBackups();
    
    // Test session management
    void testSaveSession();
    void testRestoreSession();
    void testClearSession();
    
    // Test backup management
    void testGetBackupFiles();
    void testGetLatestBackup();
    void testHasBackups();
    
    // Test serialization
    void testToJson();
    void testFromJson();
    
    // Test signals
    void testSignals();

private:
    AutoSaveManager *m_manager;
    DocumentManager *m_documentManager;
    QString m_testDirectory;
};

void TestAutoSaveManager::initTestCase()
{
    // Initialize QApplication if not already done
    if (!QApplication::instance()) {
        int argc = 0;
        char **argv = nullptr;
        new QApplication(argc, argv);
    }
    
    // Create test directory
    m_testDirectory = QDir::tempPath() + "/PhotoEditorAutoSaveTest";
    QDir().mkpath(m_testDirectory);
}

void TestAutoSaveManager::cleanupTestCase()
{
    // Clean up test directory
    QDir(m_testDirectory).removeRecursively();
}

void TestAutoSaveManager::init()
{
    m_manager = new AutoSaveManager();
    m_documentManager = new DocumentManager();
    m_manager->setDocumentManager(m_documentManager);
    m_manager->setAutoSaveDirectory(m_testDirectory);
}

void TestAutoSaveManager::cleanup()
{
    delete m_manager;
    delete m_documentManager;
}

void TestAutoSaveManager::testSetEnabled()
{
    QVERIFY(m_manager->isEnabled());
    
    m_manager->setEnabled(false);
    QVERIFY(!m_manager->isEnabled());
    
    m_manager->setEnabled(true);
    QVERIFY(m_manager->isEnabled());
}

void TestAutoSaveManager::testSetInterval()
{
    QCOMPARE(m_manager->getInterval(), 5);
    
    m_manager->setInterval(10);
    QCOMPARE(m_manager->getInterval(), 10);
    
    m_manager->setInterval(1);
    QCOMPARE(m_manager->getInterval(), 1);
    
    // Test invalid interval
    m_manager->setInterval(0);
    QCOMPARE(m_manager->getInterval(), 1); // Should remain unchanged
    
    m_manager->setInterval(-5);
    QCOMPARE(m_manager->getInterval(), 1); // Should remain unchanged
}

void TestAutoSaveManager::testSetMaxBackups()
{
    QCOMPARE(m_manager->getMaxBackups(), 10);
    
    m_manager->setMaxBackups(20);
    QCOMPARE(m_manager->getMaxBackups(), 20);
    
    m_manager->setMaxBackups(5);
    QCOMPARE(m_manager->getMaxBackups(), 5);
    
    // Test invalid count
    m_manager->setMaxBackups(0);
    QCOMPARE(m_manager->getMaxBackups(), 5); // Should remain unchanged
    
    m_manager->setMaxBackups(-5);
    QCOMPARE(m_manager->getMaxBackups(), 5); // Should remain unchanged
}

void TestAutoSaveManager::testSetAutoSaveDirectory()
{
    QString newDir = m_testDirectory + "/new_autosave";
    m_manager->setAutoSaveDirectory(newDir);
    QCOMPARE(m_manager->getAutoSaveDirectory(), newDir);
    QVERIFY(QDir(newDir).exists());
}

void TestAutoSaveManager::testRegisterDocument()
{
    // Create a test document
    ImageDocument *doc = new ImageDocument();
    doc->newDocument(800, 600, Qt::white);
    
    int documentId = 1;
    m_manager->registerDocument(documentId, doc);
    
    // Verify document is registered
    QVERIFY(m_manager->hasBackups(documentId));
    
    delete doc;
}

void TestAutoSaveManager::testUnregisterDocument()
{
    // Create and register a test document
    ImageDocument *doc = new ImageDocument();
    doc->newDocument(800, 600, Qt::white);
    
    int documentId = 1;
    m_manager->registerDocument(documentId, doc);
    
    // Unregister document
    m_manager->unregisterDocument(documentId);
    
    // Verify document is unregistered
    QVERIFY(!m_manager->hasBackups(documentId));
    
    delete doc;
}

void TestAutoSaveManager::testSaveDocument()
{
    // Create and register a test document
    ImageDocument *doc = new ImageDocument();
    doc->newDocument(800, 600, Qt::white);
    
    int documentId = 1;
    m_manager->registerDocument(documentId, doc);
    
    // Save document
    bool success = m_manager->saveDocument(documentId);
    QVERIFY(success);
    
    // Verify backup was created
    QVERIFY(m_manager->hasBackups(documentId));
    QVERIFY(!m_manager->getBackupFiles(documentId).isEmpty());
    
    delete doc;
}

void TestAutoSaveManager::testRestoreDocument()
{
    // Create and register a test document
    ImageDocument *doc = new ImageDocument();
    doc->newDocument(800, 600, Qt::white);
    
    int documentId = 1;
    m_manager->registerDocument(documentId, doc);
    
    // Save document
    m_manager->saveDocument(documentId);
    
    // Get backup files
    QStringList backupFiles = m_manager->getBackupFiles(documentId);
    QVERIFY(!backupFiles.isEmpty());
    
    // Restore document
    bool success = m_manager->restoreDocument(documentId, backupFiles.first());
    QVERIFY(success);
    
    delete doc;
}

void TestAutoSaveManager::testCleanupOldBackups()
{
    // Set max backups to 2
    m_manager->setMaxBackups(2);
    
    // Create and register a test document
    ImageDocument *doc = new ImageDocument();
    doc->newDocument(800, 600, Qt::white);
    
    int documentId = 1;
    m_manager->registerDocument(documentId, doc);
    
    // Save multiple times to create multiple backups
    for (int i = 0; i < 5; ++i) {
        m_manager->saveDocument(documentId);
        QThread::msleep(10); // Small delay to ensure different timestamps
    }
    
    // Verify only max backups are kept
    QStringList backupFiles = m_manager->getBackupFiles(documentId);
    QCOMPARE(backupFiles.size(), 2);
    
    delete doc;
}

void TestAutoSaveManager::testSaveSession()
{
    // Create and register a test document
    ImageDocument *doc = new ImageDocument();
    doc->newDocument(800, 600, Qt::white);
    
    int documentId = 1;
    m_manager->registerDocument(documentId, doc);
    
    // Save session
    m_manager->saveSession();
    
    // Verify session file was created
    QString sessionFile = m_testDirectory + "/session.json";
    QVERIFY(QFile::exists(sessionFile));
    
    delete doc;
}

void TestAutoSaveManager::testRestoreSession()
{
    // First save a session
    ImageDocument *doc = new ImageDocument();
    doc->newDocument(800, 600, Qt::white);
    
    int documentId = 1;
    m_manager->registerDocument(documentId, doc);
    m_manager->saveSession();
    
    delete doc;
    
    // Create new manager and restore session
    AutoSaveManager *newManager = new AutoSaveManager();
    newManager->setAutoSaveDirectory(m_testDirectory);
    newManager->restoreSession();
    
    // Verify session was restored
    QVERIFY(newManager->hasBackups(documentId));
    
    delete newManager;
}

void TestAutoSaveManager::testClearSession()
{
    // Create and register a test document
    ImageDocument *doc = new ImageDocument();
    doc->newDocument(800, 600, Qt::white);
    
    int documentId = 1;
    m_manager->registerDocument(documentId, doc);
    m_manager->saveDocument(documentId);
    
    // Clear session
    m_manager->clearSession();
    
    // Verify session was cleared
    QVERIFY(!m_manager->hasBackups(documentId));
    
    delete doc;
}

void TestAutoSaveManager::testGetBackupFiles()
{
    // Create and register a test document
    ImageDocument *doc = new ImageDocument();
    doc->newDocument(800, 600, Qt::white);
    
    int documentId = 1;
    m_manager->registerDocument(documentId, doc);
    
    // Initially no backups
    QStringList backupFiles = m_manager->getBackupFiles(documentId);
    QVERIFY(backupFiles.isEmpty());
    
    // Save document
    m_manager->saveDocument(documentId);
    
    // Now should have backups
    backupFiles = m_manager->getBackupFiles(documentId);
    QVERIFY(!backupFiles.isEmpty());
    
    delete doc;
}

void TestAutoSaveManager::testGetLatestBackup()
{
    // Create and register a test document
    ImageDocument *doc = new ImageDocument();
    doc->newDocument(800, 600, Qt::white);
    
    int documentId = 1;
    m_manager->registerDocument(documentId, doc);
    
    // Initially no latest backup
    QString latestBackup = m_manager->getLatestBackup(documentId);
    QVERIFY(latestBackup.isEmpty());
    
    // Save document
    m_manager->saveDocument(documentId);
    
    // Now should have latest backup
    latestBackup = m_manager->getLatestBackup(documentId);
    QVERIFY(!latestBackup.isEmpty());
    
    delete doc;
}

void TestAutoSaveManager::testHasBackups()
{
    // Create and register a test document
    ImageDocument *doc = new ImageDocument();
    doc->newDocument(800, 600, Qt::white);
    
    int documentId = 1;
    m_manager->registerDocument(documentId, doc);
    
    // Initially no backups
    QVERIFY(!m_manager->hasBackups(documentId));
    
    // Save document
    m_manager->saveDocument(documentId);
    
    // Now should have backups
    QVERIFY(m_manager->hasBackups(documentId));
    
    delete doc;
}

void TestAutoSaveManager::testToJson()
{
    // Set some properties
    m_manager->setEnabled(false);
    m_manager->setInterval(15);
    m_manager->setMaxBackups(25);
    m_manager->setAutoSaveDirectory(m_testDirectory);
    
    // Convert to JSON
    QJsonObject json = m_manager->toJson();
    
    // Verify JSON contains expected values
    QVERIFY(json.contains("enabled"));
    QVERIFY(json.contains("interval"));
    QVERIFY(json.contains("maxBackups"));
    QVERIFY(json.contains("autoSaveDirectory"));
    
    QCOMPARE(json["enabled"].toBool(), false);
    QCOMPARE(json["interval"].toInt(), 15);
    QCOMPARE(json["maxBackups"].toInt(), 25);
    QCOMPARE(json["autoSaveDirectory"].toString(), m_testDirectory);
}

void TestAutoSaveManager::testFromJson()
{
    // Create JSON object
    QJsonObject json;
    json["enabled"] = false;
    json["interval"] = 20;
    json["maxBackups"] = 30;
    json["autoSaveDirectory"] = m_testDirectory;
    
    // Load from JSON
    m_manager->fromJson(json);
    
    // Verify properties were loaded
    QCOMPARE(m_manager->isEnabled(), false);
    QCOMPARE(m_manager->getInterval(), 20);
    QCOMPARE(m_manager->getMaxBackups(), 30);
    QCOMPARE(m_manager->getAutoSaveDirectory(), m_testDirectory);
}

void TestAutoSaveManager::testSignals()
{
    // Test signals
    QSignalSpy enabledSpy(m_manager, &AutoSaveManager::monitoringStarted);
    QSignalSpy disabledSpy(m_manager, &AutoSaveManager::monitoringStopped);
    
    // Start monitoring
    m_manager->startMonitoring();
    QCOMPARE(enabledSpy.count(), 1);
    
    // Stop monitoring
    m_manager->stopMonitoring();
    QCOMPARE(disabledSpy.count(), 1);
}

QTEST_MAIN(TestAutoSaveManager)
