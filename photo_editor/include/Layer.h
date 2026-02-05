#ifndef LAYER_H
#define LAYER_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QColor>
#include <QPainter>

class Layer : public QObject
{
    Q_OBJECT

public:
    explicit Layer(QObject *parent = nullptr);
    Layer(const QString &name, const QImage &image, QObject *parent = nullptr);
    ~Layer();

    // Getters
    QString name() const { return m_name; }
    QImage image() const { return m_image; }
    bool visible() const { return m_visible; }
    bool locked() const { return m_locked; }
    float opacity() const { return m_opacity; }
    int index() const { return m_index; }

    // Setters
    void setName(const QString &name);
    void setImage(const QImage &image);
    void setVisible(bool visible);
    void setLocked(bool locked);
    void setOpacity(float opacity);
    void setIndex(int index);

    // Image operations
    void clear();
    void resize(const QSize &size);
    QImage composite(const QImage &background) const;

signals:
    void nameChanged(const QString &name);
    void imageChanged();
    void visibilityChanged(bool visible);
    void opacityChanged(float opacity);

private:
    QString m_name;
    QImage m_image;
    bool m_visible;
    bool m_locked;
    float m_opacity;
    int m_index;
};

#endif // LAYER_H
