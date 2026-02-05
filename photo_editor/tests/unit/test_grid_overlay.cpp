#include <QtTest>
#include <QApplication>
#include <QWidget>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QJsonObject>

#include "GridOverlay.h"

class TestGridOverlay : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test grid properties
    void testSetGridVisible();
    void testSetGridSize();
    void testSetGridColor();
    void testSetGridOpacity();
    void testSetGridStyle();
    
    // Test snap to grid
    void testSetSnapToGrid();
    void testSnapToGrid();
    void testSnapRectToGrid();
    
    // Test guides
    void testAddHorizontalGuide();
    void testAddVerticalGuide();
    void testRemoveHorizontalGuide();
    void testRemoveVerticalGuide();
    void testClearGuides();
    void testSetGuidesVisible();
    void testSetGuideColor();
    
    // Test snap to guides
    void testSetSnapToGuides();
    void testSnapToGuides();
    void testSnapRectToGuides();
    
    // Test rulers
    void testSetRulersVisible();
    void testSetRulerUnit();
    void testSetRulerColor();
    
    // Test serialization
    void testToJson();
    void testFromJson();

private:
    GridOverlay *m_overlay;
    QWidget *m_parent;
};

void TestGridOverlay::initTestCase()
{
    // Initialize QApplication if not already done
    if (!QApplication::instance()) {
        int argc = 0;
        char **argv = nullptr;
        new QApplication(argc, argv);
    }
}

void TestGridOverlay::cleanupTestCase()
{
    // Cleanup handled by Qt
}

void TestGridOverlay::init()
{
    m_parent = new QWidget();
    m_overlay = new GridOverlay(m_parent);
    m_overlay->resize(QSize(800, 600));
}

void TestGridOverlay::cleanup()
{
    delete m_overlay;
    delete m_parent;
}

void TestGridOverlay::testSetGridVisible()
{
    QVERIFY(!m_overlay->isGridVisible());
    
    m_overlay->setGridVisible(true);
    QVERIFY(m_overlay->isGridVisible());
    
    m_overlay->setGridVisible(false);
    QVERIFY(!m_overlay->isGridVisible());
}

void TestGridOverlay::testSetGridSize()
{
    QCOMPARE(m_overlay->getGridSize(), 20);
    
    m_overlay->setGridSize(10);
    QCOMPARE(m_overlay->getGridSize(), 10);
    
    m_overlay->setGridSize(50);
    QCOMPARE(m_overlay->getGridSize(), 50);
    
    // Test invalid size
    m_overlay->setGridSize(0);
    QCOMPARE(m_overlay->getGridSize(), 50); // Should remain unchanged
    
    m_overlay->setGridSize(-5);
    QCOMPARE(m_overlay->getGridSize(), 50); // Should remain unchanged
}

void TestGridOverlay::testSetGridColor()
{
    QColor defaultColor = m_overlay->getGridColor();
    QVERIFY(defaultColor.isValid());
    
    QColor newColor(255, 0, 0);
    m_overlay->setGridColor(newColor);
    QCOMPARE(m_overlay->getGridColor(), newColor);
}

void TestGridOverlay::testSetGridOpacity()
{
    QCOMPARE(m_overlay->getGridOpacity(), 0.5f);
    
    m_overlay->setGridOpacity(0.8f);
    QCOMPARE(m_overlay->getGridOpacity(), 0.8f);
    
    m_overlay->setGridOpacity(1.0f);
    QCOMPARE(m_overlay->getGridOpacity(), 1.0f);
    
    // Test clamping
    m_overlay->setGridOpacity(1.5f);
    QCOMPARE(m_overlay->getGridOpacity(), 1.0f);
    
    m_overlay->setGridOpacity(-0.5f);
    QCOMPARE(m_overlay->getGridOpacity(), 0.0f);
}

void TestGridOverlay::testSetGridStyle()
{
    QCOMPARE(m_overlay->getGridStyle(), GridStyle::Lines);
    
    m_overlay->setGridStyle(GridStyle::Dots);
    QCOMPARE(m_overlay->getGridStyle(), GridStyle::Dots);
    
    m_overlay->setGridStyle(GridStyle::Crosses);
    QCOMPARE(m_overlay->getGridStyle(), GridStyle::Crosses);
}

void TestGridOverlay::testSetSnapToGrid()
{
    QVERIFY(!m_overlay->isSnapToGrid());
    
    m_overlay->setSnapToGrid(true);
    QVERIFY(m_overlay->isSnapToGrid());
    
    m_overlay->setSnapToGrid(false);
    QVERIFY(!m_overlay->isSnapToGrid());
}

