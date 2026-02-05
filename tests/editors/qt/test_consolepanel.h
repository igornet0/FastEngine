#ifndef TEST_CONSOLEPANEL_H
#define TEST_CONSOLEPANEL_H

#include <QObject>
#include <QTest>
#include <QTextEdit>

// Include the class we're testing
#include "ConsolePanel.h"

class TestConsolePanel : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Test cases
    void testCreation();
    void testMessageAddition();
    void testMessageTypes();
    void testMessageFiltering();
    void testMessageSearch();
    void testMessageClearing();
    void testMessageSaving();
    void testAutoScroll();
    void testTimestamps();
    void testLogLevels();
    void testPerformance();
    void testSignalEmission();
    
private:
    ConsolePanel *m_consolePanel;
};

#endif // TEST_CONSOLEPANEL_H


