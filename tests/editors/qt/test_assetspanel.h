#ifndef TEST_ASSETSPANEL_H
#define TEST_ASSETSPANEL_H

#include <QObject>
#include <QTest>
#include <QTreeWidget>
#include <QListWidget>
#include <QFileSystemModel>

// Include the class we're testing
#include "AssetsPanel.h"

class TestAssetsPanel : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Test cases
    void testCreation();
    void testFileSystemModel();
    void testAssetDisplay();
    void testAssetFiltering();
    void testAssetSearch();
    void testAssetImport();
    void testAssetPreview();
    void testDragAndDrop();
    void testContextMenu();
    void testViewModes();
    void testAssetTypes();
    void testSignalEmission();
    
private:
    AssetsPanel *m_assetsPanel;
};

#endif // TEST_ASSETSPANEL_H


