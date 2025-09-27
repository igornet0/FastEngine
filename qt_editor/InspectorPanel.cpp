#include "InspectorPanel.h"
#include <QFileDialog>
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

InspectorPanel::InspectorPanel(QWidget *parent)
    : QWidget(parent)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_tabWidget(nullptr)
    , m_transformTab(nullptr)
    , m_renderTab(nullptr)
    , m_physicsTab(nullptr)
    , m_audioTab(nullptr)
    , m_scriptTab(nullptr)
    , m_animationTab(nullptr)
    , m_transformGroup(nullptr)
    , m_renderGroup(nullptr)
    , m_physicsGroup(nullptr)
    , m_audioGroup(nullptr)
    , m_scriptGroup(nullptr)
    , m_animationGroup(nullptr)
    , m_positionX(nullptr)
    , m_positionY(nullptr)
    , m_positionZ(nullptr)
    , m_rotationX(nullptr)
    , m_rotationY(nullptr)
    , m_rotationZ(nullptr)
    , m_scaleX(nullptr)
    , m_scaleY(nullptr)
    , m_scaleZ(nullptr)
    , m_texturePath(nullptr)
    , m_textureButton(nullptr)
    , m_colorDialog(nullptr)
    , m_colorButton(nullptr)
    , m_colorLabel(nullptr)
    , m_opacity(nullptr)
    , m_visible(nullptr)
    , m_blendMode(nullptr)
    , m_hasRigidBody(nullptr)
    , m_mass(nullptr)
    , m_friction(nullptr)
    , m_restitution(nullptr)
    , m_isKinematic(nullptr)
    , m_collisionShape(nullptr)
    , m_audioClipPath(nullptr)
    , m_audioClipButton(nullptr)
    , m_volume(nullptr)
    , m_pitch(nullptr)
    , m_loop(nullptr)
    , m_playOnAwake(nullptr)
    , m_scriptCode(nullptr)
    , m_compileButton(nullptr)
    , m_scriptFunctions(nullptr)
    , m_addFunctionButton(nullptr)
    , m_animationClips(nullptr)
    , m_addAnimationButton(nullptr)
    , m_removeAnimationButton(nullptr)
    , m_animationSpeed(nullptr)
    , m_animationLoop(nullptr)
    , m_renameButton(nullptr)
    , m_deleteButton(nullptr)
    , m_duplicateButton(nullptr)
    , m_entityNameLabel(nullptr)
    , m_entityTypeLabel(nullptr)
    , m_entityIdLabel(nullptr)
    , m_updateTimer(nullptr)
    , m_updating(false)
    , m_hasSelection(false)
{
    setupUI();
    setupConnections();
    
    // Таймер обновления
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &InspectorPanel::updateProperties);
    m_updateTimer->start(100); // Обновление каждые 100мс
}

InspectorPanel::~InspectorPanel()
{
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
}

void InspectorPanel::setupUI()
{
    // Основной макет
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Scroll Area
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // Content Widget
    m_contentWidget = new QWidget();
    m_scrollArea->setWidget(m_contentWidget);
    m_mainLayout->addWidget(m_scrollArea);
    
    // Tab Widget
    m_tabWidget = new QTabWidget(m_contentWidget);
    QVBoxLayout *contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->addWidget(m_tabWidget);
    
    // Создание вкладок
    createPropertyWidgets();
}

