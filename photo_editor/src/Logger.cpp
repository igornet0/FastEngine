#include "Logger.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

Logger* Logger::m_instance = nullptr;

Logger::Logger(QObject *parent)
    : QObject(parent)
    , m_logLevel(Info)
    , m_logToFile(false)
    , m_logToConsole(true)
    , m_logFile(nullptr)
    , m_logStream(nullptr)
{
}

Logger::~Logger()
{
    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
    }
    if (m_logStream) {
        delete m_logStream;
    }
}

Logger* Logger::instance()
{
    if (!m_instance) {
        m_instance = new Logger();
    }
    return m_instance;
}

void Logger::setLogLevel(LogLevel level)
{
    m_logLevel = level;
}

void Logger::setLogToFile(bool enabled, const QString &filename)
{
    m_logToFile = enabled;
    
    if (enabled) {
        if (filename.isEmpty()) {
            QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
            QDir().mkpath(appDataPath);
            m_logFilename = appDataPath + "/photo_editor.log";
        } else {
            m_logFilename = filename;
        }
        
        m_logFile = new QFile(m_logFilename);
        if (m_logFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
            m_logStream = new QTextStream(m_logFile);
        }
    } else {
        if (m_logFile) {
            m_logFile->close();
            delete m_logFile;
            m_logFile = nullptr;
        }
        if (m_logStream) {
            delete m_logStream;
            m_logStream = nullptr;
        }
    }
}

void Logger::setLogToConsole(bool enabled)
{
    m_logToConsole = enabled;
}

void Logger::log(LogLevel level, const QString &category, const QString &message)
{
    if (level < m_logLevel) return;
    
    QString formattedMessage = formatMessage(level, category, message);
    
    if (m_logToConsole) {
        qDebug() << formattedMessage;
    }
    
    if (m_logToFile && m_logStream) {
        QMutexLocker locker(&m_mutex);
        *m_logStream << formattedMessage << Qt::endl;
        m_logStream->flush();
    }
}

void Logger::debug(const QString &category, const QString &message)
{
    log(Debug, category, message);
}

void Logger::info(const QString &category, const QString &message)
{
    log(Info, category, message);
}

void Logger::warning(const QString &category, const QString &message)
{
    log(Warning, category, message);
}

void Logger::error(const QString &category, const QString &message)
{
    log(Error, category, message);
}

void Logger::logToolUsage(const QString &toolName, const QString &action, const QString &details)
{
    QString message = QString("Tool: %1, Action: %2").arg(toolName, action);
    if (!details.isEmpty()) {
        message += QString(", Details: %1").arg(details);
    }
    log(Info, "TOOLS", message);
}

void Logger::logToolPropertyChange(const QString &toolName, const QString &property, const QString &value)
{
    QString message = QString("Tool: %1, Property: %2, Value: %3").arg(toolName, property, value);
    log(Info, "TOOLS", message);
}

void Logger::logImageOperation(const QString &operation, const QString &details)
{
    QString message = QString("Operation: %1").arg(operation);
    if (!details.isEmpty()) {
        message += QString(", Details: %1").arg(details);
    }
    log(Info, "IMAGE", message);
}

void Logger::logUIEvent(const QString &event, const QString &details)
{
    QString message = QString("Event: %1").arg(event);
    if (!details.isEmpty()) {
        message += QString(", Details: %1").arg(details);
    }
    log(Info, "UI", message);
}

QString Logger::levelToString(LogLevel level) const
{
    switch (level) {
        case Debug: return "DEBUG";
        case Info: return "INFO";
        case Warning: return "WARNING";
        case Error: return "ERROR";
        default: return "UNKNOWN";
    }
}

QString Logger::formatMessage(LogLevel level, const QString &category, const QString &message) const
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    return QString("[%1] [%2] [%3] %4").arg(timestamp, levelStr, category, message);
}