void TestGridOverlay::testSnapToGrid()
{
    m_overlay->setGridSize(20);
    m_overlay->setSnapToGrid(true);
    
    // Test snapping
    QPoint point1(15, 15);
    QPoint snapped1 = m_overlay->snapToGrid(point1);
    QCOMPARE(snapped1, QPoint(20, 20));
    
    QPoint point2(25, 25);
    QPoint snapped2 = m_overlay->snapToGrid(point2);
    QCOMPARE(snapped2, QPoint(20, 20));
    
    QPoint point3(35, 35);
    QPoint snapped3 = m_overlay->snapToGrid(point3);
    QCOMPARE(snapped3, QPoint(40, 40));
    
    // Test with snap disabled
    m_overlay->setSnapToGrid(false);
    QPoint point4(15, 15);
    QPoint snapped4 = m_overlay->snapToGrid(point4);
    QCOMPARE(snapped4, point4); // Should not snap
}

void TestGridOverlay::testSnapRectToGrid()
{
    m_overlay->setGridSize(20);
    m_overlay->setSnapToGrid(true);
    
    QRect rect(15, 15, 30, 30);
    QRect snapped = m_overlay->snapToGrid(rect);
    QCOMPARE(snapped, QRect(20, 20, 40, 40));
}

void TestGridOverlay::testAddHorizontalGuide()
{
    QCOMPARE(m_overlay->getHorizontalGuides().size(), 0);
    
    m_overlay->addHorizontalGuide(100);
    QCOMPARE(m_overlay->getHorizontalGuides().size(), 1);
    QVERIFY(m_overlay->getHorizontalGuides().contains(100));
    
    m_overlay->addHorizontalGuide(200);
    QCOMPARE(m_overlay->getHorizontalGuides().size(), 2);
    QVERIFY(m_overlay->getHorizontalGuides().contains(200));
    
    // Test adding duplicate
    m_overlay->addHorizontalGuide(100);
    QCOMPARE(m_overlay->getHorizontalGuides().size(), 2); // Should not add duplicate
}

void TestGridOverlay::testAddVerticalGuide()
{
    QCOMPARE(m_overlay->getVerticalGuides().size(), 0);
    
    m_overlay->addVerticalGuide(100);
    QCOMPARE(m_overlay->getVerticalGuides().size(), 1);
    QVERIFY(m_overlay->getVerticalGuides().contains(100));
    
    m_overlay->addVerticalGuide(200);
    QCOMPARE(m_overlay->getVerticalGuides().size(), 2);
    QVERIFY(m_overlay->getVerticalGuides().contains(200));
    
    // Test adding duplicate
    m_overlay->addVerticalGuide(100);
    QCOMPARE(m_overlay->getVerticalGuides().size(), 2); // Should not add duplicate
}

void TestGridOverlay::testRemoveHorizontalGuide()
{
    m_overlay->addHorizontalGuide(100);
    m_overlay->addHorizontalGuide(200);
    QCOMPARE(m_overlay->getHorizontalGuides().size(), 2);
    
    m_overlay->removeHorizontalGuide(100);
    QCOMPARE(m_overlay->getHorizontalGuides().size(), 1);
    QVERIFY(!m_overlay->getHorizontalGuides().contains(100));
    QVERIFY(m_overlay->getHorizontalGuides().contains(200));
    
    // Test removing non-existent guide
    m_overlay->removeHorizontalGuide(300);
    QCOMPARE(m_overlay->getHorizontalGuides().size(), 1); // Should remain unchanged
}

void TestGridOverlay::testRemoveVerticalGuide()
{
    m_overlay->addVerticalGuide(100);
    m_overlay->addVerticalGuide(200);
    QCOMPARE(m_overlay->getVerticalGuides().size(), 2);
    
    m_overlay->removeVerticalGuide(100);
    QCOMPARE(m_overlay->getVerticalGuides().size(), 1);
    QVERIFY(!m_overlay->getVerticalGuides().contains(100));
    QVERIFY(m_overlay->getVerticalGuides().contains(200));
    
    // Test removing non-existent guide
    m_overlay->removeVerticalGuide(300);
    QCOMPARE(m_overlay->getVerticalGuides().size(), 1); // Should remain unchanged
}

void TestGridOverlay::testClearGuides()
{
    m_overlay->addHorizontalGuide(100);
    m_overlay->addVerticalGuide(200);
    QCOMPARE(m_overlay->getHorizontalGuides().size(), 1);
    QCOMPARE(m_overlay->getVerticalGuides().size(), 1);
    
    m_overlay->clearGuides();
    QCOMPARE(m_overlay->getHorizontalGuides().size(), 0);
    QCOMPARE(m_overlay->getVerticalGuides().size(), 0);
}

