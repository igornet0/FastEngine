#ifndef EFFECTSTYLE_H
#define EFFECTSTYLE_H

#include <QObject>
#include <QWidget>
#include <QColor>
#include <QPoint>
#include <QSize>
#include <QImage>
#include <QPainter>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>

class LayerEffect : public QObject
{
    Q_OBJECT

public:
    enum EffectType {
        DropShadow,
        InnerShadow,
        OuterGlow,
        InnerGlow,
        Bevel,
        Emboss,
        Stroke,
        ColorOverlay,
        GradientOverlay,
        PatternOverlay
    };

    explicit LayerEffect(EffectType type, QObject *parent = nullptr);
    virtual ~LayerEffect();

    // Basic properties
    EffectType getType() const { return m_type; }
    QString getName() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled);

    float getOpacity() const { return m_opacity; }
    void setOpacity(float opacity);

    // Effect application
    virtual void applyEffect(QPainter &painter, const QRect &rect, const QImage &sourceImage) const = 0;
    virtual QImage getEffectPreview(const QSize &size) const = 0;

    // Serialization
    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject &json);

signals:
    void effectChanged();

protected:
    EffectType m_type;
    QString m_name;
    bool m_enabled;
    float m_opacity;
};

class DropShadowEffect : public LayerEffect
{
    Q_OBJECT

public:
    explicit DropShadowEffect(QObject *parent = nullptr);
    ~DropShadowEffect();

    // Shadow properties
    QColor getColor() const { return m_color; }
    void setColor(const QColor &color);

    QPoint getOffset() const { return m_offset; }
    void setOffset(const QPoint &offset);

    int getBlurRadius() const { return m_blurRadius; }
    void setBlurRadius(int radius);

    int getSpread() const { return m_spread; }
    void setSpread(int spread);

    // Effect application
    void applyEffect(QPainter &painter, const QRect &rect, const QImage &sourceImage) const override;
    QImage getEffectPreview(const QSize &size) const override;

    // Serialization
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;

private:
    QColor m_color;
    QPoint m_offset;
    int m_blurRadius;
    int m_spread;
};

class InnerShadowEffect : public LayerEffect
{
    Q_OBJECT

public:
    explicit InnerShadowEffect(QObject *parent = nullptr);
    ~InnerShadowEffect();

    // Shadow properties
    QColor getColor() const { return m_color; }
    void setColor(const QColor &color);

    QPoint getOffset() const { return m_offset; }
    void setOffset(const QPoint &offset);

    int getBlurRadius() const { return m_blurRadius; }
    void setBlurRadius(int radius);

    int getChoke() const { return m_choke; }
    void setChoke(int choke);

    // Effect application
    void applyEffect(QPainter &painter, const QRect &rect, const QImage &sourceImage) const override;
    QImage getEffectPreview(const QSize &size) const override;

    // Serialization
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;

private:
    QColor m_color;
    QPoint m_offset;
    int m_blurRadius;
    int m_choke;
};

class OuterGlowEffect : public LayerEffect
{
    Q_OBJECT

public:
    explicit OuterGlowEffect(QObject *parent = nullptr);
    ~OuterGlowEffect();

    // Glow properties
    QColor getColor() const { return m_color; }
    void setColor(const QColor &color);

    int getSize() const { return m_size; }
    void setSize(int size);

    int getSpread() const { return m_spread; }
    void setSpread(int spread);

    int getRange() const { return m_range; }
    void setRange(int range);

    // Effect application
    void applyEffect(QPainter &painter, const QRect &rect, const QImage &sourceImage) const override;
    QImage getEffectPreview(const QSize &size) const override;

    // Serialization
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;

private:
    QColor m_color;
    int m_size;
    int m_spread;
    int m_range;
};

class InnerGlowEffect : public LayerEffect
{
    Q_OBJECT

public:
    explicit InnerGlowEffect(QObject *parent = nullptr);
    ~InnerGlowEffect();

    // Glow properties
    QColor getColor() const { return m_color; }
    void setColor(const QColor &color);

    int getSize() const { return m_size; }
    void setSize(int size);

    int getChoke() const { return m_choke; }
    void setChoke(int choke);

    int getRange() const { return m_range; }
    void setRange(int range);

    // Effect application
    void applyEffect(QPainter &painter, const QRect &rect, const QImage &sourceImage) const override;
    QImage getEffectPreview(const QSize &size) const override;

    // Serialization
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;

private:
    QColor m_color;
    int m_size;
    int m_choke;
    int m_range;
};

class BevelEffect : public LayerEffect
{
    Q_OBJECT

public:
    enum BevelStyle {
        OuterBevel,
        InnerBevel,
        Emboss,
        PillowEmboss
    };

    explicit BevelEffect(QObject *parent = nullptr);
    ~BevelEffect();

    // Bevel properties
    BevelStyle getStyle() const { return m_style; }
    void setStyle(BevelStyle style);

    int getSize() const { return m_size; }
    void setSize(int size);

    int getSoftness() const { return m_softness; }
    void setSoftness(int softness);

