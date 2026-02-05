#include "GradientEditor.h"
#include <QPainter>
#include <QColor>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QFileDialog>
#include <QMessageBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColorDialog>
#include <QDebug>
#include <cmath>

// GradientStop implementation
GradientStop::GradientStop(float position, const QColor &color, QObject *parent)
    : QObject(parent)
    , m_position(qBound(0.0f, position, 1.0f))
    , m_color(color)
{
}

GradientStop::~GradientStop()
{
}

void GradientStop::setPosition(float position)
{
    position = qBound(0.0f, position, 1.0f);
    if (m_position != position) {
        m_position = position;
        emit positionChanged(position);
        emit stopChanged();
    }
}

void GradientStop::setColor(const QColor &color)
{
    if (m_color != color) {
        m_color = color;
        emit colorChanged(color);
        emit stopChanged();
    }
}

QJsonObject GradientStop::toJson() const
{
    QJsonObject json;
    json["position"] = m_position;
    json["color"] = m_color.name();
    json["alpha"] = m_color.alpha();
    return json;
}

void GradientStop::fromJson(const QJsonObject &json)
{
    m_position = json["position"].toDouble();
    QString colorName = json["color"].toString();
    int alpha = json["alpha"].toInt(255);
    m_color = QColor(colorName);
    m_color.setAlpha(alpha);
}

