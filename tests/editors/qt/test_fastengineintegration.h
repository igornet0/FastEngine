#ifndef TEST_FASTENGINEINTEGRATION_H
#define TEST_FASTENGINEINTEGRATION_H

#include <QObject>
#include <QTest>

// Include the class we're testing
#include "FastEngineIntegration.h"

class TestFastEngineIntegration : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Test cases
    void testCreation();
    void testInitialization();
    void testShutdown();
    void testEngineUpdate();
    void testSceneManagement();
    void testEntityManagement();
    void testComponentManagement();
    void testResourceLoading();
    void testCameraControls();
    void testLightingControls();
    void testRenderingControls();
    void testSignalEmission();
    
private:
    FastEngineIntegration *m_integration;
};

#endif // TEST_FASTENGINEINTEGRATION_H


