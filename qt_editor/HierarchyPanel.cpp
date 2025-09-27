#include "HierarchyPanel.h"
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

HierarchyPanel::HierarchyPanel(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_treeWidget(nullptr)
    , m_buttonLayout(nullptr)
    , m_addButton(nullptr)
    , m_deleteButton(nullptr)
    , m_renameButton(nullptr)
    , m_duplicateButton(nullptr)
    , m_moveUpButton(nullptr)
    , m_moveDownButton(nullptr)
    , m_contextMenu(nullptr)
    , m_addAction(nullptr)
    , m_deleteAction(nullptr)
    , m_renameAction(nullptr)
    , m_duplicateAction(nullptr)
    , m_moveUpAction(nullptr)
    , m_moveDownAction(nullptr)
    , m_setParentAction(nullptr)
    , m_unparentAction(nullptr)
    , m_updating(false)
{
    setupUI();
    setupConnections();
    setupContextMenu();
    updateItemIcons();
}

HierarchyPanel::~HierarchyPanel()
{
}

void HierarchyPanel::setupUI()
{
    // Основной макет
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Панель кнопок
    m_buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton("+");
    m_addButton->setToolTip("Add Entity");
    m_deleteButton = new QPushButton("-");
    m_deleteButton->setToolTip("Delete Selected");
    m_renameButton = new QPushButton("R");
    m_renameButton->setToolTip("Rename");
    m_duplicateButton = new QPushButton("D");
    m_duplicateButton->setToolTip("Duplicate");
    m_moveUpButton = new QPushButton("↑");
    m_moveUpButton->setToolTip("Move Up");
    m_moveDownButton = new QPushButton("↓");
    m_moveDownButton->setToolTip("Move Down");
    
    m_buttonLayout->addWidget(m_addButton);
    m_buttonLayout->addWidget(m_deleteButton);
    m_buttonLayout->addWidget(m_renameButton);
    m_buttonLayout->addWidget(m_duplicateButton);
    m_buttonLayout->addWidget(m_moveUpButton);
    m_buttonLayout->addWidget(m_moveDownButton);
    m_buttonLayout->addStretch();
    
    m_mainLayout->addLayout(m_buttonLayout);
    
    // Tree Widget
    m_treeWidget = new QTreeWidget();
    m_treeWidget->setHeaderLabel("Hierarchy");
    m_treeWidget->setDragDropMode(QTreeWidget::InternalMove);
    m_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeWidget->setRootIsDecorated(true);
    m_treeWidget->setAlternatingRowColors(true);
    
    m_mainLayout->addWidget(m_treeWidget);
    
    // Создаем тестовые объекты
    // createTestEntities(); // Временно отключено
}

void HierarchyPanel::setupConnections()
{
    // Соединения кнопок
    connect(m_addButton, &QPushButton::clicked, this, &HierarchyPanel::onAddEntity);
    connect(m_deleteButton, &QPushButton::clicked, this, &HierarchyPanel::onDeleteEntity);
    connect(m_renameButton, &QPushButton::clicked, this, &HierarchyPanel::onRenameEntity);
    connect(m_duplicateButton, &QPushButton::clicked, this, &HierarchyPanel::onDuplicateEntity);
    connect(m_moveUpButton, &QPushButton::clicked, this, &HierarchyPanel::onMoveUp);
    connect(m_moveDownButton, &QPushButton::clicked, this, &HierarchyPanel::onMoveDown);
    
    // Соединения дерева
    connect(m_treeWidget, &QTreeWidget::itemSelectionChanged, this, &HierarchyPanel::onItemSelectionChanged);
    connect(m_treeWidget, &QTreeWidget::itemDoubleClicked, this, &HierarchyPanel::onItemDoubleClicked);
    connect(m_treeWidget, &QTreeWidget::itemChanged, this, &HierarchyPanel::onItemChanged);
    
    // Контекстное меню
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested, this, &HierarchyPanel::onContextMenuRequested);
}

