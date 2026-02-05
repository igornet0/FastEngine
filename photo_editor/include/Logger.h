#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDebug>

class Logger : public QObject
{
public:
    enum LogLevel {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3
    };

    static Logger* instance();
    
    void setLogLevel(LogLevel level);
    void setLogToFile(bool enabled, const QString &filename = "");
    void setLogToConsole(bool enabled);
    
    void log(LogLevel level, const QString &category, const QString &message);
    void debug(const QString &category, const QString &message);
    void info(const QString &category, const QString &message);
    void warning(const QString &category, const QString &message);
    void error(const QString &category, const QString &message);
    
    // Convenience methods for tool logging
    void logToolUsage(const QString &toolName, const QString &action, const QString &details = "");
    void logToolPropertyChange(const QString &toolName, const QString &property, const QString &value);
    void logImageOperation(const QString &operation, const QString &details = "");
    void logUIEvent(const QString &event, const QString &details = "");

private:
    Logger(QObject *parent = nullptr);
    ~Logger();
    
    QString levelToString(LogLevel level) const;
    QString formatMessage(LogLevel level, const QString &category, const QString &message) const;
    
    static Logger *m_instance;
    LogLevel m_logLevel;
    bool m_logToFile;
    bool m_logToConsole;
    QString m_logFilename;
    QFile *m_logFile;
    QTextStream *m_logStream;
    QMutex m_mutex;
};

// Convenience macros
#define LOG_DEBUG(category, message) Logger::instance()->debug(category, message)
#define LOG_INFO(category, message) Logger::instance()->info(category, message)
#define LOG_WARNING(category, message) Logger::instance()->warning(category, message)
#define LOG_ERROR(category, message) Logger::instance()->error(category, message)

#define LOG_TOOL_USAGE(tool, action, details) Logger::instance()->logToolUsage(tool, action, details)
#define LOG_TOOL_PROPERTY(tool, property, value) Logger::instance()->logToolPropertyChange(tool, property, value)
#define LOG_IMAGE_OP(operation, details) Logger::instance()->logImageOperation(operation, details)
#define LOG_UI_EVENT(event, details) Logger::instance()->logUIEvent(event, details)

#endif // LOGGER_H
