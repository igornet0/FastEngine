#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QColorDialog>
#include <QSlider>
#include <QTextEdit>
#include <QTabWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QFrame>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QFont>
#include <QPalette>
#include <QColor>
#include <QVector3D>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaType>
#include <QObject>
#include <QSignalMapper>
#include <QTimer>
#include <QApplication>
#include <QDebug>

class InspectorPanel : public QWidget
{
    Q_OBJECT

public:
    explicit InspectorPanel(QWidget *parent = nullptr);
    ~InspectorPanel();

    // Управление объектами
    void setSelectedEntity(const QString &entityName);
    void clearSelection();
    void updateProperties();
    
    // Получение информации
    QString getSelectedEntity() const;
    bool hasSelection() const;

signals:
    void propertyChanged(const QString &propertyName, const QVariant &value);
    void entityRenamed(const QString &oldName, const QString &newName);
    void entityDeleted(const QString &entityName);

public slots:
    void onSelectionChanged(const QStringList &selectedEntities);

private slots:
    void onPropertyChanged();
    void onEntityRenamed();
    void onDeleteEntity();
    void onAddComponent();
    void onRemoveComponent();
    void onPropertyDoubleClicked(QTreeWidgetItem *item, int column);
    void onPropertySelectionChanged();

private:
    void setupUI();
    void setupConnections();
    void createPropertyWidgets();
    void updatePropertyWidgets();
    void clearPropertyWidgets();
    
    // Создание виджетов свойств
    QWidget* createTransformWidget();
    QWidget* createRenderWidget();
    QWidget* createPhysicsWidget();
    QWidget* createAudioWidget();
    QWidget* createScriptWidget();
    QWidget* createAnimationWidget();
    
    // Обработка свойств
    void addProperty(const QString &name, const QVariant &value, const QString &type);
    void updateProperty(const QString &name, const QVariant &value);
    void removeProperty(const QString &name);
    
    // UI компоненты
    QScrollArea *m_scrollArea;
    QWidget *m_contentWidget;
    QVBoxLayout *m_mainLayout;
    QTabWidget *m_tabWidget;
    
    // Вкладки
    QWidget *m_transformTab;
    QWidget *m_renderTab;
    QWidget *m_physicsTab;
    QWidget *m_audioTab;
    QWidget *m_scriptTab;
    QWidget *m_animationTab;
    
    // Виджеты свойств
    QMap<QString, QWidget*> m_propertyWidgets;
    QMap<QString, QVariant> m_propertyValues;
    
    // Текущий выбранный объект
    QString m_selectedEntity;
    
    // Группы свойств
    QGroupBox *m_transformGroup;
    QGroupBox *m_renderGroup;
    QGroupBox *m_physicsGroup;
    QGroupBox *m_audioGroup;
    QGroupBox *m_scriptGroup;
    QGroupBox *m_animationGroup;
    
    // Свойства Transform
    QDoubleSpinBox *m_positionX;
    QDoubleSpinBox *m_positionY;
    QDoubleSpinBox *m_positionZ;
    QDoubleSpinBox *m_rotationX;
    QDoubleSpinBox *m_rotationY;
    QDoubleSpinBox *m_rotationZ;
    QDoubleSpinBox *m_scaleX;
    QDoubleSpinBox *m_scaleY;
    QDoubleSpinBox *m_scaleZ;
    
    // Свойства Render
    QLineEdit *m_texturePath;
    QPushButton *m_textureButton;
    QColorDialog *m_colorDialog;
    QPushButton *m_colorButton;
    QLabel *m_colorLabel;
    QDoubleSpinBox *m_opacity;
    QCheckBox *m_visible;
    QComboBox *m_blendMode;
    
    // Свойства Physics
    QCheckBox *m_hasRigidBody;
    QDoubleSpinBox *m_mass;
    QDoubleSpinBox *m_friction;
    QDoubleSpinBox *m_restitution;
    QCheckBox *m_isKinematic;
    QComboBox *m_collisionShape;
    
    // Свойства Audio
    QLineEdit *m_audioClipPath;
    QPushButton *m_audioClipButton;
    QDoubleSpinBox *m_volume;
    QDoubleSpinBox *m_pitch;
    QCheckBox *m_loop;
    QCheckBox *m_playOnAwake;
    
    // Свойства Script
    QTextEdit *m_scriptCode;
    QPushButton *m_compileButton;
    QListWidget *m_scriptFunctions;
    QPushButton *m_addFunctionButton;
    
    // Свойства Animation
    QListWidget *m_animationClips;
    QPushButton *m_addAnimationButton;
    QPushButton *m_removeAnimationButton;
    QDoubleSpinBox *m_animationSpeed;
    QCheckBox *m_animationLoop;
    
    // Кнопки управления
    QPushButton *m_renameButton;
    QPushButton *m_deleteButton;
    QPushButton *m_duplicateButton;
    
    // Информация об объекте
    QLabel *m_entityNameLabel;
    QLabel *m_entityTypeLabel;
    QLabel *m_entityIdLabel;
    
    // Таймер обновления
    QTimer *m_updateTimer;
    
    // Состояние
    bool m_updating;
    bool m_hasSelection;
};

// Класс для редактирования свойств
class PropertyEditor : public QWidget
{
    Q_OBJECT

public:
    explicit PropertyEditor(const QString &name, const QVariant &value, QWidget *parent = nullptr);
    
    QVariant getValue() const;
    void setValue(const QVariant &value);
    QString getPropertyName() const { return m_propertyName; }

signals:
    void valueChanged(const QString &propertyName, const QVariant &value);

private slots:
    void onValueChanged();

private:
    void setupUI();
    QWidget* createWidgetForType(const QVariant &value);
    
    QString m_propertyName;
    QVariant m_value;
    QWidget *m_valueWidget;
    QLabel *m_nameLabel;
    QHBoxLayout *m_layout;
};

// Класс для редактирования векторов
class Vector3Editor : public QWidget
{
    Q_OBJECT

public:
    explicit Vector3Editor(const QString &name, const QVector3D &value, QWidget *parent = nullptr);
    
    QVector3D getValue() const;
    void setValue(const QVector3D &value);

signals:
    void valueChanged(const QVector3D &value);

private slots:
    void onValueChanged();

private:
    void setupUI();
    
    QString m_name;
    QVector3D m_value;
    QDoubleSpinBox *m_xSpinBox;
    QDoubleSpinBox *m_ySpinBox;
    QDoubleSpinBox *m_zSpinBox;
    QLabel *m_nameLabel;
    QHBoxLayout *m_layout;
};

// Класс для редактирования цветов
class ColorEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ColorEditor(const QString &name, const QColor &value, QWidget *parent = nullptr);
    
    QColor getValue() const;
    void setValue(const QColor &value);

signals:
    void valueChanged(const QColor &value);

private slots:
    void onColorButtonClicked();
    void onColorChanged(const QColor &color);

private:
    void setupUI();
    void updateColorButton();
    
    QString m_name;
    QColor m_value;
    QPushButton *m_colorButton;
    QLabel *m_nameLabel;
    QHBoxLayout *m_layout;
};
