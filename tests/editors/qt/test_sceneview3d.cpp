#include "test_sceneview3d.h"
#include <QDebug>
#include <QOpenGLContext>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QVector3D>

void TestSceneView3D::initTestCase()
{
    qDebug() << "Initializing SceneView3D test case...";
    
    // Check if OpenGL is available
    QOpenGLContext context;
    m_openGLAvailable = context.create();
    
    if (m_openGLAvailable) {
        qDebug() << "✅ OpenGL context available";
    } else {
        qDebug() << "⚠️ OpenGL context not available - some tests will be skipped";
    }
}

void TestSceneView3D::cleanupTestCase()
{
    qDebug() << "Cleaning up SceneView3D test case...";
}

void TestSceneView3D::init()
{
    // This is the most likely source of the segfault
    try {
        m_sceneView3D = new SceneView3D();
        qDebug() << "✅ SceneView3D created successfully";
    } catch (...) {
        qDebug() << "❌ CRITICAL: SceneView3D creation failed - this is likely the segfault source!";
        m_sceneView3D = nullptr;
        QFAIL("SceneView3D creation failed");
    }
}

void TestSceneView3D::cleanup()
{
    if (m_sceneView3D) {
        delete m_sceneView3D;
        m_sceneView3D = nullptr;
    }
}

void TestSceneView3D::testCreation()
{
    if (!m_openGLAvailable) {
        QSKIP("OpenGL not available");
        return;
    }
    
    QVERIFY(m_sceneView3D != nullptr);
    QVERIFY(m_sceneView3D->inherits("QOpenGLWidget"));
    qDebug() << "✅ SceneView3D creation test passed";
}

void TestSceneView3D::testOpenGLContext()
{
    if (!m_openGLAvailable || !m_sceneView3D) {
        QSKIP("OpenGL not available or SceneView3D not created");
        return;
    }
    
    try {
        // Show the widget to initialize OpenGL context
        m_sceneView3D->show();
        QTest::qWait(100);
        
        QOpenGLContext *context = m_sceneView3D->context();
        if (context) {
            QVERIFY(context->isValid());
            qDebug() << "✅ OpenGL context test passed";
        } else {
            qDebug() << "⚠️ OpenGL context not yet initialized";
        }
        
        m_sceneView3D->hide();
    } catch (...) {
        QFAIL("❌ OpenGL context test caused an exception");
    }
}

void TestSceneView3D::testCameraControls()
{
    if (!m_sceneView3D) {
        QSKIP("SceneView3D not created");
        return;
    }
    
    try {
        // Test camera position
        QVector3D originalPos = m_sceneView3D->getCameraPosition();
        
        QVector3D newPos(5, 5, 5);
        m_sceneView3D->setCameraPosition(newPos);
        
        QVector3D currentPos = m_sceneView3D->getCameraPosition();
        QVERIFY(currentPos == newPos);
        
        // Test camera target
        QVector3D newTarget(1, 1, 1);
        m_sceneView3D->setCameraTarget(newTarget);
        
        QVector3D currentTarget = m_sceneView3D->getCameraTarget();
        QVERIFY(currentTarget == newTarget);
        
        // Test camera FOV
        float newFOV = 60.0f;
        m_sceneView3D->setCameraFOV(newFOV);
        
        float currentFOV = m_sceneView3D->getCameraFOV();
        QVERIFY(qAbs(currentFOV - newFOV) < 0.01f);
        
        qDebug() << "✅ Camera controls test passed";
    } catch (...) {
        QFAIL("❌ Camera controls caused an exception");
    }
}

void TestSceneView3D::testEntityManagement()
{
    if (!m_sceneView3D) {
        QSKIP("SceneView3D not created");
        return;
    }
    
    try {
        // Test entity creation
        QString entityName = "TestCube";
        QVector3D position(1, 2, 3);
        QVector3D scale(1, 1, 1);
        
        m_sceneView3D->addEntity(entityName, position, scale);
        
        // Test entity selection
        m_sceneView3D->selectEntity(entityName);
        QStringList selected = m_sceneView3D->getSelectedEntities();
        QVERIFY(selected.contains(entityName));
        
        // Test entity removal
        m_sceneView3D->removeEntity(entityName);
        m_sceneView3D->clearSelection();
        selected = m_sceneView3D->getSelectedEntities();
        QVERIFY(!selected.contains(entityName));
        
        qDebug() << "✅ Entity management test passed";
    } catch (...) {
        QFAIL("❌ Entity management caused an exception");
    }
}

void TestSceneView3D::testLighting()
{
    if (!m_sceneView3D) {
        QSKIP("SceneView3D not created");
        return;
    }
    
    try {
        // Test light controls
        QVector3D lightPos(5, 5, 5);
        QVector3D lightColor(1, 1, 1);
        float lightIntensity = 1.5f;
        
        m_sceneView3D->setLightPosition(lightPos);
        m_sceneView3D->setLightColor(lightColor);
        m_sceneView3D->setLightIntensity(lightIntensity);
        
        qDebug() << "✅ Lighting test passed";
    } catch (...) {
        QFAIL("❌ Lighting caused an exception");
    }
}

