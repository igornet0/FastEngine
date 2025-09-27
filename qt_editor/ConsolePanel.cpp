#include "ConsolePanel.h"
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

ConsolePanel::ConsolePanel(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_toolbarLayout(nullptr)
    , m_textEdit(nullptr)
    , m_clearButton(nullptr)
    , m_saveButton(nullptr)
    , m_loadButton(nullptr)
    , m_searchEdit(nullptr)
    , m_filterCombo(nullptr)
    , m_autoScrollCheckBox(nullptr)
    , m_timestampCheckBox(nullptr)
    , m_autoScroll(true)
    , m_showTimestamp(true)
    , m_updateTimer(nullptr)
{
    setupUI();
    setupConnections();
    
    // Таймер обновления
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &ConsolePanel::updateDisplay);
    m_updateTimer->start(100); // Обновление каждые 100мс
}

ConsolePanel::~ConsolePanel()
{
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
}

void ConsolePanel::setupUI()
{
    // Основной макет
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Панель инструментов
    m_toolbarLayout = new QHBoxLayout();
    
    m_clearButton = new QPushButton("Clear");
    m_saveButton = new QPushButton("Save");
    m_loadButton = new QPushButton("Load");
    
    m_toolbarLayout->addWidget(m_clearButton);
    m_toolbarLayout->addWidget(m_saveButton);
    m_toolbarLayout->addWidget(m_loadButton);
    m_toolbarLayout->addStretch();
    
    // Поиск и фильтры
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Search messages...");
    m_filterCombo = new QComboBox();
    m_filterCombo->addItems({"All", "Info", "Warning", "Error", "Debug", "Success"});
    m_autoScrollCheckBox = new QCheckBox("Auto Scroll");
    m_autoScrollCheckBox->setChecked(true);
    m_timestampCheckBox = new QCheckBox("Timestamp");
    m_timestampCheckBox->setChecked(true);
    
    m_toolbarLayout->addWidget(m_searchEdit);
    m_toolbarLayout->addWidget(m_filterCombo);
    m_toolbarLayout->addWidget(m_autoScrollCheckBox);
    m_toolbarLayout->addWidget(m_timestampCheckBox);
    
    m_mainLayout->addLayout(m_toolbarLayout);
    
    // Text Edit
    m_textEdit = new QTextEdit();
    m_textEdit->setReadOnly(true);
    m_textEdit->setFont(QFont("Consolas", 10));
    m_textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    m_mainLayout->addWidget(m_textEdit);
    
    // Настраиваем форматы
    setupFormats();
    
    // Добавляем тестовые сообщения
    addMessage("FastEngine Qt Editor started", MessageType::Info);
    addMessage("Loading project...", MessageType::Info);
    addMessage("Project loaded successfully", MessageType::Success);
    addMessage("Warning: Some assets are missing", MessageType::Warning);
    addMessage("Debug: Initializing renderer", MessageType::Debug);
}

void ConsolePanel::setupConnections()
{
    // Соединения кнопок
    connect(m_clearButton, &QPushButton::clicked, this, &ConsolePanel::onClearButtonClicked);
    connect(m_saveButton, &QPushButton::clicked, this, &ConsolePanel::onSaveButtonClicked);
    connect(m_loadButton, &QPushButton::clicked, this, &ConsolePanel::onLoadButtonClicked);
    
    // Соединения поиска и фильтров
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ConsolePanel::onSearchChanged);
    connect(m_filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConsolePanel::onFilterChanged);
    connect(m_autoScrollCheckBox, &QCheckBox::toggled, this, &ConsolePanel::onAutoScrollToggled);
    connect(m_timestampCheckBox, &QCheckBox::toggled, this, &ConsolePanel::onTimestampToggled);
}

void ConsolePanel::setupFormats()
{
    // Настраиваем форматы для разных типов сообщений
    m_infoFormat.setForeground(QBrush(QColor(200, 200, 200)));
    m_warningFormat.setForeground(QBrush(QColor(255, 255, 0)));
    m_errorFormat.setForeground(QBrush(QColor(255, 100, 100)));
    m_debugFormat.setForeground(QBrush(QColor(100, 255, 100)));
    m_successFormat.setForeground(QBrush(QColor(100, 255, 100)));
    
    // Создаем иконки
    m_infoIcon = createIcon(QColor(100, 150, 255));
    m_warningIcon = createIcon(QColor(255, 255, 0));
    m_errorIcon = createIcon(QColor(255, 100, 100));
    m_debugIcon = createIcon(QColor(100, 255, 100));
    m_successIcon = createIcon(QColor(100, 255, 100));
}

