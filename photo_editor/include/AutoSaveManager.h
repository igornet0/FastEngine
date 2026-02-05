#ifndef AUTOSAVEMANAGER_H
#define AUTOSAVEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <QDir>
#include <QJsonObject>
#include <QDateTime>

class ImageDocument;
class DocumentManager;

class AutoSaveManager : public QObject
{
    Q_OBJECT

public:
    explicit AutoSaveManager(QObject *parent = nullptr);
    ~AutoSaveManager();

    // Configuration
    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }

    void setInterval(int minutes);
    int getInterval() const { return m_intervalMinutes; }

    void setMaxBackups(int count);
    int getMaxBackups() const { return m_maxBackups; }

    void setAutoSaveDirectory(const QString &directory);
    QString getAutoSaveDirectory() const { return m_autoSaveDirectory; }

    // Document management
    void setDocumentManager(DocumentManager *manager);
    void registerDocument(int documentId, ImageDocument *document);
    void unregisterDocument(int documentId);

    // Manual operations
    void saveAllDocuments();
    bool saveDocument(int documentId);
    bool restoreDocument(int documentId, const QString &backupFile);
    void cleanupOldBackups();
    
    // Monitoring control
    void startMonitoring(int intervalMs = 60000);
    void stopMonitoring();
    bool isMonitoring() const;

    // Session management
    void saveSession();
    void restoreSession();
    void clearSession();

    // Backup management
    QStringList getBackupFiles(int documentId) const;
    QString getLatestBackup(int documentId) const;
    bool hasBackups(int documentId) const;

    // Serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

signals:
    void autoSaveStarted(int documentId);
    void autoSaveCompleted(int documentId, const QString &backupFile);
    void autoSaveFailed(int documentId, const QString &error);
    void sessionRestored();
    void monitoringStarted();
    void monitoringStopped();
    void sessionCleared();

private slots:
    void onAutoSaveTimer();
    void onDocumentModified(int documentId, bool modified);

private:
    void setupAutoSaveDirectory();
    QString generateBackupFileName(int documentId) const;
    QString getBackupFilePath(int documentId, const QString &fileName) const;
    void updateTimer();

    bool m_enabled;
    int m_intervalMinutes;
    int m_maxBackups;
    QString m_autoSaveDirectory;
    
    QTimer *m_autoSaveTimer;
    DocumentManager *m_documentManager;
    
    struct DocumentInfo {
        int documentId;
        ImageDocument *document;
        QDateTime lastSaveTime;
        QString originalFileName;
        bool isModified;
    };
    
    QHash<int, DocumentInfo> m_documents;
    QHash<int, QStringList> m_backupFiles;
};

#endif // AUTOSAVEMANAGER_H
