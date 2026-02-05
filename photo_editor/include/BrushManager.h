#ifndef BRUSHMANAGER_H
#define BRUSHMANAGER_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QList>
#include <QColor>
#include <QPoint>
#include <QPainter>
#include <QJsonObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QMap>

class BrushPreset : public QObject
{
    Q_OBJECT

public:
    enum BrushType {
        BasicBrush,      // Обычная круглая кисть
        TextureBrush,    // Текстурированная кисть
        ImageBrush,      // Кисть из изображения
        CustomBrush      // Пользовательская кисть
    };

    explicit BrushPreset(QObject *parent = nullptr);
    BrushPreset(const QString &name, BrushType type, QObject *parent = nullptr);
    ~BrushPreset();

    // Основные свойства
    QString getName() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    BrushType getType() const { return m_type; }
    void setType(BrushType type) { m_type = type; }

    QImage getBrushImage() const { return m_brushImage; }
    void setBrushImage(const QImage &image) { m_brushImage = image; }

    // Параметры кисти
    int getSize() const { return m_size; }
    void setSize(int size) { m_size = qBound(1, size, 1000); }

    float getOpacity() const { return m_opacity; }
    void setOpacity(float opacity) { m_opacity = qBound(0.0f, opacity, 1.0f); }

    float getHardness() const { return m_hardness; }
    void setHardness(float hardness) { m_hardness = qBound(0.0f, hardness, 1.0f); }

    float getSpacing() const { return m_spacing; }
    void setSpacing(float spacing) { m_spacing = qBound(0.1f, spacing, 5.0f); }

    float getFlow() const { return m_flow; }
    void setFlow(float flow) { m_flow = qBound(0.0f, flow, 1.0f); }

    QColor getColor() const { return m_color; }
    void setColor(const QColor &color) { m_color = color; }

    // Дополнительные параметры
    bool isPressureSensitive() const { return m_pressureSensitive; }
    void setPressureSensitive(bool enabled) { m_pressureSensitive = enabled; }

    bool isScattering() const { return m_scattering; }
    void setScattering(bool enabled) { m_scattering = enabled; }

    float getScatterAmount() const { return m_scatterAmount; }
    void setScatterAmount(float amount) { m_scatterAmount = qBound(0.0f, amount, 1.0f); }

    bool isRotationEnabled() const { return m_rotationEnabled; }
    void setRotationEnabled(bool enabled) { m_rotationEnabled = enabled; }

    // Операции с кистью
    void applyBrush(QPainter &painter, const QPoint &position, float pressure = 1.0f) const;
    void applyBrush(QPainter &painter, const QPoint &point, const QColor &color, float pressure) const;
    QImage generateBrushPreview(int size) const;
    QImage generateBrushStamp(int size) const;
    
    // Предварительный просмотр
    QImage getPreview(int size = 64) const;
    QImage getThumbnail(int size = 32) const;
    
    // Текстура
    QImage getTexture() const { return m_texture; }
    void setTexture(const QImage &texture) { m_texture = texture; }
    
    // Генерация маски кисти
    void generateBrushMask();
    
    // Дополнительные методы
    void applyBrushStroke(QPainter &painter, const QPoint &from, const QPoint &to, const QColor &color, float pressure) const;
    QPoint calculateScatterOffset() const;
    float calculateRotation() const;

    // Сериализация
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

    // Клонирование
    BrushPreset* clone() const;

signals:
    void brushChanged();
    void nameChanged(const QString &name);

private:
    QString m_name;
    BrushType m_type;
    QImage m_brushImage;
    
    // Основные параметры
    int m_size;
    float m_opacity;
    float m_hardness;
    float m_spacing;
    float m_flow;
    QColor m_color;
    
    // Дополнительные параметры
    bool m_pressureSensitive;
    bool m_scattering;
    float m_scatterAmount;
    bool m_rotationEnabled;
    
    // Текстура и предварительный просмотр
    QImage m_texture;
    mutable QImage m_preview;
    mutable QImage m_thumbnail;
    mutable bool m_previewValid;
    mutable bool m_thumbnailValid;
    QImage m_brushMask;
};

class BrushManager : public QObject
{
    Q_OBJECT

public:
    explicit BrushManager(QObject *parent = nullptr);
    ~BrushManager();

    // Управление кистями
    void addBrush(BrushPreset *brush);
    void removeBrush(const QString &name);
    void removeBrush(BrushPreset *brush);
    BrushPreset* getBrush(const QString &name) const;
    BrushPreset* getBrush(int index) const;
    int getBrushCount() const;
    QStringList getBrushNames() const;

    // Текущая кисть
    BrushPreset* getCurrentBrush() const { return m_currentBrush; }
    void setCurrentBrush(BrushPreset *brush);
    void setCurrentBrush(const QString &name);

