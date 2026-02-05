#ifndef TEST_HIERARCHYPANEL_H
#define TEST_HIERARCHYPANEL_H

#include <QObject>
#include <QTest>
#include <QTreeWidget>
#include <QTreeWidgetItem>

// Include the class we're testing
#include "HierarchyPanel.h"

class TestHierarchyPanel : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Test cases
    void testCreation();
    void testTreeWidgetSetup();
    void testEntityAddition();
    void testEntityRemoval();
    void testEntitySelection();
    void testEntityRenaming();
    void testEntityDuplication();
    void testEntityHierarchy();
    void testContextMenu();
    void testDragAndDrop();
    void testFiltering();
    void testSignalEmission();
    
private:
    HierarchyPanel *m_hierarchyPanel;
};

#endif // TEST_HIERARCHYPANEL_H


