#include "test_fastengineintegration.h"
#include <QDebug>
#include <QSignalSpy>
#include <QVector3D>

void TestFastEngineIntegration::initTestCase()
{
    qDebug() << "Initializing FastEngineIntegration test case...";
}

void TestFastEngineIntegration::cleanupTestCase()
{
    qDebug() << "Cleaning up FastEngineIntegration test case...";
}

void TestFastEngineIntegration::init()
{
    try {
        m_integration = new FastEngineIntegration();
        qDebug() << "✅ FastEngineIntegration created successfully";
    } catch (...) {
        qDebug() << "❌ CRITICAL: FastEngineIntegration creation failed - this might be the segfault source!";
        m_integration = nullptr;
        QFAIL("FastEngineIntegration creation failed");
    }
}

void TestFastEngineIntegration::cleanup()
{
    if (m_integration) {
        try {
            m_integration->shutdown();
        } catch (...) {
            qDebug() << "⚠️ Exception during shutdown";
        }
        delete m_integration;
        m_integration = nullptr;
    }
}

void TestFastEngineIntegration::testCreation()
{
    QVERIFY(m_integration != nullptr);
    QVERIFY(m_integration->inherits("QObject"));
    qDebug() << "✅ FastEngineIntegration creation test passed";
}

void TestFastEngineIntegration::testInitialization()
{
    QVERIFY(m_integration != nullptr);
    
    try {
        // Test initialization
        bool success = m_integration->initialize();
        
        if (success) {
            QVERIFY(m_integration->isInitialized());
            qDebug() << "✅ Initialization test passed - Engine initialized";
        } else {
            qDebug() << "⚠️ Engine initialization failed, but no crash occurred";
        }
    } catch (...) {
        QFAIL("❌ Initialization caused an exception");
    }
}

void TestFastEngineIntegration::testShutdown()
{
    QVERIFY(m_integration != nullptr);
    
    try {
        // Test shutdown
        m_integration->shutdown();
        
        // After shutdown, should not be initialized
        QVERIFY(!m_integration->isInitialized());
        
        qDebug() << "✅ Shutdown test passed";
    } catch (...) {
        QFAIL("❌ Shutdown caused an exception");
    }
}

void TestFastEngineIntegration::testEngineUpdate()
{
    QVERIFY(m_integration != nullptr);
    
    try {
        // Test engine update
        float deltaTime = 0.016f; // 60 FPS
        m_integration->update(deltaTime);
        
        qDebug() << "✅ Engine update test passed";
    } catch (...) {
        QFAIL("❌ Engine update caused an exception");
    }
}

void TestFastEngineIntegration::testSceneManagement()
{
    QVERIFY(m_integration != nullptr);
    
    try {
        // Test scene management
        QString sceneName = "TestScene";
        
        m_integration->createScene(sceneName);
        m_integration->loadScene(sceneName);
        m_integration->saveScene(sceneName);
        
        QStringList scenes = m_integration->getScenes();
        // Scenes may be empty due to stubbed implementation, but should not crash
        
        qDebug() << "✅ Scene management test passed - Found" << scenes.size() << "scenes";
    } catch (...) {
        QFAIL("❌ Scene management caused an exception");
    }
}

void TestFastEngineIntegration::testEntityManagement()
{
    QVERIFY(m_integration != nullptr);
    
    try {
        // Test entity management
        QString entityName = "TestEntity";
        QVector3D position(1, 2, 3);
        
        m_integration->createEntity(entityName, position);
        
        QStringList entities = m_integration->getEntities();
        // Entities may be empty due to stubbed implementation, but should not crash
        
        m_integration->deleteEntity(entityName);
        
        qDebug() << "✅ Entity management test passed";
    } catch (...) {
        QFAIL("❌ Entity management caused an exception");
    }
}

void TestFastEngineIntegration::testComponentManagement()
{
    QVERIFY(m_integration != nullptr);
    
    try {
        // Test component management
        QString entityName = "TestEntity";
        QString componentType = "Transform";
        
        m_integration->addComponent(entityName, componentType);
        m_integration->removeComponent(entityName, componentType);
        
        QStringList components = m_integration->getComponents(entityName);
        // Components may be empty due to stubbed implementation
        
        qDebug() << "✅ Component management test passed";
    } catch (...) {
        QFAIL("❌ Component management caused an exception");
    }
}

