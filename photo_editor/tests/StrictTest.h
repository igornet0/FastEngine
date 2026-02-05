#ifndef STRICTTEST_H
#define STRICTTEST_H

#include <QObject>
#include <QTest>
#include <QSignalSpy>
#include <QWidget>
#include <QAction>
#include <QElapsedTimer>
#include "HotkeyManager.h"
#include "TooltipManager.h"
#include "MoveTool.h"
#include "EyedropperTool.h"

class StrictTest : public QObject
{
    Q_OBJECT

public:
    explicit StrictTest(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Строгие тесты функциональности
    void testHotkeyManagerRealFunctionality();
    void testTooltipManagerRealFunctionality();
    void testMoveToolRealFunctionality();
    void testEyedropperToolRealFunctionality();
    void testIntegrationRealFunctionality();
    void testErrorConditions();
    void testEdgeCases();

private:
    HotkeyManager *m_hotkeyManager;
    TooltipManager *m_tooltipManager;
    MoveTool *m_moveTool;
    EyedropperTool *m_eyedropperTool;
    QWidget *m_testWidget;
    QApplication *m_app;
};

#endif // STRICTTEST_H
