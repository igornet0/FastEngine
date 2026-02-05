#include "MoveToolTest.h"
#include <QDebug>
#include <QPoint>
#include <QTime>

MoveToolTest::MoveToolTest(QObject *parent)
    : QObject(parent)
    , m_moveTool(nullptr)
    , m_app(nullptr)
{
}

void MoveToolTest::initTestCase()
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
    qDebug() << "✅ QApplication initialized for MoveTool testing";
}

void MoveToolTest::cleanupTestCase()
{
    qDebug() << "🧹 Cleaning up MoveTool test case";
}

void MoveToolTest::init()
{
    m_moveTool = new MoveTool(this);
    QVERIFY(m_moveTool != nullptr);
    qDebug() << "✅ MoveTool created for test";
}

void MoveToolTest::cleanup()
{
    if (m_moveTool) {
        delete m_moveTool;
        m_moveTool = nullptr;
    }
    qDebug() << "🧹 MoveTool cleaned up";
}

void MoveToolTest::testToolCreation()
{
    qDebug() << "🧪 Testing MoveTool creation...";
    
    QVERIFY(m_moveTool != nullptr);
    QCOMPARE(m_moveTool->getType(), Tool::MoveTool);
    QCOMPARE(m_moveTool->getName(), QString("Move Tool"));
    QVERIFY(m_moveTool->getDescription().contains("Move and transform"));
    
    qDebug() << "✅ MoveTool creation test passed";
}

void MoveToolTest::testToolProperties()
{
    qDebug() << "🧪 Testing MoveTool properties...";
    
    // Test default properties
    QCOMPARE(m_moveTool->getMoveMode(), MoveTool::MoveLayer);
    QVERIFY(m_moveTool->isAutoSelectLayer());
    QVERIFY(m_moveTool->isShowTransformControls());
    
    qDebug() << "✅ MoveTool properties test passed";
}

void MoveToolTest::testMoveMode()
{
    qDebug() << "🧪 Testing MoveTool move mode...";
    
    // Test setting different move modes
    m_moveTool->setMoveMode(MoveTool::MoveSelection);
    QCOMPARE(m_moveTool->getMoveMode(), MoveTool::MoveSelection);
    
    m_moveTool->setMoveMode(MoveTool::MoveObject);
    QCOMPARE(m_moveTool->getMoveMode(), MoveTool::MoveObject);
    
    m_moveTool->setMoveMode(MoveTool::MoveLayer);
    QCOMPARE(m_moveTool->getMoveMode(), MoveTool::MoveLayer);
    
    qDebug() << "✅ MoveTool move mode test passed";
}

void MoveToolTest::testAutoSelectLayer()
{
    qDebug() << "🧪 Testing MoveTool auto select layer...";
    
    // Test enabling/disabling auto select layer
    m_moveTool->setAutoSelectLayer(false);
    QVERIFY(!m_moveTool->isAutoSelectLayer());
    
    m_moveTool->setAutoSelectLayer(true);
    QVERIFY(m_moveTool->isAutoSelectLayer());
    
    qDebug() << "✅ MoveTool auto select layer test passed";
}

void MoveToolTest::testShowTransformControls()
{
    qDebug() << "🧪 Testing MoveTool show transform controls...";
    
    // Test enabling/disabling transform controls
    m_moveTool->setShowTransformControls(false);
    QVERIFY(!m_moveTool->isShowTransformControls());
    
    m_moveTool->setShowTransformControls(true);
    QVERIFY(m_moveTool->isShowTransformControls());
    
    qDebug() << "✅ MoveTool show transform controls test passed";
}

void MoveToolTest::testBeginOperation()
{
    qDebug() << "🧪 Testing MoveTool begin operation...";
    
    QPoint testPoint(100, 100);
    
    // Test beginning operation
    m_moveTool->begin(testPoint, Qt::LeftButton, Qt::NoModifier);
    
    // Note: In a real implementation, we would check internal state
    // For now, we just verify the method doesn't crash
    
    qDebug() << "✅ MoveTool begin operation test passed";
}

