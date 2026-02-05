#ifndef TEST_PROJECTMANAGER_H
#define TEST_PROJECTMANAGER_H

#include <QObject>
#include <QTest>
#include <QTemporaryDir>

// Include the class we're testing
#include "ProjectManager.h"

class TestProjectManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Test cases
    void testCreation();
    void testProjectCreation();
    void testProjectLoading();
    void testProjectSaving();
    void testProjectSettings();
    void testSceneManagement();
    void testAssetManagement();
    void testAutoSave();
    void testProjectValidation();
    void testBackupCreation();
    void testSignalEmission();
    
private:
    ProjectManager *m_projectManager;
    QTemporaryDir *m_tempDir;
};

#endif // TEST_PROJECTMANAGER_H


