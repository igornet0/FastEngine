#include "TooltipManagerTest.h"
#include <QDebug>
#include <QPushButton>
#include <QLabel>

TooltipManagerTest::TooltipManagerTest(QObject *parent)
    : QObject(parent)
    , m_tooltipManager(nullptr)
    , m_app(nullptr)
    , m_testWidget(nullptr)
{
}

void TooltipManagerTest::initTestCase()
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
    qDebug() << "✅ QApplication initialized for TooltipManager testing";
}

void TooltipManagerTest::cleanupTestCase()
{
    qDebug() << "🧹 Cleaning up TooltipManager test case";
}

void TooltipManagerTest::init()
{
    m_tooltipManager = new TooltipManager(this);
    QVERIFY(m_tooltipManager != nullptr);
    
    m_testWidget = new QPushButton("Test Button", nullptr);
    QVERIFY(m_testWidget != nullptr);
    
    qDebug() << "✅ TooltipManager and test widget created";
}

void TooltipManagerTest::cleanup()
{
    if (m_testWidget) {
        delete m_testWidget;
        m_testWidget = nullptr;
    }
    
    if (m_tooltipManager) {
        delete m_tooltipManager;
        m_tooltipManager = nullptr;
    }
    
    qDebug() << "🧹 TooltipManager test cleaned up";
}

void TooltipManagerTest::testRegisterTooltip()
{
    qDebug() << "🧪 Testing basic tooltip registration...";
    
    QString tooltipText = "This is a test tooltip";
    m_tooltipManager->registerTooltip(m_testWidget, tooltipText);
    
    QCOMPARE(m_testWidget->toolTip(), tooltipText);
    
    qDebug() << "✅ Basic tooltip registration test passed";
}

void TooltipManagerTest::testRegisterTooltipWithHotkey()
{
    qDebug() << "🧪 Testing tooltip registration with hotkey...";
    
    QString tooltipText = "Test tooltip";
    QString hotkey = "Ctrl+T";
    QString expectedTooltip = QString("%1\nHotkey: %2").arg(tooltipText, hotkey);
    
    m_tooltipManager->registerTooltipWithHotkey(m_testWidget, tooltipText, hotkey);
    
    QCOMPARE(m_testWidget->toolTip(), expectedTooltip);
    
    qDebug() << "✅ Tooltip with hotkey registration test passed";
}

void TooltipManagerTest::testToolTooltips()
{
    qDebug() << "🧪 Testing tool tooltips...";
    
    // Test various tool tooltips
    QString moveTooltip = m_tooltipManager->getToolTooltip(0); // Move Tool
    QVERIFY(!moveTooltip.isEmpty());
    QVERIFY(moveTooltip.contains("Move Tool"));
    
    QString brushTooltip = m_tooltipManager->getToolTooltip(5); // Brush Tool
    QVERIFY(!brushTooltip.isEmpty());
    QVERIFY(brushTooltip.contains("Brush Tool"));
    
    qDebug() << "✅ Tool tooltips test passed";
}

void TooltipManagerTest::testToolHotkeys()
{
    qDebug() << "🧪 Testing tool hotkeys...";
    
    // Test various tool hotkeys
    QString moveHotkey = m_tooltipManager->getToolHotkey(0); // Move Tool
    QCOMPARE(moveHotkey, QString("V"));
    
    QString brushHotkey = m_tooltipManager->getToolHotkey(5); // Brush Tool
    QCOMPARE(brushHotkey, QString("B"));
    
    QString eyedropperHotkey = m_tooltipManager->getToolHotkey(13); // Eyedropper Tool
    QCOMPARE(eyedropperHotkey, QString("I"));
    
    qDebug() << "✅ Tool hotkeys test passed";
}

void TooltipManagerTest::testMoveToolTooltip()
{
    qDebug() << "🧪 Testing Move Tool tooltip...";
    
    QString tooltip = m_tooltipManager->getToolTooltip(0);
    QString hotkey = m_tooltipManager->getToolHotkey(0);
    
    QVERIFY(tooltip.contains("Move Tool"));
    QVERIFY(tooltip.contains("Move and transform"));
    QCOMPARE(hotkey, QString("V"));
    
    qDebug() << "✅ Move Tool tooltip test passed";
}

void TooltipManagerTest::testBrushToolTooltip()
{
    qDebug() << "🧪 Testing Brush Tool tooltip...";
    
    QString tooltip = m_tooltipManager->getToolTooltip(5);
    QString hotkey = m_tooltipManager->getToolHotkey(5);
    
    QVERIFY(tooltip.contains("Brush Tool"));
    QVERIFY(tooltip.contains("Paint with"));
    QCOMPARE(hotkey, QString("B"));
    
    qDebug() << "✅ Brush Tool tooltip test passed";
}

void TooltipManagerTest::testEyedropperToolTooltip()
{
    qDebug() << "🧪 Testing Eyedropper Tool tooltip...";
    
    QString tooltip = m_tooltipManager->getToolTooltip(13);
    QString hotkey = m_tooltipManager->getToolHotkey(13);
    
    QVERIFY(tooltip.contains("Eyedropper Tool"));
    QVERIFY(tooltip.contains("Sample colors"));
    QCOMPARE(hotkey, QString("I"));
    
    qDebug() << "✅ Eyedropper Tool tooltip test passed";
}

void TooltipManagerTest::testGradientToolTooltip()
{
    qDebug() << "🧪 Testing Gradient Tool tooltip...";
    
    QString tooltip = m_tooltipManager->getToolTooltip(10);
    QString hotkey = m_tooltipManager->getToolHotkey(10);
    
    QVERIFY(tooltip.contains("Gradient Tool"));
    QVERIFY(tooltip.contains("Create color gradients"));
    QCOMPARE(hotkey, QString("G"));
    
    qDebug() << "✅ Gradient Tool tooltip test passed";
}

void TooltipManagerTest::testNullWidget()
{
    qDebug() << "🧪 Testing null widget handling...";
    
    // Should not crash when passing null widget
    m_tooltipManager->registerTooltip(nullptr, "Test tooltip");
    m_tooltipManager->registerTooltipWithHotkey(nullptr, "Test tooltip", "Ctrl+T");
    
    qDebug() << "✅ Null widget handling test passed";
}

void TooltipManagerTest::testEmptyTooltip()
{
    qDebug() << "🧪 Testing empty tooltip handling...";
    
    QPushButton *testButton = new QPushButton("Empty Test", nullptr);
    
    // Test with empty tooltip
    m_tooltipManager->registerTooltip(testButton, "");
    m_tooltipManager->registerTooltipWithHotkey(testButton, "", "Ctrl+E");
    
    delete testButton;
    
    qDebug() << "✅ Empty tooltip handling test passed";
}

void TooltipManagerTest::testInvalidToolType()
{
    qDebug() << "🧪 Testing invalid tool type handling...";
    
    // Test with invalid tool type
    QString tooltip = m_tooltipManager->getToolTooltip(999);
    QString hotkey = m_tooltipManager->getToolHotkey(999);
    
    QCOMPARE(tooltip, QString("Unknown Tool"));
    QCOMPARE(hotkey, QString(""));
    
    qDebug() << "✅ Invalid tool type handling test passed";
}

// QTEST_MAIN removed - using main.cpp instead
#include "TooltipManagerTest.moc"