void HierarchyPanel::setupContextMenu()
{
    m_contextMenu = new QMenu(this);
    
    m_addAction = new QAction("Add Entity", this);
    m_deleteAction = new QAction("Delete", this);
    m_renameAction = new QAction("Rename", this);
    m_duplicateAction = new QAction("Duplicate", this);
    m_moveUpAction = new QAction("Move Up", this);
    m_moveDownAction = new QAction("Move Down", this);
    m_setParentAction = new QAction("Set Parent", this);
    m_unparentAction = new QAction("Unparent", this);
    
    m_contextMenu->addAction(m_addAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_renameAction);
    m_contextMenu->addAction(m_duplicateAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_moveUpAction);
    m_contextMenu->addAction(m_moveDownAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_setParentAction);
    m_contextMenu->addAction(m_unparentAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_deleteAction);
    
    // Соединения действий
    connect(m_addAction, &QAction::triggered, this, &HierarchyPanel::onAddEntity);
    connect(m_deleteAction, &QAction::triggered, this, &HierarchyPanel::onDeleteEntity);
    connect(m_renameAction, &QAction::triggered, this, &HierarchyPanel::onRenameEntity);
    connect(m_duplicateAction, &QAction::triggered, this, &HierarchyPanel::onDuplicateEntity);
    connect(m_moveUpAction, &QAction::triggered, this, &HierarchyPanel::onMoveUp);
    connect(m_moveDownAction, &QAction::triggered, this, &HierarchyPanel::onMoveDown);
    connect(m_setParentAction, &QAction::triggered, this, &HierarchyPanel::onSetParent);
    connect(m_unparentAction, &QAction::triggered, this, &HierarchyPanel::onUnparent);
}

void HierarchyPanel::updateItemIcons()
{
    // Создаем иконки для разных типов объектов
    m_entityIcon = QIcon(":/icons/entity.png");
    m_folderIcon = QIcon(":/icons/folder.png");
    m_cameraIcon = QIcon(":/icons/camera.png");
    m_lightIcon = QIcon(":/icons/light.png");
    m_meshIcon = QIcon(":/icons/mesh.png");
    m_spriteIcon = QIcon(":/icons/sprite.png");
    m_audioIcon = QIcon(":/icons/audio.png");
    m_scriptIcon = QIcon(":/icons/script.png");
    m_animationIcon = QIcon(":/icons/animation.png");
    
    // Если иконки не найдены, создаем простые
    if (m_entityIcon.isNull()) {
        m_entityIcon = createSimpleIcon(QColor(100, 150, 200));
        m_folderIcon = createSimpleIcon(QColor(200, 200, 100));
        m_cameraIcon = createSimpleIcon(QColor(200, 100, 100));
        m_lightIcon = createSimpleIcon(QColor(255, 255, 100));
        m_meshIcon = createSimpleIcon(QColor(150, 100, 200));
        m_spriteIcon = createSimpleIcon(QColor(100, 200, 100));
        m_audioIcon = createSimpleIcon(QColor(200, 150, 100));
        m_scriptIcon = createSimpleIcon(QColor(100, 200, 200));
        m_animationIcon = createSimpleIcon(QColor(255, 100, 200));
    }
}

QIcon HierarchyPanel::createSimpleIcon(const QColor &color) const
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

void HierarchyPanel::onSelectionChanged(const QStringList &selectedEntities)
{
    // Обработка изменения выбора сущностей
    m_selectedEntities = selectedEntities;
    
    // Обновляем выделение в дереве
    for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_treeWidget->topLevelItem(i);
        bool selected = selectedEntities.contains(item->text(0));
        item->setSelected(selected);
    }
    
    emit selectionChanged(selectedEntities);
}

void HierarchyPanel::onItemMoved(QTreeWidgetItem *item, int oldIndex, int newIndex)
{
    // Обработка перемещения объекта в иерархии
    // Здесь можно добавить логику обновления порядка объектов в сцене
    Q_UNUSED(item);
    Q_UNUSED(oldIndex);
    Q_UNUSED(newIndex);
}

// void HierarchyPanel::createTestEntities() // Временно отключено
// {
//     // Создаем тестовые объекты
//     addEntity("Main Camera", "Camera");
//     addEntity("Directional Light", "Light");
//     addEntity("Player", "Sprite");
//     addEntity("Enemy", "Sprite");
//     addEntity("Ground", "Mesh");
//     addEntity("Background Music", "Audio");
//     addEntity("Player Script", "Script");
// }

void HierarchyPanel::addEntity(const QString &name, const QString &parent)
{
    QTreeWidgetItem *parentItem = nullptr;
    if (!parent.isEmpty()) {
        parentItem = findItem(parent);
    }
    
    QTreeWidgetItem *item = createItem(name, parent);
    if (parentItem) {
        parentItem->addChild(item);
    } else {
        m_treeWidget->addTopLevelItem(item);
    }
    
    emit entityCreated(name, parent);
}

void HierarchyPanel::removeEntity(const QString &name)
{
    QTreeWidgetItem *item = findItem(name);
    if (item) {
        QTreeWidgetItem *parent = item->parent();
        if (parent) {
            parent->removeChild(item);
        } else {
            m_treeWidget->takeTopLevelItem(m_treeWidget->indexOfTopLevelItem(item));
        }
        delete item;
        
        emit entityDeleted(name);
    }
}

