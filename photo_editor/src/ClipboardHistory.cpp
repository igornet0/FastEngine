#include "ClipboardHistory.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QFileDialog>
#include <QMessageBox>

ClipboardHistory::ClipboardHistory(QObject *parent)
    : QObject(parent)
    , m_maxItems(50)
    , m_enabled(true)
    , m_monitoring(false)
    , m_clipboard(QApplication::clipboard())
    , m_monitoringTimer(new QTimer(this))
{
    m_monitoringTimer->setInterval(1000); // Check every second
    connect(m_monitoringTimer, &QTimer::timeout, this, &ClipboardHistory::onMonitoringTimeout);
}

ClipboardHistory::~ClipboardHistory()
{
    stopMonitoring();
}

void ClipboardHistory::addItem(const ClipboardItem &item)
{
    if (!m_enabled || isDuplicate(item)) {
        return;
    }
    
    m_items.prepend(item);
    trimHistory();
    
    emit itemAdded(0);
}

void ClipboardHistory::addImage(const QImage &image, const QString &description)
{
    if (image.isNull()) {
        return;
    }
    
    ClipboardItem item(image, description);
    addItem(item);
}

void ClipboardHistory::addText(const QString &text, const QString &description)
{
    if (text.isEmpty()) {
        return;
    }
    
    ClipboardItem item(text, description);
    addItem(item);
}

void ClipboardHistory::removeItem(int index)
{
    if (index >= 0 && index < m_items.size()) {
        m_items.removeAt(index);
        emit itemRemoved(index);
    }
}

void ClipboardHistory::clearHistory()
{
    m_items.clear();
    emit historyCleared();
}

ClipboardItem ClipboardHistory::getItem(int index) const
{
    if (index >= 0 && index < m_items.size()) {
        return m_items[index];
    }
    return ClipboardItem();
}

QImage ClipboardHistory::getImage(int index) const
{
    return getItem(index).image;
}

QString ClipboardHistory::getText(int index) const
{
    return getItem(index).text;
}

void ClipboardHistory::setMaxItems(int count)
{
    if (count > 0 && count != m_maxItems) {
        m_maxItems = count;
        trimHistory();
    }
}

void ClipboardHistory::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        if (!enabled) {
            stopMonitoring();
        }
    }
}

void ClipboardHistory::startMonitoring()
{
    if (!m_enabled || m_monitoring) {
        return;
    }
    
    m_monitoring = true;
    m_monitoringTimer->start();
    emit monitoringStarted();
}

void ClipboardHistory::stopMonitoring()
{
    if (!m_monitoring) {
        return;
    }
    
    m_monitoring = false;
    m_monitoringTimer->stop();
    emit monitoringStopped();
}

QList<int> ClipboardHistory::searchItems(const QString &query) const
{
    QList<int> results;
    
    if (query.isEmpty()) {
        for (int i = 0; i < m_items.size(); ++i) {
            results.append(i);
        }
        return results;
    }
    
    QString lowerQuery = query.toLower();
    
    for (int i = 0; i < m_items.size(); ++i) {
        const ClipboardItem &item = m_items[i];
        
        if (item.description.toLower().contains(lowerQuery) ||
            item.text.toLower().contains(lowerQuery) ||
            item.type.toLower().contains(lowerQuery)) {
            results.append(i);
        }
    }
    
    return results;
}

QList<int> ClipboardHistory::getItemsByType(const QString &type) const
{
    QList<int> results;
    
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].type == type) {
            results.append(i);
        }
    }
    
    return results;
}

QList<int> ClipboardHistory::getRecentItems(int count) const
{
    QList<int> results;
    
    int maxCount = qMin(count, m_items.size());
    for (int i = 0; i < maxCount; ++i) {
        results.append(i);
    }
    
    return results;
}

bool ClipboardHistory::exportHistory(const QString &fileName) const
{
    QJsonObject json = toJson();
    QJsonDocument doc(json);
    
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        return true;
    }
    
    return false;
}

