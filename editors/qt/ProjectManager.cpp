#include "ProjectManager.h"
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
#include <QTimer>
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

ProjectManager::ProjectManager(QObject *parent)
    : QObject(parent)
    , m_projectName("Untitled Project")
    , m_projectPath()
    , m_projectDirectory()
    , m_projectModified(false)
    , m_hasProject(false)
    , m_scenes()
    , m_currentScene()
    , m_settings()
    , m_history()
    , m_historyIndex(0)
    , m_maxHistorySize(100)
    , m_fileSystemWatcher(nullptr)
    , m_autoSaveTimer(nullptr)
{
    setupConnections();
    
    // Таймер автосохранения
    m_autoSaveTimer = new QTimer(this);
    connect(m_autoSaveTimer, &QTimer::timeout, this, [this]() { saveProject(m_projectPath); });
    m_autoSaveTimer->start(300000); // Автосохранение каждые 5 минут
}

ProjectManager::~ProjectManager()
{
    if (m_fileSystemWatcher) {
        delete m_fileSystemWatcher;
    }
    if (m_autoSaveTimer) {
        m_autoSaveTimer->stop();
    }
}

void ProjectManager::setupConnections()
{
    // Настройка файлового наблюдателя
    m_fileSystemWatcher = new QFileSystemWatcher(this);
    connect(m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &path) {
        Q_UNUSED(path)
        // TODO: Обработка изменений файлов проекта
    });
}

void ProjectManager::newProject(const QString &name)
{
    m_projectName = name;
    m_projectPath.clear();
    m_projectDirectory.clear();
    m_projectModified = false;
    m_hasProject = true;
    
    // Очищаем данные
    m_scenes.clear();
    m_currentScene.clear();
    m_settings.clear();
    m_history.clear();
    m_historyIndex = 0;
    
    // Создаем структуру проекта
    createProjectStructure();
    
    // Добавляем в историю
    addToHistory("New Project: " + name);
    
    emit projectChanged();
}

bool ProjectManager::loadProject(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON Parse Error:" << error.errorString();
        return false;
    }
    
    QJsonObject root = doc.object();
    
    // Загружаем основные данные
    m_projectName = root["name"].toString();
    m_projectPath = filePath;
    m_projectDirectory = QFileInfo(filePath).absolutePath();
    m_projectModified = false;
    m_hasProject = true;
    
    // Загружаем сцены
    m_scenes.clear();
    QJsonArray scenesArray = root["scenes"].toArray();
    for (const QJsonValue &value : scenesArray) {
        m_scenes.append(value.toString());
    }
    
    // Загружаем текущую сцену
    m_currentScene = root["currentScene"].toString();
    if (m_currentScene.isEmpty() && !m_scenes.isEmpty()) {
        m_currentScene = m_scenes.first();
    }
    
    // Загружаем настройки
    m_settings.clear();
    QJsonObject settingsObj = root["settings"].toObject();
    for (auto it = settingsObj.begin(); it != settingsObj.end(); ++it) {
        m_settings[it.key()] = it.value().toVariant();
    }
    
    // Очищаем историю
    m_history.clear();
    m_historyIndex = 0;
    
    // Добавляем в историю
    addToHistory("Load Project: " + m_projectName);
    
    // Настраиваем наблюдатель файлов
    if (m_fileSystemWatcher) {
        m_fileSystemWatcher->removePaths(m_fileSystemWatcher->files());
        m_fileSystemWatcher->addPath(filePath);
    }
    
    emit projectLoaded(m_projectName);
    emit projectChanged();
    
    return true;
}