void HierarchyPanel::renameEntity(const QString &oldName, const QString &newName)
{
    QTreeWidgetItem *item = findItem(oldName);
    if (item) {
        item->setText(0, newName);
        emit entityRenamed(oldName, newName);
    }
}

void HierarchyPanel::selectEntity(const QString &name)
{
    QTreeWidgetItem *item = findItem(name);
    if (item) {
        m_treeWidget->setCurrentItem(item);
        m_treeWidget->scrollToItem(item);
    }
}

void HierarchyPanel::clearSelection()
{
    m_treeWidget->clearSelection();
    m_selectedEntities.clear();
    emit selectionChanged(QStringList());
}

QStringList HierarchyPanel::getSelectedEntities() const
{
    return m_selectedEntities;
}

QString HierarchyPanel::getParent(const QString &name) const
{
    QTreeWidgetItem *item = findItem(name);
    if (item && item->parent()) {
        return item->parent()->text(0);
    }
    return QString();
}

QStringList HierarchyPanel::getChildren(const QString &name) const
{
    QStringList children;
    QTreeWidgetItem *item = findItem(name);
    if (item) {
        for (int i = 0; i < item->childCount(); ++i) {
            children.append(item->child(i)->text(0));
        }
    }
    return children;
}

bool HierarchyPanel::hasSelection() const
{
    return !m_selectedEntities.isEmpty();
}

QString HierarchyPanel::getSelectedEntity() const
{
    return m_selectedEntities.isEmpty() ? QString() : m_selectedEntities.first();
}

void HierarchyPanel::onItemSelectionChanged()
{
    m_selectedEntities.clear();
    QList<QTreeWidgetItem*> selectedItems = m_treeWidget->selectedItems();
    
    for (QTreeWidgetItem *item : selectedItems) {
        m_selectedEntities.append(item->text(0));
    }
    
    emit selectionChanged(m_selectedEntities);
}

void HierarchyPanel::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    
    if (item) {
        // Включаем режим редактирования
        m_treeWidget->editItem(item, 0);
    }
}

void HierarchyPanel::onItemChanged(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    
    if (item && !m_updating) {
        QString newName = item->text(0);
        // Здесь должна быть логика обновления имени объекта
        qDebug() << "Entity renamed to:" << newName;
    }
}

void HierarchyPanel::onContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = m_treeWidget->itemAt(pos);
    
    // Обновляем доступность действий
    bool hasSelection = !m_selectedEntities.isEmpty();
    m_deleteAction->setEnabled(hasSelection);
    m_renameAction->setEnabled(hasSelection);
    m_duplicateAction->setEnabled(hasSelection);
    m_moveUpAction->setEnabled(hasSelection);
    m_moveDownAction->setEnabled(hasSelection);
    m_setParentAction->setEnabled(hasSelection);
    m_unparentAction->setEnabled(hasSelection);
    
    // Показываем контекстное меню
    m_contextMenu->exec(m_treeWidget->mapToGlobal(pos));
}

void HierarchyPanel::onAddEntity()
{
    QString name = QString("Entity_%1").arg(m_treeWidget->topLevelItemCount() + 1);
    addEntity(name);
}

void HierarchyPanel::onDeleteEntity()
{
    QList<QTreeWidgetItem*> selectedItems = m_treeWidget->selectedItems();
    for (QTreeWidgetItem *item : selectedItems) {
        removeEntity(item->text(0));
    }
}

void HierarchyPanel::onRenameEntity()
{
    QTreeWidgetItem *item = m_treeWidget->currentItem();
    if (item) {
        m_treeWidget->editItem(item, 0);
    }
}

void HierarchyPanel::onDuplicateEntity()
{
    QTreeWidgetItem *item = m_treeWidget->currentItem();
    if (item) {
        QString name = item->text(0) + "_Copy";
        addEntity(name);
    }
}

void HierarchyPanel::onMoveUp()
{
    QTreeWidgetItem *item = m_treeWidget->currentItem();
    if (item) {
        QTreeWidgetItem *parent = item->parent();
        if (parent) {
            int index = parent->indexOfChild(item);
            if (index > 0) {
                parent->removeChild(item);
                parent->insertChild(index - 1, item);
            }
        } else {
            int index = m_treeWidget->indexOfTopLevelItem(item);
            if (index > 0) {
                m_treeWidget->takeTopLevelItem(index);
                m_treeWidget->insertTopLevelItem(index - 1, item);
            }
        }
    }
}