bool ClipboardHistory::importHistory(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    fromJson(doc.object());
    
    return true;
}

QJsonObject ClipboardHistory::toJson() const
{
    QJsonObject json;
    json["maxItems"] = m_maxItems;
    json["enabled"] = m_enabled;
    json["monitoring"] = m_monitoring;
    
    QJsonArray items;
    for (const ClipboardItem &item : m_items) {
        QJsonObject itemJson;
        itemJson["description"] = item.description;
        itemJson["timestamp"] = item.timestamp.toString(Qt::ISODate);
        itemJson["type"] = item.type;
        itemJson["size"] = item.size;
        itemJson["imageSize"] = QJsonObject{
            {"width", item.imageSize.width()},
            {"height", item.imageSize.height()}
        };
        
        if (!item.text.isEmpty()) {
            itemJson["text"] = item.text;
        }
        
        if (!item.image.isNull()) {
            // Convert image to base64
            QBuffer buffer;
            buffer.open(QIODevice::WriteOnly);
            item.image.save(&buffer, "PNG");
            itemJson["imageData"] = QString::fromLatin1(buffer.data().toBase64());
        }
        
        items.append(itemJson);
    }
    
    json["items"] = items;
    return json;
}

void ClipboardHistory::fromJson(const QJsonObject &json)
{
    m_maxItems = json["maxItems"].toInt();
    m_enabled = json["enabled"].toBool();
    m_monitoring = json["monitoring"].toBool();
    
    m_items.clear();
    QJsonArray items = json["items"].toArray();
    
    for (const QJsonValue &value : items) {
        QJsonObject itemJson = value.toObject();
        ClipboardItem item;
        
        item.description = itemJson["description"].toString();
        item.timestamp = QDateTime::fromString(itemJson["timestamp"].toString(), Qt::ISODate);
        item.type = itemJson["type"].toString();
        item.size = itemJson["size"].toInt();
        
        QJsonObject sizeJson = itemJson["imageSize"].toObject();
        item.imageSize = QSize(sizeJson["width"].toInt(), sizeJson["height"].toInt());
        
        if (itemJson.contains("text")) {
            item.text = itemJson["text"].toString();
        }
        
        if (itemJson.contains("imageData")) {
            QByteArray imageData = QByteArray::fromBase64(itemJson["imageData"].toString().toLatin1());
            item.image.loadFromData(imageData);
        }
        
        m_items.append(item);
    }
    
    if (m_monitoring) {
        startMonitoring();
    }
}

void ClipboardHistory::onClipboardChanged()
{
    if (!m_enabled) {
        return;
    }
    
    const QMimeData *mimeData = m_clipboard->mimeData();
    if (!mimeData) {
        return;
    }
    
    ClipboardItem newItem;
    bool hasImage = false;
    bool hasText = false;
    
    // Check for image
    if (mimeData->hasImage()) {
        QVariant imageVariant = mimeData->imageData();
        if (imageVariant.canConvert<QImage>()) {
            newItem.image = imageVariant.value<QImage>();
            hasImage = true;
        }
    }
    
    // Check for text
    if (mimeData->hasText()) {
        newItem.text = mimeData->text();
        hasText = true;
    }
    
    if (hasImage && hasText) {
        newItem.type = "mixed";
    } else if (hasImage) {
        newItem.type = "image";
    } else if (hasText) {
        newItem.type = "text";
    } else {
        return; // No supported content
    }
    
    newItem.timestamp = QDateTime::currentDateTime();
    newItem.size = hasImage ? newItem.image.sizeInBytes() : newItem.text.size();
    newItem.imageSize = newItem.image.size();
    
    updateDescription(newItem);
    
    if (!isDuplicate(newItem)) {
        addItem(newItem);
    }
}

void ClipboardHistory::onMonitoringTimeout()
{
    onClipboardChanged();
}

void ClipboardHistory::trimHistory()
{
    while (m_items.size() > m_maxItems) {
        m_items.removeLast();
    }
}

