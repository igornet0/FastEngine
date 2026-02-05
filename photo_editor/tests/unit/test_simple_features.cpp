#include <QtTest>
#include <QApplication>
#include <QImage>
#include <QColor>
#include <QSize>

// Simple test for basic functionality
class TestSimpleFeatures : public QObject
{
    Q_OBJECT

private slots:
    void testBasicFunctionality();
};

void TestSimpleFeatures::testBasicFunctionality()
{
    // Test basic Qt functionality
    QImage testImage(100, 100, QImage::Format_ARGB32);
    testImage.fill(Qt::red);
    
    QVERIFY(!testImage.isNull());
    QCOMPARE(testImage.size(), QSize(100, 100));
    
    // Test color
    QColor pixelColor = testImage.pixelColor(50, 50);
    QCOMPARE(pixelColor, QColor(Qt::red));
    
    qDebug() << "Basic functionality test passed!";
}

QTEST_MAIN(TestSimpleFeatures)
