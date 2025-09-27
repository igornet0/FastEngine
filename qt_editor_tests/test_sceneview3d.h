#ifndef TEST_SCENEVIEW3D_H
#define TEST_SCENEVIEW3D_H

#include <QObject>
#include <QTest>
#include <QOpenGLWidget>

// Include the class we're testing
#include "SceneView3D.h"

class TestSceneView3D : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Test cases
    void testCreation();
    void testOpenGLContext();
    void testCameraControls();
    void testEntityManagement();
    void testLighting();
    void testShaderPrograms();
    void testBufferManagement();
    void testMouseInteraction();
    void testKeyboardControls();
    void testRendering();
    void testPerformance();
    
private:
    SceneView3D *m_sceneView3D;
    bool m_openGLAvailable;
};

#endif // TEST_SCENEVIEW3D_H
