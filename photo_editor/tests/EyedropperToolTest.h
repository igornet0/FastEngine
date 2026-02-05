#ifndef EYEDROPPERTOOLTEST_H
#define EYEDROPPERTOOLTEST_H

#include <QObject>
#include <QTest>
#include <QSignalSpy>
#include <QPoint>
#include <QColor>
#include <QImage>
#include <QApplication>
#include "EyedropperTool.h"

class EyedropperToolTest : public QObject
{
    Q_OBJECT

public:
    explicit EyedropperToolTest(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Basic functionality tests
    void testToolCreation();
    void testToolProperties();
    void testSampleSize();
    void testSampleFromAllLayers();
    void testShowColorInfo();
    
    // Tool operations tests
    void testBeginOperation();
    void testUpdateOperation();
    void testEndOperation();
    void testCancelOperation();
    
    // Signal tests
    void testColorPickedSignal();
    void testForegroundColorChangedSignal();
    void testBackgroundColorChangedSignal();
    
    // Property tests
    void testPropertyGetSet();
    void testInvalidProperties();
    
    // Color sampling tests
    void testColorSampling();

private:
    EyedropperTool *m_eyedropperTool;
    QApplication *m_app;
    QImage m_testImage;
};

#endif // EYEDROPPERTOOLTEST_H