// GradientPreset implementation
GradientPreset::GradientPreset(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_type(LinearGradient)
    , m_startPoint(0, 0)
    , m_endPoint(1, 0)
    , m_centerPoint(0.5, 0.5)
    , m_radius(0.5)
    , m_angle(0)
    , m_cachedGradient(nullptr)
    , m_gradientValid(false)
{
    // Add default stops
    addStop(new GradientStop(0.0f, Qt::black, this));
    addStop(new GradientStop(1.0f, Qt::white, this));
}

GradientPreset::~GradientPreset()
{
    qDeleteAll(m_stops);
    delete m_cachedGradient;
}

void GradientPreset::setType(GradientType type)
{
    if (m_type != type) {
        m_type = type;
        m_gradientValid = false;
        emit gradientChanged();
    }
}

void GradientPreset::addStop(GradientStop *stop)
{
    if (!stop) {
        return;
    }
    
    m_stops.append(stop);
    sortStops();
    m_gradientValid = false;
    
    int index = m_stops.indexOf(stop);
    emit stopAdded(index);
    emit gradientChanged();
}

void GradientPreset::removeStop(int index)
{
    if (index < 0 || index >= m_stops.size()) {
        return;
    }
    
    GradientStop *stop = m_stops.takeAt(index);
    delete stop;
    m_gradientValid = false;
    
    emit stopRemoved(index);
    emit gradientChanged();
}

void GradientPreset::removeStop(GradientStop *stop)
{
    int index = m_stops.indexOf(stop);
    if (index >= 0) {
        removeStop(index);
    }
}

GradientStop* GradientPreset::getStop(int index) const
{
    if (index < 0 || index >= m_stops.size()) {
        return nullptr;
    }
    return m_stops[index];
}

int GradientPreset::getStopCount() const
{
    return m_stops.size();
}

void GradientPreset::clearStops()
{
    qDeleteAll(m_stops);
    m_stops.clear();
    m_gradientValid = false;
    emit gradientChanged();
}

QLinearGradient GradientPreset::getLinearGradient() const
{
    QLinearGradient gradient(m_startPoint, m_endPoint);
    
    for (GradientStop *stop : m_stops) {
        gradient.setColorAt(stop->getPosition(), stop->getColor());
    }
    
    return gradient;
}

QRadialGradient GradientPreset::getRadialGradient() const
{
    QRadialGradient gradient(m_centerPoint, m_radius);
    
    for (GradientStop *stop : m_stops) {
        gradient.setColorAt(stop->getPosition(), stop->getColor());
    }
    
    return gradient;
}

QConicalGradient GradientPreset::getConicalGradient() const
{
    QConicalGradient gradient(m_centerPoint, m_angle);
    
    for (GradientStop *stop : m_stops) {
        gradient.setColorAt(stop->getPosition(), stop->getColor());
    }
    
    return gradient;
}

QGradient* GradientPreset::getGradient() const
{
    if (!m_gradientValid || !m_cachedGradient) {
        const_cast<GradientPreset*>(this)->updateGradient();
    }
    return m_cachedGradient;
}

QImage GradientPreset::getPreview(const QSize &size) const
{
    QImage image(size, QImage::Format_ARGB32);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect rect(0, 0, size.width(), size.height());
    applyGradient(painter, rect);
    
    painter.end();
    return image;
}

QImage GradientPreset::getThumbnail(const QSize &size) const
{
    return getPreview(size);
}

QJsonObject GradientPreset::toJson() const
{
    QJsonObject json;
    json["name"] = m_name;
    json["type"] = static_cast<int>(m_type);
    json["startX"] = m_startPoint.x();
    json["startY"] = m_startPoint.y();
    json["endX"] = m_endPoint.x();
    json["endY"] = m_endPoint.y();
    json["centerX"] = m_centerPoint.x();
    json["centerY"] = m_centerPoint.y();
    json["radius"] = m_radius;
    json["angle"] = m_angle;
    
    QJsonArray stopsArray;
    for (GradientStop *stop : m_stops) {
        stopsArray.append(stop->toJson());
    }
    json["stops"] = stopsArray;
    
    return json;
}

void GradientPreset::fromJson(const QJsonObject &json)
{
    m_name = json["name"].toString();
    m_type = static_cast<GradientType>(json["type"].toInt());
    m_startPoint = QPointF(json["startX"].toDouble(), json["startY"].toDouble());
    m_endPoint = QPointF(json["endX"].toDouble(), json["endY"].toDouble());
    m_centerPoint = QPointF(json["centerX"].toDouble(), json["centerY"].toDouble());
    m_radius = json["radius"].toDouble();
    m_angle = json["angle"].toDouble();
    
    clearStops();
    QJsonArray stopsArray = json["stops"].toArray();
    for (const QJsonValue &value : stopsArray) {
        if (value.isObject()) {
            GradientStop *stop = new GradientStop(0.0f, QColor(), this);
            stop->fromJson(value.toObject());
            addStop(stop);
        }
    }
    
    m_gradientValid = false;
    emit gradientChanged();
}

void GradientPreset::applyGradient(QPainter &painter, const QRect &rect) const
{
    QGradient *gradient = getGradient();
    if (!gradient) {
        return;
    }
    
    // Scale gradient to rect
    if (m_type == LinearGradient) {
        QLinearGradient *linearGradient = static_cast<QLinearGradient*>(gradient);
        linearGradient->setStart(rect.topLeft() + QPointF(m_startPoint.x() * rect.width(), m_startPoint.y() * rect.height()));
        linearGradient->setFinalStop(rect.topLeft() + QPointF(m_endPoint.x() * rect.width(), m_endPoint.y() * rect.height()));
    } else if (m_type == RadialGradient) {
        QRadialGradient *radialGradient = static_cast<QRadialGradient*>(gradient);
        radialGradient->setCenter(rect.topLeft() + QPointF(m_centerPoint.x() * rect.width(), m_centerPoint.y() * rect.height()));
        radialGradient->setRadius(qMin(rect.width(), rect.height()) * m_radius);
    } else if (m_type == ConicalGradient) {
        QConicalGradient *conicalGradient = static_cast<QConicalGradient*>(gradient);
        conicalGradient->setCenter(rect.topLeft() + QPointF(m_centerPoint.x() * rect.width(), m_centerPoint.y() * rect.height()));
        conicalGradient->setAngle(m_angle);
    }
    
    painter.setBrush(QBrush(*gradient));
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect);
}

