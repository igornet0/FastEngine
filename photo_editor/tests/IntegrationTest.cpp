#include "IntegrationTest.h"
#include <QDebug>
#include <QTimer>
#include <QTime>

IntegrationTest::IntegrationTest(QObject *parent)
    : QObject(parent)
    , m_hotkeyManager(nullptr)
    , m_tooltipManager(nullptr)
    , m_moveTool(nullptr)
    , m_eyedropperTool(nullptr)
    , m_app(nullptr)
    , m_testWidget(nullptr)
{
}

void IntegrationTest::initTestCase()
{
    // Initialize QApplication if not already done
    if (!QApplication::instance()) {
        int argc = 0;
        char **argv = nullptr;
        m_app = new QApplication(argc, argv);
    } else {
        m_app = qobject_cast<QApplication*>(QApplication::instance());
    }
    
    QVERIFY(QApplication::instance() != nullptr);
    qDebug() << "✅ QApplication initialized for integration testing";
}

void IntegrationTest::cleanupTestCase()
{
    qDebug() << "🧹 Cleaning up integration test case";
}

void IntegrationTest::init()
{
    // Create all components
    m_hotkeyManager = new HotkeyManager(this);
    m_tooltipManager = new TooltipManager(this);
    m_moveTool = new MoveTool(this);
    m_eyedropperTool = new EyedropperTool(this);
    m_testWidget = new QPushButton("Test Button", nullptr);
    
    QVERIFY(m_hotkeyManager != nullptr);
    QVERIFY(m_tooltipManager != nullptr);
    QVERIFY(m_moveTool != nullptr);
    QVERIFY(m_eyedropperTool != nullptr);
    QVERIFY(m_testWidget != nullptr);
    
    qDebug() << "✅ All components created for integration test";
}

void IntegrationTest::cleanup()
{
    if (m_testWidget) {
        delete m_testWidget;
        m_testWidget = nullptr;
    }
    
    if (m_eyedropperTool) {
        delete m_eyedropperTool;
        m_eyedropperTool = nullptr;
    }
    
    if (m_moveTool) {
        delete m_moveTool;
        m_moveTool = nullptr;
    }
    
    if (m_tooltipManager) {
        delete m_tooltipManager;
        m_tooltipManager = nullptr;
    }
    
    if (m_hotkeyManager) {
        delete m_hotkeyManager;
        m_hotkeyManager = nullptr;
    }
    
    qDebug() << "🧹 Integration test cleaned up";
}

void IntegrationTest::testHotkeyManagerWithTooltipManager()
{
    qDebug() << "🧪 Testing HotkeyManager integration with TooltipManager...";
    
    // Register tooltips with hotkeys
    QString moveTooltip = m_tooltipManager->getToolTooltip(0); // Move Tool
    QString moveHotkey = m_tooltipManager->getToolHotkey(0);
    
    QVERIFY(!moveTooltip.isEmpty());
    QCOMPARE(moveHotkey, QString("V"));
    
    // Register hotkey in HotkeyManager
    m_hotkeyManager->registerHotkey("move_tool", QKeySequence("V"));
    
    // Register tooltip with hotkey
    m_tooltipManager->registerTooltipWithHotkey(m_testWidget, moveTooltip, moveHotkey);
    
    QString expectedTooltip = QString("%1\nHotkey: %2").arg(moveTooltip, moveHotkey);
    QCOMPARE(m_testWidget->toolTip(), expectedTooltip);
    
    qDebug() << "✅ HotkeyManager-TooltipManager integration test passed";
}

void IntegrationTest::testToolIntegration()
{
    qDebug() << "🧪 Testing tool integration...";
    
    // Test that tools work together
    QCOMPARE(m_moveTool->getType(), Tool::MoveTool);
    QCOMPARE(m_eyedropperTool->getType(), Tool::EyedropperTool);
    
    // Test tool properties
    m_moveTool->setMoveMode(MoveTool::MoveSelection);
    QCOMPARE(m_moveTool->getMoveMode(), MoveTool::MoveSelection);
    
    m_eyedropperTool->setSampleSize(EyedropperTool::FiveByFive);
    QCOMPARE(m_eyedropperTool->getSampleSize(), EyedropperTool::FiveByFive);
    
    qDebug() << "✅ Tool integration test passed";
}

