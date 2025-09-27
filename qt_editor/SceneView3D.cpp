#include "SceneView3D.h"
#include "FastEngineIntegration.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QMatrix4x4>
#include <QVector3D>
#include <QQuaternion>
#include <QPointF>
#include <QApplication>
#include <QDebug>
#include <cmath>

// Вершинный шейдер
const char *vertexShaderSource = R"(
#version 120
attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

varying vec3 FragPos;
varying vec3 Normal;
varying vec2 TexCoord;
varying vec3 LightPos;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    LightPos = lightPos;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

// Фрагментный шейдер
const char *fragmentShaderSource = R"(
#version 120
varying vec3 FragPos;
varying vec3 Normal;
varying vec2 TexCoord;
varying vec3 LightPos;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float lightIntensity;
uniform bool useTexture;
uniform sampler2D texture1;

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * lightIntensity;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    
    if (useTexture) {
        result *= texture2D(texture1, TexCoord).rgb;
    }
    
    gl_FragColor = vec4(result, 1.0);
}
)";

// Шейдер для сетки
const char *gridVertexShaderSource = R"(
#version 120
attribute vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char *gridFragmentShaderSource = R"(
#version 120
uniform vec3 gridColor;

void main()
{
    gl_FragColor = vec4(gridColor, 0.5);
}
)";

SceneView3D::SceneView3D(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_shaderProgram(nullptr)
    , m_gridShaderProgram(nullptr)
    , m_dragging(false)
    , m_cameraDragging(false)
    , m_cameraDistance(10.0f)
    , m_cameraRotationX(0.0f)
    , m_cameraRotationY(0.0f)
    , m_updateTimer(nullptr)
    , m_fastEngineIntegration(std::make_unique<FastEngineIntegration>())
    , m_initialized(false)
    , m_width(800)
    , m_height(600)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    
    // Настройка камеры
    resetCamera();
    
    // Создание тестовых объектов
    createTestEntities();
    
    // Таймер обновления
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &SceneView3D::updateScene);
    m_updateTimer->start(16); // ~60 FPS
}

SceneView3D::~SceneView3D()
{
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
    
    if (m_initialized) {
        makeCurrent();
        delete m_shaderProgram;
        delete m_gridShaderProgram;
        doneCurrent();
    }
}

void SceneView3D::initializeGL()
{
    initializeOpenGLFunctions();
    
    // Настройка OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    // Создание шейдеров
    setupShaders();
    
    // Создание буферов
    setupBuffers();
    
    // Создание сетки
    createGrid();
    
    m_initialized = true;
}

void SceneView3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (!m_initialized) {
        return;
    }
    
    // Обновление матриц
    m_projectionMatrix = m_camera.getProjectionMatrix((float)m_width / m_height);
    m_viewMatrix = m_camera.getViewMatrix();
    
    // Отрисовка сетки
    renderGrid();
    
    // Отрисовка объектов
    renderEntities();
    
    // Отрисовка выделения
    renderSelection();
}

void SceneView3D::resizeGL(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}

void SceneView3D::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QString entityName = getEntityAt(event->pos());
        
        if (!entityName.isEmpty()) {
            // Выбор объекта
            if (!event->modifiers().testFlag(Qt::ControlModifier)) {
                clearSelection();
            }
            selectEntity(entityName);
        } else {
            // Начало перетаскивания камеры
            m_cameraDragging = true;
            m_lastMousePos = event->pos();
        }
    } else if (event->button() == Qt::RightButton) {
        // Создание нового объекта
        QVector3D worldPos = screenToWorld(event->pos());
        QString name = QString("Entity3D_%1").arg(m_entities.size() + 1);
        addEntity(name, worldPos, QVector3D(1, 1, 1));
    }
}

void SceneView3D::mouseMoveEvent(QMouseEvent *event)
{
    if (m_cameraDragging) {
        // Перетаскивание камеры
        QPoint delta = event->pos() - m_lastMousePos;
        
        m_cameraRotationY += delta.x() * 0.01f;
        m_cameraRotationX += delta.y() * 0.01f;
        
        // Ограничение угла поворота
        m_cameraRotationX = qBound(-M_PI/2 + 0.1f, m_cameraRotationX, M_PI/2 - 0.1f);
        
        updateCamera();
        m_lastMousePos = event->pos();
    }
}

void SceneView3D::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_cameraDragging = false;
    }
}

void SceneView3D::wheelEvent(QWheelEvent *event)
{
    // Масштабирование камеры
    float scaleFactor = 1.1f;
    if (event->angleDelta().y() < 0) {
        scaleFactor = 1.0f / scaleFactor;
    }
    
    m_cameraDistance *= scaleFactor;
    m_cameraDistance = qBound(1.0f, m_cameraDistance, 100.0f);
    
    updateCamera();
}