void ClipboardHistory::updateDescription(ClipboardItem &item)
{
    if (item.description.isEmpty()) {
        item.description = generateDescription(item);
    }
}

QString ClipboardHistory::generateDescription(const ClipboardItem &item) const
{
    if (item.type == "image") {
        return QString("Image %1x%2").arg(item.imageSize.width()).arg(item.imageSize.height());
    } else if (item.type == "text") {
        QString text = item.text;
        if (text.length() > 50) {
            text = text.left(47) + "...";
        }
        return text;
    } else if (item.type == "mixed") {
        return QString("Mixed content (Image %1x%2 + Text)").arg(item.imageSize.width()).arg(item.imageSize.height());
    }
    
    return "Unknown content";
}

bool ClipboardHistory::isDuplicate(const ClipboardItem &item) const
{
    if (m_items.isEmpty()) {
        return false;
    }
    
    const ClipboardItem &lastItem = m_items.first();
    
    if (item.type != lastItem.type) {
        return false;
    }
    
    if (item.type == "image") {
        return item.image == lastItem.image;
    } else if (item.type == "text") {
        return item.text == lastItem.text;
    } else if (item.type == "mixed") {
        return item.image == lastItem.image && item.text == lastItem.text;
    }
    
    return false;
}

// ClipboardHistoryWidget implementation

ClipboardHistoryWidget::ClipboardHistoryWidget(QWidget *parent)
    : QWidget(parent)
    , m_history(nullptr)
    , m_searchQuery(QString())
    , m_filterType("all")
{
    setupUI();
    setupConnections();
}

ClipboardHistoryWidget::~ClipboardHistoryWidget()
{
}

void ClipboardHistoryWidget::setClipboardHistory(ClipboardHistory *history)
{
    if (m_history) {
        disconnect(m_history, nullptr, this, nullptr);
    }
    
    m_history = history;
    
    if (m_history) {
        connect(m_history, &ClipboardHistory::itemAdded, this, &ClipboardHistoryWidget::onItemAdded);
        connect(m_history, &ClipboardHistory::itemRemoved, this, &ClipboardHistoryWidget::onItemRemoved);
        connect(m_history, &ClipboardHistory::historyCleared, this, &ClipboardHistoryWidget::onHistoryCleared);
    }
    
    updateItemList();
}

void ClipboardHistoryWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    // Search and filter
    m_searchLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Search clipboard history...");
    m_filterCombo = new QComboBox();
    m_filterCombo->addItems({"All", "Images", "Text", "Mixed"});
    m_clearSearchButton = new QPushButton("Clear");
    
    m_searchLayout->addWidget(m_searchEdit);
    m_searchLayout->addWidget(m_filterCombo);
    m_searchLayout->addWidget(m_clearSearchButton);
    
    // Scroll area for items
    m_scrollArea = new QScrollArea();
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    m_scrollWidget = new QWidget();
    m_scrollLayout = new QVBoxLayout(m_scrollWidget);
    m_scrollLayout->setContentsMargins(0, 0, 0, 0);
    m_scrollLayout->setSpacing(2);
    
    m_scrollArea->setWidget(m_scrollWidget);
    
    // Actions
    m_actionsLayout = new QHBoxLayout();
    m_clearButton = new QPushButton("Clear All");
    m_exportButton = new QPushButton("Export");
    m_importButton = new QPushButton("Import");
    m_refreshButton = new QPushButton("Refresh");
    
    m_actionsLayout->addWidget(m_clearButton);
    m_actionsLayout->addWidget(m_exportButton);
    m_actionsLayout->addWidget(m_importButton);
    m_actionsLayout->addWidget(m_refreshButton);
    m_actionsLayout->addStretch();
    
    m_mainLayout->addLayout(m_searchLayout);
    m_mainLayout->addWidget(m_scrollArea);
    m_mainLayout->addLayout(m_actionsLayout);
}

