#ifndef TEST_SCENEVIEW2D_H
#define TEST_SCENEVIEW2D_H

#include <QObject>
#include <QTest>
#include <QGraphicsScene>
#include <QGraphicsView>

// Include the class we're testing
#include "SceneView2D.h"

class TestSceneView2D : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Test cases
    void testCreation();
    void testSceneSetup();
    void testViewportSettings();
    void testMouseEvents();
    void testKeyboardEvents();
    void testZoomFunctionality();
    void testPanFunctionality();
    void testEntityCreation();
    void testEntitySelection();
    void testEntityMovement();
    void testGridDisplay();
    void testAssetIntegration();
    
private:
    SceneView2D *m_sceneView;
};

#endif // TEST_SCENEVIEW2D_H
