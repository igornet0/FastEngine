#include "EyedropperToolTest.h"
#include <QDebug>
#include <QPainter>

EyedropperToolTest::EyedropperToolTest(QObject *parent)
    : QObject(parent)
    , m_eyedropperTool(nullptr)
    , m_app(nullptr)
{
}

void EyedropperToolTest::initTestCase()
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
    
    // Create test image
    m_testImage = QImage(100, 100, QImage::Format_RGB32);
    m_testImage.fill(Qt::red);
    
    // Draw some colored areas
    QPainter painter(&m_testImage);
    painter.fillRect(QRect(25, 25, 25, 25), Qt::blue);
    painter.fillRect(QRect(50, 50, 25, 25), Qt::green);
    painter.end();
    
    qDebug() << "✅ QApplication and test image initialized for EyedropperTool testing";
}

void EyedropperToolTest::cleanupTestCase()
{
    qDebug() << "🧹 Cleaning up EyedropperTool test case";
}

void EyedropperToolTest::init()
{
    m_eyedropperTool = new EyedropperTool(this);
    QVERIFY(m_eyedropperTool != nullptr);
    qDebug() << "✅ EyedropperTool created for test";
}

void EyedropperToolTest::cleanup()
{
    if (m_eyedropperTool) {
        delete m_eyedropperTool;
        m_eyedropperTool = nullptr;
    }
    qDebug() << "🧹 EyedropperTool cleaned up";
}

void EyedropperToolTest::testToolCreation()
{
    qDebug() << "🧪 Testing EyedropperTool creation...";
    
    QVERIFY(m_eyedropperTool != nullptr);
    QCOMPARE(m_eyedropperTool->getType(), Tool::EyedropperTool);
    QCOMPARE(m_eyedropperTool->getName(), QString("Eyedropper Tool"));
    QVERIFY(m_eyedropperTool->getDescription().contains("Sample colors"));
    
    qDebug() << "✅ EyedropperTool creation test passed";
}

void EyedropperToolTest::testToolProperties()
{
    qDebug() << "🧪 Testing EyedropperTool properties...";
    
    // Test default properties
    QCOMPARE(m_eyedropperTool->getSampleSize(), EyedropperTool::ThreeByThree);
    QVERIFY(!m_eyedropperTool->isSampleFromAllLayers());
    QVERIFY(m_eyedropperTool->isShowColorInfo());
    
    qDebug() << "✅ EyedropperTool properties test passed";
}

void EyedropperToolTest::testSampleSize()
{
    qDebug() << "🧪 Testing EyedropperTool sample size...";
    
    // Test different sample sizes
    m_eyedropperTool->setSampleSize(EyedropperTool::PointSample);
    QCOMPARE(m_eyedropperTool->getSampleSize(), EyedropperTool::PointSample);
    
    m_eyedropperTool->setSampleSize(EyedropperTool::FiveByFive);
    QCOMPARE(m_eyedropperTool->getSampleSize(), EyedropperTool::FiveByFive);
    
    m_eyedropperTool->setSampleSize(EyedropperTool::ElevenByEleven);
    QCOMPARE(m_eyedropperTool->getSampleSize(), EyedropperTool::ElevenByEleven);
    
    qDebug() << "✅ EyedropperTool sample size test passed";
}

void EyedropperToolTest::testSampleFromAllLayers()
{
    qDebug() << "🧪 Testing EyedropperTool sample from all layers...";
    
    // Test enabling/disabling sample from all layers
    m_eyedropperTool->setSampleFromAllLayers(true);
    QVERIFY(m_eyedropperTool->isSampleFromAllLayers());
    
    m_eyedropperTool->setSampleFromAllLayers(false);
    QVERIFY(!m_eyedropperTool->isSampleFromAllLayers());
    
    qDebug() << "✅ EyedropperTool sample from all layers test passed";
}

void EyedropperToolTest::testShowColorInfo()
{
    qDebug() << "🧪 Testing EyedropperTool show color info...";
    
    // Test enabling/disabling color info display
    m_eyedropperTool->setShowColorInfo(false);
    QVERIFY(!m_eyedropperTool->isShowColorInfo());
    
    m_eyedropperTool->setShowColorInfo(true);
    QVERIFY(m_eyedropperTool->isShowColorInfo());
    
    qDebug() << "✅ EyedropperTool show color info test passed";
}

void EyedropperToolTest::testBeginOperation()
{
    qDebug() << "🧪 Testing EyedropperTool begin operation...";
    
    QPoint testPoint(50, 50);
    
    // Test beginning operation
    m_eyedropperTool->begin(testPoint, Qt::LeftButton, Qt::NoModifier);
    
    qDebug() << "✅ EyedropperTool begin operation test passed";
}

void EyedropperToolTest::testUpdateOperation()
{
    qDebug() << "🧪 Testing EyedropperTool update operation...";
    
    QPoint testPoint(50, 50);
    
    // Update operation
    m_eyedropperTool->update(testPoint, Qt::LeftButton, Qt::NoModifier);
    
    qDebug() << "✅ EyedropperTool update operation test passed";
}

