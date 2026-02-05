#include <QApplication>
#include <QTest>
#include <QDebug>

#include "test_fastengineintegration.h"
#include "test_projectmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    int status = 0;
    
    qDebug() << "=== Testing FastEngine Integration ===";
    
    // Test ProjectManager first (safer)
    {
        qDebug() << "\n--- Testing ProjectManager ---";
        TestProjectManager test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Test FastEngineIntegration (potential segfault source)
    {
        qDebug() << "\n--- Testing FastEngineIntegration (potential segfault source) ---";
        TestFastEngineIntegration test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    qDebug() << "\n=== Integration tests completed ===";
    
    if (status == 0) {
        qDebug() << "✅ ALL INTEGRATION TESTS PASSED!";
    } else {
        qDebug() << "❌ Some integration tests failed. Exit code:" << status;
    }
    
    return status;
}