QIcon ConsolePanel::createIcon(const QColor &color) const
{
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(color));
    painter.setPen(QPen(Qt::black, 1));
    painter.drawEllipse(2, 2, 12, 12);
    
    return QIcon(pixmap);
}

void ConsolePanel::onLogMessage(const QString &message, LogLevel level)
{
    MessageType type = MessageType::Info;
    switch (level) {
        case LogLevel::Info: type = MessageType::Info; break;
        case LogLevel::Warning: type = MessageType::Warning; break;
        case LogLevel::Error: type = MessageType::Error; break;
        case LogLevel::Debug: type = MessageType::Debug; break;
        case LogLevel::Success: type = MessageType::Success; break;
    }
    addMessage(message, type);
}

void ConsolePanel::onClearClicked()
{
    clearMessages();
}

void ConsolePanel::onSaveClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Console Log", "", "Text Files (*.txt);;All Files (*)");
    if (!fileName.isEmpty()) {
        saveMessages(fileName);
    }
}

void ConsolePanel::addMessage(const QString &message, MessageType type)
{
    QString formattedMessage = formatMessage(message, type);
    m_messages.append(formattedMessage);
    
    // Ограничиваем количество сообщений
    if (m_messages.size() > 1000) {
        m_messages.removeFirst();
    }
    
    // Добавляем в TextEdit
    QTextCursor cursor = m_textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    
    QTextCharFormat format = getMessageFormat(type);
    cursor.setCharFormat(format);
    cursor.insertText(formattedMessage + "\n");
    
    // Автопрокрутка
    if (m_autoScroll) {
        scrollToBottom();
    }
    
    emit messageAdded(message, type);
}

void ConsolePanel::clearMessages()
{
    m_messages.clear();
    m_textEdit->clear();
    emit messagesCleared();
}

void ConsolePanel::saveMessages(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        for (const QString &message : m_messages) {
            stream << message << "\n";
        }
        file.close();
    }
}

void ConsolePanel::loadMessages(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        m_messages.clear();
        m_textEdit->clear();
        
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (!line.isEmpty()) {
                m_messages.append(line);
                m_textEdit->append(line);
            }
        }
        file.close();
    }
}

void ConsolePanel::setMessageFilter(MessageType type, bool enabled)
{
    m_typeFilters[type] = enabled;
    updateDisplay();
    emit filterChanged();
}

void ConsolePanel::setMessageFilter(const QString &text, bool enabled)
{
    m_textFilters[text] = enabled;
    updateDisplay();
    emit filterChanged();
}

void ConsolePanel::clearFilters()
{
    m_typeFilters.clear();
    m_textFilters.clear();
    updateDisplay();
    emit filterChanged();
}

int ConsolePanel::getMessageCount() const
{
    return m_messages.size();
}

QStringList ConsolePanel::getMessages() const
{
    return m_messages;
}

QStringList ConsolePanel::getMessages(MessageType type) const
{
    QStringList filteredMessages;
    for (const QString &message : m_messages) {
        // Простая проверка типа по префиксу
        if ((type == MessageType::Info && message.startsWith("[INFO]")) ||
            (type == MessageType::Warning && message.startsWith("[WARNING]")) ||
            (type == MessageType::Error && message.startsWith("[ERROR]")) ||
            (type == MessageType::Debug && message.startsWith("[DEBUG]")) ||
            (type == MessageType::Success && message.startsWith("[SUCCESS]"))) {
            filteredMessages.append(message);
        }
    }
    return filteredMessages;
}

void ConsolePanel::onClearButtonClicked()
{
    clearMessages();
}

void ConsolePanel::onSaveButtonClicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Save Console Messages", "", "Text Files (*.txt);;All Files (*)");
    if (!filePath.isEmpty()) {
        saveMessages(filePath);
    }
}

void ConsolePanel::onLoadButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Load Console Messages", "", "Text Files (*.txt);;All Files (*)");
    if (!filePath.isEmpty()) {
        loadMessages(filePath);
    }
}

void ConsolePanel::onFilterChanged()
{
    updateDisplay();
    emit filterChanged();
}

void ConsolePanel::onSearchChanged()
{
    updateDisplay();
    emit filterChanged();
}

void ConsolePanel::onAutoScrollToggled(bool enabled)
{
    m_autoScroll = enabled;
    if (enabled) {
        scrollToBottom();
    }
}

void ConsolePanel::onTimestampToggled(bool enabled)
{
    m_showTimestamp = enabled;
    updateDisplay();
}