void TestGridOverlay::testSetGuidesVisible()
{
    QVERIFY(m_overlay->areGuidesVisible());
    
    m_overlay->setGuidesVisible(false);
    QVERIFY(!m_overlay->areGuidesVisible());
    
    m_overlay->setGuidesVisible(true);
    QVERIFY(m_overlay->areGuidesVisible());
}

void TestGridOverlay::testSetGuideColor()
{
    QColor defaultColor = m_overlay->getGuideColor();
    QVERIFY(defaultColor.isValid());
    
    QColor newColor(0, 255, 0);
    m_overlay->setGuideColor(newColor);
    QCOMPARE(m_overlay->getGuideColor(), newColor);
}

void TestGridOverlay::testSetSnapToGuides()
{
    QVERIFY(!m_overlay->isSnapToGuides());
    
    m_overlay->setSnapToGuides(true);
    QVERIFY(m_overlay->isSnapToGuides());
    
    m_overlay->setSnapToGuides(false);
    QVERIFY(!m_overlay->isSnapToGuides());
}

void TestGridOverlay::testSnapToGuides()
{
    m_overlay->addHorizontalGuide(100);
    m_overlay->addVerticalGuide(200);
    m_overlay->setSnapToGuides(true);
    
    // Test snapping to horizontal guide
    QPoint point1(50, 102);
    QPoint snapped1 = m_overlay->snapToGuides(point1);
    QCOMPARE(snapped1, QPoint(50, 100));
    
    // Test snapping to vertical guide
    QPoint point2(202, 50);
    QPoint snapped2 = m_overlay->snapToGuides(point2);
    QCOMPARE(snapped2, QPoint(200, 50));
    
    // Test snapping to both guides
    QPoint point3(202, 102);
    QPoint snapped3 = m_overlay->snapToGuides(point3);
    QCOMPARE(snapped3, QPoint(200, 100));
    
    // Test no snapping when far from guides
    QPoint point4(50, 50);
    QPoint snapped4 = m_overlay->snapToGuides(point4);
    QCOMPARE(snapped4, point4); // Should not snap
}

void TestGridOverlay::testSnapRectToGuides()
{
    m_overlay->addHorizontalGuide(100);
    m_overlay->addVerticalGuide(200);
    m_overlay->setSnapToGuides(true);
    
    QRect rect(202, 102, 50, 50);
    QRect snapped = m_overlay->snapToGuides(rect);
    QCOMPARE(snapped, QRect(200, 100, 50, 50));
}

void TestGridOverlay::testSetRulersVisible()
{
    QVERIFY(m_overlay->areRulersVisible());
    
    m_overlay->setRulersVisible(false);
    QVERIFY(!m_overlay->areRulersVisible());
    
    m_overlay->setRulersVisible(true);
    QVERIFY(m_overlay->areRulersVisible());
}

void TestGridOverlay::testSetRulerUnit()
{
    QCOMPARE(m_overlay->getRulerUnit(), RulerUnit::Pixels);
    
    m_overlay->setRulerUnit(RulerUnit::Inches);
    QCOMPARE(m_overlay->getRulerUnit(), RulerUnit::Inches);
    
    m_overlay->setRulerUnit(RulerUnit::Centimeters);
    QCOMPARE(m_overlay->getRulerUnit(), RulerUnit::Centimeters);
    
    m_overlay->setRulerUnit(RulerUnit::Millimeters);
    QCOMPARE(m_overlay->getRulerUnit(), RulerUnit::Millimeters);
}

void TestGridOverlay::testSetRulerColor()
{
    QColor defaultColor = m_overlay->getRulerColor();
    QVERIFY(defaultColor.isValid());
    
    QColor newColor(128, 128, 128);
    m_overlay->setRulerColor(newColor);
    QCOMPARE(m_overlay->getRulerColor(), newColor);
}

