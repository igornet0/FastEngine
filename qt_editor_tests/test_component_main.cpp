#include <QApplication>
#include <QTest>
#include <QDebug>

#include "test_hierarchypanel.h"
#include "test_inspectorpanel.h"
#include "test_assetspanel.h"
#include "test_consolepanel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    int status = 0;
    
    qDebug() << "=== Testing Individual Components ===";
    
    // Test HierarchyPanel
    {
        qDebug() << "\n--- Testing HierarchyPanel ---";
        TestHierarchyPanel test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Test InspectorPanel
    {
        qDebug() << "\n--- Testing InspectorPanel ---";
        TestInspectorPanel test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Test AssetsPanel
    {
        qDebug() << "\n--- Testing AssetsPanel ---";
        TestAssetsPanel test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Test ConsolePanel
    {
        qDebug() << "\n--- Testing ConsolePanel ---";
        TestConsolePanel test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    qDebug() << "\n=== Component tests completed ===";
    
    if (status == 0) {
        qDebug() << "✅ ALL COMPONENT TESTS PASSED!";
    } else {
        qDebug() << "❌ Some component tests failed. Exit code:" << status;
    }
    
    return status;
}