bool ProjectManager::saveProject(const QString &filePath)
{
    QString savePath = filePath.isEmpty() ? m_projectPath : filePath;
    if (savePath.isEmpty()) {
        return false;
    }
    
    QJsonObject root;
    root["name"] = m_projectName;
    root["version"] = "1.0.0";
    root["currentScene"] = m_currentScene;
    
    // Сохраняем сцены
    QJsonArray scenesArray;
    for (const QString &scene : m_scenes) {
        scenesArray.append(scene);
    }
    root["scenes"] = scenesArray;
    
    // Сохраняем настройки
    QJsonObject settingsObj;
    for (auto it = m_settings.begin(); it != m_settings.end(); ++it) {
        settingsObj[it.key()] = QJsonValue::fromVariant(it.value());
    }
    root["settings"] = settingsObj;
    
    QJsonDocument doc(root);
    
    QFile file(savePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream stream(&file);
    stream << doc.toJson();
    file.close();
    
    m_projectPath = savePath;
    m_projectDirectory = QFileInfo(savePath).absolutePath();
    m_projectModified = false;
    
    // Добавляем в историю
    addToHistory("Save Project: " + m_projectName);
    
    emit projectSaved(m_projectName);
    emit projectChanged();
    
    return true;
}

void ProjectManager::closeProject()
{
    m_projectName = "Untitled Project";
    m_projectPath.clear();
    m_projectDirectory.clear();
    m_projectModified = false;
    m_hasProject = false;
    
    // Очищаем данные
    m_scenes.clear();
    m_currentScene.clear();
    m_settings.clear();
    m_history.clear();
    m_historyIndex = 0;
    
    // Удаляем наблюдатель файлов
    if (m_fileSystemWatcher) {
        m_fileSystemWatcher->removePaths(m_fileSystemWatcher->files());
    }
    
    emit projectClosed();
    emit projectChanged();
}

QString ProjectManager::getProjectName() const
{
    return m_projectName;
}

QString ProjectManager::getProjectPath() const
{
    return m_projectPath;
}

QString ProjectManager::getProjectDirectory() const
{
    return m_projectDirectory;
}

bool ProjectManager::isProjectModified() const
{
    return m_projectModified;
}

bool ProjectManager::hasProject() const
{
    return m_hasProject;
}

void ProjectManager::addScene(const QString &name)
{
    if (!m_scenes.contains(name)) {
        m_scenes.append(name);
        if (m_currentScene.isEmpty()) {
            m_currentScene = name;
        }
        emit sceneChanged(name);
        emit projectChanged();
    }
}

void ProjectManager::removeScene(const QString &name)
{
    if (m_scenes.removeOne(name)) {
        if (m_currentScene == name) {
            m_currentScene = m_scenes.isEmpty() ? QString() : m_scenes.first();
        }
        emit sceneChanged(name);
        emit projectChanged();
    }
}

void ProjectManager::renameScene(const QString &oldName, const QString &newName)
{
    int index = m_scenes.indexOf(oldName);
    if (index != -1) {
        m_scenes[index] = newName;
        if (m_currentScene == oldName) {
            m_currentScene = newName;
        }
        emit sceneChanged(newName);
        emit projectChanged();
    }
}

QStringList ProjectManager::getScenes() const
{
    return m_scenes;
}

QString ProjectManager::getCurrentScene() const
{
    return m_currentScene;
}

void ProjectManager::setCurrentScene(const QString &name)
{
    if (m_scenes.contains(name) && m_currentScene != name) {
        m_currentScene = name;
        emit sceneChanged(name);
        emit projectChanged();
    }
}

void ProjectManager::setSetting(const QString &key, const QVariant &value)
{
    m_settings[key] = value;
    emit settingChanged(key, value);
    emit projectChanged();
}

QVariant ProjectManager::getSetting(const QString &key, const QVariant &defaultValue) const
{
    return m_settings.value(key, defaultValue);
}

void ProjectManager::removeSetting(const QString &key)
{
    if (m_settings.remove(key)) {
        emit settingChanged(key, QVariant());
        emit projectChanged();
    }
}

void ProjectManager::clearSettings()
{
    m_settings.clear();
    emit projectChanged();
}

void ProjectManager::addToHistory(const QString &action)
{
    // Удаляем все действия после текущего индекса
    while (m_history.size() > m_historyIndex) {
        m_history.removeLast();
    }
    
    // Добавляем новое действие
    m_history.append(action);
    m_historyIndex++;
    
    // Ограничиваем размер истории
    if (m_history.size() > m_maxHistorySize) {
        m_history.removeFirst();
        m_historyIndex--;
    }
    
    emit historyChanged();
}

void ProjectManager::undo()
{
    if (canUndo()) {
        m_historyIndex--;
        QString action = m_history[m_historyIndex];
        // TODO: Реализовать отмену действия
        qDebug() << "Undo:" << action;
        emit historyChanged();
    }
}

void ProjectManager::redo()
{
    if (canRedo()) {
        QString action = m_history[m_historyIndex];
        m_historyIndex++;
        // TODO: Реализовать повтор действия
        qDebug() << "Redo:" << action;
        emit historyChanged();
    }
}

bool ProjectManager::canUndo() const
{
    return m_historyIndex > 0;
}

bool ProjectManager::canRedo() const
{
    return m_historyIndex < m_history.size();
}

QStringList ProjectManager::getHistory() const
{
    return m_history;
}

void ProjectManager::clearHistory()
{
    m_history.clear();
    m_historyIndex = 0;
    emit historyChanged();
}

void ProjectManager::loadProjectSettings()
{
    // TODO: Реализовать загрузку настроек проекта
}

void ProjectManager::saveProjectSettings()
{
    // TODO: Реализовать сохранение настроек проекта
}

void ProjectManager::createProjectStructure()
{
    if (m_projectDirectory.isEmpty()) {
        return;
    }
    
    QDir projectDir(m_projectDirectory);
    if (!projectDir.exists()) {
        projectDir.mkpath(".");
    }
    
    // Создаем структуру папок
    projectDir.mkpath("Assets");
    projectDir.mkpath("Assets/Textures");
    projectDir.mkpath("Assets/Models");
    projectDir.mkpath("Assets/Audio");
    projectDir.mkpath("Assets/Scripts");
    projectDir.mkpath("Assets/Shaders");
    projectDir.mkpath("Scenes");
    projectDir.mkpath("Build");
    projectDir.mkpath("Settings");
    
    // Создаем файл проекта
    QString projectFile = projectDir.absoluteFilePath(m_projectName + ".feproj");
    saveProject(projectFile);
    
    // Добавляем в наблюдатель файлов
    if (m_fileSystemWatcher) {
        m_fileSystemWatcher->addPath(projectFile);
    }
}
