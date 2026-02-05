#ifndef INTEGRATIONTEST_H
#define INTEGRATIONTEST_H

#include <QObject>
#include <QTest>
#include <QSignalSpy>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include "HotkeyManager.h"
#include "TooltipManager.h"
#include "MoveTool.h"
#include "EyedropperTool.h"

class IntegrationTest : public QObject
{
    Q_OBJECT

public:
    explicit IntegrationTest(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Integration tests
    void testHotkeyManagerWithTooltipManager();
    void testToolIntegration();
    void testSignalIntegration();
    void testPropertyIntegration();
    
    // System tests
    void testCompleteWorkflow();
    void testErrorHandling();
    void testPerformance();

private:
    HotkeyManager *m_hotkeyManager;
    TooltipManager *m_tooltipManager;
    MoveTool *m_moveTool;
    EyedropperTool *m_eyedropperTool;
    QApplication *m_app;
    QWidget *m_testWidget;
};

#endif // INTEGRATIONTEST_H