void SceneView3D::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Delete:
            // Удаление выбранных объектов
            for (const QString &name : m_selectedEntities) {
                removeEntity(name);
            }
            break;
            
        case Qt::Key_Escape:
            clearSelection();
            break;
            
        case Qt::Key_0:
            resetCamera();
            break;
            
        case Qt::Key_W:
            m_camera.position += m_camera.target * 0.1f;
            updateCamera();
            break;
            
        case Qt::Key_S:
            m_camera.position -= m_camera.target * 0.1f;
            updateCamera();
            break;
            
        case Qt::Key_A:
            m_camera.position -= QVector3D::crossProduct(m_camera.target, m_camera.up).normalized() * 0.1f;
            updateCamera();
            break;
            
        case Qt::Key_D:
            m_camera.position += QVector3D::crossProduct(m_camera.target, m_camera.up).normalized() * 0.1f;
            updateCamera();
            break;
            
        default:
            QOpenGLWidget::keyPressEvent(event);
            break;
    }
}

void SceneView3D::resetCamera()
{
    m_camera.position = QVector3D(0, 0, 10);
    m_camera.target = QVector3D(0, 0, 0);
    m_camera.up = QVector3D(0, 1, 0);
    m_camera.fov = 45.0f;
    m_cameraDistance = 10.0f;
    m_cameraRotationX = 0.0f;
    m_cameraRotationY = 0.0f;
    
    updateCamera();
}

void SceneView3D::setCameraPosition(const QVector3D &position)
{
    m_camera.position = position;
    updateCamera();
}

void SceneView3D::setCameraTarget(const QVector3D &target)
{
    m_camera.target = target;
    updateCamera();
}

void SceneView3D::setCameraFOV(float fov)
{
    m_camera.fov = fov;
    updateCamera();
}

void SceneView3D::setLightPosition(const QVector3D &position)
{
    m_light.position = position;
}

void SceneView3D::setLightColor(const QVector3D &color)
{
    m_light.color = color;
}

void SceneView3D::setLightIntensity(float intensity)
{
    m_light.intensity = intensity;
}

void SceneView3D::addEntity(const QString &name, const QVector3D &position, const QVector3D &scale)
{
    SceneEntity3D entity;
    entity.name = name;
    entity.position = position;
    entity.scale = scale;
    m_entities.push_back(entity);
    
    emit entityCreated(name, position);
}

void SceneView3D::removeEntity(const QString &name)
{
    for (auto it = m_entities.begin(); it != m_entities.end(); ++it) {
        if (it->name == name) {
            m_entities.erase(it);
            emit entityDeleted(name);
            break;
        }
    }
}

void SceneView3D::selectEntity(const QString &name)
{
    for (auto &entity : m_entities) {
        if (entity.name == name) {
            entity.selected = true;
            if (!m_selectedEntities.contains(name)) {
                m_selectedEntities.append(name);
            }
            emit selectionChanged(m_selectedEntities);
            break;
        }
    }
}

void SceneView3D::clearSelection()
{
    for (auto &entity : m_entities) {
        entity.selected = false;
    }
    m_selectedEntities.clear();
    emit selectionChanged(QStringList());
}

QStringList SceneView3D::getSelectedEntities() const
{
    return m_selectedEntities;
}

QVector3D SceneView3D::getCameraPosition() const
{
    return m_camera.position;
}

QVector3D SceneView3D::getCameraTarget() const
{
    return m_camera.target;
}

float SceneView3D::getCameraFOV() const
{
    return m_camera.fov;
}

void SceneView3D::updateScene()
{
    // Обновление сцены (вызывается по таймеру)
    update();
}

void SceneView3D::setupShaders()
{
    // Основной шейдер
    m_shaderProgram = new QOpenGLShaderProgram(this);
    m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_shaderProgram->link();
    
    // Шейдер для сетки
    m_gridShaderProgram = new QOpenGLShaderProgram(this);
    m_gridShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, gridVertexShaderSource);
    m_gridShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, gridFragmentShaderSource);
    m_gridShaderProgram->link();
}

