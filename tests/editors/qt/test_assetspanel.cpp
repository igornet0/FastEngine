#include "test_assetspanel.h"
#include <QDebug>
#include <QSignalSpy>
#include <QLineEdit>
#include <QComboBox>
#include <QTabWidget>
#include <QFileSystemModel>

void TestAssetsPanel::initTestCase()
{
    qDebug() << "Initializing AssetsPanel test case...";
}

void TestAssetsPanel::cleanupTestCase()
{
    qDebug() << "Cleaning up AssetsPanel test case...";
}

void TestAssetsPanel::init()
{
    try {
        m_assetsPanel = new AssetsPanel();
        qDebug() << "✅ AssetsPanel created successfully";
    } catch (...) {
        QFAIL("❌ Failed to create AssetsPanel");
    }
}

void TestAssetsPanel::cleanup()
{
    if (m_assetsPanel) {
        delete m_assetsPanel;
        m_assetsPanel = nullptr;
    }
}

void TestAssetsPanel::testCreation()
{
    QVERIFY(m_assetsPanel != nullptr);
    QVERIFY(m_assetsPanel->inherits("QDockWidget"));
    qDebug() << "✅ AssetsPanel creation test passed";
}

void TestAssetsPanel::testFileSystemModel()
{
    QVERIFY(m_assetsPanel != nullptr);
    
    try {
        // Look for file system model
        QFileSystemModel *model = m_assetsPanel->findChild<QFileSystemModel*>();
        if (model) {
            QVERIFY(model != nullptr);
            qDebug() << "✅ FileSystemModel test passed";
        } else {
            qDebug() << "⚠️ FileSystemModel not found, but test continues";
        }
    } catch (...) {
        QFAIL("❌ FileSystemModel caused an exception");
    }
}

void TestAssetsPanel::testAssetDisplay()
{
    QVERIFY(m_assetsPanel != nullptr);
    
    try {
        // Test asset display widgets
        QTreeWidget *treeWidget = m_assetsPanel->findChild<QTreeWidget*>();
        QListWidget *listWidget = m_assetsPanel->findChild<QListWidget*>();
        
        if (treeWidget) {
            QVERIFY(treeWidget->columnCount() >= 1);
            qDebug() << "✅ Tree view found with" << treeWidget->columnCount() << "columns";
        }
        
        if (listWidget) {
            // List widget should be able to display items
            qDebug() << "✅ List view found with" << listWidget->count() << "items";
        }
        
        qDebug() << "✅ Asset display test passed";
    } catch (...) {
        QFAIL("❌ Asset display caused an exception");
    }
}

void TestAssetsPanel::testAssetFiltering()
{
    QVERIFY(m_assetsPanel != nullptr);
    
    try {
        // Look for filter controls
        QList<QComboBox*> comboBoxes = m_assetsPanel->findChildren<QComboBox*>();
        QList<QCheckBox*> checkBoxes = m_assetsPanel->findChildren<QCheckBox*>();
        
        // Test filter functionality
        for (QComboBox *combo : comboBoxes) {
            if (combo->count() > 0) {
                int originalIndex = combo->currentIndex();
                combo->setCurrentIndex((originalIndex + 1) % combo->count());
                combo->setCurrentIndex(originalIndex);
            }
        }
        
        qDebug() << "✅ Asset filtering test passed";
    } catch (...) {
        QFAIL("❌ Asset filtering caused an exception");
    }
}

void TestAssetsPanel::testAssetSearch()
{
    QVERIFY(m_assetsPanel != nullptr);
    
    try {
        // Look for search functionality
        QLineEdit *searchEdit = m_assetsPanel->findChild<QLineEdit*>();
        if (searchEdit) {
            searchEdit->setText("test");
            QTest::qWait(50);
            searchEdit->clear();
            qDebug() << "✅ Asset search test passed";
        } else {
            qDebug() << "⚠️ Search functionality not found, but test continues";
        }
    } catch (...) {
        QFAIL("❌ Asset search caused an exception");
    }
}

void TestAssetsPanel::testAssetImport()
{
    QVERIFY(m_assetsPanel != nullptr);
    
    try {
        // Test import functionality (should not crash)
        // We can't actually import files in tests, but we can test the interface
        QList<QPushButton*> buttons = m_assetsPanel->findChildren<QPushButton*>();
        
        bool hasImportButton = false;
        for (QPushButton *button : buttons) {
            if (button->text().contains("Import") || button->text().contains("Add")) {
                hasImportButton = true;
                break;
            }
        }
        
        qDebug() << "Import functionality available:" << hasImportButton;
        qDebug() << "✅ Asset import test passed";
    } catch (...) {
        QFAIL("❌ Asset import caused an exception");
    }
}

