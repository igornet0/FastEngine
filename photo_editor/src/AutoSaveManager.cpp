#include "AutoSaveManager.h"
#include "ImageDocument.h"
#include "DocumentManager.h"
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDebug>
#include <QDateTime>
#include <QApplication>

AutoSaveManager::AutoSaveManager(QObject *parent)
    : QObject(parent)
    , m_enabled(true)
    , m_intervalMinutes(5)
    , m_maxBackups(10)
    , m_autoSaveDirectory(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/PhotoEditor/AutoSave")
    , m_autoSaveTimer(new QTimer(this))
    , m_documentManager(nullptr)
{
    setupAutoSaveDirectory();
    updateTimer();
    
    connect(m_autoSaveTimer, &QTimer::timeout, this, &AutoSaveManager::onAutoSaveTimer);
}

AutoSaveManager::~AutoSaveManager()
{
    saveSession();
}

void AutoSaveManager::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        updateTimer();
    }
}

void AutoSaveManager::setInterval(int minutes)
{
    if (m_intervalMinutes != minutes && minutes > 0) {
        m_intervalMinutes = minutes;
        updateTimer();
    }
}

void AutoSaveManager::setMaxBackups(int count)
{
    if (m_maxBackups != count && count > 0) {
        m_maxBackups = count;
        cleanupOldBackups();
    }
}

void AutoSaveManager::setAutoSaveDirectory(const QString &directory)
{
    if (m_autoSaveDirectory != directory) {
        m_autoSaveDirectory = directory;
        setupAutoSaveDirectory();
    }
}

void AutoSaveManager::setDocumentManager(DocumentManager *manager)
{
    m_documentManager = manager;
}

void AutoSaveManager::registerDocument(int documentId, ImageDocument *document)
{
    DocumentInfo info;
    info.documentId = documentId;
    info.document = document;
    info.lastSaveTime = QDateTime::currentDateTime();
    info.originalFileName = QString();
    info.isModified = false;
    
    m_documents[documentId] = info;
    m_backupFiles[documentId] = QStringList();
    
    // Connect to document signals
    connect(document, &ImageDocument::modifiedChanged, [this, documentId](bool modified) {
        onDocumentModified(documentId, modified);
    });
}

void AutoSaveManager::unregisterDocument(int documentId)
{
    if (m_documents.contains(documentId)) {
        m_documents.remove(documentId);
        m_backupFiles.remove(documentId);
    }
}

void AutoSaveManager::saveAllDocuments()
{
    for (auto it = m_documents.begin(); it != m_documents.end(); ++it) {
        if (it.value().isModified) {
            saveDocument(it.key());
        }
    }
}

bool AutoSaveManager::saveDocument(int documentId)
{
    if (!m_documents.contains(documentId)) {
        return false;
    }
    
    DocumentInfo &info = m_documents[documentId];
    if (!info.isModified) {
        return true;
    }
    
    emit autoSaveStarted(documentId);
    
    QString backupFileName = generateBackupFileName(documentId);
    QString backupFilePath = getBackupFilePath(documentId, backupFileName);
    
    // Save document to backup file
    bool success = info.document->saveDocument(backupFilePath);
    
    if (success) {
        // Add to backup list
        m_backupFiles[documentId].append(backupFileName);
        
        // Clean up old backups
        while (m_backupFiles[documentId].size() > m_maxBackups) {
            QString oldBackup = m_backupFiles[documentId].takeFirst();
            QString oldBackupPath = getBackupFilePath(documentId, oldBackup);
            QFile::remove(oldBackupPath);
        }
        
        info.lastSaveTime = QDateTime::currentDateTime();
        info.isModified = false;
        
        emit autoSaveCompleted(documentId, backupFilePath);
        return true;
    } else {
        emit autoSaveFailed(documentId, "Failed to save backup file");
        return false;
    }
}

bool AutoSaveManager::restoreDocument(int documentId, const QString &backupFile)
{
    if (!m_documents.contains(documentId)) {
        return false;
    }
    
    DocumentInfo &info = m_documents[documentId];
    QString backupFilePath = getBackupFilePath(documentId, backupFile);
    
    if (QFile::exists(backupFilePath)) {
        // Load backup into document
        info.document->openDocument(backupFilePath);
        info.isModified = true;
        return true;
    }
    return false;
}

void AutoSaveManager::cleanupOldBackups()
{
    for (auto it = m_backupFiles.begin(); it != m_backupFiles.end(); ++it) {
        int documentId = it.key();
        QStringList &backups = it.value();
        
        while (backups.size() > m_maxBackups) {
            QString oldBackup = backups.takeFirst();
            QString oldBackupPath = getBackupFilePath(documentId, oldBackup);
            QFile::remove(oldBackupPath);
        }
    }
}

void AutoSaveManager::saveSession()
{
    QJsonObject session;
    session["version"] = "1.0";
    session["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    session["enabled"] = m_enabled;
    session["interval"] = m_intervalMinutes;
    session["maxBackups"] = m_maxBackups;
    session["autoSaveDirectory"] = m_autoSaveDirectory;
    
    QJsonArray documents;
    for (auto it = m_documents.begin(); it != m_documents.end(); ++it) {
        QJsonObject docInfo;
        docInfo["documentId"] = it.key();
        docInfo["originalFileName"] = it.value().originalFileName;
        docInfo["lastSaveTime"] = it.value().lastSaveTime.toString(Qt::ISODate);
        docInfo["isModified"] = it.value().isModified;
        documents.append(docInfo);
    }
    session["documents"] = documents;
    
    QJsonArray backupFiles;
    for (auto it = m_backupFiles.begin(); it != m_backupFiles.end(); ++it) {
        QJsonObject backupInfo;
        backupInfo["documentId"] = it.key();
        QJsonArray files;
        for (const QString &file : it.value()) {
            files.append(file);
        }
        backupInfo["files"] = files;
        backupFiles.append(backupInfo);
    }
    session["backupFiles"] = backupFiles;
    
    QString sessionFile = m_autoSaveDirectory + "/session.json";
    QFile file(sessionFile);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(session);
        file.write(doc.toJson());
    }
}

