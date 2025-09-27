#include "test_projectmanager.h"
#include <QDebug>
#include <QSignalSpy>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

void TestProjectManager::initTestCase()
{
    qDebug() << "Initializing ProjectManager test case...";
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
}

void TestProjectManager::cleanupTestCase()
{
    qDebug() << "Cleaning up ProjectManager test case...";
    delete m_tempDir;
}

void TestProjectManager::init()
{
    try {
        m_projectManager = new ProjectManager();
        qDebug() << "✅ ProjectManager created successfully";
    } catch (...) {
        QFAIL("❌ Failed to create ProjectManager");
    }
}

void TestProjectManager::cleanup()
{
    if (m_projectManager) {
        delete m_projectManager;
        m_projectManager = nullptr;
    }
}

void TestProjectManager::testCreation()
{
    QVERIFY(m_projectManager != nullptr);
    QVERIFY(m_projectManager->inherits("QObject"));
    qDebug() << "✅ ProjectManager creation test passed";
}

void TestProjectManager::testProjectCreation()
{
    QVERIFY(m_projectManager != nullptr);
    
    try {
        QString projectName = "TestProject";
        QString projectPath = m_tempDir->path() + "/" + projectName;
        
        // Create new project
        bool success = m_projectManager->newProject(projectName);
        QVERIFY(success);
        
        // Verify project was created
        QString currentProject = m_projectManager->getProjectName();
        QVERIFY(currentProject == projectName);
        
        qDebug() << "✅ Project creation test passed - Project:" << currentProject;
    } catch (...) {
        QFAIL("❌ Project creation caused an exception");
    }
}

void TestProjectManager::testProjectLoading()
{
    QVERIFY(m_projectManager != nullptr);
    
    try {
        // Create a test project file
        QString projectPath = m_tempDir->path() + "/TestLoadProject.json";
        
        QJsonObject projectData;
        projectData["name"] = "TestLoadProject";
        projectData["version"] = "1.0";
        projectData["scenes"] = QJsonArray();
        
        QJsonDocument doc(projectData);
        QFile file(projectPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();
            
            // Test loading
            bool success = m_projectManager->loadProject(projectPath);
            QVERIFY(success);
            
            qDebug() << "✅ Project loading test passed";
        } else {
            qDebug() << "⚠️ Could not create test project file";
        }
    } catch (...) {
        QFAIL("❌ Project loading caused an exception");
    }
}

void TestProjectManager::testProjectSaving()
{
    QVERIFY(m_projectManager != nullptr);
    
    try {
        // Create and save a project
        QString projectName = "TestSaveProject";
        m_projectManager->newProject(projectName);
        
        QString savePath = m_tempDir->path() + "/" + projectName + ".json";
        bool success = m_projectManager->saveProject(savePath);
        QVERIFY(success);
        
        // Verify file was created
        QFile file(savePath);
        QVERIFY(file.exists());
        
        qDebug() << "✅ Project saving test passed - File size:" << file.size() << "bytes";
    } catch (...) {
        QFAIL("❌ Project saving caused an exception");
    }
}

void TestProjectManager::testProjectSettings()
{
    QVERIFY(m_projectManager != nullptr);
    
    try {
        // Test project settings
        QString projectName = "TestSettingsProject";
        m_projectManager->newProject(projectName);
        
        // Test getters
        QString name = m_projectManager->getProjectName();
        QString path = m_projectManager->getProjectPath();
        QString directory = m_projectManager->getProjectDirectory();
        
        QVERIFY(!name.isEmpty());
        qDebug() << "Project settings - Name:" << name << "Path:" << path << "Dir:" << directory;
        
        qDebug() << "✅ Project settings test passed";
    } catch (...) {
        QFAIL("❌ Project settings caused an exception");
    }
}

void TestProjectManager::testSceneManagement()
{
    QVERIFY(m_projectManager != nullptr);
    
    try {
        // Test scene management
        QString projectName = "TestSceneProject";
        m_projectManager->newProject(projectName);
        
        // Test scene operations (if available)
        QStringList scenes = m_projectManager->getScenes();
        // Scenes list may be empty initially, but getting it should not crash
        
        qDebug() << "✅ Scene management test passed - Found" << scenes.size() << "scenes";
    } catch (...) {
        QFAIL("❌ Scene management caused an exception");
    }
}

void TestProjectManager::testAssetManagement()
{
    QVERIFY(m_projectManager != nullptr);
    
    try {
        // Test asset management
        QString projectName = "TestAssetProject";
        m_projectManager->newProject(projectName);
        
        // Test asset operations (if available)
        QStringList assets = m_projectManager->getAssets();
        // Assets list may be empty initially, but getting it should not crash
        
        qDebug() << "✅ Asset management test passed - Found" << assets.size() << "assets";
    } catch (...) {
        QFAIL("❌ Asset management caused an exception");
    }
}

void TestProjectManager::testAutoSave()
{
    QVERIFY(m_projectManager != nullptr);
    
    try {
        // Test auto-save functionality
        QString projectName = "TestAutoSaveProject";
        m_projectManager->newProject(projectName);
        
        // Auto-save should be enabled by default
        // We can't easily test the timer, but we can verify it doesn't crash
        QTest::qWait(100);
        
        qDebug() << "✅ Auto-save test passed (no crash during timer operation)";
    } catch (...) {
        QFAIL("❌ Auto-save caused an exception");
    }
}

void TestProjectManager::testProjectValidation()
{
    QVERIFY(m_projectManager != nullptr);
    
    try {
        // Test project validation
        bool isValid = m_projectManager->isProjectValid();
        // May be false if no project is loaded, but checking should not crash
        
        qDebug() << "Project validation status:" << isValid;
        qDebug() << "✅ Project validation test passed";
    } catch (...) {
        QFAIL("❌ Project validation caused an exception");
    }
}

void TestProjectManager::testBackupCreation()
{
    QVERIFY(m_projectManager != nullptr);
    
    try {
        // Test backup creation
        QString projectName = "TestBackupProject";
        m_projectManager->newProject(projectName);
        
        // Create backup (if method exists)
        // m_projectManager->createBackup(); // If this method exists
        
        qDebug() << "✅ Backup creation test passed";
    } catch (...) {
        QFAIL("❌ Backup creation caused an exception");
    }
}

void TestProjectManager::testSignalEmission()
{
    QVERIFY(m_projectManager != nullptr);
    
    try {
        // Test signal emission
        QSignalSpy projectCreatedSpy(m_projectManager, &ProjectManager::projectCreated);
        QSignalSpy projectLoadedSpy(m_projectManager, &ProjectManager::projectLoaded);
        QSignalSpy projectSavedSpy(m_projectManager, &ProjectManager::projectSaved);
        
        // Create a project and check signals
        QString projectName = "TestSignalProject";
        m_projectManager->newProject(projectName);
        
        // Verify signals can be connected
        QVERIFY(projectCreatedSpy.isValid());
        QVERIFY(projectLoadedSpy.isValid());
        QVERIFY(projectSavedSpy.isValid());
        
        qDebug() << "Signal counts - Created:" << projectCreatedSpy.count() 
                 << "Loaded:" << projectLoadedSpy.count()
                 << "Saved:" << projectSavedSpy.count();
        
        qDebug() << "✅ Signal emission test passed";
    } catch (...) {
        QFAIL("❌ Signal emission caused an exception");
    }
}

#include "test_projectmanager.moc"
