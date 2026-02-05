#ifndef HOTKEYMANAGERTEST_H
#define HOTKEYMANAGERTEST_H

#include <QObject>
#include <QTest>
#include <QSignalSpy>
#include <QShortcut>
#include <QKeySequence>
#include <QApplication>
#include "HotkeyManager.h"

class HotkeyManagerTest : public QObject
{
    Q_OBJECT

public:
    explicit HotkeyManagerTest(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Basic functionality tests
    void testRegisterHotkey();
    void testUnregisterHotkey();
    void testToolHotkeys();
    void testFileHotkeys();
    void testViewHotkeys();
    void testEditHotkeys();
    
    // Signal tests
    void testHotkeyTriggeredSignal();
    void testToolHotkeyTriggeredSignal();
    
    // Edge cases
    void testDuplicateHotkeys();
    void testInvalidHotkeys();
    void testNullWidget();

private:
    HotkeyManager *m_hotkeyManager;
    QApplication *m_app;
};

#endif // HOTKEYMANAGERTEST_H
