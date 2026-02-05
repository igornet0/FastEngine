#include "AssetsPanel.h"
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
#include <QDateTime>
#include <QTime>
#include <QDate>
#include <QLocale>
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

AssetsPanel::AssetsPanel(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_toolbarLayout(nullptr)
    , m_treeWidget(nullptr)
    , m_listWidget(nullptr)
    , m_tabWidget(nullptr)
    , m_importButton(nullptr)
    , m_refreshButton(nullptr)
    , m_deleteButton(nullptr)
    , m_renameButton(nullptr)
    , m_duplicateButton(nullptr)
    , m_propertiesButton(nullptr)
    , m_showInExplorerButton(nullptr)
    , m_searchEdit(nullptr)
    , m_filterCombo(nullptr)
    , m_viewModeCombo(nullptr)
    , m_showHiddenCheckBox(nullptr)
    , m_contextMenu(nullptr)
    , m_importAction(nullptr)
    , m_deleteAction(nullptr)
    , m_renameAction(nullptr)
    , m_duplicateAction(nullptr)
    , m_propertiesAction(nullptr)
    , m_showInExplorerAction(nullptr)
    , m_refreshAction(nullptr)
    , m_selectedAssets()
    , m_projectPath()
    , m_currentFilter()
    , m_currentSearch()
    , m_viewMode(0)
    , m_fileSystemWatcher(nullptr)
    , m_updateTimer(nullptr)
{
    setupUI();
    setupConnections();
    setupContextMenu();
    setupFileSystemWatcher();
    updateViewMode();
}

AssetsPanel::~AssetsPanel()
{
    if (m_fileSystemWatcher) {
        delete m_fileSystemWatcher;
    }
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
}

void AssetsPanel::setupUI()
{
    // Основной макет
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Панель инструментов
    m_toolbarLayout = new QHBoxLayout();
    
    m_importButton = new QPushButton("Import");
    m_refreshButton = new QPushButton("Refresh");
    m_deleteButton = new QPushButton("Delete");
    m_renameButton = new QPushButton("Rename");
    m_duplicateButton = new QPushButton("Duplicate");
    m_propertiesButton = new QPushButton("Properties");
    m_showInExplorerButton = new QPushButton("Show in Explorer");
    
    m_toolbarLayout->addWidget(m_importButton);
    m_toolbarLayout->addWidget(m_refreshButton);
    m_toolbarLayout->addWidget(m_deleteButton);
    m_toolbarLayout->addWidget(m_renameButton);
    m_toolbarLayout->addWidget(m_duplicateButton);
    m_toolbarLayout->addWidget(m_propertiesButton);
    m_toolbarLayout->addWidget(m_showInExplorerButton);
    m_toolbarLayout->addStretch();
    
    m_mainLayout->addLayout(m_toolbarLayout);
    
    // Поиск и фильтры
    QHBoxLayout *searchLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Search assets...");
    m_filterCombo = new QComboBox();
    m_filterCombo->addItems({"All", "Images", "Audio", "Models", "Scripts", "Shaders"});
    m_viewModeCombo = new QComboBox();
    m_viewModeCombo->addItems({"Tree", "List", "Grid"});
    m_showHiddenCheckBox = new QCheckBox("Show Hidden");
    
    searchLayout->addWidget(m_searchEdit);
    searchLayout->addWidget(m_filterCombo);
    searchLayout->addWidget(m_viewModeCombo);
    searchLayout->addWidget(m_showHiddenCheckBox);
    
    m_mainLayout->addLayout(searchLayout);
    
    // Tab Widget
    m_tabWidget = new QTabWidget();
    m_mainLayout->addWidget(m_tabWidget);
    
    // Tree View
    m_treeWidget = new QTreeWidget();
    m_treeWidget->setHeaderLabel("Assets");
    m_treeWidget->setDragDropMode(QTreeWidget::InternalMove);
    m_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeWidget->setRootIsDecorated(true);
    m_treeWidget->setAlternatingRowColors(true);
    
    m_tabWidget->addTab(m_treeWidget, "Tree");
    
    // List View
    m_listWidget = new QListWidget();
    m_listWidget->setViewMode(QListWidget::IconMode);
    m_listWidget->setResizeMode(QListWidget::Adjust);
    m_listWidget->setMovement(QListWidget::Static);
    m_listWidget->setGridSize(QSize(100, 100));
    
    m_tabWidget->addTab(m_listWidget, "List");
    
    // Создаем тестовые ресурсы
    createTestAssets();
    
    // Подключаем сигналы
    // connect(m_assetList, &QListWidget::itemDoubleClicked, this, &AssetsPanel::onItemDoubleClicked);
    
    // Инициализируем переменные
    m_updating = false;
}

void AssetsPanel::setupConnections()
{
    // Соединения кнопок
    connect(m_importButton, &QPushButton::clicked, this, &AssetsPanel::onImportAsset);
    connect(m_refreshButton, &QPushButton::clicked, this, &AssetsPanel::onRefreshAssets);
    connect(m_deleteButton, &QPushButton::clicked, this, &AssetsPanel::onDeleteAsset);
    connect(m_renameButton, &QPushButton::clicked, this, &AssetsPanel::onRenameAsset);
    connect(m_duplicateButton, &QPushButton::clicked, this, &AssetsPanel::onDuplicateAsset);
    connect(m_propertiesButton, &QPushButton::clicked, this, &AssetsPanel::onAssetProperties);
    connect(m_showInExplorerButton, &QPushButton::clicked, this, &AssetsPanel::onShowInExplorer);
    
    // Соединения поиска и фильтров
    connect(m_searchEdit, &QLineEdit::textChanged, this, &AssetsPanel::onSearchChanged);
    connect(m_filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AssetsPanel::onFilterChanged);
    connect(m_viewModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AssetsPanel::onViewModeChanged);
    connect(m_showHiddenCheckBox, &QCheckBox::toggled, this, &AssetsPanel::onFilterChanged);
    
    // Соединения дерева
    connect(m_treeWidget, &QTreeWidget::itemSelectionChanged, this, &AssetsPanel::onItemSelectionChanged);
    // Эти соединения временно отключены из-за проблем с сигнатурами
    // connect(m_treeWidget, &QTreeWidget::itemDoubleClicked, this, &AssetsPanel::onItemDoubleClicked);
    connect(m_treeWidget, &QTreeWidget::itemChanged, this, &AssetsPanel::onItemChanged);
    
    // Соединения списка
    connect(m_listWidget, &QListWidget::itemSelectionChanged, this, &AssetsPanel::onItemSelectionChanged);
    // connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &AssetsPanel::onItemDoubleClicked);
    
    // Контекстное меню
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested, this, &AssetsPanel::onContextMenuRequested);
    connect(m_listWidget, &QListWidget::customContextMenuRequested, this, &AssetsPanel::onContextMenuRequested);
}