    int getDepth() const { return m_depth; }
    void setDepth(int depth);

    int getDirection() const { return m_direction; }
    void setDirection(int direction);

    int getAltitude() const { return m_altitude; }
    void setAltitude(int altitude);

    QColor getHighlightColor() const { return m_highlightColor; }
    void setHighlightColor(const QColor &color);

    QColor getShadowColor() const { return m_shadowColor; }
    void setShadowColor(const QColor &color);

    // Effect application
    void applyEffect(QPainter &painter, const QRect &rect, const QImage &sourceImage) const override;
    QImage getEffectPreview(const QSize &size) const override;

    // Serialization
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;

private:
    BevelStyle m_style;
    int m_size;
    int m_softness;
    int m_depth;
    int m_direction;
    int m_altitude;
    QColor m_highlightColor;
    QColor m_shadowColor;
};

class StrokeEffect : public LayerEffect
{
    Q_OBJECT

public:
    enum StrokePosition {
        Outside,
        Inside,
        Center
    };

    explicit StrokeEffect(QObject *parent = nullptr);
    ~StrokeEffect();

    // Stroke properties
    int getSize() const { return m_size; }
    void setSize(int size);

    StrokePosition getPosition() const { return m_position; }
    void setPosition(StrokePosition position);

    QColor getColor() const { return m_color; }
    void setColor(const QColor &color);

    int getOpacity() const { return m_opacity; }
    void setOpacity(int opacity);

    // Effect application
    void applyEffect(QPainter &painter, const QRect &rect, const QImage &sourceImage) const override;
    QImage getEffectPreview(const QSize &size) const override;

    // Serialization
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;

private:
    int m_size;
    StrokePosition m_position;
    QColor m_color;
    int m_opacity;
};

class ColorOverlayEffect : public LayerEffect
{
    Q_OBJECT

public:
    explicit ColorOverlayEffect(QObject *parent = nullptr);
    ~ColorOverlayEffect();

    // Overlay properties
    QColor getColor() const { return m_color; }
    void setColor(const QColor &color);

    // Effect application
    void applyEffect(QPainter &painter, const QRect &rect, const QImage &sourceImage) const override;
    QImage getEffectPreview(const QSize &size) const override;

    // Serialization
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;

private:
    QColor m_color;
};

class GradientOverlayEffect : public LayerEffect
{
    Q_OBJECT

public:
    explicit GradientOverlayEffect(QObject *parent = nullptr);
    ~GradientOverlayEffect();

    // Gradient properties
    QLinearGradient getGradient() const { return m_gradient; }
    void setGradient(const QLinearGradient &gradient);

    int getAngle() const { return m_angle; }
    void setAngle(int angle);

    int getScale() const { return m_scale; }
    void setScale(int scale);

    // Effect application
    void applyEffect(QPainter &painter, const QRect &rect, const QImage &sourceImage) const override;
    QImage getEffectPreview(const QSize &size) const override;

    // Serialization
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;

private:
    QLinearGradient m_gradient;
    int m_angle;
    int m_scale;
};

class PatternOverlayEffect : public LayerEffect
{
    Q_OBJECT

public:
    explicit PatternOverlayEffect(QObject *parent = nullptr);
    ~PatternOverlayEffect();

    // Pattern properties
    QImage getPattern() const { return m_pattern; }
    void setPattern(const QImage &pattern);

    int getScale() const { return m_scale; }
    void setScale(int scale);

    int getAngle() const { return m_angle; }
    void setAngle(int angle);

    // Effect application
    void applyEffect(QPainter &painter, const QRect &rect, const QImage &sourceImage) const override;
    QImage getEffectPreview(const QSize &size) const override;

    // Serialization
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;

private:
    QImage m_pattern;
    int m_scale;
    int m_angle;
};

class EffectStyle : public QObject
{
    Q_OBJECT

public:
    explicit EffectStyle(QObject *parent = nullptr);
    ~EffectStyle();

    // Effect management
    void addEffect(LayerEffect *effect);
    void removeEffect(int index);
    void removeEffect(LayerEffect *effect);
    LayerEffect* getEffect(int index) const;
    LayerEffect* getEffect(LayerEffect::EffectType type) const;
    int getEffectCount() const;
    int getEffectIndex(LayerEffect *effect) const;

    // Effect application
    void applyEffects(QPainter &painter, const QRect &rect, const QImage &sourceImage) const;
    QImage getEffectPreview(const QSize &size) const;

    // Serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);
    
    // Drop Shadow
    bool hasDropShadow() const;
    void setDropShadowEnabled(bool enabled);
    QColor getDropShadowColor() const;
    void setDropShadowColor(const QColor &color);
    int getDropShadowBlur() const;
    void setDropShadowBlur(int blur);
    QPoint getDropShadowOffset() const;
    void setDropShadowOffset(const QPoint &offset);
    float getDropShadowOpacity() const;
    void setDropShadowOpacity(float opacity);
    
