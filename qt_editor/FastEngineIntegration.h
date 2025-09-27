#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QMap>
#include <QVector3D>
#include <QVector2D>
#include <QColor>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QDebug>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <memory>
#include <vector>
#include <map>
#include <string>

// Forward declarations для FastEngine
namespace FastEngine {
    class Engine;
    class World;
    class Entity;
    class Component;
    class Renderer;
    class Camera;
    class Texture;
    class Sprite;
    class Mesh;
    class Shader;
    class AudioManager;
    class InputManager;
    class PhysicsSystem;
    class AnimationSystem;
    class ResourceManager;
}

// Структуры данных для интеграции
struct EntityData {
    QString name;
    QString type;
    QVector3D position;
    QQuaternion rotation;
    QVector3D scale;
    QColor color;
    bool visible;
    bool selected;
    QMap<QString, QVariant> properties;
    
    EntityData() 
        : position(0, 0, 0)
        , rotation(1, 0, 0, 0)
        , scale(1, 1, 1)
        , color(Qt::blue)
        , visible(true)
        , selected(false) {}
};

struct ComponentData {
    QString name;
    QString type;
    QMap<QString, QVariant> properties;
    
    ComponentData() {}
    ComponentData(const QString &name, const QString &type) 
        : name(name), type(type) {}
};

struct SceneData {
    QString name;
    QStringList entities;
    QMap<QString, EntityData> entityData;
    QVector3D cameraPosition;
    QVector3D cameraTarget;
    float cameraFOV;
    QColor backgroundColor;
    QVector3D ambientLight;
    QVector3D directionalLight;
    
    SceneData() 
        : name("Untitled Scene")
        , cameraPosition(0, 0, 10)
        , cameraTarget(0, 0, 0)
        , cameraFOV(45.0f)
        , backgroundColor(Qt::black)
        , ambientLight(0.2f, 0.2f, 0.2f)
        , directionalLight(1.0f, 1.0f, 1.0f) {}
};

class FastEngineIntegration : public QObject
{
    Q_OBJECT

public:
    explicit FastEngineIntegration(QObject *parent = nullptr);
    ~FastEngineIntegration();

    // Инициализация и завершение
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized; }
    
    // Управление движком
    void update(float deltaTime);
    void render();
    void setViewport(int x, int y, int width, int height);
    
    // Управление сценами
    void createScene(const QString &name);
    void loadScene(const QString &name);
    void saveScene(const QString &name);
    void deleteScene(const QString &name);
    QStringList getScenes() const;
    QString getCurrentScene() const;
    
    // Управление объектами
    void createEntity(const QString &name, const QVector3D &position = QVector3D(0, 0, 0));
    void deleteEntity(const QString &name);
    void selectEntity(const QString &name);
    void clearSelection();
    QStringList getSelectedEntities() const;
    QStringList getEntities() const;
    
    // Управление компонентами
    void addComponent(const QString &entityName, const QString &componentType);
    void removeComponent(const QString &entityName, const QString &componentType);
    void setComponentProperty(const QString &entityName, const QString &componentType, 
                            const QString &propertyName, const QVariant &value);
    QVariant getComponentProperty(const QString &entityName, const QString &componentType, 
                                const QString &propertyName) const;
    
    // Управление свойствами объектов
    void setEntityPosition(const QString &name, const QVector3D &position);
    void setEntityRotation(const QString &name, const QQuaternion &rotation);
    void setEntityScale(const QString &name, const QVector3D &scale);
    void setEntityVisible(const QString &name, bool visible);
    void setEntityColor(const QString &name, const QColor &color);
    
    QVector3D getEntityPosition(const QString &name) const;
    QQuaternion getEntityRotation(const QString &name) const;
    QVector3D getEntityScale(const QString &name) const;
    bool getEntityVisible(const QString &name) const;
    QColor getEntityColor(const QString &name) const;
    
    // Управление камерой
    void setCameraPosition(const QVector3D &position);
    void setCameraTarget(const QVector3D &target);
    void setCameraFOV(float fov);
    QVector3D getCameraPosition() const;
    QVector3D getCameraTarget() const;
    float getCameraFOV() const;
    
    // Управление освещением
    void setAmbientLight(const QVector3D &color);
    void setDirectionalLight(const QVector3D &direction, const QVector3D &color);
    QVector3D getAmbientLight() const;
    QVector3D getDirectionalLight() const;
    
    // Управление ресурсами
    void loadTexture(const QString &name, const QString &filePath);
    void loadMesh(const QString &name, const QString &filePath);
    void loadAudio(const QString &name, const QString &filePath);
    void loadShader(const QString &name, const QString &filePath);
    
    QStringList getTextures() const;
    QStringList getMeshes() const;
    QStringList getAudioClips() const;
    QStringList getShaders() const;
    
    // Управление проектом
    void newProject(const QString &name);
    void loadProject(const QString &filePath);
    void saveProject(const QString &filePath);
    void closeProject();
    
    QString getProjectName() const;
    QString getProjectPath() const;
    bool isProjectModified() const;
    
    // Получение данных
    EntityData getEntityData(const QString &name) const;
    SceneData getSceneData(const QString &name) const;
    QMap<QString, ComponentData> getEntityComponents(const QString &name) const;

signals:
    void entityCreated(const QString &name);
    void entityDeleted(const QString &name);
    void entitySelected(const QString &name);
    void selectionChanged(const QStringList &selectedEntities);
    void entityMoved(const QString &name, const QVector3D &position);
    void entityRotated(const QString &name, const QQuaternion &rotation);
    void entityScaled(const QString &name, const QVector3D &scale);
    void entityPropertyChanged(const QString &name, const QString &property, const QVariant &value);
    
    void sceneCreated(const QString &name);
    void sceneLoaded(const QString &name);
    void sceneSaved(const QString &name);
    void sceneDeleted(const QString &name);
    
    void projectLoaded(const QString &name);
    void projectSaved(const QString &name);
    void projectClosed();
    
    void errorOccurred(const QString &message);
    void warningOccurred(const QString &message);
    void infoOccurred(const QString &message);

private slots:
    void onUpdateTimer();

private:
    void setupConnections();
    void updateEntityData();
    void updateSceneData();
    void processUpdateQueue();
    
    // Внутренние методы
    FastEngine::Entity* findEntity(const QString &name) const;
    FastEngine::Component* findComponent(const QString &entityName, const QString &componentType) const;
    
    // FastEngine объекты
    FastEngine::Engine* m_engine;
    FastEngine::World* m_world;
    FastEngine::Renderer* m_renderer;
    FastEngine::Camera* m_camera;
    FastEngine::AudioManager* m_audioManager;
    FastEngine::InputManager* m_inputManager;
    FastEngine::PhysicsSystem* m_physicsSystem;
    FastEngine::AnimationSystem* m_animationSystem;
    // FastEngine::ResourceManager* m_resourceManager;
    
    // Состояние
    bool m_initialized;
    QString m_currentScene;
    QString m_projectName;
    QString m_projectPath;
    bool m_projectModified;
    
    // Данные
    QMap<QString, EntityData> m_entityData;
    QMap<QString, SceneData> m_sceneData;
    QStringList m_selectedEntities;
    
    // Таймер обновления
    QTimer *m_updateTimer;
    
    // Поток обновления
    QThread *m_updateThread;
    QMutex m_updateMutex;
    QWaitCondition m_updateCondition;
    bool m_stopUpdate;
    
    // Очередь команд
    QQueue<QString> m_commandQueue;
    QMutex m_commandMutex;
};

// Forward declaration для ProjectManager
class ProjectManager;