void AssetsPanel::setupContextMenu()
{
    m_contextMenu = new QMenu(this);
    
    m_importAction = new QAction("Import Asset", this);
    m_deleteAction = new QAction("Delete", this);
    m_renameAction = new QAction("Rename", this);
    m_duplicateAction = new QAction("Duplicate", this);
    m_propertiesAction = new QAction("Properties", this);
    m_showInExplorerAction = new QAction("Show in Explorer", this);
    m_refreshAction = new QAction("Refresh", this);
    
    m_contextMenu->addAction(m_importAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_renameAction);
    m_contextMenu->addAction(m_duplicateAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_propertiesAction);
    m_contextMenu->addAction(m_showInExplorerAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_refreshAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_deleteAction);
    
    // Соединения действий
    connect(m_importAction, &QAction::triggered, this, &AssetsPanel::onImportAsset);
    connect(m_deleteAction, &QAction::triggered, this, &AssetsPanel::onDeleteAsset);
    connect(m_renameAction, &QAction::triggered, this, &AssetsPanel::onRenameAsset);
    connect(m_duplicateAction, &QAction::triggered, this, &AssetsPanel::onDuplicateAsset);
    connect(m_propertiesAction, &QAction::triggered, this, &AssetsPanel::onAssetProperties);
    connect(m_showInExplorerAction, &QAction::triggered, this, &AssetsPanel::onShowInExplorer);
    connect(m_refreshAction, &QAction::triggered, this, &AssetsPanel::onRefreshAssets);
}

