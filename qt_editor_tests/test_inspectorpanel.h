#ifndef TEST_INSPECTORPANEL_H
#define TEST_INSPECTORPANEL_H

#include <QObject>
#include <QTest>
#include <QScrollArea>
#include <QTabWidget>
#include <QFormLayout>

// Include the class we're testing
#include "InspectorPanel.h"

class TestInspectorPanel : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Test cases
    void testCreation();
    void testScrollAreaSetup();
    void testTabWidgetSetup();
    void testPropertyEditing();
    void testTransformProperties();
    void testRenderProperties();
    void testPhysicsProperties();
    void testAudioProperties();
    void testScriptProperties();
    void testAnimationProperties();
    void testComponentManagement();
    void testSelectionHandling();
    void testPropertyValidation();
    void testSignalEmission();
    
private:
    InspectorPanel *m_inspectorPanel;
};

#endif // TEST_INSPECTORPANEL_H