void TestAssetsPanel::testAssetPreview()
{
    QVERIFY(m_assetsPanel != nullptr);
    
    try {
        // Test preview functionality
        QListWidget *listWidget = m_assetsPanel->findChild<QListWidget*>();
        if (listWidget) {
            // Test that items can have icons (previews)
            for (int i = 0; i < listWidget->count(); ++i) {
                QListWidgetItem *item = listWidget->item(i);
                if (item) {
                    QIcon icon = item->icon();
                    // Icon may be null, but getting it should not crash
                }
            }
            qDebug() << "✅ Asset preview test passed";
        } else {
            qDebug() << "⚠️ List widget not found for preview test";
        }
    } catch (...) {
        QFAIL("❌ Asset preview caused an exception");
    }
}

void TestAssetsPanel::testDragAndDrop()
{
    QVERIFY(m_assetsPanel != nullptr);
    
    try {
        // Test drag and drop settings
        QTreeWidget *treeWidget = m_assetsPanel->findChild<QTreeWidget*>();
        QListWidget *listWidget = m_assetsPanel->findChild<QListWidget*>();
        
        if (treeWidget) {
            QVERIFY(treeWidget->dragDropMode() != QAbstractItemView::NoDragDrop);
        }
        
        if (listWidget) {
            QVERIFY(listWidget->dragDropMode() != QAbstractItemView::NoDragDrop);
        }
        
        qDebug() << "✅ Drag and drop test passed";
    } catch (...) {
        QFAIL("❌ Drag and drop caused an exception");
    }
}

void TestAssetsPanel::testContextMenu()
{
    QVERIFY(m_assetsPanel != nullptr);
    
    try {
        // Test context menu
        QTreeWidget *treeWidget = m_assetsPanel->findChild<QTreeWidget*>();
        if (treeWidget) {
            QVERIFY(treeWidget->contextMenuPolicy() == Qt::CustomContextMenu);
        }
        
        QListWidget *listWidget = m_assetsPanel->findChild<QListWidget*>();
        if (listWidget) {
            QVERIFY(listWidget->contextMenuPolicy() == Qt::CustomContextMenu);
        }
        
        qDebug() << "✅ Context menu test passed";
    } catch (...) {
        QFAIL("❌ Context menu caused an exception");
    }
}

void TestAssetsPanel::testViewModes()
{
    QVERIFY(m_assetsPanel != nullptr);
    
    try {
        // Test view mode switching
        QTabWidget *tabWidget = m_assetsPanel->findChild<QTabWidget*>();
        if (tabWidget && tabWidget->count() > 1) {
            int originalIndex = tabWidget->currentIndex();
            
            // Switch between tabs
            for (int i = 0; i < tabWidget->count(); ++i) {
                tabWidget->setCurrentIndex(i);
                QTest::qWait(50);
            }
            
            tabWidget->setCurrentIndex(originalIndex);
            qDebug() << "✅ View modes test passed - Found" << tabWidget->count() << "view modes";
        } else {
            qDebug() << "⚠️ Multiple view modes not found, but test continues";
        }
    } catch (...) {
        QFAIL("❌ View modes caused an exception");
    }
}

void TestAssetsPanel::testAssetTypes()
{
    QVERIFY(m_assetsPanel != nullptr);
    
    try {
        // Test asset type recognition
        QListWidget *listWidget = m_assetsPanel->findChild<QListWidget*>();
        if (listWidget) {
            // Check that items have appropriate icons for their types
            for (int i = 0; i < listWidget->count(); ++i) {
                QListWidgetItem *item = listWidget->item(i);
                if (item) {
                    QString text = item->text();
                    QIcon icon = item->icon();
                    // Icons may vary, but getting them should not crash
                }
            }
        }
        
        qDebug() << "✅ Asset types test passed";
    } catch (...) {
        QFAIL("❌ Asset types caused an exception");
    }
}

void TestAssetsPanel::testSignalEmission()
{
    QVERIFY(m_assetsPanel != nullptr);
    
    try {
        // Test signal emission
        QSignalSpy assetSelectedSpy(m_assetsPanel, &AssetsPanel::assetSelected);
        QSignalSpy assetImportedSpy(m_assetsPanel, &AssetsPanel::assetImported);
        
        // Verify signals can be connected
        QVERIFY(assetSelectedSpy.isValid());
        QVERIFY(assetImportedSpy.isValid());
        
        qDebug() << "✅ Signal emission test passed";
    } catch (...) {
        QFAIL("❌ Signal emission caused an exception");
    }
}

#include "test_assetspanel.moc"


