#include <QtTest>
#include <QApplication>
#include <QTabWidget>
#include <QWidget>
#include <QSignalSpy>
#include <QDir>
#include <QFile>
#include <QColor>

#include "DocumentManager.h"
#include "ImageDocument.h"

class TestDocumentManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test document creation
    void testCreateNewDocument();
    void testCreateMultipleDocuments();
    void testDocumentCount();
    
    // Test document switching
    void testSetCurrentDocument();
    void testGetCurrentDocument();
    void testGetCurrentCanvas();
    void testGetCurrentLayerPanel();
    
    // Test document information
    void testGetDocumentFileName();
    void testGetDocumentDisplayName();
    void testIsDocumentModified();
    void testSetDocumentModified();
    
    // Test tab management
    void testUpdateTabTitle();
    void testTabWidget();
    
    // Test document operations
    void testCloseDocument();
    void testCloseAllDocuments();
    void testSaveDocument();
    void testExportDocument();
    
    // Test actions
    void testActions();
    void testActionStates();

private:
    DocumentManager *m_manager;
    QString m_testDirectory;
};

void TestDocumentManager::initTestCase()
{
    // Create test directory
    m_testDirectory = QDir::tempPath() + "/PhotoEditorTest";
    QDir().mkpath(m_testDirectory);
}

void TestDocumentManager::cleanupTestCase()
{
    // Clean up test directory
    QDir(m_testDirectory).removeRecursively();
}

void TestDocumentManager::init()
{
    m_manager = new DocumentManager();
}

void TestDocumentManager::cleanup()
{
    delete m_manager;
}

void TestDocumentManager::testCreateNewDocument()
{
    // Test creating a new document
    int documentId = m_manager->createNewDocument(800, 600, Qt::white);
    
    QVERIFY(documentId >= 0);
    QCOMPARE(m_manager->getDocumentCount(), 1);
    QVERIFY(m_manager->getCurrentDocument() != nullptr);
    QVERIFY(m_manager->getCurrentCanvas() != nullptr);
    QVERIFY(m_manager->getCurrentLayerPanel() != nullptr);
}

void TestDocumentManager::testCreateMultipleDocuments()
{
    // Create multiple documents
    int doc1 = m_manager->createNewDocument(800, 600, Qt::white);
    int doc2 = m_manager->createNewDocument(1024, 768, Qt::black);
    int doc3 = m_manager->createNewDocument(640, 480, Qt::red);
    
    QVERIFY(doc1 >= 0);
    QVERIFY(doc2 >= 0);
    QVERIFY(doc3 >= 0);
    QCOMPARE(m_manager->getDocumentCount(), 3);
}

void TestDocumentManager::testDocumentCount()
{
    QCOMPARE(m_manager->getDocumentCount(), 0);
    
    m_manager->createNewDocument(800, 600);
    QCOMPARE(m_manager->getDocumentCount(), 1);
    
    m_manager->createNewDocument(800, 600);
    QCOMPARE(m_manager->getDocumentCount(), 2);
}

void TestDocumentManager::testSetCurrentDocument()
{
    // Create multiple documents
    int doc1 = m_manager->createNewDocument(800, 600);
    int doc2 = m_manager->createNewDocument(800, 600);
    int doc3 = m_manager->createNewDocument(800, 600);
    
    // Test setting current document
    m_manager->setCurrentDocument(1);
    QCOMPARE(m_manager->getCurrentDocumentIndex(), 1);
    
    m_manager->setCurrentDocument(0);
    QCOMPARE(m_manager->getCurrentDocumentIndex(), 0);
    
    // Test invalid index
    m_manager->setCurrentDocument(10);
    QCOMPARE(m_manager->getCurrentDocumentIndex(), 0); // Should remain unchanged
}

void TestDocumentManager::testGetCurrentDocument()
{
    // No documents initially
    QVERIFY(m_manager->getCurrentDocument() == nullptr);
    
    // Create document
    m_manager->createNewDocument(800, 600);
    QVERIFY(m_manager->getCurrentDocument() != nullptr);
    
    // Test document properties
    ImageDocument *doc = m_manager->getCurrentDocument();
    QCOMPARE(doc->getImageSize(), QSize(800, 600));
}

void TestDocumentManager::testGetCurrentCanvas()
{
    // No documents initially
    QVERIFY(m_manager->getCurrentCanvas() == nullptr);
    
    // Create document
    m_manager->createNewDocument(800, 600);
    QVERIFY(m_manager->getCurrentCanvas() != nullptr);
}

void TestDocumentManager::testGetCurrentLayerPanel()
{
    // No documents initially
    QVERIFY(m_manager->getCurrentLayerPanel() == nullptr);
    
    // Create document
    m_manager->createNewDocument(800, 600);
    QVERIFY(m_manager->getCurrentLayerPanel() != nullptr);
}

void TestDocumentManager::testGetDocumentFileName()
{
    // Create document
    int docId = m_manager->createNewDocument(800, 600);
    QCOMPARE(m_manager->getDocumentFileName(docId), QString());
    
    // Test invalid index
    QCOMPARE(m_manager->getDocumentFileName(10), QString());
}