void GradientPreset::sortStops()
{
    std::sort(m_stops.begin(), m_stops.end(), [](GradientStop *a, GradientStop *b) {
        return a->getPosition() < b->getPosition();
    });
}

void GradientPreset::updateGradient()
{
    delete m_cachedGradient;
    m_cachedGradient = nullptr;
    
    switch (m_type) {
    case LinearGradient:
        m_cachedGradient = new QLinearGradient(getLinearGradient());
        break;
    case RadialGradient:
        m_cachedGradient = new QRadialGradient(getRadialGradient());
        break;
    case ConicalGradient:
        m_cachedGradient = new QConicalGradient(getConicalGradient());
        break;
    }
    
    m_gradientValid = true;
}

// GradientEditorWidget implementation
GradientEditorWidget::GradientEditorWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentGradient(nullptr)
{
    setupUI();
}

GradientEditorWidget::~GradientEditorWidget()
{
}

void GradientEditorWidget::setGradient(GradientPreset *gradient)
{
    m_currentGradient = gradient;
    updateGradientProperties();
    updateStopList();
    updatePreview();
}

void GradientEditorWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    // Preview section
    QHBoxLayout *previewLayout = new QHBoxLayout();
    m_previewLabel = new QLabel(this);
    m_previewLabel->setFixedSize(200, 50);
    m_previewLabel->setStyleSheet("border: 1px solid gray; background: white;");
    m_previewLabel->setAlignment(Qt::AlignCenter);
    previewLayout->addWidget(m_previewLabel);
    
    m_gradientInfoLabel = new QLabel("Gradient Info", this);
    m_gradientInfoLabel->setWordWrap(true);
    previewLayout->addWidget(m_gradientInfoLabel);
    
    m_mainLayout->addLayout(previewLayout);
    
    // Gradient type
    m_typeGroup = new QGroupBox("Gradient Type", this);
    m_typeLayout = new QVBoxLayout(m_typeGroup);
    
    m_linearRadio = new QRadioButton("Linear", this);
    m_radialRadio = new QRadioButton("Radial", this);
    m_conicalRadio = new QRadioButton("Conical", this);
    
    m_linearRadio->setChecked(true);
    
    m_typeLayout->addWidget(m_linearRadio);
    m_typeLayout->addWidget(m_radialRadio);
    m_typeLayout->addWidget(m_conicalRadio);
    
    m_mainLayout->addWidget(m_typeGroup);
    
    // Gradient properties
    m_propertiesGroup = new QGroupBox("Gradient Properties", this);
    m_propertiesLayout = new QFormLayout(m_propertiesGroup);
    
    m_startXSpinBox = new QDoubleSpinBox(this);
    m_startXSpinBox->setRange(0.0, 1.0);
    m_startXSpinBox->setSingleStep(0.1);
    m_startXSpinBox->setValue(0.0);
    m_propertiesLayout->addRow("Start X:", m_startXSpinBox);
    
    m_startYSpinBox = new QDoubleSpinBox(this);
    m_startYSpinBox->setRange(0.0, 1.0);
    m_startYSpinBox->setSingleStep(0.1);
    m_startYSpinBox->setValue(0.0);
    m_propertiesLayout->addRow("Start Y:", m_startYSpinBox);
    
    m_endXSpinBox = new QDoubleSpinBox(this);
    m_endXSpinBox->setRange(0.0, 1.0);
    m_endXSpinBox->setSingleStep(0.1);
    m_endXSpinBox->setValue(1.0);
    m_propertiesLayout->addRow("End X:", m_endXSpinBox);
    
    m_endYSpinBox = new QDoubleSpinBox(this);
    m_endYSpinBox->setRange(0.0, 1.0);
    m_endYSpinBox->setSingleStep(0.1);
    m_endYSpinBox->setValue(0.0);
    m_propertiesLayout->addRow("End Y:", m_endYSpinBox);
    
    m_centerXSpinBox = new QDoubleSpinBox(this);
    m_centerXSpinBox->setRange(0.0, 1.0);
    m_centerXSpinBox->setSingleStep(0.1);
    m_centerXSpinBox->setValue(0.5);
    m_propertiesLayout->addRow("Center X:", m_centerXSpinBox);
    
    m_centerYSpinBox = new QDoubleSpinBox(this);
    m_centerYSpinBox->setRange(0.0, 1.0);
    m_centerYSpinBox->setSingleStep(0.1);
    m_centerYSpinBox->setValue(0.5);
    m_propertiesLayout->addRow("Center Y:", m_centerYSpinBox);
    
    m_radiusSpinBox = new QDoubleSpinBox(this);
    m_radiusSpinBox->setRange(0.0, 1.0);
    m_radiusSpinBox->setSingleStep(0.1);
    m_radiusSpinBox->setValue(0.5);
    m_propertiesLayout->addRow("Radius:", m_radiusSpinBox);
    
    m_angleSpinBox = new QDoubleSpinBox(this);
    m_angleSpinBox->setRange(0.0, 360.0);
    m_angleSpinBox->setSingleStep(10.0);
    m_angleSpinBox->setValue(0.0);
    m_propertiesLayout->addRow("Angle:", m_angleSpinBox);
    
    m_mainLayout->addWidget(m_propertiesGroup);
    
    // Stops
    m_stopsGroup = new QGroupBox("Gradient Stops", this);
    m_stopsLayout = new QVBoxLayout(m_stopsGroup);
    
    m_stopsList = new QListWidget(this);
    m_stopsList->setMaximumHeight(100);
    m_stopsLayout->addWidget(m_stopsList);
    
    m_stopsButtonsLayout = new QHBoxLayout();
    m_addStopButton = new QPushButton("Add Stop", this);
    m_removeStopButton = new QPushButton("Remove Stop", this);
    m_stopsButtonsLayout->addWidget(m_addStopButton);
    m_stopsButtonsLayout->addWidget(m_removeStopButton);
    m_stopsButtonsLayout->addStretch();
    
    m_stopsLayout->addLayout(m_stopsButtonsLayout);
    
    m_mainLayout->addWidget(m_stopsGroup);
    
    // Stop properties
    m_stopGroup = new QGroupBox("Stop Properties", this);
    m_stopLayout = new QFormLayout(m_stopGroup);
    
    m_positionSpinBox = new QDoubleSpinBox(this);
    m_positionSpinBox->setRange(0.0, 1.0);
    m_positionSpinBox->setSingleStep(0.01);
    m_positionSpinBox->setDecimals(2);
    m_stopLayout->addRow("Position:", m_positionSpinBox);
    
    m_colorButton = new QPushButton("Choose Color", this);
    m_colorPreview = new QLabel(this);
    m_colorPreview->setFixedSize(30, 30);
    m_colorPreview->setStyleSheet("border: 1px solid gray; background: black;");
    
    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->addWidget(m_colorButton);
    colorLayout->addWidget(m_colorPreview);
    colorLayout->addStretch();
    
    m_stopLayout->addRow("Color:", colorLayout);
    
    m_mainLayout->addWidget(m_stopGroup);
    
    // Actions
    m_actionsGroup = new QGroupBox("Actions", this);
    m_actionsLayout = new QHBoxLayout(m_actionsGroup);
    
    m_saveButton = new QPushButton("Save Gradient", this);
    m_loadButton = new QPushButton("Load Gradient", this);
    m_resetButton = new QPushButton("Reset", this);
    
    m_actionsLayout->addWidget(m_saveButton);
    m_actionsLayout->addWidget(m_loadButton);
    m_actionsLayout->addWidget(m_resetButton);
    
    m_mainLayout->addWidget(m_actionsGroup);
    
    // Connect signals
    connect(m_linearRadio, &QRadioButton::toggled, this, &GradientEditorWidget::onGradientTypeChanged);
    connect(m_radialRadio, &QRadioButton::toggled, this, &GradientEditorWidget::onGradientTypeChanged);
    connect(m_conicalRadio, &QRadioButton::toggled, this, &GradientEditorWidget::onGradientTypeChanged);
    
    connect(m_startXSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GradientEditorWidget::onGradientPropertiesChanged);
    connect(m_startYSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GradientEditorWidget::onGradientPropertiesChanged);
    connect(m_endXSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GradientEditorWidget::onGradientPropertiesChanged);
    connect(m_endYSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GradientEditorWidget::onGradientPropertiesChanged);
    connect(m_centerXSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GradientEditorWidget::onGradientPropertiesChanged);
    connect(m_centerYSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GradientEditorWidget::onGradientPropertiesChanged);
    connect(m_radiusSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GradientEditorWidget::onGradientPropertiesChanged);
    connect(m_angleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GradientEditorWidget::onGradientPropertiesChanged);
    
    connect(m_addStopButton, &QPushButton::clicked, this, &GradientEditorWidget::onStopAdded);
    connect(m_removeStopButton, &QPushButton::clicked, this, &GradientEditorWidget::onStopRemoved);
    connect(m_stopsList, &QListWidget::currentRowChanged, this, &GradientEditorWidget::updateStopControls);
    
    connect(m_positionSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GradientEditorWidget::onStopChanged);
    connect(m_colorButton, &QPushButton::clicked, this, &GradientEditorWidget::onStopChanged);
    
    connect(m_saveButton, &QPushButton::clicked, this, &GradientEditorWidget::onSaveGradient);
    connect(m_loadButton, &QPushButton::clicked, this, &GradientEditorWidget::onLoadGradient);
    connect(m_resetButton, &QPushButton::clicked, this, &GradientEditorWidget::onResetGradient);
}