void HierarchyPanel::onMoveDown()
{
    QTreeWidgetItem *item = m_treeWidget->currentItem();
    if (item) {
        QTreeWidgetItem *parent = item->parent();
        if (parent) {
            int index = parent->indexOfChild(item);
            if (index < parent->childCount() - 1) {
                parent->removeChild(item);
                parent->insertChild(index + 1, item);
            }
        } else {
            int index = m_treeWidget->indexOfTopLevelItem(item);
            if (index < m_treeWidget->topLevelItemCount() - 1) {
                m_treeWidget->takeTopLevelItem(index);
                m_treeWidget->insertTopLevelItem(index + 1, item);
            }
        }
    }
}

void HierarchyPanel::onSetParent()
{
    // TODO: Реализовать установку родителя
}

void HierarchyPanel::onUnparent()
{
    QTreeWidgetItem *item = m_treeWidget->currentItem();
    if (item && item->parent()) {
        QTreeWidgetItem *parent = item->parent();
        parent->removeChild(item);
        m_treeWidget->addTopLevelItem(item);
    }
}

QTreeWidgetItem* HierarchyPanel::findItem(const QString &name) const
{
    QTreeWidgetItemIterator it(m_treeWidget);
    while (*it) {
        if ((*it)->text(0) == name) {
            return *it;
        }
        ++it;
    }
    return nullptr;
}

QTreeWidgetItem* HierarchyPanel::createItem(const QString &name, const QString &parent)
{
    Q_UNUSED(parent)
    
    HierarchyItem *item = new HierarchyItem(name, "Entity");
    item->setIcon(0, m_entityIcon);
    return item;
}

void HierarchyPanel::updateItemText(QTreeWidgetItem *item, const QString &name)
{
    if (item) {
        item->setText(0, name);
    }
}

void HierarchyPanel::updateItemIcon(QTreeWidgetItem *item, const QString &type)
{
    if (item) {
        QIcon icon = m_entityIcon;
        if (type == "Camera") icon = m_cameraIcon;
        else if (type == "Light") icon = m_lightIcon;
        else if (type == "Mesh") icon = m_meshIcon;
        else if (type == "Sprite") icon = m_spriteIcon;
        else if (type == "Audio") icon = m_audioIcon;
        else if (type == "Script") icon = m_scriptIcon;
        else if (type == "Animation") icon = m_animationIcon;
        
        item->setIcon(0, icon);
    }
}

void HierarchyPanel::setupDragAndDrop()
{
    // TODO: Реализовать drag & drop
}

bool HierarchyPanel::canDrop(QTreeWidgetItem *item, QTreeWidgetItem *parent) const
{
    Q_UNUSED(item)
    Q_UNUSED(parent)
    // TODO: Реализовать проверку возможности drop
    return true;
}

void HierarchyPanel::handleDrop(QTreeWidgetItem *item, QTreeWidgetItem *parent)
{
    Q_UNUSED(item)
    Q_UNUSED(parent)
    // TODO: Реализовать обработку drop
}

// Реализация HierarchyItem
HierarchyItem::HierarchyItem(const QString &name, const QString &type, QTreeWidgetItem *parent)
    : QTreeWidgetItem(parent)
    , m_name(name)
    , m_type(type)
    , m_visible(true)
    , m_locked(false)
{
    setText(0, name);
    updateIcon();
}

QVariant HierarchyItem::data(int column, int role) const
{
    if (role == Qt::DisplayRole) {
        if (column == 0) {
            return m_name;
        }
    } else if (role == Qt::DecorationRole) {
        if (column == 0) {
            return getIconForType(m_type);
        }
    } else if (role == Qt::CheckStateRole) {
        if (column == 0) {
            return m_visible ? Qt::Checked : Qt::Unchecked;
        }
    }
    
    return QTreeWidgetItem::data(column, role);
}

void HierarchyItem::setData(int column, int role, const QVariant &value)
{
    if (role == Qt::CheckStateRole && column == 0) {
        m_visible = (value.toInt() == Qt::Checked);
        updateIcon();
    } else if (role == Qt::DisplayRole && column == 0) {
        m_name = value.toString();
    }
    
    QTreeWidgetItem::setData(column, role, value);
}

void HierarchyItem::updateIcon()
{
    setIcon(0, getIconForType(m_type));
}

QIcon HierarchyItem::getIconForType(const QString &type) const
{
    // Простые иконки для разных типов
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QColor color = Qt::blue;
    if (type == "Camera") color = Qt::red;
    else if (type == "Light") color = Qt::yellow;
    else if (type == "Mesh") color = Qt::green;
    else if (type == "Sprite") color = Qt::cyan;
    else if (type == "Audio") color = Qt::magenta;
    else if (type == "Script") color = Qt::darkBlue;
    else if (type == "Animation") color = Qt::darkRed;
    
    painter.setBrush(QBrush(color));
    painter.setPen(QPen(Qt::black, 1));
    painter.drawEllipse(2, 2, 12, 12);
    
    return QIcon(pixmap);
}
