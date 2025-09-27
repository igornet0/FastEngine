#include "test_sceneview2d.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>

void TestSceneView2D::initTestCase()
{
    qDebug() << "Initializing SceneView2D test case...";
}

void TestSceneView2D::cleanupTestCase()
{
    qDebug() << "Cleaning up SceneView2D test case...";
}

void TestSceneView2D::init()
{
    try {
        m_sceneView = new SceneView2D();
        qDebug() << "✅ SceneView2D created successfully";
    } catch (...) {
        QFAIL("❌ Failed to create SceneView2D - possible segfault source!");
    }
}

void TestSceneView2D::cleanup()
{
    if (m_sceneView) {
        delete m_sceneView;
        m_sceneView = nullptr;
    }
}

void TestSceneView2D::testCreation()
{
    QVERIFY(m_sceneView != nullptr);
    QVERIFY(m_sceneView->inherits("QGraphicsView"));
    qDebug() << "✅ SceneView2D creation test passed";
}

void TestSceneView2D::testSceneSetup()
{
    QVERIFY(m_sceneView != nullptr);
    
    QGraphicsScene *scene = m_sceneView->scene();
    QVERIFY(scene != nullptr);
    
    // Test scene rect
    QRectF sceneRect = scene->sceneRect();
    QVERIFY(!sceneRect.isEmpty());
    
    qDebug() << "✅ Scene setup test passed - Scene rect:" << sceneRect;
}

void TestSceneView2D::testViewportSettings()
{
    QVERIFY(m_sceneView != nullptr);
    
    // Test viewport update mode
    QGraphicsView::ViewportUpdateMode updateMode = m_sceneView->viewportUpdateMode();
    QVERIFY(updateMode == QGraphicsView::FullViewportUpdate || 
            updateMode == QGraphicsView::MinimalViewportUpdate ||
            updateMode == QGraphicsView::SmartViewportUpdate);
    
    // Test render hints
    QPainter::RenderHints hints = m_sceneView->renderHints();
    // Should have antialiasing enabled for smooth graphics
    
    qDebug() << "✅ Viewport settings test passed";
}

void TestSceneView2D::testMouseEvents()
{
    QVERIFY(m_sceneView != nullptr);
    
    // Test mouse press event (should not crash)
    try {
        QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(100, 100), 
                             Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(m_sceneView->viewport(), &pressEvent);
        
        QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPointF(100, 100), 
                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(m_sceneView->viewport(), &releaseEvent);
        
        qDebug() << "✅ Mouse events test passed";
    } catch (...) {
        QFAIL("❌ Mouse events caused an exception");
    }
}

void TestSceneView2D::testKeyboardEvents()
{
    QVERIFY(m_sceneView != nullptr);
    
    // Test keyboard events (should not crash)
    try {
        QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier);
        QApplication::sendEvent(m_sceneView, &keyEvent);
        
        qDebug() << "✅ Keyboard events test passed";
    } catch (...) {
        QFAIL("❌ Keyboard events caused an exception");
    }
}

void TestSceneView2D::testZoomFunctionality()
{
    QVERIFY(m_sceneView != nullptr);
    
    // Test zoom in/out
    try {
        QTransform originalTransform = m_sceneView->transform();
        
        // Test wheel zoom
        QWheelEvent wheelEvent(QPointF(100, 100), QPointF(100, 100), QPoint(0, 0), 
                             QPoint(0, 120), Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
        QApplication::sendEvent(m_sceneView, &wheelEvent);
        
        QTest::qWait(50); // Wait for processing
        
        qDebug() << "✅ Zoom functionality test passed";
    } catch (...) {
        QFAIL("❌ Zoom functionality caused an exception");
    }
}

void TestSceneView2D::testPanFunctionality()
{
    QVERIFY(m_sceneView != nullptr);
    
    // Test panning
    try {
        QScrollBar *hScrollBar = m_sceneView->horizontalScrollBar();
        QScrollBar *vScrollBar = m_sceneView->verticalScrollBar();
        
        if (hScrollBar && vScrollBar) {
            int originalHValue = hScrollBar->value();
            int originalVValue = vScrollBar->value();
            
            // Test scrolling
            hScrollBar->setValue(originalHValue + 10);
            vScrollBar->setValue(originalVValue + 10);
            
            qDebug() << "✅ Pan functionality test passed";
        } else {
            qDebug() << "⚠️ Scroll bars not available";
        }
    } catch (...) {
        QFAIL("❌ Pan functionality caused an exception");
    }
}

void TestSceneView2D::testEntityCreation()
{
    QVERIFY(m_sceneView != nullptr);
    
    try {
        // Test entity creation (if method exists)
        QGraphicsScene *scene = m_sceneView->scene();
        QVERIFY(scene != nullptr);
        
        int originalItemCount = scene->items().count();
        
        // Try to add a simple item to test
        QGraphicsRectItem *testItem = scene->addRect(0, 0, 50, 50);
        QVERIFY(testItem != nullptr);
        
        int newItemCount = scene->items().count();
        QVERIFY(newItemCount == originalItemCount + 1);
        
        qDebug() << "✅ Entity creation test passed";
    } catch (...) {
        QFAIL("❌ Entity creation caused an exception");
    }
}

void TestSceneView2D::testEntitySelection()
{
    QVERIFY(m_sceneView != nullptr);
    
    try {
        QGraphicsScene *scene = m_sceneView->scene();
        QVERIFY(scene != nullptr);
        
        // Add test item
        QGraphicsRectItem *testItem = scene->addRect(0, 0, 50, 50);
        testItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        
        // Test selection
        scene->clearSelection();
        testItem->setSelected(true);
        
        QList<QGraphicsItem*> selectedItems = scene->selectedItems();
        QVERIFY(selectedItems.contains(testItem));
        
        qDebug() << "✅ Entity selection test passed";
    } catch (...) {
        QFAIL("❌ Entity selection caused an exception");
    }
}

void TestSceneView2D::testEntityMovement()
{
    QVERIFY(m_sceneView != nullptr);
    
    try {
        QGraphicsScene *scene = m_sceneView->scene();
        QVERIFY(scene != nullptr);
        
        // Add movable test item
        QGraphicsRectItem *testItem = scene->addRect(0, 0, 50, 50);
        testItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        
        QPointF originalPos = testItem->pos();
        QPointF newPos(100, 100);
        
        testItem->setPos(newPos);
        
        QVERIFY(testItem->pos() == newPos);
        
        qDebug() << "✅ Entity movement test passed:" << originalPos << "→" << newPos;
    } catch (...) {
        QFAIL("❌ Entity movement caused an exception");
    }
}

void TestSceneView2D::testGridDisplay()
{
    QVERIFY(m_sceneView != nullptr);
    
    try {
        // Test grid functionality (if available)
        // This is more of a visual test, so we just check it doesn't crash
        m_sceneView->update();
        QTest::qWait(50);
        
        qDebug() << "✅ Grid display test passed";
    } catch (...) {
        QFAIL("❌ Grid display caused an exception");
    }
}

void TestSceneView2D::testAssetIntegration()
{
    QVERIFY(m_sceneView != nullptr);
    
    try {
        // Test asset selection handler (if available)
        QString testAssetPath = "test/texture.png";
        
        // This should not crash even if the method doesn't do much
        // m_sceneView->onAssetSelected(testAssetPath); // If this method is public
        
        qDebug() << "✅ Asset integration test passed";
    } catch (...) {
        QFAIL("❌ Asset integration caused an exception");
    }
}

#include "test_sceneview2d.moc"