void GradientEditorWidget::updatePreview()
{
    if (!m_currentGradient) {
        m_previewLabel->setPixmap(QPixmap());
        m_gradientInfoLabel->setText("No gradient selected");
        return;
    }
    
    QImage preview = m_currentGradient->getPreview(QSize(200, 50));
    m_previewLabel->setPixmap(QPixmap::fromImage(preview));
    
    QString info = QString("Name: %1\nType: %2\nStops: %3")
                   .arg(m_currentGradient->getName())
                   .arg(static_cast<int>(m_currentGradient->getType()))
                   .arg(m_currentGradient->getStopCount());
    m_gradientInfoLabel->setText(info);
}

void GradientEditorWidget::updateStopList()
{
    if (!m_currentGradient) {
        m_stopsList->clear();
        return;
    }
    
    m_stopsList->clear();
    for (int i = 0; i < m_currentGradient->getStopCount(); ++i) {
        GradientStop *stop = m_currentGradient->getStop(i);
        QString text = QString("Stop %1: %2% - %3")
                       .arg(i + 1)
                       .arg(static_cast<int>(stop->getPosition() * 100))
                       .arg(stop->getColor().name());
        m_stopsList->addItem(text);
    }
}

void GradientEditorWidget::updateGradientProperties()
{
    if (!m_currentGradient) {
        return;
    }
    
    m_startXSpinBox->setValue(m_currentGradient->getStartPoint().x());
    m_startYSpinBox->setValue(m_currentGradient->getStartPoint().y());
    m_endXSpinBox->setValue(m_currentGradient->getEndPoint().x());
    m_endYSpinBox->setValue(m_currentGradient->getEndPoint().y());
    m_centerXSpinBox->setValue(m_currentGradient->getCenterPoint().x());
    m_centerYSpinBox->setValue(m_currentGradient->getCenterPoint().y());
    m_radiusSpinBox->setValue(m_currentGradient->getRadius());
    m_angleSpinBox->setValue(m_currentGradient->getAngle());
    
    // Update radio buttons
    switch (m_currentGradient->getType()) {
    case GradientPreset::LinearGradient:
        m_linearRadio->setChecked(true);
        break;
    case GradientPreset::RadialGradient:
        m_radialRadio->setChecked(true);
        break;
    case GradientPreset::ConicalGradient:
        m_conicalRadio->setChecked(true);
        break;
    }
}

