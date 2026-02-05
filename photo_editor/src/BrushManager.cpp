#include "BrushManager.h"
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
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QBuffer>
#include <cmath>
#include <cstdlib>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

// BrushPreset implementation
BrushPreset::BrushPreset(QObject *parent)
    : QObject(parent)
    , m_name("Default Brush")
    , m_type(BasicBrush)
    , m_size(20)
    , m_opacity(1.0f)
    , m_hardness(1.0f)
    , m_spacing(0.25f)
    , m_flow(1.0f)
    , m_color(Qt::black)
    , m_pressureSensitive(false)
    , m_scattering(false)
    , m_scatterAmount(0.0f)
    , m_rotationEnabled(false)
    , m_previewValid(false)
    , m_thumbnailValid(false)
{
    generateBrushMask();
}

BrushPreset::BrushPreset(const QString &name, BrushType type, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_type(type)
    , m_size(20)
    , m_opacity(1.0f)
    , m_hardness(1.0f)
    , m_spacing(0.25f)
    , m_flow(1.0f)
    , m_pressureSensitive(true)
    , m_scattering(false)
    , m_scatterAmount(0.0f)
    , m_rotationEnabled(false)
    , m_previewValid(false)
    , m_thumbnailValid(false)
{
    generateBrushMask();
}

BrushPreset::~BrushPreset()
{
}

QImage BrushPreset::getPreview(int size) const
{
    if (!m_previewValid || m_preview.size() != QSize(size, size)) {
        m_preview = QImage(size, size, QImage::Format_ARGB32);
        m_preview.fill(Qt::transparent);
        
        QPainter painter(&m_preview);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Draw brush preview
        QRect previewRect(0, 0, size, size);
        QPoint center = previewRect.center();
        
        if (m_type == BasicBrush) {
            // Draw circular brush
            int brushSize = qMin(size * 0.8f, static_cast<float>(m_size));
            QRect brushRect(center.x() - brushSize/2, center.y() - brushSize/2, brushSize, brushSize);
            
            QRadialGradient gradient(center, brushSize/2);
            gradient.setColorAt(0, QColor(0, 0, 0, 255 * m_opacity));
            gradient.setColorAt(m_hardness, QColor(0, 0, 0, 255 * m_opacity));
            gradient.setColorAt(1, QColor(0, 0, 0, 0));
            
            painter.setBrush(QBrush(gradient));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(brushRect);
        } else if (m_type == TextureBrush || m_type == ImageBrush) {
            // Draw texture brush
            if (!m_texture.isNull()) {
                QRect textureRect = m_texture.rect();
                textureRect.moveCenter(center);
                painter.setOpacity(m_opacity);
                painter.drawImage(textureRect, m_texture);
            }
        }
        
        painter.end();
        m_previewValid = true;
    }
    
    return m_preview;
}

QImage BrushPreset::getThumbnail(int size) const
{
    if (!m_thumbnailValid || m_thumbnail.size() != QSize(size, size)) {
        m_thumbnail = getPreview(size);
        m_thumbnailValid = true;
    }
    
    return m_thumbnail;
}

QJsonObject BrushPreset::toJson() const
{
    QJsonObject json;
    json["name"] = m_name;
    json["type"] = static_cast<int>(m_type);
    json["size"] = m_size;
    json["opacity"] = m_opacity;
    json["hardness"] = m_hardness;
    json["spacing"] = m_spacing;
    json["flow"] = m_flow;
    json["pressureSensitive"] = m_pressureSensitive;
    json["scattering"] = m_scattering;
    json["scatterAmount"] = m_scatterAmount;
    json["rotationEnabled"] = m_rotationEnabled;
    
    // Save texture as base64
    if (!m_texture.isNull()) {
        QByteArray textureData;
        QBuffer buffer(&textureData);
        buffer.open(QIODevice::WriteOnly);
        m_texture.save(&buffer, "PNG");
        json["texture"] = QString::fromLatin1(textureData.toBase64());
    }
    
    return json;
}

void BrushPreset::fromJson(const QJsonObject &json)
{
    m_name = json["name"].toString();
    m_type = static_cast<BrushType>(json["type"].toInt());
    m_size = json["size"].toInt();
    m_opacity = json["opacity"].toDouble();
    m_hardness = json["hardness"].toDouble();
    m_spacing = json["spacing"].toDouble();
    m_flow = json["flow"].toDouble();
    m_pressureSensitive = json["pressureSensitive"].toBool();
    m_scattering = json["scattering"].toBool();
    m_scatterAmount = json["scatterAmount"].toDouble();
    m_rotationEnabled = json["rotationEnabled"].toBool();
    
    // Load texture from base64
    if (json.contains("texture")) {
        QByteArray textureData = QByteArray::fromBase64(json["texture"].toString().toLatin1());
        m_texture.loadFromData(textureData, "PNG");
    }
    
    generateBrushMask();
    m_previewValid = false;
    m_thumbnailValid = false;
    emit brushChanged();
}

void BrushPreset::applyBrush(QPainter &painter, const QPoint &point, const QColor &color, float pressure) const
{
    if (m_brushMask.isNull()) {
        return;
    }
    
    float actualOpacity = m_opacity * (m_pressureSensitive ? pressure : 1.0f);
    QColor brushColor = color;
    brushColor.setAlphaF(actualOpacity);
    
    painter.setOpacity(actualOpacity);
    
    if (m_type == BasicBrush) {
        // Apply circular brush
        QRadialGradient gradient(point, m_size / 2.0f);
        gradient.setColorAt(0, brushColor);
        gradient.setColorAt(m_hardness, brushColor);
        gradient.setColorAt(1, QColor(brushColor.red(), brushColor.green(), brushColor.blue(), 0));
        
        painter.setBrush(QBrush(gradient));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(point, m_size / 2, m_size / 2);
    } else if (m_type == TextureBrush || m_type == ImageBrush) {
        // Apply texture brush
        if (!m_texture.isNull()) {
            QRect textureRect = m_texture.rect();
            textureRect.moveCenter(point);
            painter.drawImage(textureRect, m_texture);
        }
    }
}

void BrushPreset::applyBrushStroke(QPainter &painter, const QPoint &from, const QPoint &to, const QColor &color, float pressure) const
{
    if (m_brushMask.isNull()) {
        return;
    }
    
    float actualOpacity = m_opacity * (m_pressureSensitive ? pressure : 1.0f);
    QColor brushColor = color;
    brushColor.setAlphaF(actualOpacity);
    
    painter.setOpacity(actualOpacity);
    
    // Calculate stroke points based on spacing
    float distance = QLineF(from, to).length();
    int steps = static_cast<int>(distance / (m_size * m_spacing));
    
    if (steps == 0) {
        applyBrush(painter, to, color, pressure);
        return;
    }
    
    for (int i = 0; i <= steps; ++i) {
        float t = static_cast<float>(i) / steps;
        QPoint point = QPoint(
            static_cast<int>(from.x() + t * (to.x() - from.x())),
            static_cast<int>(from.y() + t * (to.y() - from.y()))
        );
        
        // Apply scatter if enabled
        if (m_scattering) {
            QPoint scatterOffset = calculateScatterOffset();
            point += scatterOffset;
        }
        
        // Apply rotation if enabled
        if (m_rotationEnabled) {
            float rotation = calculateRotation();
            painter.save();
            painter.translate(point);
            painter.rotate(rotation);
            painter.translate(-point);
        }
        
        applyBrush(painter, point, color, pressure);
        
        if (m_rotationEnabled) {
            painter.restore();
        }
    }
}

void BrushPreset::generateBrushMask()
{
    if (m_type == BasicBrush) {
        m_brushMask = QImage(m_size, m_size, QImage::Format_ARGB32);
        m_brushMask.fill(Qt::transparent);
        
        QPainter painter(&m_brushMask);
        painter.setRenderHint(QPainter::Antialiasing);
        
        QRadialGradient gradient(m_size/2, m_size/2, m_size/2);
        gradient.setColorAt(0, QColor(255, 255, 255, 255));
        gradient.setColorAt(m_hardness, QColor(255, 255, 255, 255));
        gradient.setColorAt(1, QColor(255, 255, 255, 0));
        
        painter.setBrush(QBrush(gradient));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(0, 0, m_size, m_size);
        painter.end();
    } else if (m_type == TextureBrush || m_type == ImageBrush) {
        if (!m_texture.isNull()) {
            m_brushMask = m_texture.scaled(m_size, m_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }
}

QPoint BrushPreset::calculateScatterOffset() const
{
    if (!m_scattering) {
        return QPoint(0, 0);
    }
    
    float scatterRadius = m_size * m_scatterAmount * 0.5f;
    float angle = (static_cast<float>(rand()) / RAND_MAX) * 2.0f * M_PI;
    float distance = (static_cast<float>(rand()) / RAND_MAX) * scatterRadius;
    
    return QPoint(
        static_cast<int>(distance * cos(angle)),
        static_cast<int>(distance * sin(angle))
    );
}

float BrushPreset::calculateRotation() const
{
    if (!m_rotationEnabled) {
        return 0.0f;
    }
    
    return (static_cast<float>(rand()) / RAND_MAX) * 360.0f;
}

// BrushManager implementation
BrushManager::BrushManager(QObject *parent)
    : QObject(parent)
    , m_brushLibraryPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/brushes")
{
    QDir().mkpath(m_brushLibraryPath);
    setupDefaultBrushes();
    loadBrushLibrary();
}

BrushManager::~BrushManager()
{
    saveBrushLibrary();
    qDeleteAll(m_brushes);
}

void BrushManager::addBrush(BrushPreset *brush)
{
    if (!brush) {
        return;
    }
    
    m_brushes.append(brush);
    emit brushAdded(brush);
}

void BrushManager::removeBrush(int index)
{
    if (index < 0 || index >= m_brushes.size()) {
        return;
    }
    
    BrushPreset *brush = m_brushes.takeAt(index);
    delete brush;
    emit brushRemoved(m_brushes[index]->getName());
}

void BrushManager::removeBrush(BrushPreset *brush)
{
    int index = m_brushes.indexOf(brush);
    if (index >= 0) {
        removeBrush(index);
    }
}

BrushPreset* BrushManager::getBrush(int index) const
{
    if (index < 0 || index >= m_brushes.size()) {
        return nullptr;
    }
    return m_brushes[index];
}

BrushPreset* BrushManager::getBrush(const QString &name) const
{
    for (BrushPreset *brush : m_brushes) {
        if (brush->getName() == name) {
            return brush;
        }
    }
    return nullptr;
}

int BrushManager::getBrushCount() const
{
    return m_brushes.size();
}

int BrushManager::getBrushIndex(BrushPreset *brush) const
{
    return m_brushes.indexOf(brush);
}

void BrushManager::createDefaultBrushes()
{
    // Basic brushes
    createBasicBrush("Small Round", 5, 1.0f);
    createBasicBrush("Medium Round", 20, 1.0f);
    createBasicBrush("Large Round", 50, 1.0f);
    createBasicBrush("Soft Round", 20, 0.3f);
    createBasicBrush("Hard Round", 20, 1.0f);
    
    // Texture brushes
    QImage noiseTexture(64, 64, QImage::Format_ARGB32);
    for (int y = 0; y < 64; ++y) {
        for (int x = 0; x < 64; ++x) {
            int noise = rand() % 256;
            noiseTexture.setPixelColor(x, y, QColor(noise, noise, noise, 255));
        }
    }
    createTextureBrush("Noise Brush", noiseTexture, 30);
    
    // Square brush
    QImage squareTexture(32, 32, QImage::Format_ARGB32);
    squareTexture.fill(Qt::white);
    createTextureBrush("Square Brush", squareTexture, 25);
}

BrushPreset* BrushManager::createBasicBrush(const QString &name, int size, float hardness)
{
    BrushPreset *brush = new BrushPreset(name, BrushPreset::BasicBrush, this);
    brush->setSize(size);
    brush->setHardness(hardness);
    addBrush(brush);
    return brush;
}

BrushPreset* BrushManager::createTextureBrush(const QString &name, const QImage &texture, int size)
{
    BrushPreset *brush = new BrushPreset(name, BrushPreset::TextureBrush, this);
    brush->setTexture(texture);
    brush->setSize(size);
    addBrush(brush);
    return brush;
}

BrushPreset* BrushManager::createImageBrush(const QString &name, const QImage &image, int size)
{
    BrushPreset *brush = new BrushPreset(name, BrushPreset::BasicBrush, this);
    brush->setType(BrushPreset::ImageBrush);
    brush->setTexture(image);
    brush->setSize(size);
    addBrush(brush);
    return brush;
}

QStringList BrushManager::getCategories() const
{
    return m_categories.keys();
}

QList<BrushPreset*> BrushManager::getBrushesInCategory(const QString &category) const
{
    return m_categories.value(category);
}

void BrushManager::setBrushCategory(BrushPreset *brush, const QString &category)
{
    if (!brush) {
        return;
    }
    
    // Remove from old category
    for (auto it = m_categories.begin(); it != m_categories.end(); ++it) {
        it.value().removeAll(brush);
    }
    
    // Add to new category
    m_categories[category].append(brush);
    emit brushCategoryChanged(brush, category);
}

bool BrushManager::importBrush(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        return false;
    }
    
    BrushPreset *brush = new BrushPreset(this);
    brush->fromJson(doc.object());
    addBrush(brush);
    
    return true;
}

bool BrushManager::exportBrush(BrushPreset *brush, const QString &filePath)
{
    if (!brush) {
        return false;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QJsonDocument doc(brush->toJson());
    file.write(doc.toJson());
    
    return true;
}

bool BrushManager::importBrushSet(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull() || !doc.isArray()) {
        return false;
    }
    
    QJsonArray brushArray = doc.array();
    for (const QJsonValue &value : brushArray) {
        if (value.isObject()) {
            BrushPreset *brush = new BrushPreset(this);
            brush->fromJson(value.toObject());
            addBrush(brush);
        }
    }
    
    return true;
}

bool BrushManager::exportBrushSet(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QJsonArray brushArray;
    for (BrushPreset *brush : m_brushes) {
        brushArray.append(brush->toJson());
    }
    
    QJsonDocument doc(brushArray);
    file.write(doc.toJson());
    
    return true;
}

void BrushManager::saveBrushPreset(BrushPreset *brush)
{
    if (!brush) {
        return;
    }
    
    QString fileName = m_brushLibraryPath + "/" + brush->getName() + ".brush";
    exportBrush(brush, fileName);
}

void BrushManager::loadBrushPreset(const QString &name)
{
    QString fileName = m_brushLibraryPath + "/" + name + ".brush";
    importBrush(fileName);
}

QStringList BrushManager::getAvailablePresets() const
{
    QStringList presets;
    QDir dir(m_brushLibraryPath);
    QStringList filters;
    filters << "*.brush";
    
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
    for (const QFileInfo &fileInfo : fileList) {
        presets.append(fileInfo.baseName());
    }
    
    return presets;
}

BrushPreset* BrushManager::createBrushFromImage(const QImage &image, const QString &name)
{
    BrushPreset *brush = new BrushPreset(name, BrushPreset::BasicBrush, this);
    brush->setType(BrushPreset::ImageBrush);
    brush->setTexture(image);
    brush->setSize(qMin(image.width(), image.height()));
    addBrush(brush);
    return brush;
}

void BrushManager::setupDefaultBrushes()
{
    createDefaultBrushes();
}

void BrushManager::loadBrushLibrary()
{
    QStringList presets = getAvailablePresets();
    for (const QString &preset : presets) {
        loadBrushPreset(preset);
    }
}

void BrushManager::saveBrushLibrary()
{
    for (BrushPreset *brush : m_brushes) {
        saveBrushPreset(brush);
    }
}

QString BrushManager::getBrushLibraryPath() const
{
    return m_brushLibraryPath;
}

// BrushEditorWidget implementation
BrushEditorWidget::BrushEditorWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentBrush(nullptr)
    , m_brushManager(nullptr)
{
    setupUI();
}

BrushEditorWidget::~BrushEditorWidget()
{
}

void BrushEditorWidget::setBrush(BrushPreset *brush)
{
    m_currentBrush = brush;
    updateBrushParameters();
    updatePreview();
}

void BrushEditorWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    // Preview section
    m_previewLayout = new QHBoxLayout();
    m_previewLabel = new QLabel(this);
    m_previewLabel->setFixedSize(128, 128);
    m_previewLabel->setStyleSheet("border: 1px solid gray; background: white;");
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLayout->addWidget(m_previewLabel);
    
    m_brushInfoLabel = new QLabel("Brush Info", this);
    m_brushInfoLabel->setWordWrap(true);
    m_previewLayout->addWidget(m_brushInfoLabel);
    
    m_mainLayout->addLayout(m_previewLayout);
    
    // Basic parameters
    m_basicGroup = new QGroupBox("Basic Parameters", this);
    m_basicLayout = new QFormLayout(m_basicGroup);
    
    m_sizeSpinBox = new QSpinBox(this);
    m_sizeSpinBox->setRange(1, 1000);
    m_sizeSpinBox->setValue(20);
    m_basicLayout->addRow("Size:", m_sizeSpinBox);
    
    m_opacitySpinBox = new QDoubleSpinBox(this);
    m_opacitySpinBox->setRange(0.0, 1.0);
    m_opacitySpinBox->setSingleStep(0.01);
    m_opacitySpinBox->setValue(1.0);
    m_basicLayout->addRow("Opacity:", m_opacitySpinBox);

    m_hardnessSpinBox = new QDoubleSpinBox(this);
    m_hardnessSpinBox->setRange(0.0, 1.0);
    m_hardnessSpinBox->setSingleStep(0.01);
    m_hardnessSpinBox->setValue(1.0);
    m_basicLayout->addRow("Hardness:", m_hardnessSpinBox);

    m_spacingSpinBox = new QDoubleSpinBox(this);
    m_spacingSpinBox->setRange(0.1, 5.0);
    m_spacingSpinBox->setSingleStep(0.1);
    m_spacingSpinBox->setValue(0.25);
    m_basicLayout->addRow("Spacing:", m_spacingSpinBox);

    m_flowSpinBox = new QDoubleSpinBox(this);
    m_flowSpinBox->setRange(0.0, 1.0);
    m_flowSpinBox->setSingleStep(0.01);
    m_flowSpinBox->setValue(1.0);
    m_basicLayout->addRow("Flow:", m_flowSpinBox);
    
    m_pressureCheckBox = new QCheckBox("Pressure Sensitive", this);
    m_pressureCheckBox->setChecked(true);
    m_basicLayout->addRow(m_pressureCheckBox);
    
    m_mainLayout->addWidget(m_basicGroup);
    
    // Advanced parameters
    m_advancedGroup = new QGroupBox("Advanced Parameters", this);
    m_advancedLayout = new QFormLayout(m_advancedGroup);
    
    m_scatteringCheckBox = new QCheckBox("Enable Scattering", this);
    m_advancedLayout->addRow(m_scatteringCheckBox);
    
    m_scatterAmountSpinBox = new QDoubleSpinBox(this);
    m_scatterAmountSpinBox->setRange(0.0, 1.0);
    m_scatterAmountSpinBox->setSingleStep(0.01);
    m_scatterAmountSpinBox->setValue(0.0);
    m_advancedLayout->addRow("Scatter Amount:", m_scatterAmountSpinBox);
    
    m_rotationCheckBox = new QCheckBox("Enable Rotation", this);
    m_advancedLayout->addRow(m_rotationCheckBox);
    
    m_mainLayout->addWidget(m_advancedGroup);
    
    // Texture section
    m_textureGroup = new QGroupBox("Texture", this);
    m_textureLayout = new QVBoxLayout(m_textureGroup);
    
    m_loadTextureButton = new QPushButton("Load Texture", this);
    m_clearTextureButton = new QPushButton("Clear Texture", this);
    m_textureLayout->addWidget(m_loadTextureButton);
    m_textureLayout->addWidget(m_clearTextureButton);
    
    m_texturePreview = new QLabel(this);
    m_texturePreview->setFixedSize(64, 64);
    m_texturePreview->setStyleSheet("border: 1px solid gray; background: white;");
    m_texturePreview->setAlignment(Qt::AlignCenter);
    m_textureLayout->addWidget(m_texturePreview);
    
    m_mainLayout->addWidget(m_textureGroup);
    
    // Actions
    m_actionsGroup = new QGroupBox("Actions", this);
    m_actionsLayout = new QHBoxLayout(m_actionsGroup);
    
    m_saveButton = new QPushButton("Save Brush", this);
    m_loadButton = new QPushButton("Load Brush", this);
    m_resetButton = new QPushButton("Reset", this);
    
    m_actionsLayout->addWidget(m_saveButton);
    m_actionsLayout->addWidget(m_loadButton);
    m_actionsLayout->addWidget(m_resetButton);
    
    m_mainLayout->addWidget(m_actionsGroup);
    
    // Connect signals
    connect(m_sizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &BrushEditorWidget::onBrushParameterChanged);
    connect(m_opacitySpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &BrushEditorWidget::onBrushParameterChanged);
    connect(m_hardnessSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &BrushEditorWidget::onBrushParameterChanged);
    connect(m_spacingSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &BrushEditorWidget::onBrushParameterChanged);
    connect(m_flowSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &BrushEditorWidget::onBrushParameterChanged);
    connect(m_pressureCheckBox, &QCheckBox::toggled, this, &BrushEditorWidget::onBrushParameterChanged);
    connect(m_scatteringCheckBox, &QCheckBox::toggled, this, &BrushEditorWidget::onBrushParameterChanged);
    connect(m_scatterAmountSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &BrushEditorWidget::onBrushParameterChanged);
    connect(m_rotationCheckBox, &QCheckBox::toggled, this, &BrushEditorWidget::onBrushParameterChanged);
    
    connect(m_loadTextureButton, &QPushButton::clicked, this, &BrushEditorWidget::onLoadBrush);
    connect(m_clearTextureButton, &QPushButton::clicked, this, &BrushEditorWidget::onTextureChanged);
    connect(m_saveButton, &QPushButton::clicked, this, &BrushEditorWidget::onSaveBrush);
    connect(m_loadButton, &QPushButton::clicked, this, &BrushEditorWidget::onLoadBrush);
    connect(m_resetButton, &QPushButton::clicked, this, &BrushEditorWidget::onBrushParameterChanged);
}

void BrushEditorWidget::updatePreview()
{
    if (!m_currentBrush) {
        m_previewLabel->setPixmap(QPixmap());
        m_brushInfoLabel->setText("No brush selected");
        return;
    }
    
    QImage preview = m_currentBrush->getPreview(128);
    m_previewLabel->setPixmap(QPixmap::fromImage(preview));
    
    QString info = QString("Name: %1\nSize: %2\nType: %3")
                   .arg(m_currentBrush->getName())
                   .arg(m_currentBrush->getSize())
                   .arg(static_cast<int>(m_currentBrush->getType()));
    m_brushInfoLabel->setText(info);
}

void BrushEditorWidget::updateBrushParameters()
{
    if (!m_currentBrush) {
        return;
    }
    
    m_sizeSpinBox->setValue(m_currentBrush->getSize());
    m_opacitySpinBox->setValue(m_currentBrush->getOpacity());
    m_hardnessSpinBox->setValue(m_currentBrush->getHardness());
    m_spacingSpinBox->setValue(m_currentBrush->getSpacing());
    m_flowSpinBox->setValue(m_currentBrush->getFlow());
    m_pressureCheckBox->setChecked(m_currentBrush->isPressureSensitive());
    m_scatteringCheckBox->setChecked(m_currentBrush->isScattering());
    m_scatterAmountSpinBox->setValue(m_currentBrush->getScatterAmount());
    m_rotationCheckBox->setChecked(m_currentBrush->isRotationEnabled());
    
    // Update texture preview
    if (!m_currentBrush->getTexture().isNull()) {
        QPixmap texturePixmap = QPixmap::fromImage(m_currentBrush->getTexture().scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_texturePreview->setPixmap(texturePixmap);
    } else {
        m_texturePreview->setPixmap(QPixmap());
    }
}

void BrushEditorWidget::onBrushParameterChanged()
{
    if (!m_currentBrush) {
        return;
    }
    
    m_currentBrush->setSize(m_sizeSpinBox->value());
    m_currentBrush->setOpacity(m_opacitySpinBox->value());
    m_currentBrush->setHardness(m_hardnessSpinBox->value());
    m_currentBrush->setSpacing(m_spacingSpinBox->value());
    m_currentBrush->setFlow(m_flowSpinBox->value());
    m_currentBrush->setPressureSensitive(m_pressureCheckBox->isChecked());
    m_currentBrush->setScattering(m_scatteringCheckBox->isChecked());
    m_currentBrush->setScatterAmount(m_scatterAmountSpinBox->value());
    m_currentBrush->setRotationEnabled(m_rotationCheckBox->isChecked());
    
    updatePreview();
    emit brushChanged(m_currentBrush);
}

void BrushEditorWidget::onTextureChanged()
{
    if (!m_currentBrush) {
        return;
    }
    
    m_currentBrush->setTexture(QImage());
    updatePreview();
    emit brushChanged(m_currentBrush);
}

void BrushEditorWidget::onSaveBrush()
{
    if (!m_currentBrush) {
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this, "Save Brush", "", "Brush Files (*.brush)");
    if (!fileName.isEmpty()) {
        if (m_brushManager) {
            m_brushManager->exportBrush(m_currentBrush, fileName);
        }
    }
}

void BrushEditorWidget::onLoadBrush()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Load Brush", "", "Brush Files (*.brush);;Image Files (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        if (fileName.endsWith(".brush") && m_brushManager) {
            if (m_brushManager->importBrush(fileName)) {
                // Brush loaded, update UI
                updatePreview();
            }
        } else {
            // Load as image brush
            QImage image(fileName);
            if (!image.isNull() && m_currentBrush) {
                m_currentBrush->setTexture(image);
                m_currentBrush->setType(BrushPreset::ImageBrush);
                updatePreview();
                emit brushChanged(m_currentBrush);
            }
        }
    }
}