void TestDocumentManager::testGetDocumentDisplayName()
{
    // Create document
    int docId = m_manager->createNewDocument(800, 600);
    QString displayName = m_manager->getDocumentDisplayName(docId);
    QVERIFY(!displayName.isEmpty());
    QVERIFY(displayName.contains("Untitled"));
    
    // Test invalid index
    QCOMPARE(m_manager->getDocumentDisplayName(10), QString());
}

void TestDocumentManager::testIsDocumentModified()
{
    // Create document
    int docId = m_manager->createNewDocument(800, 600);
    QVERIFY(!m_manager->isDocumentModified(docId));
    
    // Test invalid index
    QVERIFY(!m_manager->isDocumentModified(10));
}

void TestDocumentManager::testSetDocumentModified()
{
    // Create document
    int docId = m_manager->createNewDocument(800, 600);
    
    // Set as modified
    m_manager->setDocumentModified(docId, true);
    QVERIFY(m_manager->isDocumentModified(docId));
    
    // Set as not modified
    m_manager->setDocumentModified(docId, false);
    QVERIFY(!m_manager->isDocumentModified(docId));
    
    // Test invalid index
    m_manager->setDocumentModified(10, true);
    QVERIFY(!m_manager->isDocumentModified(10));
}

void TestDocumentManager::testUpdateTabTitle()
{
    // Create document
    int docId = m_manager->createNewDocument(800, 600);
    
    // Test tab title update
    m_manager->updateTabTitle(docId);
    // Note: This would need to be tested with actual UI components
}

void TestDocumentManager::testTabWidget()
{
    QVERIFY(m_manager->getTabWidget() != nullptr);
    QVERIFY(m_manager->getTabWidget()->count() == 0);
    
    // Create document
    m_manager->createNewDocument(800, 600);
    QCOMPARE(m_manager->getTabWidget()->count(), 1);
}

void TestDocumentManager::testCloseDocument()
{
    // Create documents
    int doc1 = m_manager->createNewDocument(800, 600);
    int doc2 = m_manager->createNewDocument(800, 600);
    
    QCOMPARE(m_manager->getDocumentCount(), 2);
    
    // Close first document
    bool success = m_manager->closeDocument(doc1);
    QVERIFY(success);
    QCOMPARE(m_manager->getDocumentCount(), 1);
    
    // Close invalid document
    success = m_manager->closeDocument(10);
    QVERIFY(!success);
    QCOMPARE(m_manager->getDocumentCount(), 1);
}

void TestDocumentManager::testCloseAllDocuments()
{
    // Create documents
    m_manager->createNewDocument(800, 600);
    m_manager->createNewDocument(800, 600);
    m_manager->createNewDocument(800, 600);
    
    QCOMPARE(m_manager->getDocumentCount(), 3);
    
    // Close all documents
    bool success = m_manager->closeAllDocuments();
    QVERIFY(success);
    QCOMPARE(m_manager->getDocumentCount(), 0);
}

void TestDocumentManager::testSaveDocument()
{
    // Create document
    int docId = m_manager->createNewDocument(800, 600);
    
    // Save document (should fail because no filename is set)
    bool success = m_manager->saveDocument(docId);
    QVERIFY(!success);
    
    // Test invalid document
    success = m_manager->saveDocument(10);
    QVERIFY(!success);
}

void TestDocumentManager::testExportDocument()
{
    // Create document
    int docId = m_manager->createNewDocument(800, 600);
    
    // Export document
    QString fileName = m_testDirectory + "/test_export.png";
    bool success = m_manager->exportDocument(docId, fileName, "PNG");
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
    
    // Test invalid document
    success = m_manager->exportDocument(10, fileName, "PNG");
    QVERIFY(!success);
}

void TestDocumentManager::testActions()
{
    // Test action creation
    QVERIFY(m_manager->getNewAction() != nullptr);
    QVERIFY(m_manager->getOpenAction() != nullptr);
    QVERIFY(m_manager->getCloseAction() != nullptr);
    QVERIFY(m_manager->getCloseAllAction() != nullptr);
    QVERIFY(m_manager->getSaveAction() != nullptr);
    QVERIFY(m_manager->getSaveAsAction() != nullptr);
    QVERIFY(m_manager->getExportAction() != nullptr);
}

void TestDocumentManager::testActionStates()
{
    // Initially no documents, so most actions should be disabled
    QVERIFY(!m_manager->getCloseAction()->isEnabled());
    QVERIFY(!m_manager->getCloseAllAction()->isEnabled());
    QVERIFY(!m_manager->getSaveAction()->isEnabled());
    QVERIFY(!m_manager->getSaveAsAction()->isEnabled());
    QVERIFY(!m_manager->getExportAction()->isEnabled());
    
    // Create document
    m_manager->createNewDocument(800, 600);
    
    // Now some actions should be enabled
    QVERIFY(m_manager->getCloseAction()->isEnabled());
    QVERIFY(m_manager->getCloseAllAction()->isEnabled());
    QVERIFY(m_manager->getSaveAsAction()->isEnabled());
    QVERIFY(m_manager->getExportAction()->isEnabled());
    
    // Save action should still be disabled (document not modified)
    QVERIFY(!m_manager->getSaveAction()->isEnabled());
}

QTEST_MAIN(TestDocumentManager)