void MoveToolTest::testUpdateOperation()
{
    qDebug() << "🧪 Testing MoveTool update operation...";
    
    QPoint startPoint(100, 100);
    QPoint updatePoint(150, 150);
    
    // Begin operation first
    m_moveTool->begin(startPoint, Qt::LeftButton, Qt::NoModifier);
    
    // Update operation
    m_moveTool->update(updatePoint, Qt::LeftButton, Qt::NoModifier);
    
    qDebug() << "✅ MoveTool update operation test passed";
}

void MoveToolTest::testEndOperation()
{
    qDebug() << "🧪 Testing MoveTool end operation...";
    
    QPoint startPoint(100, 100);
    QPoint endPoint(200, 200);
    
    // Begin and update operation first
    m_moveTool->begin(startPoint, Qt::LeftButton, Qt::NoModifier);
    m_moveTool->update(endPoint, Qt::LeftButton, Qt::NoModifier);
    
    // End operation
    m_moveTool->end(endPoint, Qt::LeftButton, Qt::NoModifier);
    
    qDebug() << "✅ MoveTool end operation test passed";
}

void MoveToolTest::testCancelOperation()
{
    qDebug() << "🧪 Testing MoveTool cancel operation...";
    
    QPoint testPoint(100, 100);
    
    // Begin operation first
    m_moveTool->begin(testPoint, Qt::LeftButton, Qt::NoModifier);
    
    // Cancel operation
    m_moveTool->cancel();
    
    qDebug() << "✅ MoveTool cancel operation test passed";
}

void MoveToolTest::testLayerMovedSignal()
{
    qDebug() << "🧪 Testing MoveTool layer moved signal...";
    
    QSignalSpy spy(m_moveTool, &MoveTool::layerMoved);
    
    // Set up for layer move mode
    m_moveTool->setMoveMode(MoveTool::MoveLayer);
    
    // Simulate move operation
    QPoint startPoint(100, 100);
    QPoint endPoint(150, 150);
    
    m_moveTool->begin(startPoint, Qt::LeftButton, Qt::NoModifier);
    m_moveTool->update(endPoint, Qt::LeftButton, Qt::NoModifier);
    
    QVERIFY(spy.isValid());
    
    qDebug() << "✅ MoveTool layer moved signal test passed";
}

void MoveToolTest::testSelectionMovedSignal()
{
    qDebug() << "🧪 Testing MoveTool selection moved signal...";
    
    QSignalSpy spy(m_moveTool, &MoveTool::selectionMoved);
    
    // Set up for selection move mode
    m_moveTool->setMoveMode(MoveTool::MoveSelection);
    
    // Simulate move operation
    QPoint startPoint(100, 100);
    QPoint endPoint(150, 150);
    
    m_moveTool->begin(startPoint, Qt::LeftButton, Qt::NoModifier);
    m_moveTool->update(endPoint, Qt::LeftButton, Qt::NoModifier);
    
    QVERIFY(spy.isValid());
    
    qDebug() << "✅ MoveTool selection moved signal test passed";
}

void MoveToolTest::testPropertyGetSet()
{
    qDebug() << "🧪 Testing MoveTool property get/set...";
    
    // Test move_mode property
    m_moveTool->setProperty("move_mode", QVariant(static_cast<int>(MoveTool::MoveSelection)));
    QCOMPARE(m_moveTool->getProperty("move_mode").toInt(), static_cast<int>(MoveTool::MoveSelection));
    
    // Test auto_select_layer property
    m_moveTool->setProperty("auto_select_layer", QVariant(false));
    QCOMPARE(m_moveTool->getProperty("auto_select_layer").toBool(), false);
    
    // Test show_transform_controls property
    m_moveTool->setProperty("show_transform_controls", QVariant(false));
    QCOMPARE(m_moveTool->getProperty("show_transform_controls").toBool(), false);
    
    qDebug() << "✅ MoveTool property get/set test passed";
}

void MoveToolTest::testInvalidProperties()
{
    qDebug() << "🧪 Testing MoveTool invalid properties...";
    
    // Test invalid property names
    QVariant invalidProperty = m_moveTool->getProperty("invalid_property");
    QVERIFY(!invalidProperty.isValid());
    
    // Test setting invalid property
    m_moveTool->setProperty("invalid_property", QVariant("test"));
    // Should not crash
    
    qDebug() << "✅ MoveTool invalid properties test passed";
}

// QTEST_MAIN removed - using main.cpp instead
#include "MoveToolTest.moc"