void SceneView3D::setupBuffers()
{
    // Создание VAO
    m_vao.create();
    m_vao.bind();
    
    // Создание VBO для куба
    float vertices[] = {
        // Позиция          // Нормаль           // Текстурные координаты
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    
    m_vertexBuffer.create();
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(vertices, sizeof(vertices));
    
    // Настройка атрибутов (получаем locations из шейдера)
    if (m_shaderProgram) {
        int posLocation = m_shaderProgram->attributeLocation("aPos");
        int normalLocation = m_shaderProgram->attributeLocation("aNormal");
        int texCoordLocation = m_shaderProgram->attributeLocation("aTexCoord");
        
        if (posLocation >= 0) {
            glEnableVertexAttribArray(posLocation);
            glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        }
        if (normalLocation >= 0) {
            glEnableVertexAttribArray(normalLocation);
            glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        if (texCoordLocation >= 0) {
            glEnableVertexAttribArray(texCoordLocation);
            glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        }
    }
    
    m_vao.release();
}

void SceneView3D::setupTextures()
{
    // TODO: Загрузка текстур
}

void SceneView3D::setupConnections()
{
    // Соединения для обработки событий
}

void SceneView3D::renderGrid()
{
    if (m_gridVertices.empty()) {
        return;
    }
    
    m_gridShaderProgram->bind();
    m_gridShaderProgram->setUniformValue("model", QMatrix4x4());
    m_gridShaderProgram->setUniformValue("view", m_viewMatrix);
    m_gridShaderProgram->setUniformValue("projection", m_projectionMatrix);
    m_gridShaderProgram->setUniformValue("gridColor", QVector3D(0.5f, 0.5f, 0.5f));
    
    glDrawArrays(GL_LINES, 0, m_gridVertices.size());
    
    m_gridShaderProgram->release();
}

void SceneView3D::renderEntities()
{
    m_shaderProgram->bind();
    
    // Установка uniform переменных
    m_shaderProgram->setUniformValue("view", m_viewMatrix);
    m_shaderProgram->setUniformValue("projection", m_projectionMatrix);
    m_shaderProgram->setUniformValue("lightPos", m_light.position);
    m_shaderProgram->setUniformValue("lightColor", m_light.color);
    m_shaderProgram->setUniformValue("lightIntensity", m_light.intensity);
    m_shaderProgram->setUniformValue("viewPos", m_camera.position);
    m_shaderProgram->setUniformValue("useTexture", false);
    
    m_vao.bind();
    
    for (const auto &entity : m_entities) {
        if (!entity.visible) {
            continue;
        }
        
        // Матрица модели
        QMatrix4x4 model = entity.getModelMatrix();
        m_shaderProgram->setUniformValue("model", model);
        m_shaderProgram->setUniformValue("objectColor", QVector3D(entity.color.redF(), entity.color.greenF(), entity.color.blueF()));
        
        // Отрисовка куба
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    
    m_vao.release();
    m_shaderProgram->release();
}

void SceneView3D::renderSelection()
{
    // Отрисовка выделенных объектов с другим цветом
    for (const auto &entity : m_entities) {
        if (entity.selected) {
            // TODO: Отрисовка рамки выделения
        }
    }
}

QVector3D SceneView3D::screenToWorld(const QPoint &screenPos) const
{
    // TODO: Реализовать преобразование экранных координат в мировые
    Q_UNUSED(screenPos)
    return QVector3D(0, 0, 0);
}

QPoint SceneView3D::worldToScreen(const QVector3D &worldPos) const
{
    // TODO: Реализовать преобразование мировых координат в экранные
    Q_UNUSED(worldPos)
    return QPoint(0, 0);
}

QString SceneView3D::getEntityAt(const QPoint &screenPos) const
{
    // TODO: Реализовать определение объекта под курсором
    Q_UNUSED(screenPos)
    return QString();
}

void SceneView3D::createGrid()
{
    m_gridVertices.clear();
    m_gridIndices.clear();
    
    float gridSize = 1.0f;
    int gridCount = 20;
    
    for (int i = -gridCount; i <= gridCount; ++i) {
        // Вертикальные линии
        m_gridVertices.push_back(QVector3D(i * gridSize, 0, -gridCount * gridSize));
        m_gridVertices.push_back(QVector3D(i * gridSize, 0, gridCount * gridSize));
        
        // Горизонтальные линии
        m_gridVertices.push_back(QVector3D(-gridCount * gridSize, 0, i * gridSize));
        m_gridVertices.push_back(QVector3D(gridCount * gridSize, 0, i * gridSize));
    }
}

void SceneView3D::createTestEntities()
{
    // Создание тестовых объектов
    addEntity("Cube1", QVector3D(0, 0, 0), QVector3D(1, 1, 1));
    addEntity("Cube2", QVector3D(3, 0, 0), QVector3D(1, 1, 1));
    addEntity("Cube3", QVector3D(-3, 0, 0), QVector3D(1, 1, 1));
}

void SceneView3D::updateCamera()
{
    // Обновление позиции камеры на основе углов поворота
    float x = m_cameraDistance * cos(m_cameraRotationX) * sin(m_cameraRotationY);
    float y = m_cameraDistance * sin(m_cameraRotationX);
    float z = m_cameraDistance * cos(m_cameraRotationX) * cos(m_cameraRotationY);
    
    m_camera.position = QVector3D(x, y, z);
}

// Реализация Camera3D
QMatrix4x4 Camera3D::getViewMatrix() const
{
    QMatrix4x4 view;
    view.lookAt(position, target, up);
    return view;
}

QMatrix4x4 Camera3D::getProjectionMatrix(float aspectRatio) const
{
    QMatrix4x4 projection;
    projection.perspective(fov, aspectRatio, nearPlane, farPlane);
    return projection;
}

// Реализация SceneEntity3D
QMatrix4x4 SceneEntity3D::getModelMatrix() const
{
    QMatrix4x4 model;
    model.translate(position);
    model.rotate(rotation);
    model.scale(scale);
    return model;
}

void SceneView3D::onAssetSelected(const QString &assetPath)
{
    // Обработка выбора ассета
    Q_UNUSED(assetPath);
    // Здесь можно добавить логику для загрузки ассета в 3D сцену
}