void InspectorPanel::setupConnections()
{
    // Соединения для обновления свойств
    if (m_positionX) {
        connect(m_positionX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
        connect(m_positionY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
        connect(m_positionZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
    }
    
    if (m_rotationX) {
        connect(m_rotationX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
        connect(m_rotationY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
        connect(m_rotationZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
    }
    
    if (m_scaleX) {
        connect(m_scaleX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
        connect(m_scaleY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
        connect(m_scaleZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
    }
    
    if (m_visible) {
        connect(m_visible, &QCheckBox::toggled, this, &InspectorPanel::onPropertyChanged);
    }
    
    if (m_opacity) {
        connect(m_opacity, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
    }
}

void InspectorPanel::createPropertyWidgets()
{
    // Transform Tab
    m_transformTab = createTransformWidget();
    m_tabWidget->addTab(m_transformTab, "Transform");
    
    // Render Tab
    m_renderTab = createRenderWidget();
    m_tabWidget->addTab(m_renderTab, "Render");
    
    // Physics Tab
    m_physicsTab = createPhysicsWidget();
    m_tabWidget->addTab(m_physicsTab, "Physics");
    
    // Audio Tab
    m_audioTab = createAudioWidget();
    m_tabWidget->addTab(m_audioTab, "Audio");
    
    // Script Tab
    m_scriptTab = createScriptWidget();
    m_tabWidget->addTab(m_scriptTab, "Script");
    
    // Animation Tab
    m_animationTab = createAnimationWidget();
    m_tabWidget->addTab(m_animationTab, "Animation");
}

QWidget* InspectorPanel::createTransformWidget()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Transform Group
    m_transformGroup = new QGroupBox("Transform");
    QFormLayout *formLayout = new QFormLayout(m_transformGroup);
    
    // Position
    QHBoxLayout *positionLayout = new QHBoxLayout();
    m_positionX = new QDoubleSpinBox();
    m_positionX->setRange(-9999.0, 9999.0);
    m_positionX->setDecimals(2);
    m_positionY = new QDoubleSpinBox();
    m_positionY->setRange(-9999.0, 9999.0);
    m_positionY->setDecimals(2);
    m_positionZ = new QDoubleSpinBox();
    m_positionZ->setRange(-9999.0, 9999.0);
    m_positionZ->setDecimals(2);
    
    positionLayout->addWidget(new QLabel("X:"));
    positionLayout->addWidget(m_positionX);
    positionLayout->addWidget(new QLabel("Y:"));
    positionLayout->addWidget(m_positionY);
    positionLayout->addWidget(new QLabel("Z:"));
    positionLayout->addWidget(m_positionZ);
    
    formLayout->addRow("Position:", positionLayout);
    
    // Rotation
    QHBoxLayout *rotationLayout = new QHBoxLayout();
    m_rotationX = new QDoubleSpinBox();
    m_rotationX->setRange(-360.0, 360.0);
    m_rotationX->setDecimals(1);
    m_rotationY = new QDoubleSpinBox();
    m_rotationY->setRange(-360.0, 360.0);
    m_rotationY->setDecimals(1);
    m_rotationZ = new QDoubleSpinBox();
    m_rotationZ->setRange(-360.0, 360.0);
    m_rotationZ->setDecimals(1);
    
    rotationLayout->addWidget(new QLabel("X:"));
    rotationLayout->addWidget(m_rotationX);
    rotationLayout->addWidget(new QLabel("Y:"));
    rotationLayout->addWidget(m_rotationY);
    rotationLayout->addWidget(new QLabel("Z:"));
    rotationLayout->addWidget(m_rotationZ);
    
    formLayout->addRow("Rotation:", rotationLayout);
    
    // Scale
    QHBoxLayout *scaleLayout = new QHBoxLayout();
    m_scaleX = new QDoubleSpinBox();
    m_scaleX->setRange(0.01, 100.0);
    m_scaleX->setDecimals(2);
    m_scaleX->setValue(1.0);
    m_scaleY = new QDoubleSpinBox();
    m_scaleY->setRange(0.01, 100.0);
    m_scaleY->setDecimals(2);
    m_scaleY->setValue(1.0);
    m_scaleZ = new QDoubleSpinBox();
    m_scaleZ->setRange(0.01, 100.0);
    m_scaleZ->setDecimals(2);
    m_scaleZ->setValue(1.0);
    
    scaleLayout->addWidget(new QLabel("X:"));
    scaleLayout->addWidget(m_scaleX);
    scaleLayout->addWidget(new QLabel("Y:"));
    scaleLayout->addWidget(m_scaleY);
    scaleLayout->addWidget(new QLabel("Z:"));
    scaleLayout->addWidget(m_scaleZ);
    
    formLayout->addRow("Scale:", scaleLayout);
    
    layout->addWidget(m_transformGroup);
    layout->addStretch();
    
    return widget;
}

QWidget* InspectorPanel::createRenderWidget()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Render Group
    m_renderGroup = new QGroupBox("Render");
    QFormLayout *formLayout = new QFormLayout(m_renderGroup);
    
    // Texture
    QHBoxLayout *textureLayout = new QHBoxLayout();
    m_texturePath = new QLineEdit();
    m_textureButton = new QPushButton("...");
    textureLayout->addWidget(m_texturePath);
    textureLayout->addWidget(m_textureButton);
    formLayout->addRow("Texture:", textureLayout);
    
    // Color
    QHBoxLayout *colorLayout = new QHBoxLayout();
    m_colorButton = new QPushButton();
    m_colorButton->setFixedSize(30, 30);
    m_colorLabel = new QLabel("Color");
    colorLayout->addWidget(m_colorButton);
    colorLayout->addWidget(m_colorLabel);
    formLayout->addRow("Color:", colorLayout);
    
    // Opacity
    m_opacity = new QDoubleSpinBox();
    m_opacity->setRange(0.0, 1.0);
    m_opacity->setDecimals(2);
    m_opacity->setValue(1.0);
    formLayout->addRow("Opacity:", m_opacity);
    
    // Visible
    m_visible = new QCheckBox();
    m_visible->setChecked(true);
    formLayout->addRow("Visible:", m_visible);
    
    // Blend Mode
    m_blendMode = new QComboBox();
    m_blendMode->addItems({"Normal", "Add", "Multiply", "Screen"});
    formLayout->addRow("Blend Mode:", m_blendMode);
    
    layout->addWidget(m_renderGroup);
    layout->addStretch();
    
    return widget;
}

QWidget* InspectorPanel::createPhysicsWidget()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Physics Group
    m_physicsGroup = new QGroupBox("Physics");
    QFormLayout *formLayout = new QFormLayout(m_physicsGroup);
    
    // Has Rigid Body
    m_hasRigidBody = new QCheckBox();
    formLayout->addRow("Has Rigid Body:", m_hasRigidBody);
    
    // Mass
    m_mass = new QDoubleSpinBox();
    m_mass->setRange(0.01, 1000.0);
    m_mass->setDecimals(2);
    m_mass->setValue(1.0);
    formLayout->addRow("Mass:", m_mass);
    
    // Friction
    m_friction = new QDoubleSpinBox();
    m_friction->setRange(0.0, 1.0);
    m_friction->setDecimals(2);
    m_friction->setValue(0.5);
    formLayout->addRow("Friction:", m_friction);
    
    // Restitution
    m_restitution = new QDoubleSpinBox();
    m_restitution->setRange(0.0, 1.0);
    m_restitution->setDecimals(2);
    m_restitution->setValue(0.0);
    formLayout->addRow("Restitution:", m_restitution);
    
    // Is Kinematic
    m_isKinematic = new QCheckBox();
    formLayout->addRow("Is Kinematic:", m_isKinematic);
    
    // Collision Shape
    m_collisionShape = new QComboBox();
    m_collisionShape->addItems({"Box", "Sphere", "Capsule", "Mesh"});
    formLayout->addRow("Collision Shape:", m_collisionShape);
    
    layout->addWidget(m_physicsGroup);
    layout->addStretch();
    
    return widget;
}

QWidget* InspectorPanel::createAudioWidget()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Audio Group
    m_audioGroup = new QGroupBox("Audio");
    QFormLayout *formLayout = new QFormLayout(m_audioGroup);
    
    // Audio Clip
    QHBoxLayout *audioLayout = new QHBoxLayout();
    m_audioClipPath = new QLineEdit();
    m_audioClipButton = new QPushButton("...");
    audioLayout->addWidget(m_audioClipPath);
    audioLayout->addWidget(m_audioClipButton);
    formLayout->addRow("Audio Clip:", audioLayout);
    
    // Volume
    m_volume = new QDoubleSpinBox();
    m_volume->setRange(0.0, 1.0);
    m_volume->setDecimals(2);
    m_volume->setValue(1.0);
    formLayout->addRow("Volume:", m_volume);
    
    // Pitch
    m_pitch = new QDoubleSpinBox();
    m_pitch->setRange(0.1, 3.0);
    m_pitch->setDecimals(2);
    m_pitch->setValue(1.0);
    formLayout->addRow("Pitch:", m_pitch);
    
    // Loop
    m_loop = new QCheckBox();
    formLayout->addRow("Loop:", m_loop);
    
    // Play On Awake
    m_playOnAwake = new QCheckBox();
    m_playOnAwake->setChecked(true);
    formLayout->addRow("Play On Awake:", m_playOnAwake);
    
    layout->addWidget(m_audioGroup);
    layout->addStretch();
    
    return widget;
}

QWidget* InspectorPanel::createScriptWidget()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Script Group
    m_scriptGroup = new QGroupBox("Script");
    QVBoxLayout *scriptLayout = new QVBoxLayout(m_scriptGroup);
    
    // Script Code
    m_scriptCode = new QTextEdit();
    m_scriptCode->setMaximumHeight(200);
    scriptLayout->addWidget(m_scriptCode);
    
    // Compile Button
    m_compileButton = new QPushButton("Compile");
    scriptLayout->addWidget(m_compileButton);
    
    // Script Functions
    QLabel *functionsLabel = new QLabel("Functions:");
    scriptLayout->addWidget(functionsLabel);
    
    m_scriptFunctions = new QListWidget();
    m_scriptFunctions->setMaximumHeight(100);
    scriptLayout->addWidget(m_scriptFunctions);
    
    // Add Function Button
    m_addFunctionButton = new QPushButton("Add Function");
    scriptLayout->addWidget(m_addFunctionButton);
    
    layout->addWidget(m_scriptGroup);
    layout->addStretch();
    
    return widget;
}

QWidget* InspectorPanel::createAnimationWidget()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Animation Group
    m_animationGroup = new QGroupBox("Animation");
    QVBoxLayout *animationLayout = new QVBoxLayout(m_animationGroup);
    
    // Animation Clips
    QLabel *clipsLabel = new QLabel("Animation Clips:");
    animationLayout->addWidget(clipsLabel);
    
    m_animationClips = new QListWidget();
    m_animationClips->setMaximumHeight(150);
    animationLayout->addWidget(m_animationClips);
    
    // Animation Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_addAnimationButton = new QPushButton("Add");
    m_removeAnimationButton = new QPushButton("Remove");
    buttonLayout->addWidget(m_addAnimationButton);
    buttonLayout->addWidget(m_removeAnimationButton);
    animationLayout->addLayout(buttonLayout);
    
    // Animation Speed
    m_animationSpeed = new QDoubleSpinBox();
    m_animationSpeed->setRange(0.1, 5.0);
    m_animationSpeed->setDecimals(1);
    m_animationSpeed->setValue(1.0);
    animationLayout->addWidget(new QLabel("Speed:"));
    animationLayout->addWidget(m_animationSpeed);
    
    // Animation Loop
    m_animationLoop = new QCheckBox("Loop");
    animationLayout->addWidget(m_animationLoop);
    
    layout->addWidget(m_animationGroup);
    layout->addStretch();
    
    return widget;
}

void InspectorPanel::setSelectedEntity(const QString &entityName)
{
    m_selectedEntity = entityName;
    m_hasSelection = !entityName.isEmpty();
    updatePropertyWidgets();
}

void InspectorPanel::clearSelection()
{
    m_selectedEntity.clear();
    m_hasSelection = false;
    updatePropertyWidgets();
}

void InspectorPanel::updateProperties()
{
    if (m_hasSelection && !m_updating) {
        // Обновляем свойства выбранного объекта
        updatePropertyWidgets();
    }
}

void InspectorPanel::updatePropertyWidgets()
{
    m_updating = true;
    
    if (m_hasSelection) {
        // Обновляем виджеты с данными выбранного объекта
        // Здесь должна быть логика получения данных объекта из FastEngine
        
        // Обновляем информацию об объекте
        if (m_entityNameLabel) {
            m_entityNameLabel->setText("Name: " + m_selectedEntity);
        }
        if (m_entityTypeLabel) {
            m_entityTypeLabel->setText("Type: Entity");
        }
        if (m_entityIdLabel) {
            m_entityIdLabel->setText("ID: " + QString::number(qHash(m_selectedEntity)));
        }
    } else {
        // Очищаем виджеты
        if (m_entityNameLabel) {
            m_entityNameLabel->setText("No object selected");
        }
        if (m_entityTypeLabel) {
            m_entityTypeLabel->setText("");
        }
        if (m_entityIdLabel) {
            m_entityIdLabel->setText("");
        }
    }
    
    m_updating = false;
}

void InspectorPanel::clearPropertyWidgets()
{
    // Очищаем все виджеты свойств
    if (m_positionX) m_positionX->setValue(0.0);
    if (m_positionY) m_positionY->setValue(0.0);
    if (m_positionZ) m_positionZ->setValue(0.0);
    if (m_rotationX) m_rotationX->setValue(0.0);
    if (m_rotationY) m_rotationY->setValue(0.0);
    if (m_rotationZ) m_rotationZ->setValue(0.0);
    if (m_scaleX) m_scaleX->setValue(1.0);
    if (m_scaleY) m_scaleY->setValue(1.0);
    if (m_scaleZ) m_scaleZ->setValue(1.0);
    if (m_visible) m_visible->setChecked(true);
    if (m_opacity) m_opacity->setValue(1.0);
}

QString InspectorPanel::getSelectedEntity() const
{
    return m_selectedEntity;
}

bool InspectorPanel::hasSelection() const
{
    return m_hasSelection;
}

void InspectorPanel::onPropertyChanged()
{
    if (m_updating || !m_hasSelection) {
        return;
    }
    
    // Отправляем сигнал об изменении свойства
    // Здесь должна быть логика обновления объекта в FastEngine
    
    qDebug() << "Property changed for entity:" << m_selectedEntity;
}


void InspectorPanel::onEntityRenamed()
{
    // TODO: Реализовать переименование объекта
}

void InspectorPanel::onDeleteEntity()
{
    // TODO: Реализовать удаление объекта
}

void InspectorPanel::onAddComponent()
{
    // TODO: Реализовать добавление компонента
}

void InspectorPanel::onRemoveComponent()
{
    // TODO: Реализовать удаление компонента
}

void InspectorPanel::addProperty(const QString &name, const QVariant &value, const QString &type)
{
    // TODO: Реализовать добавление динамического свойства
}

void InspectorPanel::updateProperty(const QString &name, const QVariant &value)
{
    // TODO: Реализовать обновление свойства
}

void InspectorPanel::removeProperty(const QString &name)
{
    // TODO: Реализовать удаление свойства
}

void InspectorPanel::onPropertyDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (!item || column != 1) return;
    
    // Обработка двойного клика по свойству
    QString propertyName = item->text(0);
    QString propertyValue = item->text(1);
    
    // Здесь можно добавить специальную обработку для разных типов свойств
    if (propertyName.contains("Color")) {
        // Открыть диалог выбора цвета
        QColor currentColor = QColor(propertyValue);
        QColor newColor = QColorDialog::getColor(currentColor, this, "Select Color");
        if (newColor.isValid()) {
            item->setText(1, newColor.name());
            onPropertyChanged();
        }
    } else if (propertyName.contains("File") || propertyName.contains("Path")) {
        // Открыть диалог выбора файла
        QString fileName = QFileDialog::getOpenFileName(this, "Select File", "", "All Files (*)");
        if (!fileName.isEmpty()) {
            item->setText(1, fileName);
            onPropertyChanged();
        }
    }
}

void InspectorPanel::onPropertySelectionChanged()
{
    // Обработка изменения выбора свойства
    // Временно заглушено из-за отсутствия m_propertyWidget
    // Здесь можно добавить дополнительную информацию о свойстве
    // Например, показать подсказку или описание
}

// PropertyEditor implementation
PropertyEditor::PropertyEditor(const QString &name, const QVariant &value, QWidget *parent)
    : QWidget(parent), m_propertyName(name), m_value(value)
{
    setupUI();
}

QVariant PropertyEditor::getValue() const
{
    return m_value;
}

void PropertyEditor::setValue(const QVariant &value)
{
    m_value = value;
    // Update widget
}

void PropertyEditor::onValueChanged()
{
    emit valueChanged(m_propertyName, m_value);
}

void PropertyEditor::setupUI()
{
    m_layout = new QHBoxLayout(this);
    m_nameLabel = new QLabel(m_propertyName, this);
    m_valueWidget = createWidgetForType(m_value);
    
    m_layout->addWidget(m_nameLabel);
    m_layout->addWidget(m_valueWidget);
}

QWidget* PropertyEditor::createWidgetForType(const QVariant &value)
{
    switch (value.typeId()) {
        case QMetaType::Bool:
            return new QCheckBox(this);
        case QMetaType::Int:
            return new QSpinBox(this);
        case QMetaType::Double:
            return new QDoubleSpinBox(this);
        case QMetaType::QString:
        default:
            return new QLineEdit(this);
    }
}

// Vector3Editor implementation  
Vector3Editor::Vector3Editor(const QString &name, const QVector3D &value, QWidget *parent)
    : QWidget(parent), m_name(name), m_value(value)
{
    setupUI();
}

QVector3D Vector3Editor::getValue() const
{
    return m_value;
}

void Vector3Editor::setValue(const QVector3D &value)
{
    m_value = value;
    m_xSpinBox->setValue(value.x());
    m_ySpinBox->setValue(value.y());
    m_zSpinBox->setValue(value.z());
}

void Vector3Editor::onValueChanged()
{
    m_value.setX(m_xSpinBox->value());
    m_value.setY(m_ySpinBox->value());
    m_value.setZ(m_zSpinBox->value());
    emit valueChanged(m_value);
}

void Vector3Editor::setupUI()
{
    m_layout = new QHBoxLayout(this);
    m_nameLabel = new QLabel(m_name, this);
    m_xSpinBox = new QDoubleSpinBox(this);
    m_ySpinBox = new QDoubleSpinBox(this);
    m_zSpinBox = new QDoubleSpinBox(this);
    
    m_layout->addWidget(m_nameLabel);
    m_layout->addWidget(m_xSpinBox);
    m_layout->addWidget(m_ySpinBox);
    m_layout->addWidget(m_zSpinBox);
    
    connect(m_xSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Vector3Editor::onValueChanged);
    connect(m_ySpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Vector3Editor::onValueChanged);
    connect(m_zSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Vector3Editor::onValueChanged);
}

// ColorEditor implementation
ColorEditor::ColorEditor(const QString &name, const QColor &value, QWidget *parent)
    : QWidget(parent), m_name(name), m_value(value)
{
    setupUI();
}

QColor ColorEditor::getValue() const
{
    return m_value;
}

void ColorEditor::setValue(const QColor &value)
{
    m_value = value;
    updateColorButton();
}

void ColorEditor::onColorButtonClicked()
{
    QColor newColor = QColorDialog::getColor(m_value, this);
    if (newColor.isValid()) {
        setValue(newColor);
        emit valueChanged(m_value);
    }
}

void ColorEditor::onColorChanged(const QColor &color)
{
    m_value = color;
    updateColorButton();
    emit valueChanged(m_value);
}

void ColorEditor::setupUI()
{
    m_layout = new QHBoxLayout(this);
    m_nameLabel = new QLabel(m_name, this);
    m_colorButton = new QPushButton(this);
    
    m_layout->addWidget(m_nameLabel);
    m_layout->addWidget(m_colorButton);
    
    connect(m_colorButton, &QPushButton::clicked, this, &ColorEditor::onColorButtonClicked);
    
    updateColorButton();
}

void ColorEditor::updateColorButton()
{
    m_colorButton->setStyleSheet(QString("background-color: %1").arg(m_value.name()));
}

void InspectorPanel::onSelectionChanged(const QStringList &selectedEntities)
{
    // Обработка изменения выбора сущностей
    if (selectedEntities.isEmpty()) {
        m_selectedEntity = QString();
        m_hasSelection = false;
        clearPropertyWidgets();
    } else {
        m_selectedEntity = selectedEntities.first();
        m_hasSelection = true;
        updatePropertyWidgets();
    }
}
