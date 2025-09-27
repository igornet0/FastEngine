#ifndef TEST_MAINWINDOW_H
#define TEST_MAINWINDOW_H

#include <QObject>
#include <QTest>
#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QTimer>

// Include the class we're testing
#include "MainWindow.h"

class TestMainWindow : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Test cases
    void testWindowCreation();
    void testWindowTitle();
    void testMenuBarCreation();
    void testStatusBarCreation();
    void testDockWidgetsCreation();
    void testCentralWidget();
    void testWindowResize();
    void testMenuActions();
    void testToolBarCreation();
    void testPanelVisibility();
    void testProjectActions();
    void testSceneSwitching();
    
private:
    MainWindow *m_mainWindow;
};

#endif // TEST_MAINWINDOW_H
