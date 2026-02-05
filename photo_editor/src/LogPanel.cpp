#include "LogPanel.h"
#include "Logger.h"
#include <QScrollBar>
#include <QTextCharFormat>
#include <QColor>

LogPanel::LogPanel(QWidget *parent)
    : QWidget(parent)
    , m_logTextEdit(nullptr)
    , m_clearButton(nullptr)
    , m_levelComboBox(nullptr)
    , m_autoScrollCheckBox(nullptr)
    , m_fileLoggingCheckBox(nullptr)
    , m_consoleLoggingCheckBox(nullptr)
    , m_statusLabel(nullptr)
    , m_autoScroll(true)
    , m_maxLines(1000)
{
    setupUI();
    setupConnections();
    
    // Add initial log message
    addLogMessage(Logger::Info, "LOG_PANEL", "Log panel initialized", 
                  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
}

LogPanel::~LogPanel()
{
}

void LogPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    // Control panel
    QHBoxLayout *controlLayout = new QHBoxLayout();
    
    m_clearButton = new QPushButton("Clear");
    m_clearButton->setMaximumWidth(80);
    
    m_levelComboBox = new QComboBox();
    m_levelComboBox->addItem("Debug", Logger::Debug);
    m_levelComboBox->addItem("Info", Logger::Info);
    m_levelComboBox->addItem("Warning", Logger::Warning);
    m_levelComboBox->addItem("Error", Logger::Error);
    m_levelComboBox->setCurrentIndex(1); // Default to Info
    m_levelComboBox->setMaximumWidth(100);
    
    m_autoScrollCheckBox = new QCheckBox("Auto Scroll");
    m_autoScrollCheckBox->setChecked(true);
    
    m_fileLoggingCheckBox = new QCheckBox("File Logging");
    m_fileLoggingCheckBox->setChecked(false);
    
    m_consoleLoggingCheckBox = new QCheckBox("Console Logging");
    m_consoleLoggingCheckBox->setChecked(true);
    
    m_statusLabel = new QLabel("Ready");
    m_statusLabel->setStyleSheet("color: #888888; font-size: 10px;");
    
    controlLayout->addWidget(m_clearButton);
    controlLayout->addWidget(new QLabel("Level:"));
    controlLayout->addWidget(m_levelComboBox);
    controlLayout->addWidget(m_autoScrollCheckBox);
    controlLayout->addWidget(m_fileLoggingCheckBox);
    controlLayout->addWidget(m_consoleLoggingCheckBox);
    controlLayout->addStretch();
    controlLayout->addWidget(m_statusLabel);
    
    // Log text area
    m_logTextEdit = new QTextEdit();
    m_logTextEdit->setReadOnly(true);
    m_logTextEdit->setFont(QFont("Consolas", 9));
    m_logTextEdit->setStyleSheet(
        "QTextEdit {"
        "    background-color: #1e1e1e;"
        "    color: #ffffff;"
        "    border: 1px solid #555555;"
        "    border-radius: 3px;"
        "}"
    );
    
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(m_logTextEdit);
    
    // Set initial logger settings
    Logger::instance()->setLogLevel(Logger::Info);
    Logger::instance()->setLogToConsole(true);
    Logger::instance()->setLogToFile(false);
}

void LogPanel::setupConnections()
{
    connect(m_clearButton, &QPushButton::clicked, this, &LogPanel::clearLogs);
    connect(m_autoScrollCheckBox, &QCheckBox::toggled, this, &LogPanel::toggleAutoScroll);
    connect(m_levelComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &LogPanel::setLogLevel);
    connect(m_fileLoggingCheckBox, &QCheckBox::toggled, this, &LogPanel::toggleFileLogging);
    connect(m_consoleLoggingCheckBox, &QCheckBox::toggled, this, &LogPanel::toggleConsoleLogging);
}

void LogPanel::addLogMessage(Logger::LogLevel level, const QString &category, 
                            const QString &message, const QString &timestamp)
{
    QString formattedMessage = formatLogMessage(level, category, message, timestamp);
    
    // Add to text edit
    QTextCursor cursor = m_logTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    
    // Set color based on log level
    QTextCharFormat format;
    format.setForeground(QColor(getLevelColor(level)));
    cursor.setCharFormat(format);
    
    cursor.insertText(formattedMessage + "\n");
    
    // Limit number of lines
    QString text = m_logTextEdit->toPlainText();
    QStringList lines = text.split('\n');
    if (lines.size() > m_maxLines) {
        lines = lines.mid(lines.size() - m_maxLines);
        m_logTextEdit->setPlainText(lines.join('\n'));
    }
    
    // Auto scroll to bottom
    if (m_autoScroll) {
        QScrollBar *scrollBar = m_logTextEdit->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
    
    // Update status
    m_statusLabel->setText(QString("Logs: %1").arg(lines.size()));
}

void LogPanel::clearLogs()
{
    m_logTextEdit->clear();
    m_statusLabel->setText("Cleared");
    LOG_INFO("LOG_PANEL", "Logs cleared by user");
}

void LogPanel::toggleAutoScroll(bool enabled)
{
    m_autoScroll = enabled;
    LOG_DEBUG("LOG_PANEL", QString("Auto scroll %1").arg(enabled ? "enabled" : "disabled"));
}

void LogPanel::setLogLevel(int level)
{
    Logger::LogLevel logLevel = static_cast<Logger::LogLevel>(m_levelComboBox->itemData(level).toInt());
    Logger::instance()->setLogLevel(logLevel);
    LOG_INFO("LOG_PANEL", QString("Log level set to %1").arg(m_levelComboBox->currentText()));
}

void LogPanel::toggleFileLogging(bool enabled)
{
    Logger::instance()->setLogToFile(enabled);
    LOG_INFO("LOG_PANEL", QString("File logging %1").arg(enabled ? "enabled" : "disabled"));
}

void LogPanel::toggleConsoleLogging(bool enabled)
{
    Logger::instance()->setLogToConsole(enabled);
    LOG_INFO("LOG_PANEL", QString("Console logging %1").arg(enabled ? "enabled" : "disabled"));
}

QString LogPanel::formatLogMessage(Logger::LogLevel level, const QString &category, 
                                  const QString &message, const QString &timestamp)
{
    QString levelStr;
    switch (level) {
        case Logger::Debug: levelStr = "DEBUG"; break;
        case Logger::Info: levelStr = "INFO "; break;
        case Logger::Warning: levelStr = "WARN "; break;
        case Logger::Error: levelStr = "ERROR"; break;
    }
    
    return QString("[%1] [%2] [%3] %4")
           .arg(timestamp)
           .arg(levelStr)
           .arg(category)
           .arg(message);
}

QString LogPanel::getLevelColor(Logger::LogLevel level)
{
    switch (level) {
        case Logger::Debug: return "#888888"; // Gray
        case Logger::Info: return "#ffffff";  // White
        case Logger::Warning: return "#ffaa00"; // Orange
        case Logger::Error: return "#ff4444";   // Red
        default: return "#ffffff";
    }
}