void TestGridOverlay::testToJson()
{
    // Set up some properties
    m_overlay->setGridVisible(true);
    m_overlay->setGridSize(25);
    m_overlay->setGridColor(QColor(255, 0, 0));
    m_overlay->setGridOpacity(0.7f);
    m_overlay->setGridStyle(GridStyle::Dots);
    m_overlay->setSnapToGrid(true);
    
    m_overlay->addHorizontalGuide(100);
    m_overlay->addVerticalGuide(200);
    m_overlay->setGuidesVisible(true);
    m_overlay->setGuideColor(QColor(0, 255, 0));
    m_overlay->setSnapToGuides(true);
    
    m_overlay->setRulersVisible(false);
    m_overlay->setRulerUnit(RulerUnit::Inches);
    m_overlay->setRulerColor(QColor(128, 128, 128));
    
    // Convert to JSON
    QJsonObject json = m_overlay->toJson();
    
    // Verify JSON contains expected values
    QVERIFY(json.contains("gridVisible"));
    QVERIFY(json.contains("gridSize"));
    QVERIFY(json.contains("gridColor"));
    QVERIFY(json.contains("gridOpacity"));
    QVERIFY(json.contains("gridStyle"));
    QVERIFY(json.contains("snapToGrid"));
    QVERIFY(json.contains("guidesVisible"));
    QVERIFY(json.contains("guideColor"));
    QVERIFY(json.contains("snapToGuides"));
    QVERIFY(json.contains("rulersVisible"));
    QVERIFY(json.contains("rulerUnit"));
    QVERIFY(json.contains("rulerColor"));
    QVERIFY(json.contains("horizontalGuides"));
    QVERIFY(json.contains("verticalGuides"));
    
    QCOMPARE(json["gridVisible"].toBool(), true);
    QCOMPARE(json["gridSize"].toInt(), 25);
    QCOMPARE(json["gridColor"].toString(), QColor(255, 0, 0).name());
    QCOMPARE(json["gridOpacity"].toDouble(), 0.7);
    QCOMPARE(json["gridStyle"].toInt(), static_cast<int>(GridStyle::Dots));
    QCOMPARE(json["snapToGrid"].toBool(), true);
    QCOMPARE(json["guidesVisible"].toBool(), true);
    QCOMPARE(json["guideColor"].toString(), QColor(0, 255, 0).name());
    QCOMPARE(json["snapToGuides"].toBool(), true);
    QCOMPARE(json["rulersVisible"].toBool(), false);
    QCOMPARE(json["rulerUnit"].toInt(), static_cast<int>(RulerUnit::Inches));
    QCOMPARE(json["rulerColor"].toString(), QColor(128, 128, 128).name());
}

void TestGridOverlay::testFromJson()
{
    // Create JSON object
    QJsonObject json;
    json["gridVisible"] = true;
    json["gridSize"] = 30;
    json["gridColor"] = QColor(0, 0, 255).name();
    json["gridOpacity"] = 0.8;
    json["gridStyle"] = static_cast<int>(GridStyle::Crosses);
    json["snapToGrid"] = false;
    json["guidesVisible"] = false;
    json["guideColor"] = QColor(255, 255, 0).name();
    json["snapToGuides"] = false;
    json["rulersVisible"] = true;
    json["rulerUnit"] = static_cast<int>(RulerUnit::Centimeters);
    json["rulerColor"] = QColor(64, 64, 64).name();
    
    QJsonArray hGuides;
    hGuides.append(150);
    hGuides.append(250);
    json["horizontalGuides"] = hGuides;
    
    QJsonArray vGuides;
    vGuides.append(300);
    vGuides.append(400);
    json["verticalGuides"] = vGuides;
    
    // Load from JSON
    m_overlay->fromJson(json);
    
    // Verify properties were loaded
    QCOMPARE(m_overlay->isGridVisible(), true);
    QCOMPARE(m_overlay->getGridSize(), 30);
    QCOMPARE(m_overlay->getGridColor(), QColor(0, 0, 255));
    QCOMPARE(m_overlay->getGridOpacity(), 0.8f);
    QCOMPARE(m_overlay->getGridStyle(), GridStyle::Crosses);
    QCOMPARE(m_overlay->isSnapToGrid(), false);
    QCOMPARE(m_overlay->areGuidesVisible(), false);
    QCOMPARE(m_overlay->getGuideColor(), QColor(255, 255, 0));
    QCOMPARE(m_overlay->isSnapToGuides(), false);
    QCOMPARE(m_overlay->areRulersVisible(), true);
    QCOMPARE(m_overlay->getRulerUnit(), RulerUnit::Centimeters);
    QCOMPARE(m_overlay->getRulerColor(), QColor(64, 64, 64));
    
    QList<int> hGuidesList = m_overlay->getHorizontalGuides();
    QCOMPARE(hGuidesList.size(), 2);
    QVERIFY(hGuidesList.contains(150));
    QVERIFY(hGuidesList.contains(250));
    
    QList<int> vGuidesList = m_overlay->getVerticalGuides();
    QCOMPARE(vGuidesList.size(), 2);
    QVERIFY(vGuidesList.contains(300));
    QVERIFY(vGuidesList.contains(400));
}

QTEST_MAIN(TestGridOverlay)