void GradientEditorWidget::updateStopControls()
{
    if (!m_currentGradient) {
        return;
    }
    
    int currentRow = m_stopsList->currentRow();
    if (currentRow >= 0 && currentRow < m_currentGradient->getStopCount()) {
        GradientStop *stop = m_currentGradient->getStop(currentRow);
        m_positionSpinBox->setValue(stop->getPosition());
        m_colorPreview->setStyleSheet(QString("border: 1px solid gray; background: %1;").arg(stop->getColor().name()));
    }
}

void GradientEditorWidget::onGradientTypeChanged()
{
    if (!m_currentGradient) {
        return;
    }
    
    if (m_linearRadio->isChecked()) {
        m_currentGradient->setType(GradientPreset::LinearGradient);
    } else if (m_radialRadio->isChecked()) {
        m_currentGradient->setType(GradientPreset::RadialGradient);
    } else if (m_conicalRadio->isChecked()) {
        m_currentGradient->setType(GradientPreset::ConicalGradient);
    }
    
    updatePreview();
    emit gradientChanged(m_currentGradient);
}

void GradientEditorWidget::onStopAdded()
{
    if (!m_currentGradient) {
        return;
    }
    
    GradientStop *stop = new GradientStop(0.5f, Qt::gray, m_currentGradient);
    m_currentGradient->addStop(stop);
    updateStopList();
    updatePreview();
    emit gradientChanged(m_currentGradient);
}

