#ifndef FILTERMANAGER_H
#define FILTERMANAGER_H

#include <QObject>
#include <QImage>
#include <QStringList>
#include <QVariantMap>

class FilterManager : public QObject
{
    Q_OBJECT

public:
    explicit FilterManager(QObject *parent = nullptr);
    ~FilterManager();

    QImage applyFilter(const QImage &image, const QString &filterName, const QVariantMap &parameters = QVariantMap());
    QStringList getAvailableFilters() const;
    QVariantMap getFilterParameters(const QString &filterName) const;

signals:
    void filterApplied(const QString &filterName);

private:
    QImage applyBlur(const QImage &image, const QVariantMap &parameters);
    QImage applyBrightnessContrast(const QImage &image, const QVariantMap &parameters);
    QImage applyGaussianBlur(const QImage &image, const QVariantMap &parameters);
    QImage applySharpen(const QImage &image, const QVariantMap &parameters);
};

#endif // FILTERMANAGER_H