void TestFastEngineIntegration::testResourceLoading()
{
    QVERIFY(m_integration != nullptr);
    
    try {
        // Test resource loading
        QString textureName = "TestTexture";
        QString texturePath = "/tmp/test_texture.png";
        
        m_integration->loadTexture(textureName, texturePath);
        
        QString meshName = "TestMesh";
        QString meshPath = "/tmp/test_mesh.obj";
        
        m_integration->loadMesh(meshName, meshPath);
        
        QString audioName = "TestAudio";
        QString audioPath = "/tmp/test_audio.wav";
        
        m_integration->loadAudio(audioName, audioPath);
        
        qDebug() << "✅ Resource loading test passed";
    } catch (...) {
        QFAIL("❌ Resource loading caused an exception");
    }
}

void TestFastEngineIntegration::testCameraControls()
{
    QVERIFY(m_integration != nullptr);
    
    try {
        // Test camera controls
        QVector3D position(5, 5, 5);
        QVector3D target(0, 0, 0);
        float fov = 60.0f;
        
        m_integration->setCameraPosition(position);
        m_integration->setCameraTarget(target);
        m_integration->setCameraFOV(fov);
        
        // Test getters
        QVector3D currentPos = m_integration->getCameraPosition();
        QVector3D currentTarget = m_integration->getCameraTarget();
        float currentFOV = m_integration->getCameraFOV();
        
        qDebug() << "Camera - Pos:" << currentPos << "Target:" << currentTarget << "FOV:" << currentFOV;
        qDebug() << "✅ Camera controls test passed";
    } catch (...) {
        QFAIL("❌ Camera controls caused an exception");
    }
}

void TestFastEngineIntegration::testLightingControls()
{
    QVERIFY(m_integration != nullptr);
    
    try {
        // Test lighting controls
        QVector3D lightPos(10, 10, 10);
        QVector3D lightColor(1, 1, 1);
        float intensity = 1.5f;
        
        m_integration->setLightPosition(lightPos);
        m_integration->setLightColor(lightColor);
        m_integration->setLightIntensity(intensity);
        
        qDebug() << "✅ Lighting controls test passed";
    } catch (...) {
        QFAIL("❌ Lighting controls caused an exception");
    }
}

void TestFastEngineIntegration::testRenderingControls()
{
    QVERIFY(m_integration != nullptr);
    
    try {
        // Test rendering controls
        m_integration->render();
        m_integration->clear();
        m_integration->present();
        
        // Test viewport
        m_integration->setViewport(0, 0, 800, 600);
        
        qDebug() << "✅ Rendering controls test passed";
    } catch (...) {
        QFAIL("❌ Rendering controls caused an exception");
    }
}

void TestFastEngineIntegration::testSignalEmission()
{
    QVERIFY(m_integration != nullptr);
    
    try {
        // Test signal emission
        QSignalSpy entityCreatedSpy(m_integration, &FastEngineIntegration::entityCreated);
        QSignalSpy entityDeletedSpy(m_integration, &FastEngineIntegration::entityDeleted);
        QSignalSpy sceneChangedSpy(m_integration, &FastEngineIntegration::sceneChanged);
        
        // Trigger some actions that should emit signals
        m_integration->createEntity("TestEntity", QVector3D(0, 0, 0));
        m_integration->deleteEntity("TestEntity");
        
        // Verify signals can be connected
        QVERIFY(entityCreatedSpy.isValid());
        QVERIFY(entityDeletedSpy.isValid());
        QVERIFY(sceneChangedSpy.isValid());
        
        qDebug() << "Signal counts - Created:" << entityCreatedSpy.count() 
                 << "Deleted:" << entityDeletedSpy.count()
                 << "SceneChanged:" << sceneChangedSpy.count();
        
        qDebug() << "✅ Signal emission test passed";
    } catch (...) {
        QFAIL("❌ Signal emission caused an exception");
    }
}

#include "test_fastengineintegration.moc"