void ClipboardHistoryWidget::setupConnections()
{
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ClipboardHistoryWidget::onSearchChanged);
    connect(m_filterCombo, &QComboBox::currentTextChanged, this, &ClipboardHistoryWidget::onFilterChanged);
    connect(m_clearSearchButton, &QPushButton::clicked, [this]() {
        m_searchEdit->clear();
        m_filterCombo->setCurrentText("All");
    });
    
    connect(m_clearButton, &QPushButton::clicked, this, &ClipboardHistoryWidget::onClearHistory);
    connect(m_exportButton, &QPushButton::clicked, this, &ClipboardHistoryWidget::onExportHistory);
    connect(m_importButton, &QPushButton::clicked, this, &ClipboardHistoryWidget::onImportHistory);
    connect(m_refreshButton, &QPushButton::clicked, [this]() { updateItemList(); });
}

void ClipboardHistoryWidget::onItemAdded(int index)
{
    Q_UNUSED(index)
    updateItemList();
}

void ClipboardHistoryWidget::onItemRemoved(int index)
{
    Q_UNUSED(index)
    updateItemList();
}

void ClipboardHistoryWidget::onHistoryCleared()
{
    updateItemList();
}

void ClipboardHistoryWidget::onItemClicked()
{
    ClipboardItemWidget *widget = qobject_cast<ClipboardItemWidget*>(sender());
    if (widget) {
        emit itemSelected(widget->getItem());
    }
}

void ClipboardHistoryWidget::onItemDoubleClicked()
{
    ClipboardItemWidget *widget = qobject_cast<ClipboardItemWidget*>(sender());
    if (widget) {
        emit itemDoubleClicked(widget->getItem());
    }
}

void ClipboardHistoryWidget::onClearHistory()
{
    if (m_history) {
        m_history->clearHistory();
    }
}

void ClipboardHistoryWidget::onExportHistory()
{
    if (!m_history) {
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Clipboard History",
        QString(),
        "JSON Files (*.json);;All Files (*)"
    );
    
    if (!fileName.isEmpty()) {
        if (m_history->exportHistory(fileName)) {
            QMessageBox::information(this, "Export Complete", "Clipboard history exported successfully.");
        } else {
            QMessageBox::warning(this, "Export Error", "Failed to export clipboard history.");
        }
    }
}

void ClipboardHistoryWidget::onImportHistory()
{
    if (!m_history) {
        return;
    }
    
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Import Clipboard History",
        QString(),
        "JSON Files (*.json);;All Files (*)"
    );
    
    if (!fileName.isEmpty()) {
        if (m_history->importHistory(fileName)) {
            QMessageBox::information(this, "Import Complete", "Clipboard history imported successfully.");
        } else {
            QMessageBox::warning(this, "Import Error", "Failed to import clipboard history.");
        }
    }
}

void ClipboardHistoryWidget::onSearchChanged()
{
    m_searchQuery = m_searchEdit->text();
    updateItemList();
}

void ClipboardHistoryWidget::onFilterChanged()
{
    m_filterType = m_filterCombo->currentText().toLower();
    updateItemList();
}