    // Stroke
    bool hasStroke() const;
    void setStrokeEnabled(bool enabled);
    QColor getStrokeColor() const;
    void setStrokeColor(const QColor &color);
    int getStrokeSize() const;
    void setStrokeSize(int size);
    float getStrokeOpacity() const;
    void setStrokeOpacity(float opacity);
    
    // Outer Glow
    bool hasOuterGlow() const;
    void setOuterGlowEnabled(bool enabled);
    QColor getOuterGlowColor() const;
    void setOuterGlowColor(const QColor &color);
    int getOuterGlowSize() const;
    void setOuterGlowSize(int size);
    float getOuterGlowOpacity() const;
    void setOuterGlowOpacity(float opacity);
    
    // Inner Glow
    bool hasInnerGlow() const;
    void setInnerGlowEnabled(bool enabled);
    QColor getInnerGlowColor() const;
    void setInnerGlowColor(const QColor &color);
    int getInnerGlowSize() const;
    void setInnerGlowSize(int size);
    float getInnerGlowOpacity() const;
    void setInnerGlowOpacity(float opacity);
    
    // Overlay
    bool hasOverlay() const;
    void setOverlayEnabled(bool enabled);
    QColor getOverlayColor() const;
    void setOverlayColor(const QColor &color);
    float getOverlayOpacity() const;
    void setOverlayOpacity(float opacity);
    int getOverlayBlendMode() const;
    void setOverlayBlendMode(int mode);
    
    // Additional methods
    QImage applyToImage(const QImage &sourceImage) const;
    QPixmap applyToPixmap(const QPixmap &sourcePixmap) const;
    QImage applyDropShadow(const QImage &sourceImage) const;
    QImage applyStroke(const QImage &sourceImage) const;
    QImage applyOuterGlow(const QImage &sourceImage) const;
    QImage applyInnerGlow(const QImage &sourceImage) const;
    QImage applyOverlay(const QImage &sourceImage) const;
    QImage getPreview(const QImage &sourceImage, const QSize &size) const;
    QImage getThumbnail(const QImage &sourceImage, const QSize &size) const;
    QImage renderEffect(const QImage &sourceImage, const QSize &size, QImage::Format format) const;
    QImage mixWithEffect(const QImage &sourceImage, EffectStyle *otherEffect, float mixFactor) const;
    void resetEffects();
    
    // Additional utility methods
    EffectStyle* copy() const;
    void savePreset(const QString &name);
    QStringList getAvailablePresets() const;
    void loadPreset(const QString &name);
    QVariant getProperty(const QString &name) const;
    void setProperty(const QString &name, const QVariant &value);

signals:
    void effectAdded(int index);
    void effectRemoved(int index);
    void effectChanged(int index);
    void styleChanged();
    void changed();

private:
    QList<LayerEffect*> m_effects;
    
    // Drop Shadow
    bool m_dropShadowEnabled;
    QColor m_dropShadowColor;
    int m_dropShadowBlur;
    QPoint m_dropShadowOffset;
    float m_dropShadowOpacity;
    
    // Stroke
    bool m_strokeEnabled;
    QColor m_strokeColor;
    int m_strokeSize;
    float m_strokeOpacity;
    
    // Outer Glow
    bool m_outerGlowEnabled;
    QColor m_outerGlowColor;
    int m_outerGlowSize;
    float m_outerGlowOpacity;
    
    // Inner Glow
    bool m_innerGlowEnabled;
    QColor m_innerGlowColor;
    int m_innerGlowSize;
    float m_innerGlowOpacity;
    
    // Overlay
    bool m_overlayEnabled;
    QColor m_overlayColor;
    float m_overlayOpacity;
    int m_overlayBlendMode;
};

class EffectStyleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EffectStyleWidget(QWidget *parent = nullptr);
    ~EffectStyleWidget();

    void setEffectStyle(EffectStyle *style);
    EffectStyle* getEffectStyle() const { return m_currentStyle; }

signals:
    void effectStyleChanged(EffectStyle *style);

private slots:
    void onEffectAdded();
    void onEffectRemoved();
    void onEffectChanged();
    void onEffectEnabledToggled();
    void onEffectParameterChanged();

private:
    void setupUI();
    void updateEffectList();
    void updateEffectControls();
    void createEffectControls(LayerEffect *effect);

    EffectStyle *m_currentStyle;
    
    // UI components
    QVBoxLayout *m_mainLayout;
    
    // Effect list
    QGroupBox *m_effectsGroup;
    QVBoxLayout *m_effectsLayout;
    QListWidget *m_effectsList;
    QHBoxLayout *m_effectsButtonsLayout;
    QPushButton *m_addEffectButton;
    QPushButton *m_removeEffectButton;
    
    // Effect controls
    QGroupBox *m_controlsGroup;
    QVBoxLayout *m_controlsLayout;
    QScrollArea *m_controlsScrollArea;
    QWidget *m_controlsWidget;
    
    // Actions
    QGroupBox *m_actionsGroup;
    QHBoxLayout *m_actionsLayout;
    QPushButton *m_saveStyleButton;
    QPushButton *m_loadStyleButton;
    QPushButton *m_resetStyleButton;
};

#endif // EFFECTSTYLE_H