void GradientEditorWidget::onStopRemoved()
{
    if (!m_currentGradient) {
        return;
    }
    
    int currentRow = m_stopsList->currentRow();
    if (currentRow >= 0) {
        m_currentGradient->removeStop(currentRow);
        updateStopList();
        updatePreview();
        emit gradientChanged(m_currentGradient);
    }
}

void GradientEditorWidget::onStopChanged()
{
    if (!m_currentGradient) {
        return;
    }
    
    int currentRow = m_stopsList->currentRow();
    if (currentRow >= 0 && currentRow < m_currentGradient->getStopCount()) {
        GradientStop *stop = m_currentGradient->getStop(currentRow);
        stop->setPosition(m_positionSpinBox->value());
        
        // Color selection
        QColor color = QColorDialog::getColor(stop->getColor(), this, "Choose Color");
        if (color.isValid()) {
            stop->setColor(color);
            m_colorPreview->setStyleSheet(QString("border: 1px solid gray; background: %1;").arg(color.name()));
        }
        
        updateStopList();
        updatePreview();
        emit gradientChanged(m_currentGradient);
    }
}

void GradientEditorWidget::onGradientPropertiesChanged()
{
    if (!m_currentGradient) {
        return;
    }
    
    m_currentGradient->setStartPoint(QPointF(m_startXSpinBox->value(), m_startYSpinBox->value()));
    m_currentGradient->setEndPoint(QPointF(m_endXSpinBox->value(), m_endYSpinBox->value()));
    m_currentGradient->setCenterPoint(QPointF(m_centerXSpinBox->value(), m_centerYSpinBox->value()));
    m_currentGradient->setRadius(m_radiusSpinBox->value());
    m_currentGradient->setAngle(m_angleSpinBox->value());
    
    updatePreview();
    emit gradientChanged(m_currentGradient);
}