void AssetsPanel::setupFileSystemWatcher()
{
    m_fileSystemWatcher = new QFileSystemWatcher(this);
    connect(m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, &AssetsPanel::onFileSystemChanged);
    
    // Таймер обновления
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &AssetsPanel::updateFilter);
    m_updateTimer->start(1000); // Обновление каждую секунду
}

void AssetsPanel::createTestAssets()
{
    // Создаем тестовые ресурсы
    addAsset("Textures/player.png");
    addAsset("Textures/enemy.png");
    addAsset("Textures/background.jpg");
    addAsset("Models/player.obj");
    addAsset("Models/enemy.obj");
    addAsset("Audio/background_music.mp3");
    addAsset("Audio/jump.wav");
    addAsset("Scripts/player_controller.js");
    addAsset("Scripts/enemy_ai.js");
    addAsset("Shaders/sprite.vert");
    addAsset("Shaders/sprite.frag");
}

void AssetsPanel::setProjectPath(const QString &path)
{
    m_projectPath = path;
    
    if (m_fileSystemWatcher) {
        m_fileSystemWatcher->removePaths(m_fileSystemWatcher->directories());
        if (!path.isEmpty()) {
            m_fileSystemWatcher->addPath(path);
        }
    }
    
    refreshAssets();
    emit projectPathChanged(path);
}

void AssetsPanel::refreshAssets()
{
    if (m_projectPath.isEmpty()) {
        return;
    }
    
    // Очищаем текущие элементы
    m_treeWidget->clear();
    m_listWidget->clear();
    
    // Сканируем директорию проекта
    scanDirectory(m_projectPath);
}

void AssetsPanel::addAsset(const QString &filePath)
{
    createAssetItem(filePath);
}

void AssetsPanel::removeAsset(const QString &filePath)
{
    removeAssetItem(filePath);
}

void AssetsPanel::renameAsset(const QString &oldPath, const QString &newPath)
{
    // TODO: Реализовать переименование ресурса
    Q_UNUSED(oldPath)
    Q_UNUSED(newPath)
}

QStringList AssetsPanel::getSelectedAssets() const
{
    return m_selectedAssets;
}

QString AssetsPanel::getProjectPath() const
{
    return m_projectPath;
}

bool AssetsPanel::hasSelection() const
{
    return !m_selectedAssets.isEmpty();
}

QString AssetsPanel::getSelectedAsset() const
{
    return m_selectedAssets.isEmpty() ? QString() : m_selectedAssets.first();
}

void AssetsPanel::onItemSelectionChanged()
{
    m_selectedAssets.clear();
    
    // Получаем выбранные элементы из активного вида
    if (m_tabWidget->currentWidget() == m_treeWidget) {
        QList<QTreeWidgetItem*> selectedItems = m_treeWidget->selectedItems();
        for (QTreeWidgetItem *item : selectedItems) {
            if (auto assetItem = dynamic_cast<AssetItem*>(item)) {
                m_selectedAssets.append(assetItem->getFilePath());
            }
        }
    } else if (m_tabWidget->currentWidget() == m_listWidget) {
        QList<QListWidgetItem*> selectedItems = m_listWidget->selectedItems();
        for (QListWidgetItem *item : selectedItems) {
            if (auto assetItem = dynamic_cast<AssetListItem*>(item)) {
                m_selectedAssets.append(assetItem->getFilePath());
            }
        }
    }
    
    emit assetSelected(m_selectedAssets.isEmpty() ? QString() : m_selectedAssets.first());
}

void AssetsPanel::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    
    if (item) {
        if (auto assetItem = dynamic_cast<AssetItem*>(item)) {
            emit assetDoubleClicked(assetItem->getFilePath());
        }
    }
}

void AssetsPanel::onItemDoubleClicked(QListWidgetItem *item)
{
    if (item) {
        if (auto assetItem = dynamic_cast<AssetListItem*>(item)) {
            emit assetDoubleClicked(assetItem->getFilePath());
        }
    }
}