    // Предустановленные кисти
    void loadDefaultBrushes();
    void loadBrushesFromDirectory(const QString &directory);
    void saveBrushesToDirectory(const QString &directory);

    // Импорт/экспорт
    bool importBrush(const QString &fileName);
    bool exportBrush(BrushPreset *brush, const QString &fileName);
    bool exportBrush(const QString &name, const QString &fileName);

    // Сериализация
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

signals:
    void brushAdded(BrushPreset *brush);
    void brushRemoved(const QString &name);
    void currentBrushChanged(BrushPreset *brush);
    void brushChanged(BrushPreset *brush);
    void brushCategoryChanged(BrushPreset *brush, const QString &category);

private:
    QList<BrushPreset*> m_brushes;
    BrushPreset *m_currentBrush;
    QString m_brushesDirectory;
    QString m_brushLibraryPath;
    QMap<QString, QList<BrushPreset*>> m_categories;

    void createDefaultBrushes();
    void setupDefaultBrushes();
    void loadBrushLibrary();
    void saveBrushLibrary();
    BrushPreset* findBrush(const QString &name) const;
    
    // Создание кистей
    BrushPreset* createBasicBrush(const QString &name, int size, float hardness);
    BrushPreset* createTextureBrush(const QString &name, const QImage &texture, int size);
    
    // Удаление кистей
    void removeBrush(int index);
    int getBrushIndex(BrushPreset *brush) const;
    
    // Категории кистей
    QStringList getCategories() const;
    QList<BrushPreset*> getBrushesInCategory(const QString &category) const;
    void setBrushCategory(BrushPreset *brush, const QString &category);
    
    // Импорт/экспорт наборов кистей
    bool importBrushSet(const QString &filePath);
    bool exportBrushSet(const QString &filePath);
    
    // Сохранение/загрузка пресетов
    void saveBrushPreset(BrushPreset *brush);
    void loadBrushPreset(const QString &name);
    QStringList getAvailablePresets() const;
    
    // Создание кистей из изображений
    BrushPreset* createImageBrush(const QString &name, const QImage &image, int size);
    BrushPreset* createBrushFromImage(const QImage &image, const QString &name);
    
    // Путь к библиотеке кистей
    QString getBrushLibraryPath() const;
};

// BrushEditorWidget - виджет для редактирования кистей
class BrushEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BrushEditorWidget(QWidget *parent = nullptr);
    ~BrushEditorWidget();
    
    // Запрещаем копирование
    BrushEditorWidget(const BrushEditorWidget &) = delete;
    BrushEditorWidget &operator=(const BrushEditorWidget &) = delete;

    void setBrush(BrushPreset *brush);
    void setBrushManager(BrushManager *manager);

signals:
    void brushChanged(BrushPreset *brush);

private slots:
    void updatePreview();
    void updateBrushParameters();
    void onBrushParameterChanged();
    void onTextureChanged();
    void onSaveBrush();
    void onLoadBrush();

private:
    void setupUI();

    BrushPreset *m_currentBrush;
    BrushManager *m_brushManager;

    // UI элементы
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_previewLayout;
    QLabel *m_previewLabel;
    QLabel *m_brushInfoLabel;
    
    QGroupBox *m_basicGroup;
    QFormLayout *m_basicLayout;
    QSpinBox *m_sizeSpinBox;
    QDoubleSpinBox *m_opacitySpinBox;
    QDoubleSpinBox *m_hardnessSpinBox;
    QDoubleSpinBox *m_spacingSpinBox;
    QDoubleSpinBox *m_flowSpinBox;
    QPushButton *m_colorButton;
    
    QGroupBox *m_advancedGroup;
    QFormLayout *m_advancedLayout;
    QCheckBox *m_pressureSensitiveCheckBox;
    QCheckBox *m_scatteringCheckBox;
    QDoubleSpinBox *m_scatterAmountSpinBox;
    QCheckBox *m_rotationEnabledCheckBox;
    
    // Дополнительные члены
    QCheckBox *m_pressureCheckBox;
    QCheckBox *m_rotationCheckBox;
    QLabel *m_texturePreview;
    QGroupBox *m_actionsGroup;
    QHBoxLayout *m_actionsLayout;
    
    QGroupBox *m_textureGroup;
    QVBoxLayout *m_textureLayout;
    QLabel *m_textureLabel;
    QPushButton *m_loadTextureButton;
    QPushButton *m_clearTextureButton;
    
    QHBoxLayout *m_buttonsLayout;
    QPushButton *m_saveButton;
    QPushButton *m_loadButton;
    QPushButton *m_resetButton;
};

#endif // BRUSHMANAGER_H
