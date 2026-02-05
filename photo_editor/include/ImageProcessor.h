#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QImage>
#include <QSize>
#include <QRect>

class ImageProcessor : public QObject
{
    Q_OBJECT

public:
    explicit ImageProcessor(QObject *parent = nullptr);
    ~ImageProcessor();

    // Basic image adjustments
    QImage applyBrightness(const QImage &image, int brightness);
    QImage applyContrast(const QImage &image, float contrast);
    QImage applySaturation(const QImage &image, float saturation);
    
    // Image transformations
    QImage resizeImage(const QImage &image, const QSize &size);
    QImage cropImage(const QImage &image, const QRect &rect);
};

#endif // IMAGEPROCESSOR_H