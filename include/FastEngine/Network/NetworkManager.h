#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <glm/glm.hpp>

namespace FastEngine {

/**
 * Типы сетевых сообщений
 */
enum class NetworkMessageType {
    Connect,
    Disconnect,
    PlayerJoin,
    PlayerLeave,
    PlayerUpdate,
    ObjectSpawn,
    ObjectDestroy,
    ObjectUpdate,
    Custom
};

/**
 * Сетевое сообщение
 */
struct NetworkMessage {
    NetworkMessageType type;
    std::string data;
    std::string senderId;
    uint32_t timestamp;
    uint32_t sequence;
    
    NetworkMessage() : type(NetworkMessageType::Custom), timestamp(0), sequence(0) {}
    NetworkMessage(NetworkMessageType t, const std::string& d, const std::string& sender = "")
        : type(t), data(d), senderId(sender), timestamp(0), sequence(0) {}
};

/**
 * Информация о игроке
 */
struct PlayerInfo {
    std::string id;
    std::string name;
    glm::vec3 position;
    glm::vec3 rotation;
    bool connected;
    uint32_t lastUpdateTime;
    
    PlayerInfo() : position(0.0f), rotation(0.0f), connected(false), lastUpdateTime(0) {}
    PlayerInfo(const std::string& playerId, const std::string& playerName)
        : id(playerId), name(playerName), position(0.0f), rotation(0.0f), connected(true), lastUpdateTime(0) {}
};

/**
 * Сетевой объект
 */
class NetworkObject {
public:
    NetworkObject();
    ~NetworkObject() = default;
    
    // Идентификация
    const std::string& GetId() const { return m_id; }
    void SetId(const std::string& id) { m_id = id; }
    
    const std::string& GetOwnerId() const { return m_ownerId; }
    void SetOwnerId(const std::string& ownerId) { m_ownerId = ownerId; }
    
    // Позиция и поворот
    const glm::vec3& GetPosition() const { return m_position; }
    void SetPosition(const glm::vec3& position) { m_position = position; }
    
    const glm::vec3& GetRotation() const { return m_rotation; }
    void SetRotation(const glm::vec3& rotation) { m_rotation = rotation; }
    
    // Состояние
    bool IsDirty() const { return m_dirty; }
    void SetDirty(bool dirty) { m_dirty = dirty; }
    
    // Синхронизация
    virtual std::string Serialize() const;
    virtual void Deserialize(const std::string& data);
    
    // Обновление
    virtual void Update(float deltaTime) {}
    
private:
    std::string m_id;
    std::string m_ownerId;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    bool m_dirty;
};

/**
 * Профилировщик сети
 */
class NetworkProfiler {
public:
    NetworkProfiler();
    ~NetworkProfiler() = default;
    
    // Метрики
    void RecordMessageSent(const NetworkMessage& message);
    void RecordMessageReceived(const NetworkMessage& message);
    void RecordLatency(uint32_t latency);
    void RecordBandwidth(float bytesPerSecond);
    
    // Получение статистики
    float GetAverageLatency() const { return m_averageLatency; }
    float GetBandwidthUsage() const { return m_bandwidthUsage; }
    uint32_t GetMessagesSent() const { return m_messagesSent; }
    uint32_t GetMessagesReceived() const { return m_messagesReceived; }
    uint32_t GetPacketsLost() const { return m_packetsLost; }
    
    // Сброс статистики
    void Reset();
    
private:
    float m_averageLatency;
    float m_bandwidthUsage;
    uint32_t m_messagesSent;
    uint32_t m_messagesReceived;
    uint32_t m_packetsLost;
    std::vector<uint32_t> m_latencyHistory;
    std::vector<float> m_bandwidthHistory;
    
    void UpdateAverageLatency();
    void UpdateBandwidthUsage();
};

/**
 * Менеджер сети
 */
class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();
    
    // Инициализация
    bool Initialize();
    void Shutdown();
    
    // Подключение
    bool Connect(const std::string& host, int port);
    bool Disconnect();
    bool IsConnected() const { return m_connected; }
    
