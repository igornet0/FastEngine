#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QMap>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QProgressDialog>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QDateTime>
#include <QTime>
#include <QDate>
#include <QLocale>
// #include <QTextCodec> // Не существует в Qt6
// #include <QTextEncoder> // Не существует в Qt6
// #include <QTextDecoder> // Не существует в Qt6
#include <QTextStream>
#include <QIODevice>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QTimer>
#include <QDateTime>
#include <QTime>
#include <QDate>
#include <QLocale>
#include <QTextBoundaryFinder>

class ProjectManager : public QObject
{
    Q_OBJECT

public:
    explicit ProjectManager(QObject *parent = nullptr);
    ~ProjectManager();
    
    // Управление проектами
    void newProject(const QString &name);
    bool loadProject(const QString &filePath);
    bool saveProject(const QString &filePath);
    void closeProject();
    
    // Получение информации
    QString getProjectName() const;
    QString getProjectPath() const;
    QString getProjectDirectory() const;
    bool isProjectModified() const;
    bool hasProject() const;
    
    // Управление сценами
    void addScene(const QString &name);
    void removeScene(const QString &name);
    void renameScene(const QString &oldName, const QString &newName);
    QStringList getScenes() const;
    QString getCurrentScene() const;
    void setCurrentScene(const QString &name);
    
    // Управление настройками
    void setSetting(const QString &key, const QVariant &value);
    QVariant getSetting(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void removeSetting(const QString &key);
    void clearSettings();
    
    // Управление историей
    void addToHistory(const QString &action);
    void undo();
    void redo();
    bool canUndo() const;
    bool canRedo() const;
    QStringList getHistory() const;
    void clearHistory();

signals:
    void projectChanged();
    void projectLoaded(const QString &name);
    void projectSaved(const QString &name);
    void projectClosed();
    void sceneChanged(const QString &name);
    void settingChanged(const QString &key, const QVariant &value);
    void historyChanged();

private:
    void setupConnections();
    void loadProjectSettings();
    void saveProjectSettings();
    void createProjectStructure();
    
    // Состояние
    QString m_projectName;
    QString m_projectPath;
    QString m_projectDirectory;
    bool m_projectModified;
    bool m_hasProject;
    
    // Данные проекта
    QStringList m_scenes;
    QString m_currentScene;
    QMap<QString, QVariant> m_settings;
    
    // История
    QStringList m_history;
    int m_historyIndex;
    int m_maxHistorySize;
    
    // Файловая система
    QFileSystemWatcher *m_fileSystemWatcher;
    
    // Таймер автосохранения
    QTimer *m_autoSaveTimer;
};