void AssetsPanel::onItemChanged(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    
    if (item && !m_updating) {
        if (auto assetItem = dynamic_cast<AssetItem*>(item)) {
            QString newName = item->text(0);
            // TODO: Реализовать переименование
            qDebug() << "Asset renamed to:" << newName;
        }
    }
}

void AssetsPanel::onContextMenuRequested(const QPoint &pos)
{
    QWidget *widget = qobject_cast<QWidget*>(sender());
    QTreeWidgetItem *treeItem = nullptr;
    QListWidgetItem *listItem = nullptr;
    
    if (widget == m_treeWidget) {
        treeItem = m_treeWidget->itemAt(pos);
    } else if (widget == m_listWidget) {
        listItem = m_listWidget->itemAt(pos);
    }
    
    // Обновляем доступность действий
    bool hasSelection = !m_selectedAssets.isEmpty();
    m_deleteAction->setEnabled(hasSelection);
    m_renameAction->setEnabled(hasSelection);
    m_duplicateAction->setEnabled(hasSelection);
    m_propertiesAction->setEnabled(hasSelection);
    m_showInExplorerAction->setEnabled(hasSelection);
    
    // Показываем контекстное меню
    m_contextMenu->exec(widget->mapToGlobal(pos));
}

void AssetsPanel::onImportAsset()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(this, "Import Assets", "", 
        "All Files (*);;Images (*.png *.jpg *.jpeg *.bmp *.tga);;Audio (*.mp3 *.wav *.ogg);;Models (*.obj *.fbx *.dae);;Scripts (*.js *.lua *.py)");
    
    for (const QString &filePath : filePaths) {
        addAsset(filePath);
        emit assetImported(filePath);
    }
}

void AssetsPanel::onDeleteAsset()
{
    QList<QTreeWidgetItem*> selectedItems = m_treeWidget->selectedItems();
    for (QTreeWidgetItem *item : selectedItems) {
        if (auto assetItem = dynamic_cast<AssetItem*>(item)) {
            removeAsset(assetItem->getFilePath());
            emit assetDeleted(assetItem->getFilePath());
        }
    }
}

void AssetsPanel::onRenameAsset()
{
    QTreeWidgetItem *item = m_treeWidget->currentItem();
    if (item) {
        m_treeWidget->editItem(item, 0);
    }
}

void AssetsPanel::onDuplicateAsset()
{
    QTreeWidgetItem *item = m_treeWidget->currentItem();
    if (item && dynamic_cast<AssetItem*>(item)) {
        // TODO: Реализовать дублирование ресурса
        qDebug() << "Duplicate asset:" << item->text(0);
    }
}

void AssetsPanel::onRefreshAssets()
{
    refreshAssets();
}

void AssetsPanel::onShowInExplorer()
{
    QTreeWidgetItem *item = m_treeWidget->currentItem();
    if (item && dynamic_cast<AssetItem*>(item)) {
        // TODO: Реализовать показ в проводнике
        qDebug() << "Show in explorer:" << item->text(0);
    }
}

void AssetsPanel::onAssetProperties()
{
    QTreeWidgetItem *item = m_treeWidget->currentItem();
    if (item && dynamic_cast<AssetItem*>(item)) {
        // TODO: Реализовать показ свойств ресурса
        qDebug() << "Asset properties:" << item->text(0);
    }
}

void AssetsPanel::onViewModeChanged()
{
    m_viewMode = m_viewModeCombo->currentIndex();
    updateViewMode();
}

void AssetsPanel::onFilterChanged()
{
    m_currentFilter = m_filterCombo->currentText();
    m_currentSearch = m_searchEdit->text();
    updateFilter();
}

void AssetsPanel::onSearchChanged()
{
    m_currentSearch = m_searchEdit->text();
    updateFilter();
}

void AssetsPanel::onFileSystemChanged(const QString &path)
{
    Q_UNUSED(path)
    // TODO: Реализовать обновление при изменении файловой системы
    refreshAssets();
}

void AssetsPanel::updateViewMode()
{
    // TODO: Реализовать переключение режимов просмотра
}

