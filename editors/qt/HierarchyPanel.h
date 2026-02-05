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

class HierarchyPanel : public QWidget
{
    Q_OBJECT

public:
    explicit HierarchyPanel(QWidget *parent = nullptr);
    ~HierarchyPanel();

    // Управление объектами
    void addEntity(const QString &name, const QString &parent = QString());
    void removeEntity(const QString &name);
    void renameEntity(const QString &oldName, const QString &newName);
    void selectEntity(const QString &name);
    void clearSelection();
    
    // Управление иерархией
    void setParent(const QString &child, const QString &parent);
    void unparent(const QString &child);
    void moveUp(const QString &name);
    void moveDown(const QString &name);
    
    // Получение информации
    QStringList getSelectedEntities() const;
    QString getParent(const QString &name) const;
    QStringList getChildren(const QString &name) const;
    bool hasSelection() const;
    QString getSelectedEntity() const;

signals:
    void selectionChanged(const QStringList &selectedEntities);
    void entityRenamed(const QString &oldName, const QString &newName);
    void entityDeleted(const QString &entityName);
    void entityMoved(const QString &entityName, const QString &newParent);
    void entityCreated(const QString &entityName, const QString &parent);

public slots:
    void onSelectionChanged(const QStringList &selectedEntities);

private slots:
    void onItemSelectionChanged();
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
    void onItemChanged(QTreeWidgetItem *item, int column);
    void onContextMenuRequested(const QPoint &pos);
    void onAddEntity();
    void onDeleteEntity();
    void onRenameEntity();
    void onDuplicateEntity();
    void onMoveUp();
    void onMoveDown();
    void onSetParent();
    void onUnparent();
    void onItemMoved(QTreeWidgetItem *item, int oldIndex, int newIndex);

private:
    void setupUI();
    void setupConnections();
    void setupContextMenu();
    void updateItemIcons();
    
    // Работа с элементами дерева
    QTreeWidgetItem* findItem(const QString &name) const;
    QTreeWidgetItem* createItem(const QString &name, const QString &parent = QString());
    QIcon createSimpleIcon(const QColor &color) const;
    void updateItemText(QTreeWidgetItem *item, const QString &name);
    void updateItemIcon(QTreeWidgetItem *item, const QString &type);
    
    // Обработка drag & drop
    void setupDragAndDrop();
    bool canDrop(QTreeWidgetItem *item, QTreeWidgetItem *parent) const;
    void handleDrop(QTreeWidgetItem *item, QTreeWidgetItem *parent);
    
    // UI компоненты
    QVBoxLayout *m_mainLayout;
    QTreeWidget *m_treeWidget;
    QHBoxLayout *m_buttonLayout;
    
    // Кнопки управления
    QPushButton *m_addButton;
    QPushButton *m_deleteButton;
    QPushButton *m_renameButton;
    QPushButton *m_duplicateButton;
    QPushButton *m_moveUpButton;
    QPushButton *m_moveDownButton;
    
    // Контекстное меню
    QMenu *m_contextMenu;
    QAction *m_addAction;
    QAction *m_deleteAction;
    QAction *m_renameAction;
    QAction *m_duplicateAction;
    QAction *m_moveUpAction;
    QAction *m_moveDownAction;
    QAction *m_setParentAction;
    QAction *m_unparentAction;
    
    // Состояние
    QStringList m_selectedEntities;
    bool m_updating;
    
    // Иконки
    QIcon m_entityIcon;
    QIcon m_folderIcon;
    QIcon m_cameraIcon;
    QIcon m_lightIcon;
    QIcon m_meshIcon;
    QIcon m_spriteIcon;
    QIcon m_audioIcon;
    QIcon m_scriptIcon;
    QIcon m_animationIcon;
    
    // Карта типов объектов
    QMap<QString, QString> m_entityTypes;
};

// Класс для элементов иерархии
class HierarchyItem : public QTreeWidgetItem
{
public:
    explicit HierarchyItem(const QString &name, const QString &type = "Entity", QTreeWidgetItem *parent = nullptr);
    
    // Свойства
    QString getName() const { return m_name; }
    void setName(const QString &name) { m_name = name; setText(0, name); }
    
    QString getType() const { return m_type; }
    void setType(const QString &type) { m_type = type; }
    
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; updateIcon(); }
    
    bool isLocked() const { return m_locked; }
    void setLocked(bool locked) { m_locked = locked; updateIcon(); }
    
    QVariant getData(const QString &key) const { return m_data.value(key); }
    void setData(const QString &key, const QVariant &value) { m_data[key] = value; }
    
    // Переопределение методов
    QVariant data(int column, int role) const override;
    void setData(int column, int role, const QVariant &value) override;

private:
    void updateIcon();
    QIcon getIconForType(const QString &type) const;
    
    QString m_name;
    QString m_type;
    bool m_visible;
    bool m_locked;
    QMap<QString, QVariant> m_data;
};
