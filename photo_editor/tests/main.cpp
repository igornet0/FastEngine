#include <QApplication>
#include <QTest>
#include "HotkeyManagerTest.h"
#include "TooltipManagerTest.h"
#include "MoveToolTest.h"
#include "EyedropperToolTest.h"
#include "IntegrationTest.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    int result = 0;
    
    // Run individual test suites
    HotkeyManagerTest hotkeyTest;
    result |= QTest::qExec(&hotkeyTest, argc, argv);
    
    TooltipManagerTest tooltipTest;
    result |= QTest::qExec(&tooltipTest, argc, argv);
    
    MoveToolTest moveTest;
    result |= QTest::qExec(&moveTest, argc, argv);
    
    EyedropperToolTest eyedropperTest;
    result |= QTest::qExec(&eyedropperTest, argc, argv);
    
    IntegrationTest integrationTest;
    result |= QTest::qExec(&integrationTest, argc, argv);
    
    return result;
}
