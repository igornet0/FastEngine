#ifndef GRADIENTEDITOR_H
#define GRADIENTEDITOR_H

#include <QObject>
#include <QWidget>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QConicalGradient>
#include <QColor>
#include <QPoint>
#include <QList>
#include <QJsonObject>
#include <QLabel>
#include <QRadioButton>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QListWidget>
#include <QPushButton>

class GradientStop : public QObject
{
    Q_OBJECT

public:
    explicit GradientStop(float position = 0.0f, const QColor &color = Qt::black, QObject *parent = nullptr);
    ~GradientStop();

    float getPosition() const { return m_position; }
    void setPosition(float position);

    QColor getColor() const { return m_color; }
    void setColor(const QColor &color);

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

signals:
    void positionChanged(float position);
    void colorChanged(const QColor &color);
    void stopChanged();

private:
    float m_position;
    QColor m_color;
};

class GradientPreset : public QObject
{
    Q_OBJECT

public:
    enum GradientType {
        LinearGradient,
        RadialGradient,
        ConicalGradient
    };

    explicit GradientPreset(const QString &name = "New Gradient", QObject *parent = nullptr);
    ~GradientPreset();

    // Basic properties
    QString getName() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    GradientType getType() const { return m_type; }
    void setType(GradientType type);

    // Gradient stops
    void addStop(GradientStop *stop);
    void removeStop(int index);
    void removeStop(GradientStop *stop);
    GradientStop* getStop(int index) const;
    int getStopCount() const;
    void clearStops();

    // Gradient properties
    QPointF getStartPoint() const { return m_startPoint; }
    void setStartPoint(const QPointF &point) { m_startPoint = point; }

    QPointF getEndPoint() const { return m_endPoint; }
    void setEndPoint(const QPointF &point) { m_endPoint = point; }

    QPointF getCenterPoint() const { return m_centerPoint; }
    void setCenterPoint(const QPointF &point) { m_centerPoint = point; }

    float getRadius() const { return m_radius; }
    void setRadius(float radius) { m_radius = radius; }

    float getAngle() const { return m_angle; }
    void setAngle(float angle) { m_angle = angle; }

    // Gradient generation
    QLinearGradient getLinearGradient() const;
    QRadialGradient getRadialGradient() const;
    QConicalGradient getConicalGradient() const;
    QGradient* getGradient() const;

    // Preview
    QImage getPreview(const QSize &size = QSize(200, 50)) const;
    QImage getThumbnail(const QSize &size = QSize(32, 32)) const;

    // Serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

    // Gradient application
    void applyGradient(QPainter &painter, const QRect &rect) const;

signals:
    void gradientChanged();
    void stopAdded(int index);
    void stopRemoved(int index);
    void stopChanged(int index);

private:
    void sortStops();
    void updateGradient();

    QString m_name;
    GradientType m_type;
    QList<GradientStop*> m_stops;
    
    // Gradient properties
    QPointF m_startPoint;
    QPointF m_endPoint;
    QPointF m_centerPoint;
    float m_radius;
    float m_angle;
    
    // Cached gradient
    mutable QGradient *m_cachedGradient;
    mutable bool m_gradientValid;
};

class GradientEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GradientEditorWidget(QWidget *parent = nullptr);
    ~GradientEditorWidget();

    void setGradient(GradientPreset *gradient);
    GradientPreset* getGradient() const { return m_currentGradient; }

signals:
    void gradientChanged(GradientPreset *gradient);

private slots:
    void onGradientTypeChanged();
    void onStopAdded();
    void onStopRemoved();
    void onStopChanged();
    void onGradientPropertiesChanged();
    void onSaveGradient();
    void onLoadGradient();
    void onResetGradient();

private:
    void setupUI();
    void updatePreview();
    void updateStopList();
    void updateGradientProperties();
    void updateStopControls();

    GradientPreset *m_currentGradient;
    
    // UI components
    QVBoxLayout *m_mainLayout;
    
    // Preview
    QLabel *m_previewLabel;
    QLabel *m_gradientInfoLabel;
    
    // Gradient type
    QGroupBox *m_typeGroup;
    QVBoxLayout *m_typeLayout;
    QRadioButton *m_linearRadio;
    QRadioButton *m_radialRadio;
    QRadioButton *m_conicalRadio;
    
    // Gradient properties
    QGroupBox *m_propertiesGroup;
    QFormLayout *m_propertiesLayout;
    QDoubleSpinBox *m_startXSpinBox;
    QDoubleSpinBox *m_startYSpinBox;
    QDoubleSpinBox *m_endXSpinBox;
    QDoubleSpinBox *m_endYSpinBox;
    QDoubleSpinBox *m_centerXSpinBox;
    QDoubleSpinBox *m_centerYSpinBox;
    QDoubleSpinBox *m_radiusSpinBox;
    QDoubleSpinBox *m_angleSpinBox;
    
    // Stops
    QGroupBox *m_stopsGroup;
    QVBoxLayout *m_stopsLayout;
    QListWidget *m_stopsList;
    QHBoxLayout *m_stopsButtonsLayout;
    QPushButton *m_addStopButton;
    QPushButton *m_removeStopButton;
    
    // Stop properties
    QGroupBox *m_stopGroup;
    QFormLayout *m_stopLayout;
    QDoubleSpinBox *m_positionSpinBox;
    QPushButton *m_colorButton;
    QLabel *m_colorPreview;
    
    // Actions
    QGroupBox *m_actionsGroup;
    QHBoxLayout *m_actionsLayout;
    QPushButton *m_saveButton;
    QPushButton *m_loadButton;
    QPushButton *m_resetButton;
};

class GradientManager : public QObject
{
    Q_OBJECT

public:
    explicit GradientManager(QObject *parent = nullptr);
    ~GradientManager();

    // Gradient management
    void addGradient(GradientPreset *gradient);
    void removeGradient(int index);
    void removeGradient(GradientPreset *gradient);
    GradientPreset* getGradient(int index) const;
    GradientPreset* getGradient(const QString &name) const;
    int getGradientCount() const;
    int getGradientIndex(GradientPreset *gradient) const;

    // Default gradients
    void createDefaultGradients();
    GradientPreset* createLinearGradient(const QString &name, const QColor &startColor, const QColor &endColor);
    GradientPreset* createRadialGradient(const QString &name, const QColor &centerColor, const QColor &edgeColor);
    GradientPreset* createConicalGradient(const QString &name, const QColor &startColor, const QColor &endColor);

    // Gradient categories
    QStringList getCategories() const;
    QList<GradientPreset*> getGradientsInCategory(const QString &category) const;
    void setGradientCategory(GradientPreset *gradient, const QString &category);

    // Import/Export
    bool importGradient(const QString &filePath);
    bool exportGradient(GradientPreset *gradient, const QString &filePath);
    bool importGradientSet(const QString &filePath);
    bool exportGradientSet(const QString &filePath);

    // Gradient presets
    void saveGradientPreset(GradientPreset *gradient);
    void loadGradientPreset(const QString &name);
    QStringList getAvailablePresets() const;

signals:
    void gradientAdded(int index);
    void gradientRemoved(int index);
    void gradientChanged(int index);
    void gradientCategoryChanged(GradientPreset *gradient, const QString &category);

private:
    void setupDefaultGradients();
    void loadGradientLibrary();
    void saveGradientLibrary();
    QString getGradientLibraryPath() const;

    QList<GradientPreset*> m_gradients;
    QHash<QString, QList<GradientPreset*>> m_categories;
    QString m_gradientLibraryPath;
};

#endif // GRADIENTEDITOR_H
