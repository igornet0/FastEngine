#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QMatrix4x4>
#include <QVector3D>
#include <QQuaternion>
#include <QPointF>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <memory>
#include <vector>

class FastEngineIntegration;

struct Camera3D {
    QVector3D position;
    QVector3D target;
    QVector3D up;
    float fov;
    float nearPlane;
    float farPlane;
    
    Camera3D() 
        : position(0, 0, 5)
        , target(0, 0, 0)
        , up(0, 1, 0)
        , fov(45.0f)
        , nearPlane(0.1f)
        , farPlane(1000.0f) {}
    
    QMatrix4x4 getViewMatrix() const;
    QMatrix4x4 getProjectionMatrix(float aspectRatio) const;
};

struct Light3D {
    QVector3D position;
    QVector3D color;
    float intensity;
    
    Light3D() 
        : position(0, 5, 0)
        , color(1, 1, 1)
        , intensity(1.0f) {}
};

struct SceneEntity3D {
    QString name;
    QVector3D position;
    QQuaternion rotation;
    QVector3D scale;
    QColor color;
    bool visible;
    bool selected;
    
    SceneEntity3D() 
        : position(0, 0, 0)
        , rotation(1, 0, 0, 0)
        , scale(1, 1, 1)
        , color(Qt::blue)
        , visible(true)
        , selected(false) {}
    
    QMatrix4x4 getModelMatrix() const;
};

class SceneView3D : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit SceneView3D(QWidget *parent = nullptr);
    ~SceneView3D();

    // Управление камерой
    void resetCamera();
    void setCameraPosition(const QVector3D &position);
    void setCameraTarget(const QVector3D &target);
    void setCameraFOV(float fov);
    
    // Управление освещением
    void setLightPosition(const QVector3D &position);
    void setLightColor(const QVector3D &color);
    void setLightIntensity(float intensity);
    
    // Управление объектами
    void addEntity(const QString &name, const QVector3D &position, const QVector3D &scale);
    void removeEntity(const QString &name);
    void selectEntity(const QString &name);
    void clearSelection();
    void focusOnEntity(const QString &name);
    
    // Получение информации
    QStringList getSelectedEntities() const;
    QVector3D getCameraPosition() const;
    QVector3D getCameraTarget() const;
    float getCameraFOV() const;

signals:
    void selectionChanged(const QStringList &selectedEntities);
    void entityCreated(const QString &name, const QVector3D &position);
    void entityMoved(const QString &name, const QVector3D &position);
    void entityDeleted(const QString &name);
    void assetSelected(const QString &assetPath);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void onAssetSelected(const QString &assetPath);

private slots:
    void updateScene();

private:
    void setupShaders();
    void setupBuffers();
    void setupTextures();
    void setupConnections();
    
    void renderGrid();
    void renderEntities();
    void renderSelection();
    
    QVector3D screenToWorld(const QPoint &screenPos) const;
    QPoint worldToScreen(const QVector3D &worldPos) const;
    QString getEntityAt(const QPoint &screenPos) const;
    
    void createGrid();
    void createTestEntities();
    void updateCamera();
    
    // OpenGL объекты
    QOpenGLShaderProgram *m_shaderProgram;
    QOpenGLShaderProgram *m_gridShaderProgram;
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_indexBuffer;
    QOpenGLVertexArrayObject m_vao;
    
    // Камера и освещение
    Camera3D m_camera;
    Light3D m_light;
    
    // Состояние
    bool m_dragging;
    bool m_cameraDragging;
    QPoint m_lastMousePos;
    float m_cameraDistance;
    float m_cameraRotationX;
    float m_cameraRotationY;
    
    // Объекты сцены
    std::vector<SceneEntity3D> m_entities;
    QStringList m_selectedEntities;
    
    // Сетка
    std::vector<QVector3D> m_gridVertices;
    std::vector<unsigned int> m_gridIndices;
    
    // Таймер обновления
    QTimer *m_updateTimer;
    
    // Интеграция с FastEngine
    std::unique_ptr<FastEngineIntegration> m_fastEngineIntegration;
    
    // Матрицы
    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_modelMatrix;
    
    // Состояние OpenGL
    bool m_initialized;
    int m_width;
    int m_height;
};
