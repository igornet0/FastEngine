#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QHeaderView>
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
#include <QFileSystemModel>
#include <QTreeView>
#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QGridLayout>
#include <QScrollArea>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QProgressBar>
#include <QTextEdit>
#include <QTabWidget>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QTimer>

class AssetsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit AssetsPanel(QWidget *parent = nullptr);
    ~AssetsPanel();

    // Управление активами
    void setProjectPath(const QString &path);
    void refreshAssets();
    void addAsset(const QString &filePath);
    void removeAsset(const QString &filePath);
    void renameAsset(const QString &oldPath, const QString &newPath);
    
    // Получение информации
    QStringList getSelectedAssets() const;
    QString getProjectPath() const;
    bool hasSelection() const;
    QString getSelectedAsset() const;

signals:
    void assetSelected(const QString &assetPath);
    void assetDoubleClicked(const QString &assetPath);
    void assetRenamed(const QString &oldPath, const QString &newPath);
    void assetDeleted(const QString &assetPath);
    void assetImported(const QString &assetPath);
    void projectPathChanged(const QString &path);

private slots:
    void onItemSelectionChanged();
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
    void onItemChanged(QTreeWidgetItem *item, int column);
    void onContextMenuRequested(const QPoint &pos);
    void onImportAsset();
    void onDeleteAsset();
    void onRenameAsset();
    void onDuplicateAsset();
    void onRefreshAssets();
    void onShowInExplorer();
    void onAssetProperties();
    void onViewModeChanged();
    void onFilterChanged();
    void onSearchChanged();
    void onFileSystemChanged(const QString &path);

private:
    void setupUI();
    void setupConnections();
    void setupContextMenu();
    void setupFileSystemWatcher();
    void updateViewMode();
    void updateFilter();
    void updateSearch();
    
    // Работа с файловой системой
    void scanDirectory(const QString &path, QTreeWidgetItem *parent = nullptr);
    void createAssetItem(const QString &filePath, QTreeWidgetItem *parent = nullptr);
    void updateAssetItem(QTreeWidgetItem *item, const QString &filePath);
    void removeAssetItem(const QString &filePath);
    
    // Обработка drag & drop
    void setupDragAndDrop();
    bool canDrop(QTreeWidgetItem *item, QTreeWidgetItem *parent) const;
    void handleDrop(QTreeWidgetItem *item, QTreeWidgetItem *parent);
    void createTestAssets();
    void onItemDoubleClicked(QListWidgetItem *item);
    
    // Фильтрация и поиск
    bool matchesFilter(const QString &filePath) const;
    bool matchesSearch(const QString &filePath) const;
    
    // UI компоненты
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_toolbarLayout;
    QTreeWidget *m_treeWidget;
    QListWidget *m_listWidget;
    QTabWidget *m_tabWidget;
    
    // Панель инструментов
    QPushButton *m_importButton;
    QPushButton *m_refreshButton;
    QPushButton *m_deleteButton;
    QPushButton *m_renameButton;
    QPushButton *m_duplicateButton;
    QPushButton *m_propertiesButton;
    QPushButton *m_showInExplorerButton;
    
    // Фильтры и поиск
    QLineEdit *m_searchEdit;
    QComboBox *m_filterCombo;
    QComboBox *m_viewModeCombo;
    QCheckBox *m_showHiddenCheckBox;
    
    // Контекстное меню
    QMenu *m_contextMenu;
    QAction *m_importAction;
    QAction *m_deleteAction;
    QAction *m_renameAction;
    QAction *m_duplicateAction;
    QAction *m_propertiesAction;
    QAction *m_showInExplorerAction;
    QAction *m_refreshAction;
    
    // Состояние
    QString m_projectPath;
    QStringList m_selectedAssets;
    QString m_currentFilter;
    QString m_currentSearch;
    int m_viewMode; // 0 = Tree, 1 = List, 2 = Grid
    
    // Иконки
    QIcon m_folderIcon;
    QIcon m_fileIcon;
    QIcon m_imageIcon;
    QIcon m_audioIcon;
    QIcon m_videoIcon;
    QIcon m_modelIcon;
    QIcon m_scriptIcon;
    QIcon m_shaderIcon;
    QIcon m_materialIcon;
    QIcon m_animationIcon;
    QIcon m_fontIcon;
    QIcon m_dataIcon;
    
    // Карта типов файлов
    QMap<QString, QString> m_fileTypes;
    QMap<QString, QIcon> m_typeIcons;
    
    // Наблюдатель файловой системы
    QFileSystemWatcher *m_fileSystemWatcher;
    
    // Кэш превью
    QMap<QString, QPixmap> m_previewCache;
    QMutex m_cacheMutex;
    
    // Таймер обновления
    QTimer *m_updateTimer;
    
    // Дополнительные переменные
    bool m_updating;
    QListWidget *m_assetList;
};

