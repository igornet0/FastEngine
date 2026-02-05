#ifndef LOGPANEL_H
#define LOGPANEL_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include "Logger.h"

class LogPanel : public QWidget
{
public:
    explicit LogPanel(QWidget *parent = nullptr);
    ~LogPanel();

    void addLogMessage(Logger::LogLevel level, const QString &category, 
                      const QString &message, const QString &timestamp);

private slots:
    void clearLogs();
    void toggleAutoScroll(bool enabled);
    void setLogLevel(int level);
    void toggleFileLogging(bool enabled);
    void toggleConsoleLogging(bool enabled);

private:
    void setupUI();
    void setupConnections();
    QString formatLogMessage(Logger::LogLevel level, const QString &category, 
                           const QString &message, const QString &timestamp);
    QString getLevelColor(Logger::LogLevel level);
    
    QTextEdit *m_logTextEdit;
    QPushButton *m_clearButton;
    QComboBox *m_levelComboBox;
    QCheckBox *m_autoScrollCheckBox;
    QCheckBox *m_fileLoggingCheckBox;
    QCheckBox *m_consoleLoggingCheckBox;
    QLabel *m_statusLabel;
    
    bool m_autoScroll;
    int m_maxLines;
};

#endif // LOGPANEL_H