void AssetsPanel::updateFilter()
{
    // TODO: Реализовать фильтрацию ресурсов
}

void AssetsPanel::scanDirectory(const QString &path, QTreeWidgetItem *parent)
{
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }
    
    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::Name);
    
    for (const QFileInfo &entry : entries) {
        QString filePath = entry.absoluteFilePath();
        
        if (entry.isDir()) {
            QTreeWidgetItem *dirItem = new QTreeWidgetItem();
            dirItem->setText(0, entry.fileName());
            dirItem->setIcon(0, m_folderIcon);
            
            if (parent) {
                parent->addChild(dirItem);
            } else {
                m_treeWidget->addTopLevelItem(dirItem);
            }
            
            // Рекурсивно сканируем поддиректории
            scanDirectory(filePath, dirItem);
        } else {
            createAssetItem(filePath, parent);
        }
    }
}

void AssetsPanel::createAssetItem(const QString &filePath, QTreeWidgetItem *parent)
{
    AssetItem *item = new AssetItem(filePath, parent);
    
    if (parent) {
        parent->addChild(item);
    } else {
        m_treeWidget->addTopLevelItem(item);
    }
    
    // Добавляем в список
    AssetListItem *listItem = new AssetListItem(filePath, m_listWidget);
    m_listWidget->addItem(listItem);
}

void AssetsPanel::updateAssetItem(QTreeWidgetItem *item, const QString &filePath)
{
    if (auto assetItem = dynamic_cast<AssetItem*>(item)) {
        assetItem->setFilePath(filePath);
    }
}

void AssetsPanel::removeAssetItem(const QString &filePath)
{
    // Удаляем из дерева
    QTreeWidgetItemIterator it(m_treeWidget);
    while (*it) {
        if (auto assetItem = dynamic_cast<AssetItem*>(*it)) {
            if (assetItem->getFilePath() == filePath) {
                QTreeWidgetItem *parent = (*it)->parent();
                if (parent) {
                    parent->removeChild(*it);
                } else {
                    m_treeWidget->takeTopLevelItem(m_treeWidget->indexOfTopLevelItem(*it));
                }
                delete *it;
                break;
            }
        }
        ++it;
    }
    
    // Удаляем из списка
    for (int i = 0; i < m_listWidget->count(); ++i) {
        if (auto assetItem = dynamic_cast<AssetListItem*>(m_listWidget->item(i))) {
            if (assetItem->getFilePath() == filePath) {
                delete m_listWidget->takeItem(i);
                break;
            }
        }
    }
}

bool AssetsPanel::matchesFilter(const QString &filePath) const
{
    Q_UNUSED(filePath)
    // TODO: Реализовать проверку фильтра
    return true;
}

bool AssetsPanel::matchesSearch(const QString &filePath) const
{
    if (m_currentSearch.isEmpty()) {
        return true;
    }
    
    return filePath.contains(m_currentSearch, Qt::CaseInsensitive);
}

void AssetsPanel::setupDragAndDrop()
{
    // TODO: Реализовать drag & drop
}

bool AssetsPanel::canDrop(QTreeWidgetItem *item, QTreeWidgetItem *parent) const
{
    Q_UNUSED(item)
    Q_UNUSED(parent)
    // TODO: Реализовать проверку возможности drop
    return true;
}

void AssetsPanel::handleDrop(QTreeWidgetItem *item, QTreeWidgetItem *parent)
{
    Q_UNUSED(item)
    Q_UNUSED(parent)
    // TODO: Реализовать обработку drop
}

// Реализация AssetItem
AssetItem::AssetItem(const QString &filePath, QTreeWidgetItem *parent)
    : QTreeWidgetItem(parent)
    , m_filePath(filePath)
    , m_fileName()
    , m_fileType()
    , m_fileSize(0)
    , m_lastModified()
    , m_isDirectory(false)
{
    QFileInfo fileInfo(filePath);
    m_fileName = fileInfo.fileName();
    m_fileType = fileInfo.suffix();
    m_fileSize = fileInfo.size();
    m_lastModified = fileInfo.lastModified();
    m_isDirectory = fileInfo.isDir();
    
    setText(0, m_fileName);
    updateIcon();
}

