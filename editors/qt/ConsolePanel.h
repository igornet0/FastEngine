#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QScrollBar>
#include <QFrame>
#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include <QColor>
#include <QPalette>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <QObject>
#include <QSignalMapper>
#include <QTimer>
#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextList>
#include <QTextListFormat>
#include <QTextTable>
#include <QTextTableFormat>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QTextObject>
#include <QTextObjectInterface>
#include <QTextLayout>
#include <QTextOption>
#include <QTextLine>
#include <QTextFragment>
#include <QTextDocumentFragment>
#include <QTextDocumentWriter>
// #include <QTextDocumentReader> // Не существует в Qt6
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

enum class MessageType {
    Info,
    Warning,
    Error,
    Debug,
    Success
};

enum class LogLevel {
    Info,
    Warning,
    Error,
    Debug,
    Success
};

class ConsolePanel : public QWidget
{
    Q_OBJECT

public:
    explicit ConsolePanel(QWidget *parent = nullptr);
    ~ConsolePanel();

    // Управление сообщениями
    void addMessage(const QString &message, MessageType type = MessageType::Info);
    void clearMessages();
    void saveMessages(const QString &filePath);
    void loadMessages(const QString &filePath);
    
    // Фильтрация
    void setMessageFilter(MessageType type, bool enabled);
    void setMessageFilter(const QString &text, bool enabled);
    void clearFilters();
    
    // Получение информации
    int getMessageCount() const;
    QStringList getMessages() const;
    QStringList getMessages(MessageType type) const;

signals:
    void messageAdded(const QString &message, MessageType type);
    void messagesCleared();
    void filterChanged();

private slots:
    void onClearButtonClicked();
    void onSaveButtonClicked();
    void onLoadButtonClicked();
    void onFilterChanged();
    void onSearchChanged();
    void onAutoScrollToggled(bool enabled);
    void onTimestampToggled(bool enabled);
    void onLogMessage(const QString &message, LogLevel level);
    void onClearClicked();
    void onSaveClicked();

private:
    void setupUI();
    void setupConnections();
    void updateDisplay();
    void scrollToBottom();
    
    // Форматирование сообщений
    QString formatMessage(const QString &message, MessageType type) const;
    QColor getMessageColor(MessageType type) const;
    QIcon getMessageIcon(MessageType type) const;
    QTextCharFormat getMessageFormat(MessageType type) const;
    bool matchesFilter(const QString &message) const;
    void setupFormats();
    QIcon createIcon(const QColor &color) const;
    
    // UI компоненты
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_toolbarLayout;
    QTextEdit *m_textEdit;
    
    // Панель инструментов
    QPushButton *m_clearButton;
    QPushButton *m_saveButton;
    QPushButton *m_loadButton;
    QLineEdit *m_searchEdit;
    QComboBox *m_filterCombo;
    QCheckBox *m_autoScrollCheckBox;
    QCheckBox *m_timestampCheckBox;
    
    // Состояние
    QStringList m_messages;
    QMap<MessageType, bool> m_typeFilters;
    QMap<QString, bool> m_textFilters;
    bool m_autoScroll;
    bool m_showTimestamp;
    
    // Форматирование
    QTextCharFormat m_infoFormat;
    QTextCharFormat m_warningFormat;
    QTextCharFormat m_errorFormat;
    QTextCharFormat m_debugFormat;
    QTextCharFormat m_successFormat;
    
    // Иконки
    QIcon m_infoIcon;
    QIcon m_warningIcon;
    QIcon m_errorIcon;
    QIcon m_debugIcon;
    QIcon m_successIcon;
    
    // Таймер обновления
    QTimer *m_updateTimer;
};
