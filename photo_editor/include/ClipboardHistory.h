#ifndef CLIPBOARDHISTORY_H
#define CLIPBOARDHISTORY_H

#include <QObject>
#include <QList>
#include <QImage>
#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QTimer>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QEnterEvent>
#include <QBuffer>

class QClipboard;

struct ClipboardItem {
    QImage image;
    QString text;
    QString description;
    QDateTime timestamp;
    QString type; // "image", "text", "mixed"
    int size;
    QSize imageSize;
    
    ClipboardItem() : size(0) {}
    ClipboardItem(const QImage &img, const QString &desc = QString())
        : image(img)
        , description(desc)
        , timestamp(QDateTime::currentDateTime())
        , type("image")
        , size(img.sizeInBytes())
        , imageSize(img.size())
    {}
    ClipboardItem(const QString &txt, const QString &desc = QString())
        : text(txt)
        , description(desc)
        , timestamp(QDateTime::currentDateTime())
        , type("text")
        , size(txt.size())
    {}
};

class ClipboardHistory : public QObject
{
    Q_OBJECT

public:
    explicit ClipboardHistory(QObject *parent = nullptr);
    ~ClipboardHistory();

    // History management
    void addItem(const ClipboardItem &item);
    void addImage(const QImage &image, const QString &description = QString());
    void addText(const QString &text, const QString &description = QString());
    void removeItem(int index);
    void clearHistory();
    
    // Item access
    ClipboardItem getItem(int index) const;
    QImage getImage(int index) const;
    QString getText(int index) const;
    int getItemCount() const { return m_items.size(); }
    
    // History operations
    void setMaxItems(int count);
    int getMaxItems() const { return m_maxItems; }
    
    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }
    
    // Clipboard monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return m_monitoring; }
    
    // Search and filter
    QList<int> searchItems(const QString &query) const;
    QList<int> getItemsByType(const QString &type) const;
    QList<int> getRecentItems(int count) const;
    
    // Export/Import
    bool exportHistory(const QString &fileName) const;
    bool importHistory(const QString &fileName);
    
    // Serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

signals:
    void itemAdded(int index);
    void itemRemoved(int index);
    void historyCleared();
    void monitoringStarted();
    void monitoringStopped();

private slots:
    void onClipboardChanged();
    void onMonitoringTimeout();

private:
    void trimHistory();
    void updateDescription(ClipboardItem &item);
    QString generateDescription(const ClipboardItem &item) const;
    bool isDuplicate(const ClipboardItem &item) const;
    
    QList<ClipboardItem> m_items;
    int m_maxItems;
    bool m_enabled;
    bool m_monitoring;
    
    QClipboard *m_clipboard;
    QTimer *m_monitoringTimer;
    ClipboardItem m_lastItem;
};

class ClipboardHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClipboardHistoryWidget(QWidget *parent = nullptr);
    ~ClipboardHistoryWidget();

    void setClipboardHistory(ClipboardHistory *history);
    ClipboardHistory* getClipboardHistory() const { return m_history; }

signals:
    void itemSelected(const ClipboardItem &item);
    void itemDoubleClicked(const ClipboardItem &item);

private slots:
    void onItemAdded(int index);
    void onItemRemoved(int index);
    void onHistoryCleared();
    void onItemClicked();
    void onItemDoubleClicked();
    void onClearHistory();
    void onExportHistory();
    void onImportHistory();
    void onSearchChanged();
    void onFilterChanged();

private:
    void setupUI();
    void setupConnections();
    void updateItemList();
    void updateItemList(const QList<int> &indices);
    void createItemWidget(const ClipboardItem &item, int index);
    void updateButtons();
    
    ClipboardHistory *m_history;
    
    // UI components
    QVBoxLayout *m_mainLayout;
    
    // Search and filter
    QHBoxLayout *m_searchLayout;
    QLineEdit *m_searchEdit;
    QComboBox *m_filterCombo;
    QPushButton *m_clearSearchButton;
    
    // Item list
    QScrollArea *m_scrollArea;
    QWidget *m_scrollWidget;
    QVBoxLayout *m_scrollLayout;
    
    // Actions
    QHBoxLayout *m_actionsLayout;
    QPushButton *m_clearButton;
    QPushButton *m_exportButton;
    QPushButton *m_importButton;
    QPushButton *m_refreshButton;
    
    // Data
    QList<int> m_displayedIndices;
    QString m_searchQuery;
    QString m_filterType;
};

class ClipboardItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClipboardItemWidget(const ClipboardItem &item, int index, QWidget *parent = nullptr);
    ~ClipboardItemWidget();

    ClipboardItem getItem() const { return m_item; }
    int getIndex() const { return m_index; }

signals:
    void clicked(int index);
    void doubleClicked(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void setupUI();
    void updateDisplay();
    
    ClipboardItem m_item;
    int m_index;
    bool m_hovered;
    
    // UI components
    QVBoxLayout *m_layout;
    QLabel *m_thumbnailLabel;
    QLabel *m_descriptionLabel;
    QLabel *m_timestampLabel;
    QLabel *m_sizeLabel;
};

#endif // CLIPBOARDHISTORY_H