void GradientEditorWidget::onSaveGradient()
{
    if (!m_currentGradient) {
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this, "Save Gradient", "", "Gradient Files (*.gradient)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(m_currentGradient->toJson());
            file.write(doc.toJson());
        }
    }
}

void GradientEditorWidget::onLoadGradient()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Load Gradient", "", "Gradient Files (*.gradient)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            if (!doc.isNull() && m_currentGradient) {
                m_currentGradient->fromJson(doc.object());
                updateGradientProperties();
                updateStopList();
                updatePreview();
                emit gradientChanged(m_currentGradient);
            }
        }
    }
}

void GradientEditorWidget::onResetGradient()
{
    if (!m_currentGradient) {
        return;
    }
    
    m_currentGradient->clearStops();
    m_currentGradient->addStop(new GradientStop(0.0f, Qt::black, m_currentGradient));
    m_currentGradient->addStop(new GradientStop(1.0f, Qt::white, m_currentGradient));
    
    updateGradientProperties();
    updateStopList();
    updatePreview();
    emit gradientChanged(m_currentGradient);
}

// GradientManager implementation
GradientManager::GradientManager(QObject *parent)
    : QObject(parent)
    , m_gradientLibraryPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/gradients")
{
    QDir().mkpath(m_gradientLibraryPath);
    setupDefaultGradients();
    loadGradientLibrary();
}

GradientManager::~GradientManager()
{
    saveGradientLibrary();
    qDeleteAll(m_gradients);
}

void GradientManager::addGradient(GradientPreset *gradient)
{
    if (!gradient) {
        return;
    }
    
    m_gradients.append(gradient);
    emit gradientAdded(m_gradients.size() - 1);
}

void GradientManager::removeGradient(int index)
{
    if (index < 0 || index >= m_gradients.size()) {
        return;
    }
    
    GradientPreset *gradient = m_gradients.takeAt(index);
    delete gradient;
    emit gradientRemoved(index);
}

void GradientManager::removeGradient(GradientPreset *gradient)
{
    int index = m_gradients.indexOf(gradient);
    if (index >= 0) {
        removeGradient(index);
    }
}

GradientPreset* GradientManager::getGradient(int index) const
{
    if (index < 0 || index >= m_gradients.size()) {
        return nullptr;
    }
    return m_gradients[index];
}

GradientPreset* GradientManager::getGradient(const QString &name) const
{
    for (GradientPreset *gradient : m_gradients) {
        if (gradient->getName() == name) {
            return gradient;
        }
    }
    return nullptr;
}

int GradientManager::getGradientCount() const
{
    return m_gradients.size();
}

int GradientManager::getGradientIndex(GradientPreset *gradient) const
{
    return m_gradients.indexOf(gradient);
}

void GradientManager::createDefaultGradients()
{
    // Basic gradients
    createLinearGradient("Black to White", Qt::black, Qt::white);
    createLinearGradient("Red to Blue", Qt::red, Qt::blue);
    createLinearGradient("Rainbow", Qt::red, Qt::magenta);
    
    createRadialGradient("Radial Black to White", Qt::black, Qt::white);
    createRadialGradient("Radial Red to Blue", Qt::red, Qt::blue);
    
    createConicalGradient("Conical Black to White", Qt::black, Qt::white);
    createConicalGradient("Conical Rainbow", Qt::red, Qt::yellow);
}

GradientPreset* GradientManager::createLinearGradient(const QString &name, const QColor &startColor, const QColor &endColor)
{
    GradientPreset *gradient = new GradientPreset(name, this);
    gradient->setType(GradientPreset::LinearGradient);
    gradient->clearStops();
    gradient->addStop(new GradientStop(0.0f, startColor, gradient));
    gradient->addStop(new GradientStop(1.0f, endColor, gradient));
    addGradient(gradient);
    return gradient;
}

