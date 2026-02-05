#include "ImageProcessor.h"
#include <QImage>
#include <QColor>
#include <QPainter>

ImageProcessor::ImageProcessor(QObject *parent)
    : QObject(parent)
{
}

ImageProcessor::~ImageProcessor()
{
}

QImage ImageProcessor::applyBrightness(const QImage &image, int brightness)
{
    QImage result = image;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor color = result.pixelColor(x, y);
            
            int r = qBound(0, color.red() + brightness, 255);
            int g = qBound(0, color.green() + brightness, 255);
            int b = qBound(0, color.blue() + brightness, 255);
            
            result.setPixelColor(x, y, QColor(r, g, b, color.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::applyContrast(const QImage &image, float contrast)
{
    QImage result = image;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor color = result.pixelColor(x, y);
            
            int r = qBound(0, static_cast<int>((color.red() - 128) * contrast + 128), 255);
            int g = qBound(0, static_cast<int>((color.green() - 128) * contrast + 128), 255);
            int b = qBound(0, static_cast<int>((color.blue() - 128) * contrast + 128), 255);
            
            result.setPixelColor(x, y, QColor(r, g, b, color.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::applySaturation(const QImage &image, float saturation)
{
    QImage result = image;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor color = result.pixelColor(x, y);
            
            QColor hsv = color.toHsv();
            int s = qBound(0, static_cast<int>(hsv.saturation() * saturation), 255);
            hsv.setHsv(hsv.hue(), s, hsv.value(), color.alpha());
            
            result.setPixelColor(x, y, hsv.toRgb());
        }
    }
    
    return result;
}

QImage ImageProcessor::resizeImage(const QImage &image, const QSize &size)
{
    return image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QImage ImageProcessor::cropImage(const QImage &image, const QRect &rect)
{
    return image.copy(rect);
}