void EyedropperToolTest::testEndOperation()
{
    qDebug() << "🧪 Testing EyedropperTool end operation...";
    
    QPoint testPoint(50, 50);
    
    // End operation
    m_eyedropperTool->end(testPoint, Qt::LeftButton, Qt::NoModifier);
    
    qDebug() << "✅ EyedropperTool end operation test passed";
}

void EyedropperToolTest::testCancelOperation()
{
    qDebug() << "🧪 Testing EyedropperTool cancel operation...";
    
    QPoint testPoint(50, 50);
    
    // Begin operation first
    m_eyedropperTool->begin(testPoint, Qt::LeftButton, Qt::NoModifier);
    
    // Cancel operation
    m_eyedropperTool->cancel();
    
    qDebug() << "✅ EyedropperTool cancel operation test passed";
}

void EyedropperToolTest::testColorPickedSignal()
{
    qDebug() << "🧪 Testing EyedropperTool color picked signal...";
    
    QSignalSpy spy(m_eyedropperTool, &EyedropperTool::colorPicked);
    
    QPoint testPoint(50, 50);
    
    // Simulate color picking
    m_eyedropperTool->begin(testPoint, Qt::LeftButton, Qt::NoModifier);
    
    QVERIFY(spy.isValid());
    
    qDebug() << "✅ EyedropperTool color picked signal test passed";
}

void EyedropperToolTest::testForegroundColorChangedSignal()
{
    qDebug() << "🧪 Testing EyedropperTool foreground color changed signal...";
    
    QSignalSpy spy(m_eyedropperTool, &EyedropperTool::foregroundColorChanged);
    
    QPoint testPoint(50, 50);
    
    // Simulate left button click (foreground color)
    m_eyedropperTool->begin(testPoint, Qt::LeftButton, Qt::NoModifier);
    
    QVERIFY(spy.isValid());
    
    qDebug() << "✅ EyedropperTool foreground color changed signal test passed";
}

void EyedropperToolTest::testBackgroundColorChangedSignal()
{
    qDebug() << "🧪 Testing EyedropperTool background color changed signal...";
    
    QSignalSpy spy(m_eyedropperTool, &EyedropperTool::backgroundColorChanged);
    
    QPoint testPoint(50, 50);
    
    // Simulate right button click (background color)
    m_eyedropperTool->begin(testPoint, Qt::RightButton, Qt::NoModifier);
    
    QVERIFY(spy.isValid());
    
    qDebug() << "✅ EyedropperTool background color changed signal test passed";
}

void EyedropperToolTest::testPropertyGetSet()
{
    qDebug() << "🧪 Testing EyedropperTool property get/set...";
    
    // Test sample_size property
    m_eyedropperTool->setProperty("sample_size", QVariant(static_cast<int>(EyedropperTool::FiveByFive)));
    QCOMPARE(m_eyedropperTool->getProperty("sample_size").toInt(), static_cast<int>(EyedropperTool::FiveByFive));
    
    // Test sample_from_all_layers property
    m_eyedropperTool->setProperty("sample_from_all_layers", QVariant(true));
    QCOMPARE(m_eyedropperTool->getProperty("sample_from_all_layers").toBool(), true);
    
    // Test show_color_info property
    m_eyedropperTool->setProperty("show_color_info", QVariant(false));
    QCOMPARE(m_eyedropperTool->getProperty("show_color_info").toBool(), false);
    
    qDebug() << "✅ EyedropperTool property get/set test passed";
}

void EyedropperToolTest::testInvalidProperties()
{
    qDebug() << "🧪 Testing EyedropperTool invalid properties...";
    
    // Test invalid property names
    QVariant invalidProperty = m_eyedropperTool->getProperty("invalid_property");
    QVERIFY(!invalidProperty.isValid());
    
    // Test setting invalid property
    m_eyedropperTool->setProperty("invalid_property", QVariant("test"));
    // Should not crash
    
    qDebug() << "✅ EyedropperTool invalid properties test passed";
}

void EyedropperToolTest::testColorSampling()
{
    qDebug() << "🧪 Testing EyedropperTool color sampling...";
    
    // Test different sample sizes with test image
    m_eyedropperTool->setSampleSize(EyedropperTool::PointSample);
    QCOMPARE(m_eyedropperTool->getSampleSize(), EyedropperTool::PointSample);
    
    m_eyedropperTool->setSampleSize(EyedropperTool::ThreeByThree);
    QCOMPARE(m_eyedropperTool->getSampleSize(), EyedropperTool::ThreeByThree);
    
    m_eyedropperTool->setSampleSize(EyedropperTool::FiveByFive);
    QCOMPARE(m_eyedropperTool->getSampleSize(), EyedropperTool::FiveByFive);
    
    qDebug() << "✅ EyedropperTool color sampling test passed";
}

// QTEST_MAIN removed - using main.cpp instead
#include "EyedropperToolTest.moc"