GradientPreset* GradientManager::createRadialGradient(const QString &name, const QColor &centerColor, const QColor &edgeColor)
{
    GradientPreset *gradient = new GradientPreset(name, this);
    gradient->setType(GradientPreset::RadialGradient);
    gradient->clearStops();
    gradient->addStop(new GradientStop(0.0f, centerColor, gradient));
    gradient->addStop(new GradientStop(1.0f, edgeColor, gradient));
    addGradient(gradient);
    return gradient;
}

GradientPreset* GradientManager::createConicalGradient(const QString &name, const QColor &startColor, const QColor &endColor)
{
    GradientPreset *gradient = new GradientPreset(name, this);
    gradient->setType(GradientPreset::ConicalGradient);
    gradient->clearStops();
    gradient->addStop(new GradientStop(0.0f, startColor, gradient));
    gradient->addStop(new GradientStop(1.0f, endColor, gradient));
    addGradient(gradient);
    return gradient;
}

QStringList GradientManager::getCategories() const
{
    return m_categories.keys();
}

QList<GradientPreset*> GradientManager::getGradientsInCategory(const QString &category) const
{
    return m_categories.value(category);
}

void GradientManager::setGradientCategory(GradientPreset *gradient, const QString &category)
{
    if (!gradient) {
        return;
    }
    
    // Remove from old category
    for (auto it = m_categories.begin(); it != m_categories.end(); ++it) {
        it.value().removeAll(gradient);
    }
    
    // Add to new category
    m_categories[category].append(gradient);
    emit gradientCategoryChanged(gradient, category);
}

bool GradientManager::importGradient(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        return false;
    }
    
    GradientPreset *gradient = new GradientPreset("New Gradient", this);
    gradient->fromJson(doc.object());
    addGradient(gradient);
    
    return true;
}

bool GradientManager::exportGradient(GradientPreset *gradient, const QString &filePath)
{
    if (!gradient) {
        return false;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QJsonDocument doc(gradient->toJson());
    file.write(doc.toJson());
    
    return true;
}

bool GradientManager::importGradientSet(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull() || !doc.isArray()) {
        return false;
    }
    
    QJsonArray gradientArray = doc.array();
    for (const QJsonValue &value : gradientArray) {
        if (value.isObject()) {
            GradientPreset *gradient = new GradientPreset("New Gradient", this);
            gradient->fromJson(value.toObject());
            addGradient(gradient);
        }
    }
    
    return true;
}

bool GradientManager::exportGradientSet(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QJsonArray gradientArray;
    for (GradientPreset *gradient : m_gradients) {
        gradientArray.append(gradient->toJson());
    }
    
    QJsonDocument doc(gradientArray);
    file.write(doc.toJson());
    
    return true;
}

void GradientManager::saveGradientPreset(GradientPreset *gradient)
{
    if (!gradient) {
        return;
    }
    
    QString fileName = m_gradientLibraryPath + "/" + gradient->getName() + ".gradient";
    exportGradient(gradient, fileName);
}

void GradientManager::loadGradientPreset(const QString &name)
{
    QString fileName = m_gradientLibraryPath + "/" + name + ".gradient";
    importGradient(fileName);
}

QStringList GradientManager::getAvailablePresets() const
{
    QStringList presets;
    QDir dir(m_gradientLibraryPath);
    QStringList filters;
    filters << "*.gradient";
    
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
    for (const QFileInfo &fileInfo : fileList) {
        presets.append(fileInfo.baseName());
    }
    
    return presets;
}

void GradientManager::setupDefaultGradients()
{
    createDefaultGradients();
}

void GradientManager::loadGradientLibrary()
{
    QStringList presets = getAvailablePresets();
    for (const QString &preset : presets) {
        loadGradientPreset(preset);
    }
}

void GradientManager::saveGradientLibrary()
{
    for (GradientPreset *gradient : m_gradients) {
        saveGradientPreset(gradient);
    }
}

QString GradientManager::getGradientLibraryPath() const
{
    return m_gradientLibraryPath;
}
