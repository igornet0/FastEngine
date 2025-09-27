#include <QApplication>
#include <QTest>
#include <QDebug>

#include "test_sceneview2d.h"
#include "test_sceneview3d.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    int status = 0;
    
    qDebug() << "=== Testing Scene Views ===";
    
    // Test SceneView2D
    {
        qDebug() << "\n--- Testing SceneView2D ---";
        TestSceneView2D test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Test SceneView3D (this might be the segfault source)
    {
        qDebug() << "\n--- Testing SceneView3D (potential segfault source) ---";
        TestSceneView3D test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    qDebug() << "\n=== Scene view tests completed ===";
    
    if (status == 0) {
        qDebug() << "✅ ALL SCENE VIEW TESTS PASSED!";
    } else {
        qDebug() << "❌ Some scene view tests failed. Exit code:" << status;
    }
    
    return status;
}
