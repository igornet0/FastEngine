#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QPoint>
#include <QRect>
#include <QCursor>

class ToolManager;

class CanvasWidget : public QWidget
{
public:
    explicit CanvasWidget(QWidget *parent = nullptr);
    ~CanvasWidget();

    void setImage(const QImage &image);
    QImage getImage() const { return m_image; }
    
    void setToolManager(ToolManager *toolManager);
    ToolManager* getToolManager() const { return m_toolManager; }
    
    void setZoomFactor(float factor);
    float getZoomFactor() const { return m_zoomFactor; }
    
    void fitToWindow();
    void actualSize();
    void zoomIn();
    void zoomOut();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void updateCursor();
    void updateDisplayImage();
    QPoint imageToWidget(const QPoint &imagePoint) const;
    QPoint widgetToImage(const QPoint &widgetPoint) const;
    QRect imageToWidget(const QRect &imageRect) const;
    QRect widgetToImage(const QRect &widgetRect) const;
    void drawGrid(QPainter &painter);
    void drawSelection(QPainter &painter);
    void drawCropRect(QPainter &painter);
    
    QImage m_image;
    QImage m_displayImage;
    ToolManager *m_toolManager;
    
    float m_zoomFactor;
    QPoint m_offset;
    QPoint m_lastMousePos;
    bool m_isPanning;
    
    // Grid settings
    bool m_showGrid;
    int m_gridSize;
    QColor m_gridColor;
    
    // Crop
    QRect m_cropRect;
    bool m_isCropping;
};

#endif // CANVASWIDGET_H