void ConsolePanel::updateDisplay()
{
    // Очищаем TextEdit
    m_textEdit->clear();
    
    // Добавляем отфильтрованные сообщения
    for (const QString &message : m_messages) {
        if (matchesFilter(message)) {
            QTextCursor cursor = m_textEdit->textCursor();
            cursor.movePosition(QTextCursor::End);
            
            // Определяем тип сообщения по префиксу
            MessageType type = MessageType::Info;
            if (message.startsWith("[WARNING]")) type = MessageType::Warning;
            else if (message.startsWith("[ERROR]")) type = MessageType::Error;
            else if (message.startsWith("[DEBUG]")) type = MessageType::Debug;
            else if (message.startsWith("[SUCCESS]")) type = MessageType::Success;
            
            QTextCharFormat format = getMessageFormat(type);
            cursor.setCharFormat(format);
            cursor.insertText(message + "\n");
        }
    }
    
    // Автопрокрутка
    if (m_autoScroll) {
        scrollToBottom();
    }
}

void ConsolePanel::scrollToBottom()
{
    QTextCursor cursor = m_textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_textEdit->setTextCursor(cursor);
    m_textEdit->ensureCursorVisible();
}

QString ConsolePanel::formatMessage(const QString &message, MessageType type) const
{
    QString prefix;
    switch (type) {
        case MessageType::Info:
            prefix = "[INFO]";
            break;
        case MessageType::Warning:
            prefix = "[WARNING]";
            break;
        case MessageType::Error:
            prefix = "[ERROR]";
            break;
        case MessageType::Debug:
            prefix = "[DEBUG]";
            break;
        case MessageType::Success:
            prefix = "[SUCCESS]";
            break;
    }
    
    QString timestamp;
    if (m_showTimestamp) {
        timestamp = "[" + QDateTime::currentDateTime().toString("hh:mm:ss") + "] ";
    }
    
    return timestamp + prefix + " " + message;
}

QColor ConsolePanel::getMessageColor(MessageType type) const
{
    switch (type) {
        case MessageType::Info:
            return QColor(200, 200, 200);
        case MessageType::Warning:
            return QColor(255, 255, 0);
        case MessageType::Error:
            return QColor(255, 100, 100);
        case MessageType::Debug:
            return QColor(100, 255, 100);
        case MessageType::Success:
            return QColor(100, 255, 100);
    }
    return QColor(200, 200, 200);
}

QIcon ConsolePanel::getMessageIcon(MessageType type) const
{
    switch (type) {
        case MessageType::Info:
            return m_infoIcon;
        case MessageType::Warning:
            return m_warningIcon;
        case MessageType::Error:
            return m_errorIcon;
        case MessageType::Debug:
            return m_debugIcon;
        case MessageType::Success:
            return m_successIcon;
    }
    return m_infoIcon;
}

QTextCharFormat ConsolePanel::getMessageFormat(MessageType type) const
{
    QTextCharFormat format;
    format.setForeground(QBrush(getMessageColor(type)));
    return format;
}

bool ConsolePanel::matchesFilter(const QString &message) const
{
    // Проверяем фильтр по типу
    bool typeMatch = true;
    if (!m_typeFilters.isEmpty()) {
        typeMatch = false;
        for (auto it = m_typeFilters.begin(); it != m_typeFilters.end(); ++it) {
            if (it.value()) {
                QString prefix = "[";
                switch (it.key()) {
                    case MessageType::Info:
                        prefix += "INFO";
                        break;
                    case MessageType::Warning:
                        prefix += "WARNING";
                        break;
                    case MessageType::Error:
                        prefix += "ERROR";
                        break;
                    case MessageType::Debug:
                        prefix += "DEBUG";
                        break;
                    case MessageType::Success:
                        prefix += "SUCCESS";
                        break;
                }
                prefix += "]";
                
                if (message.contains(prefix)) {
                    typeMatch = true;
                    break;
                }
            }
        }
    }
    
    // Проверяем фильтр по тексту
    bool textMatch = true;
    if (!m_textFilters.isEmpty()) {
        textMatch = false;
        for (auto it = m_textFilters.begin(); it != m_textFilters.end(); ++it) {
            if (it.value() && message.contains(it.key(), Qt::CaseInsensitive)) {
                textMatch = true;
                break;
            }
        }
    }
    
    // Проверяем поиск
    bool searchMatch = true;
    if (!m_searchEdit->text().isEmpty()) {
        searchMatch = message.contains(m_searchEdit->text(), Qt::CaseInsensitive);
    }
    
    return typeMatch && textMatch && searchMatch;
}