void ClipboardHistoryWidget::updateItemList()
{
    if (!m_history) {
        return;
    }
    
    // Clear existing widgets
    QLayoutItem *item;
    while ((item = m_scrollLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    
    // Get filtered indices
    QList<int> indices;
    
    if (m_filterType == "all") {
        indices = m_history->searchItems(m_searchQuery);
    } else if (m_filterType == "images") {
        indices = m_history->getItemsByType("image");
    } else if (m_filterType == "text") {
        indices = m_history->getItemsByType("text");
    } else if (m_filterType == "mixed") {
        indices = m_history->getItemsByType("mixed");
    }
    
    // Create item widgets
    for (int index : indices) {
        ClipboardItem item = m_history->getItem(index);
        ClipboardItemWidget *widget = new ClipboardItemWidget(item, index);
        
        connect(widget, &ClipboardItemWidget::clicked, this, &ClipboardHistoryWidget::onItemClicked);
        connect(widget, &ClipboardItemWidget::doubleClicked, this, &ClipboardHistoryWidget::onItemDoubleClicked);
        
        m_scrollLayout->addWidget(widget);
    }
    
    m_displayedIndices = indices;
    updateButtons();
}

void ClipboardHistoryWidget::updateItemList(const QList<int> &indices)
{
    Q_UNUSED(indices)
    updateItemList();
}

void ClipboardHistoryWidget::createItemWidget(const ClipboardItem &item, int index)
{
    ClipboardItemWidget *widget = new ClipboardItemWidget(item, index);
    connect(widget, &ClipboardItemWidget::clicked, this, &ClipboardHistoryWidget::onItemClicked);
    connect(widget, &ClipboardItemWidget::doubleClicked, this, &ClipboardHistoryWidget::onItemDoubleClicked);
    m_scrollLayout->addWidget(widget);
}

void ClipboardHistoryWidget::updateButtons()
{
    bool hasItems = !m_displayedIndices.isEmpty();
    m_clearButton->setEnabled(hasItems);
    m_exportButton->setEnabled(hasItems);
}

// ClipboardItemWidget implementation

ClipboardItemWidget::ClipboardItemWidget(const ClipboardItem &item, int index, QWidget *parent)
    : QWidget(parent)
    , m_item(item)
    , m_index(index)
    , m_hovered(false)
{
    setupUI();
    updateDisplay();
    setFixedHeight(80);
    setMouseTracking(true);
}

ClipboardItemWidget::~ClipboardItemWidget()
{
}

void ClipboardItemWidget::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(5, 5, 5, 5);
    m_layout->setSpacing(2);
    
    m_thumbnailLabel = new QLabel();
    m_thumbnailLabel->setFixedSize(60, 60);
    m_thumbnailLabel->setScaledContents(true);
    m_thumbnailLabel->setAlignment(Qt::AlignCenter);
    
    m_descriptionLabel = new QLabel();
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setMaximumWidth(200);
    
    m_timestampLabel = new QLabel();
    m_timestampLabel->setStyleSheet("color: gray; font-size: 10px;");
    
    m_sizeLabel = new QLabel();
    m_sizeLabel->setStyleSheet("color: gray; font-size: 10px;");
    
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(m_thumbnailLabel);
    
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->addWidget(m_descriptionLabel);
    infoLayout->addWidget(m_timestampLabel);
    infoLayout->addWidget(m_sizeLabel);
    infoLayout->addStretch();
    
    mainLayout->addLayout(infoLayout);
    mainLayout->addStretch();
    
    m_layout->addLayout(mainLayout);
}

void ClipboardItemWidget::updateDisplay()
{
    // Set thumbnail
    if (!m_item.image.isNull()) {
        QPixmap pixmap = QPixmap::fromImage(m_item.image);
        m_thumbnailLabel->setPixmap(pixmap);
    } else {
        m_thumbnailLabel->setText("T");
    }
    
    // Set description
    m_descriptionLabel->setText(m_item.description);
    
    // Set timestamp
    m_timestampLabel->setText(m_item.timestamp.toString("hh:mm:ss"));
    
    // Set size
    if (m_item.type == "image") {
        m_sizeLabel->setText(QString("%1x%2").arg(m_item.imageSize.width()).arg(m_item.imageSize.height()));
    } else {
        m_sizeLabel->setText(QString("%1 bytes").arg(m_item.size));
    }
}

void ClipboardItemWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    
    if (m_hovered) {
        painter.fillRect(rect(), palette().color(QPalette::Highlight).lighter(180));
    } else {
        painter.fillRect(rect(), palette().color(QPalette::Base));
    }
    
    painter.setPen(palette().color(QPalette::WindowText));
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
}

void ClipboardItemWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked(m_index);
}

void ClipboardItemWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit doubleClicked(m_index);
}

void ClipboardItemWidget::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event)
    m_hovered = true;
    update();
}

void ClipboardItemWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    m_hovered = false;
    update();
}