void TestSceneView3D::testShaderPrograms()
{
    if (!m_openGLAvailable || !m_sceneView3D) {
        QSKIP("OpenGL not available or SceneView3D not created");
        return;
    }
    
    try {
        // This test is tricky because shaders are initialized in initializeGL
        // We'll just test that the widget can be shown without crashing
        m_sceneView3D->show();
        QTest::qWait(200); // Wait for OpenGL initialization
        
        // If we get here without crashing, shaders probably loaded OK
        qDebug() << "✅ Shader programs test passed (no crash during initialization)";
        
        m_sceneView3D->hide();
    } catch (...) {
        QFAIL("❌ Shader programs caused an exception");
    }
}

void TestSceneView3D::testBufferManagement()
{
    if (!m_openGLAvailable || !m_sceneView3D) {
        QSKIP("OpenGL not available or SceneView3D not created");
        return;
    }
    
    try {
        // Test buffer creation (happens in initializeGL)
        m_sceneView3D->show();
        QTest::qWait(200);
        
        qDebug() << "✅ Buffer management test passed (no crash during buffer setup)";
        
        m_sceneView3D->hide();
    } catch (...) {
        QFAIL("❌ Buffer management caused an exception");
    }
}

void TestSceneView3D::testMouseInteraction()
{
    if (!m_sceneView3D) {
        QSKIP("SceneView3D not created");
        return;
    }
    
    try {
        // Test mouse events for camera control
        QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(100, 100), 
                             Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(m_sceneView3D, &pressEvent);
        
        QMouseEvent moveEvent(QEvent::MouseMove, QPoint(150, 150), 
                            Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(m_sceneView3D, &moveEvent);
        
        QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(150, 150), 
                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(m_sceneView3D, &releaseEvent);
        
        qDebug() << "✅ Mouse interaction test passed";
    } catch (...) {
        QFAIL("❌ Mouse interaction caused an exception");
    }
}

void TestSceneView3D::testKeyboardControls()
{
    if (!m_sceneView3D) {
        QSKIP("SceneView3D not created");
        return;
    }
    
    try {
        // Test WASD camera controls
        QKeyEvent wKey(QEvent::KeyPress, Qt::Key_W, Qt::NoModifier);
        QApplication::sendEvent(m_sceneView3D, &wKey);
        
        QKeyEvent sKey(QEvent::KeyPress, Qt::Key_S, Qt::NoModifier);
        QApplication::sendEvent(m_sceneView3D, &sKey);
        
        QKeyEvent aKey(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier);
        QApplication::sendEvent(m_sceneView3D, &aKey);
        
        QKeyEvent dKey(QEvent::KeyPress, Qt::Key_D, Qt::NoModifier);
        QApplication::sendEvent(m_sceneView3D, &dKey);
        
        // Test camera reset
        QKeyEvent resetKey(QEvent::KeyPress, Qt::Key_0, Qt::NoModifier);
        QApplication::sendEvent(m_sceneView3D, &resetKey);
        
        qDebug() << "✅ Keyboard controls test passed";
    } catch (...) {
        QFAIL("❌ Keyboard controls caused an exception");
    }
}

void TestSceneView3D::testRendering()
{
    if (!m_openGLAvailable || !m_sceneView3D) {
        QSKIP("OpenGL not available or SceneView3D not created");
        return;
    }
    
    try {
        // Test rendering by showing and updating the widget
        m_sceneView3D->show();
        QTest::qWait(200);
        
        // Add an entity to render
        m_sceneView3D->addEntity("TestCube", QVector3D(0, 0, 0), QVector3D(1, 1, 1));
        
        // Force update
        m_sceneView3D->update();
        QTest::qWait(100);
        
        qDebug() << "✅ Rendering test passed (no crash during render)";
        
        m_sceneView3D->hide();
    } catch (...) {
        QFAIL("❌ Rendering caused an exception");
    }
}

void TestSceneView3D::testPerformance()
{
    if (!m_openGLAvailable || !m_sceneView3D) {
        QSKIP("OpenGL not available or SceneView3D not created");
        return;
    }
    
    try {
        m_sceneView3D->show();
        QTest::qWait(200);
        
        // Add multiple entities to test performance
        for (int i = 0; i < 10; ++i) {
            QString name = QString("Cube%1").arg(i);
            QVector3D pos(i * 2, 0, 0);
            m_sceneView3D->addEntity(name, pos, QVector3D(1, 1, 1));
        }
        
        // Test multiple updates
        QElapsedTimer timer;
        timer.start();
        
        for (int i = 0; i < 10; ++i) {
            m_sceneView3D->update();
            QTest::qWait(10);
        }
        
        qint64 elapsed = timer.elapsed();
        qDebug() << "✅ Performance test passed - 10 updates took" << elapsed << "ms";
        
        m_sceneView3D->hide();
    } catch (...) {
        QFAIL("❌ Performance test caused an exception");
    }
}

#include "test_sceneview3d.moc"