    // Создание сервера
    bool StartServer(int port);
    bool StopServer();
    bool IsServer() const { return m_isServer; }
    
    // Управление игроками
    void AddPlayer(const std::string& playerId, const std::string& playerName);
    void RemovePlayer(const std::string& playerId);
    const std::vector<PlayerInfo>& GetPlayers() const { return m_players; }
    PlayerInfo* GetPlayer(const std::string& playerId);
    
    // Управление объектами
    void RegisterObject(std::shared_ptr<NetworkObject> object);
    void UnregisterObject(const std::string& objectId);
    std::shared_ptr<NetworkObject> GetObject(const std::string& objectId);
    const std::vector<std::shared_ptr<NetworkObject>>& GetObjects() const { return m_objects; }
    
    // Отправка сообщений
    void SendMessage(const NetworkMessage& message);
    void SendMessageToPlayer(const std::string& playerId, const NetworkMessage& message);
    void BroadcastMessage(const NetworkMessage& message);
    
    // Обновление
    void Update(float deltaTime);
    
    // Получение сообщений
    std::vector<NetworkMessage> GetReceivedMessages();
    
    // Callbacks
    void SetOnPlayerJoin(std::function<void(const PlayerInfo&)> callback) { m_onPlayerJoin = callback; }
    void SetOnPlayerLeave(std::function<void(const PlayerInfo&)> callback) { m_onPlayerLeave = callback; }
    void SetOnMessageReceived(std::function<void(const NetworkMessage&)> callback) { m_onMessageReceived = callback; }
    
    // Профилировщик
    NetworkProfiler& GetProfiler() { return m_profiler; }
    
private:
    bool m_initialized;
    bool m_connected;
    bool m_isServer;
    std::string m_host;
    int m_port;
    
    std::vector<PlayerInfo> m_players;
    std::vector<std::shared_ptr<NetworkObject>> m_objects;
    std::unordered_map<std::string, std::shared_ptr<NetworkObject>> m_objectMap;
    
    std::queue<NetworkMessage> m_sendQueue;
    std::queue<NetworkMessage> m_receiveQueue;
    std::mutex m_sendMutex;
    std::mutex m_receiveMutex;
    
    std::thread m_networkThread;
    std::atomic<bool> m_running;
    
    NetworkProfiler m_profiler;
    
    // Callbacks
    std::function<void(const PlayerInfo&)> m_onPlayerJoin;
    std::function<void(const PlayerInfo&)> m_onPlayerLeave;
    std::function<void(const NetworkMessage&)> m_onMessageReceived;
    
    // Внутренние методы
    void NetworkThreadFunction();
    void ProcessReceivedMessages();
    void ProcessSendQueue();
    void HandleMessage(const NetworkMessage& message);
    void UpdateObjects(float deltaTime);
    void SynchronizeObjects();
    
    // Сетевые операции
    bool SendData(const std::string& data);
    std::string ReceiveData();
    void HandleConnection();
    void HandleDisconnection();
};

/**
 * Репликатор объектов
 */
class ObjectReplicator {
public:
    ObjectReplicator();
    ~ObjectReplicator() = default;
    
    // Инициализация
    void Initialize(NetworkManager* networkManager);
    
    // Управление репликацией
    void StartReplicating(std::shared_ptr<NetworkObject> object);
    void StopReplicating(const std::string& objectId);
    void UpdateReplication(float deltaTime);
    
    // Настройки
    void SetReplicationRate(float rate) { m_replicationRate = rate; }
    void SetMaxReplicationDistance(float distance) { m_maxReplicationDistance = distance; }
    
private:
    NetworkManager* m_networkManager;
    std::vector<std::shared_ptr<NetworkObject>> m_replicatedObjects;
    float m_replicationRate;
    float m_maxReplicationDistance;
    float m_lastReplicationTime;
    
    bool ShouldReplicate(std::shared_ptr<NetworkObject> object);
    void ReplicateObject(std::shared_ptr<NetworkObject> object);
};

} // namespace FastEngine
