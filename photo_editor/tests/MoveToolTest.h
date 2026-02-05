#ifndef MOVETOOLTEST_H
#define MOVETOOLTEST_H

#include <QObject>
#include <QTest>
#include <QSignalSpy>
#include <QPoint>
#include <QMouseEvent>
#include <QApplication>
#include "MoveTool.h"

class MoveToolTest : public QObject
{
    Q_OBJECT

public:
    explicit MoveToolTest(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Basic functionality tests
    void testToolCreation();
    void testToolProperties();
    void testMoveMode();
    void testAutoSelectLayer();
    void testShowTransformControls();
    
    // Tool operations tests
    void testBeginOperation();
    void testUpdateOperation();
    void testEndOperation();
    void testCancelOperation();
    
    // Signal tests
    void testLayerMovedSignal();
    void testSelectionMovedSignal();
    
    // Property tests
    void testPropertyGetSet();
    void testInvalidProperties();

private:
    MoveTool *m_moveTool;
    QApplication *m_app;
};

#endif // MOVETOOLTEST_H