void AutoSaveManager::restoreSession()
{
    QString sessionFile = m_autoSaveDirectory + "/session.json";
    QFile file(sessionFile);
    
    if (!file.exists()) {
        return;
    }
    
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject session = doc.object();
        
        // Restore configuration
        m_enabled = session["enabled"].toBool();
        m_intervalMinutes = session["interval"].toInt();
        m_maxBackups = session["maxBackups"].toInt();
        m_autoSaveDirectory = session["autoSaveDirectory"].toString();
        
        // Restore documents info
        QJsonArray documents = session["documents"].toArray();
        for (const QJsonValue &value : documents) {
            QJsonObject docInfo = value.toObject();
            int documentId = docInfo["documentId"].toInt();
            QString originalFileName = docInfo["originalFileName"].toString();
            QDateTime lastSaveTime = QDateTime::fromString(docInfo["lastSaveTime"].toString(), Qt::ISODate);
            bool isModified = docInfo["isModified"].toBool();
            
            DocumentInfo info;
            info.documentId = documentId;
            info.document = nullptr; // Will be set when document is registered
            info.originalFileName = originalFileName;
            info.lastSaveTime = lastSaveTime;
            info.isModified = isModified;
            
            m_documents[documentId] = info;
        }
        
        // Restore backup files info
        QJsonArray backupFiles = session["backupFiles"].toArray();
        for (const QJsonValue &value : backupFiles) {
            QJsonObject backupInfo = value.toObject();
            int documentId = backupInfo["documentId"].toInt();
            QJsonArray files = backupInfo["files"].toArray();
            
            QStringList fileList;
            for (const QJsonValue &fileValue : files) {
                fileList.append(fileValue.toString());
            }
            m_backupFiles[documentId] = fileList;
        }
        
        updateTimer();
        emit sessionRestored();
    }
}

void AutoSaveManager::clearSession()
{
    // Clear all backup files
    QDir dir(m_autoSaveDirectory);
    if (dir.exists()) {
        dir.removeRecursively();
    }
    
    // Clear in-memory data
    m_documents.clear();
    m_backupFiles.clear();
    
    setupAutoSaveDirectory();
    emit sessionCleared();
}

QStringList AutoSaveManager::getBackupFiles(int documentId) const
{
    if (m_backupFiles.contains(documentId)) {
        return m_backupFiles[documentId];
    }
    return QStringList();
}

QString AutoSaveManager::getLatestBackup(int documentId) const
{
    QStringList backups = getBackupFiles(documentId);
    if (!backups.isEmpty()) {
        return backups.last();
    }
    return QString();
}

bool AutoSaveManager::hasBackups(int documentId) const
{
    return !getBackupFiles(documentId).isEmpty();
}

QJsonObject AutoSaveManager::toJson() const
{
    QJsonObject json;
    json["enabled"] = m_enabled;
    json["interval"] = m_intervalMinutes;
    json["maxBackups"] = m_maxBackups;
    json["autoSaveDirectory"] = m_autoSaveDirectory;
    return json;
}

void AutoSaveManager::fromJson(const QJsonObject &json)
{
    m_enabled = json["enabled"].toBool();
    m_intervalMinutes = json["interval"].toInt();
    m_maxBackups = json["maxBackups"].toInt();
    m_autoSaveDirectory = json["autoSaveDirectory"].toString();
    
    setupAutoSaveDirectory();
    updateTimer();
}

void AutoSaveManager::onAutoSaveTimer()
{
    if (m_enabled) {
        saveAllDocuments();
    }
}

void AutoSaveManager::onDocumentModified(int documentId, bool modified)
{
    if (m_documents.contains(documentId)) {
        m_documents[documentId].isModified = modified;
    }
}

void AutoSaveManager::setupAutoSaveDirectory()
{
    QDir dir;
    if (!dir.exists(m_autoSaveDirectory)) {
        dir.mkpath(m_autoSaveDirectory);
    }
}

QString AutoSaveManager::generateBackupFileName(int documentId) const
{
    QDateTime now = QDateTime::currentDateTime();
    QString timestamp = now.toString("yyyyMMdd_hhmmss");
    return QString("backup_%1_%2.qphoto").arg(documentId).arg(timestamp);
}

QString AutoSaveManager::getBackupFilePath(int documentId, const QString &fileName) const
{
    QString documentDir = m_autoSaveDirectory + QString("/document_%1").arg(documentId);
    QDir dir;
    if (!dir.exists(documentDir)) {
        dir.mkpath(documentDir);
    }
    return documentDir + "/" + fileName;
}

void AutoSaveManager::updateTimer()
{
    if (m_enabled) {
        m_autoSaveTimer->start(m_intervalMinutes * 60 * 1000); // Convert to milliseconds
    } else {
        m_autoSaveTimer->stop();
    }
}

void AutoSaveManager::startMonitoring(int intervalMs)
{
    if (m_autoSaveTimer->isActive()) {
        m_autoSaveTimer->stop();
    }
    
    m_autoSaveTimer->setInterval(intervalMs);
    m_autoSaveTimer->start();
    emit monitoringStarted();
}

void AutoSaveManager::stopMonitoring()
{
    m_autoSaveTimer->stop();
    emit monitoringStopped();
}

bool AutoSaveManager::isMonitoring() const
{
    return m_autoSaveTimer->isActive();
}