// Класс для элементов активов
class AssetItem : public QTreeWidgetItem
{
public:
    explicit AssetItem(const QString &filePath, QTreeWidgetItem *parent = nullptr);
    
    // Свойства
    QString getFilePath() const { return m_filePath; }
    void setFilePath(const QString &filePath) { m_filePath = filePath; }
    
    QString getFileName() const { return m_fileName; }
    void setFileName(const QString &fileName) { m_fileName = fileName; setText(0, fileName); }
    
    QString getFileType() const { return m_fileType; }
    void setFileType(const QString &fileType) { m_fileType = fileType; }
    
    qint64 getFileSize() const { return m_fileSize; }
    void setFileSize(qint64 fileSize) { m_fileSize = fileSize; }
    
    QDateTime getLastModified() const { return m_lastModified; }
    void setLastModified(const QDateTime &lastModified) { m_lastModified = lastModified; }
    
    bool isDirectory() const { return m_isDirectory; }
    void setIsDirectory(bool isDirectory) { m_isDirectory = isDirectory; }
    
    QPixmap getPreview() const { return m_preview; }
    void setPreview(const QPixmap &preview) { m_preview = preview; }
    
    // Переопределение методов
    QVariant data(int column, int role) const override;
    void setData(int column, int role, const QVariant &value) override;

private:
    void updateIcon();
    QIcon getIconForType(const QString &type) const;
    QString formatFileSize(qint64 size) const;
    
    QString m_filePath;
    QString m_fileName;
    QString m_fileType;
    qint64 m_fileSize;
    QDateTime m_lastModified;
    bool m_isDirectory;
    QPixmap m_preview;
};

// Класс для элементов списка активов
class AssetListItem : public QListWidgetItem
{
public:
    explicit AssetListItem(const QString &filePath, QListWidget *parent = nullptr);
    
    // Свойства
    QString getFilePath() const { return m_filePath; }
    void setFilePath(const QString &filePath) { m_filePath = filePath; }
    
    QString getFileName() const { return m_fileName; }
    void setFileName(const QString &fileName) { m_fileName = fileName; setText(fileName); }
    
    QString getFileType() const { return m_fileType; }
    void setFileType(const QString &fileType) { m_fileType = fileType; }
    
    QPixmap getPreview() const { return m_preview; }
    void setPreview(const QPixmap &preview) { m_preview = preview; setIcon(QIcon(preview)); }
    
    // Переопределение методов
    QVariant data(int role) const override;
    void setData(int role, const QVariant &value) override;

private:
    void updateIcon();
    QIcon getIconForType(const QString &type) const;
    
    QString m_filePath;
    QString m_fileName;
    QString m_fileType;
    QPixmap m_preview;
};

// Класс для генерации превью
class PreviewGenerator : public QObject
{
    Q_OBJECT

public:
    explicit PreviewGenerator(QObject *parent = nullptr);
    ~PreviewGenerator();
    
    void generatePreview(const QString &filePath);
    void generatePreviews(const QStringList &filePaths);
    void stopGeneration();

signals:
    void previewGenerated(const QString &filePath, const QPixmap &preview);
    void generationFinished();

private slots:
    void processQueue();

private:
    void generateImagePreview(const QString &filePath);
    void generateAudioPreview(const QString &filePath);
    void generateVideoPreview(const QString &filePath);
    void generateModelPreview(const QString &filePath);
    void generateScriptPreview(const QString &filePath);
    
    QQueue<QString> m_generationQueue;
    QMutex m_queueMutex;
    QWaitCondition m_queueCondition;
    QThread *m_generationThread;
    bool m_stopGeneration;
};
