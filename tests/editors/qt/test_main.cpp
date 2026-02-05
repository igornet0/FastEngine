#include <QApplication>
#include <QTest>
#include <QDebug>

// Include all test classes
#include "test_mainwindow.h"
#include "test_sceneview2d.h"
#include "test_sceneview3d.h"
#include "test_hierarchypanel.h"
#include "test_inspectorpanel.h"
#include "test_assetspanel.h"
#include "test_consolepanel.h"
#include "test_projectmanager.h"
#include "test_fastengineintegration.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    int status = 0;
    
    qDebug() << "=== Starting FastEngine Qt Editor Tests ===";
    
    // Run MainWindow tests
    {
        qDebug() << "\n--- Testing MainWindow ---";
        TestMainWindow test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run SceneView2D tests
    {
        qDebug() << "\n--- Testing SceneView2D ---";
        TestSceneView2D test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run SceneView3D tests
    {
        qDebug() << "\n--- Testing SceneView3D ---";
        TestSceneView3D test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run HierarchyPanel tests
    {
        qDebug() << "\n--- Testing HierarchyPanel ---";
        TestHierarchyPanel test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run InspectorPanel tests
    {
        qDebug() << "\n--- Testing InspectorPanel ---";
        TestInspectorPanel test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run AssetsPanel tests
    {
        qDebug() << "\n--- Testing AssetsPanel ---";
        TestAssetsPanel test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run ConsolePanel tests
    {
        qDebug() << "\n--- Testing ConsolePanel ---";
        TestConsolePanel test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run ProjectManager tests
    {
        qDebug() << "\n--- Testing ProjectManager ---";
        TestProjectManager test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run FastEngineIntegration tests
    {
        qDebug() << "\n--- Testing FastEngineIntegration ---";
        TestFastEngineIntegration test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    qDebug() << "\n=== All tests completed ===";
    
    if (status == 0) {
        qDebug() << "✅ ALL TESTS PASSED!";
    } else {
        qDebug() << "❌ Some tests failed. Exit code:" << status;
    }
    
    return status;
}