QVariant AssetItem::data(int column, int role) const
{
    if (role == Qt::DisplayRole) {
        if (column == 0) {
            return m_fileName;
        } else if (column == 1) {
            return formatFileSize(m_fileSize);
        } else if (column == 2) {
            return m_lastModified.toString("yyyy-MM-dd hh:mm:ss");
        }
    } else if (role == Qt::DecorationRole) {
        if (column == 0) {
            return getIconForType(m_fileType);
        }
    }
    
    return QTreeWidgetItem::data(column, role);
}

void AssetItem::setData(int column, int role, const QVariant &value)
{
    if (role == Qt::DisplayRole && column == 0) {
        m_fileName = value.toString();
    }
    
    QTreeWidgetItem::setData(column, role, value);
}

void AssetItem::updateIcon()
{
    setIcon(0, getIconForType(m_fileType));
}

QIcon AssetItem::getIconForType(const QString &type) const
{
    // Простые иконки для разных типов файлов
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QColor color = Qt::blue;
    if (type == "png" || type == "jpg" || type == "jpeg" || type == "bmp") color = Qt::green;
    else if (type == "mp3" || type == "wav" || type == "ogg") color = Qt::red;
    else if (type == "obj" || type == "fbx" || type == "dae") color = Qt::yellow;
    else if (type == "js" || type == "lua" || type == "py") color = Qt::cyan;
    else if (type == "vert" || type == "frag") color = Qt::magenta;
    
    painter.setBrush(QBrush(color));
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(2, 2, 12, 12);
    
    return QIcon(pixmap);
}

QString AssetItem::formatFileSize(qint64 size) const
{
    if (size < 1024) {
        return QString::number(size) + " B";
    } else if (size < 1024 * 1024) {
        return QString::number(size / 1024.0, 'f', 1) + " KB";
    } else if (size < 1024 * 1024 * 1024) {
        return QString::number(size / (1024.0 * 1024.0), 'f', 1) + " MB";
    } else {
        return QString::number(size / (1024.0 * 1024.0 * 1024.0), 'f', 1) + " GB";
    }
}

// Реализация AssetListItem
AssetListItem::AssetListItem(const QString &filePath, QListWidget *parent)
    : QListWidgetItem(parent)
    , m_filePath(filePath)
    , m_fileName()
    , m_fileType()
{
    QFileInfo fileInfo(filePath);
    m_fileName = fileInfo.fileName();
    m_fileType = fileInfo.suffix();
    
    setText(m_fileName);
    updateIcon();
}

QVariant AssetListItem::data(int role) const
{
    if (role == Qt::DisplayRole) {
        return m_fileName;
    } else if (role == Qt::DecorationRole) {
        return getIconForType(m_fileType);
    }
    
    return QListWidgetItem::data(role);
}

void AssetListItem::setData(int role, const QVariant &value)
{
    if (role == Qt::DisplayRole) {
        m_fileName = value.toString();
    }
    
    QListWidgetItem::setData(role, value);
}

void AssetListItem::updateIcon()
{
    setIcon(getIconForType(m_fileType));
}

QIcon AssetListItem::getIconForType(const QString &type) const
{
    // Простые иконки для разных типов файлов
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QColor color = Qt::blue;
    if (type == "png" || type == "jpg" || type == "jpeg" || type == "bmp") color = Qt::green;
    else if (type == "mp3" || type == "wav" || type == "ogg") color = Qt::red;
    else if (type == "obj" || type == "fbx" || type == "dae") color = Qt::yellow;
    else if (type == "js" || type == "lua" || type == "py") color = Qt::cyan;
    else if (type == "vert" || type == "frag") color = Qt::magenta;
    
    painter.setBrush(QBrush(color));
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(4, 4, 24, 24);
    
    return QIcon(pixmap);
}

// PreviewGenerator implementation  
PreviewGenerator::PreviewGenerator(QObject *parent) : QObject(parent)
{
}

PreviewGenerator::~PreviewGenerator()
{
}

void PreviewGenerator::processQueue()
{
    // Обработка очереди генерации превью
}