void IntegrationTest::testSignalIntegration()
{
    qDebug() << "🧪 Testing signal integration...";
    
    // Test signal connections
    QSignalSpy moveSpy(m_moveTool, &MoveTool::layerMoved);
    QSignalSpy eyedropperSpy(m_eyedropperTool, &EyedropperTool::colorPicked);
    QSignalSpy hotkeySpy(m_hotkeyManager, &HotkeyManager::hotkeyTriggered);
    
    QVERIFY(moveSpy.isValid());
    QVERIFY(eyedropperSpy.isValid());
    QVERIFY(hotkeySpy.isValid());
    
    qDebug() << "✅ Signal integration test passed";
}

void IntegrationTest::testPropertyIntegration()
{
    qDebug() << "🧪 Testing property integration...";
    
    // Test property system across tools
    m_moveTool->setProperty("move_mode", QVariant(static_cast<int>(MoveTool::MoveObject)));
    QCOMPARE(m_moveTool->getProperty("move_mode").toInt(), static_cast<int>(MoveTool::MoveObject));
    
    m_eyedropperTool->setProperty("sample_size", QVariant(static_cast<int>(EyedropperTool::ThreeByThree)));
    QCOMPARE(m_eyedropperTool->getProperty("sample_size").toInt(), static_cast<int>(EyedropperTool::ThreeByThree));
    
    qDebug() << "✅ Property integration test passed";
}

void IntegrationTest::testCompleteWorkflow()
{
    qDebug() << "🧪 Testing complete workflow...";
    
    // Simulate a complete workflow
    // 1. Register hotkeys
    m_hotkeyManager->registerToolHotkeys();
    m_hotkeyManager->registerFileHotkeys();
    
    // 2. Set up tooltips
    m_tooltipManager->registerToolTooltips();
    
    // 3. Configure tools
    m_moveTool->setMoveMode(MoveTool::MoveLayer);
    m_moveTool->setAutoSelectLayer(true);
    
    m_eyedropperTool->setSampleSize(EyedropperTool::ThreeByThree);
    m_eyedropperTool->setShowColorInfo(true);
    
    // 4. Test tool operations
    QPoint testPoint(100, 100);
    
    m_moveTool->begin(testPoint, Qt::LeftButton, Qt::NoModifier);
    m_moveTool->update(QPoint(150, 150), Qt::LeftButton, Qt::NoModifier);
    m_moveTool->end(QPoint(150, 150), Qt::LeftButton, Qt::NoModifier);
    
    m_eyedropperTool->begin(testPoint, Qt::LeftButton, Qt::NoModifier);
    m_eyedropperTool->end(testPoint, Qt::LeftButton, Qt::NoModifier);
    
    qDebug() << "✅ Complete workflow test passed";
}

void IntegrationTest::testErrorHandling()
{
    qDebug() << "🧪 Testing error handling...";
    
    // Test null pointer handling
    m_hotkeyManager->registerHotkey("test", QKeySequence("Ctrl+T"), nullptr);
    m_tooltipManager->registerTooltip(nullptr, "Test tooltip");
    
    // Test invalid properties
    m_moveTool->setProperty("invalid_property", QVariant("test"));
    m_eyedropperTool->setProperty("invalid_property", QVariant("test"));
    
    // Test invalid tool types
    QString invalidTooltip = m_tooltipManager->getToolTooltip(999);
    QString invalidHotkey = m_tooltipManager->getToolHotkey(999);
    
    QCOMPARE(invalidTooltip, QString("Unknown Tool"));
    QCOMPARE(invalidHotkey, QString(""));
    
    qDebug() << "✅ Error handling test passed";
}

void IntegrationTest::testPerformance()
{
    qDebug() << "🧪 Testing performance...";
    
    // Test performance of multiple operations
    QElapsedTimer timer;
    timer.start();
    
    // Perform multiple operations
    for (int i = 0; i < 1000; ++i) {
        m_moveTool->setProperty("move_mode", QVariant(i % 3));
        m_eyedropperTool->setProperty("sample_size", QVariant(i % 6));
    }
    
    qint64 elapsed = timer.elapsed();
    qDebug() << "Performance test: 2000 operations took" << elapsed << "ms";
    
    // Should complete in reasonable time (less than 1 second)
    QVERIFY(elapsed < 1000);
    
    qDebug() << "✅ Performance test passed";
}

// QTEST_MAIN removed - using main.cpp instead
#include "IntegrationTest.moc"
