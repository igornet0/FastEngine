#ifndef TOOLTIPMANAGERTEST_H
#define TOOLTIPMANAGERTEST_H

#include <QObject>
#include <QTest>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QApplication>
#include "TooltipManager.h"

class TooltipManagerTest : public QObject
{
    Q_OBJECT

public:
    explicit TooltipManagerTest(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Basic functionality tests
    void testRegisterTooltip();
    void testRegisterTooltipWithHotkey();
    void testToolTooltips();
    void testToolHotkeys();
    
    // Tool-specific tests
    void testMoveToolTooltip();
    void testBrushToolTooltip();
    void testEyedropperToolTooltip();
    void testGradientToolTooltip();
    
    // Edge cases
    void testNullWidget();
    void testEmptyTooltip();
    void testInvalidToolType();

private:
    TooltipManager *m_tooltipManager;
    QApplication *m_app;
    QWidget *m_testWidget;
};

#endif // TOOLTIPMANAGERTEST_H
