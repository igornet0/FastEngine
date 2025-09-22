#include "FastEngine/Network/NetworkManager.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace FastEngine {

// NetworkObject implementation
NetworkObject::NetworkObject() 
    : m_position(0.0f)
    , m_rotation(0.0f)
    , m_dirty(false) {
}

std::string NetworkObject::Serialize() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"id\": \"" << m_id << "\",\n";
    ss << "  \"ownerId\": \"" << m_ownerId << "\",\n";
    ss << "  \"position\": {\"x\": " << m_position.x << ", \"y\": " << m_position.y << ", \"z\": " << m_position.z << "},\n";
    ss << "  \"rotation\": {\"x\": " << m_rotation.x << ", \"y\": " << m_rotation.y << ", \"z\": " << m_rotation.z << "}\n";
    ss << "}";
    return ss.str();
}

void NetworkObject::Deserialize(const std::string& data) {
    std::cout << "Deserializing network object: " << m_id << std::endl;
}

// NetworkProfiler implementation
NetworkProfiler::NetworkProfiler() 
    : m_averageLatency(0.0f)
    , m_bandwidthUsage(0.0f)
    , m_messagesSent(0)
    , m_messagesReceived(0)
    , m_packetsLost(0) {
}

void NetworkProfiler::RecordMessageSent(const NetworkMessage& message) {
    ++m_messagesSent;
    std::cout << "NetworkProfiler: Message sent: " << static_cast<int>(message.type) << std::endl;
}

void NetworkProfiler::RecordMessageReceived(const NetworkMessage& message) {
    ++m_messagesReceived;
    std::cout << "NetworkProfiler: Message received: " << static_cast<int>(message.type) << std::endl;
}

void NetworkProfiler::RecordLatency(uint32_t latency) {
    m_latencyHistory.push_back(latency);
    if (m_latencyHistory.size() > 100) {
        m_latencyHistory.erase(m_latencyHistory.begin());
    }
    UpdateAverageLatency();
}

void NetworkProfiler::RecordBandwidth(float bytesPerSecond) {
    m_bandwidthHistory.push_back(bytesPerSecond);
    if (m_bandwidthHistory.size() > 100) {
        m_bandwidthHistory.erase(m_bandwidthHistory.begin());
    }
    UpdateBandwidthUsage();
}

void NetworkProfiler::Reset() {
    m_averageLatency = 0.0f;
    m_bandwidthUsage = 0.0f;
    m_messagesSent = 0;
    m_messagesReceived = 0;
    m_packetsLost = 0;
    m_latencyHistory.clear();
    m_bandwidthHistory.clear();
    std::cout << "NetworkProfiler: Reset statistics" << std::endl;
}

void NetworkProfiler::UpdateAverageLatency() {
    if (m_latencyHistory.empty()) {
        m_averageLatency = 0.0f;
        return;
    }
    
    float sum = 0.0f;
    for (uint32_t latency : m_latencyHistory) {
        sum += latency;
    }
    m_averageLatency = sum / m_latencyHistory.size();
}

void NetworkProfiler::UpdateBandwidthUsage() {
    if (m_bandwidthHistory.empty()) {
        m_bandwidthUsage = 0.0f;
        return;
    }
    
    float sum = 0.0f;
    for (float bandwidth : m_bandwidthHistory) {
        sum += bandwidth;
    }
    m_bandwidthUsage = sum / m_bandwidthHistory.size();
}

// NetworkManager implementation
NetworkManager::NetworkManager() 
    : m_initialized(false)
    , m_connected(false)
    , m_isServer(false)
    , m_port(0)
    , m_running(false) {
}

NetworkManager::~NetworkManager() {
    if (m_running) {
        m_running = false;
        if (m_networkThread.joinable()) {
            m_networkThread.join();
        }
    }
}

bool NetworkManager::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    m_initialized = true;
    std::cout << "NetworkManager initialized successfully" << std::endl;
    return true;
}

void NetworkManager::Shutdown() {
    if (m_connected) {
        Disconnect();
    }
    
    if (m_isServer) {
        StopServer();
    }
    
    m_initialized = false;
    std::cout << "NetworkManager shutdown" << std::endl;
}

bool NetworkManager::Connect(const std::string& host, int port) {
    if (!m_initialized) {
        std::cerr << "NetworkManager: Not initialized" << std::endl;
        return false;
    }
    
    m_host = host;
    m_port = port;
    m_connected = true;
    
    // Запускаем сетевой поток
    m_running = true;
    m_networkThread = std::thread(&NetworkManager::NetworkThreadFunction, this);
    
    std::cout << "NetworkManager: Connected to " << host << ":" << port << std::endl;
    return true;
}

bool NetworkManager::Disconnect() {
    if (!m_connected) {
        return false;
    }
    
    m_connected = false;
    m_running = false;
    
    if (m_networkThread.joinable()) {
        m_networkThread.join();
    }
    
    std::cout << "NetworkManager: Disconnected" << std::endl;
    return true;
}

bool NetworkManager::StartServer(int port) {
    if (!m_initialized) {
        std::cerr << "NetworkManager: Not initialized" << std::endl;
        return false;
    }
    
    m_port = port;
    m_isServer = true;
    m_connected = true;
    
    // Запускаем сетевой поток
    m_running = true;
    m_networkThread = std::thread(&NetworkManager::NetworkThreadFunction, this);
    
    std::cout << "NetworkManager: Started server on port " << port << std::endl;
    return true;
}

bool NetworkManager::StopServer() {
    if (!m_isServer) {
        return false;
    }
    
    m_isServer = false;
    m_connected = false;
    m_running = false;
    
    if (m_networkThread.joinable()) {
        m_networkThread.join();
    }
    
    std::cout << "NetworkManager: Stopped server" << std::endl;
    return true;
}

void NetworkManager::AddPlayer(const std::string& playerId, const std::string& playerName) {
    PlayerInfo player(playerId, playerName);
    m_players.push_back(player);
    
    if (m_onPlayerJoin) {
        m_onPlayerJoin(player);
    }
    
    std::cout << "NetworkManager: Added player " << playerName << " (ID: " << playerId << ")" << std::endl;
}

void NetworkManager::RemovePlayer(const std::string& playerId) {
    auto it = std::find_if(m_players.begin(), m_players.end(),
        [&playerId](const PlayerInfo& player) {
            return player.id == playerId;
        });
    
    if (it != m_players.end()) {
        PlayerInfo player = *it;
        m_players.erase(it);
        
        if (m_onPlayerLeave) {
            m_onPlayerLeave(player);
        }
        
        std::cout << "NetworkManager: Removed player " << playerId << std::endl;
    }
}

PlayerInfo* NetworkManager::GetPlayer(const std::string& playerId) {
    auto it = std::find_if(m_players.begin(), m_players.end(),
        [&playerId](const PlayerInfo& player) {
            return player.id == playerId;
        });
    
    return (it != m_players.end()) ? &(*it) : nullptr;
}

void NetworkManager::RegisterObject(std::shared_ptr<NetworkObject> object) {
    if (object) {
        m_objects.push_back(object);
        m_objectMap[object->GetId()] = object;
        std::cout << "NetworkManager: Registered object " << object->GetId() << std::endl;
    }
}

void NetworkManager::UnregisterObject(const std::string& objectId) {
    auto it = m_objectMap.find(objectId);
    if (it != m_objectMap.end()) {
        m_objectMap.erase(it);
        
        m_objects.erase(
            std::remove_if(m_objects.begin(), m_objects.end(),
                [&objectId](const std::shared_ptr<NetworkObject>& obj) {
                    return obj && obj->GetId() == objectId;
                }),
            m_objects.end()
        );
        
        std::cout << "NetworkManager: Unregistered object " << objectId << std::endl;
    }
}

std::shared_ptr<NetworkObject> NetworkManager::GetObject(const std::string& objectId) {
    auto it = m_objectMap.find(objectId);
    return (it != m_objectMap.end()) ? it->second : nullptr;
}

void NetworkManager::SendMessage(const NetworkMessage& message) {
    std::lock_guard<std::mutex> lock(m_sendMutex);
    m_sendQueue.push(message);
    m_profiler.RecordMessageSent(message);
}

void NetworkManager::SendMessageToPlayer(const std::string& playerId, const NetworkMessage& message) {
    // В реальной реализации здесь бы отправлялось сообщение конкретному игроку
    SendMessage(message);
    std::cout << "NetworkManager: Sent message to player " << playerId << std::endl;
}

void NetworkManager::BroadcastMessage(const NetworkMessage& message) {
    // В реальной реализации здесь бы отправлялось сообщение всем игрокам
    SendMessage(message);
    std::cout << "NetworkManager: Broadcasted message" << std::endl;
}

void NetworkManager::Update(float deltaTime) {
    if (!m_initialized) {
        return;
    }
    
    ProcessReceivedMessages();
    ProcessSendQueue();
    UpdateObjects(deltaTime);
    SynchronizeObjects();
}

std::vector<NetworkMessage> NetworkManager::GetReceivedMessages() {
    std::lock_guard<std::mutex> lock(m_receiveMutex);
    std::vector<NetworkMessage> messages;
    
    while (!m_receiveQueue.empty()) {
        messages.push_back(m_receiveQueue.front());
        m_receiveQueue.pop();
    }
    
    return messages;
}

void NetworkManager::NetworkThreadFunction() {
    std::cout << "NetworkManager: Network thread started" << std::endl;
    
    while (m_running) {
        // В реальной реализации здесь бы обрабатывались сетевые соединения
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    std::cout << "NetworkManager: Network thread stopped" << std::endl;
}

void NetworkManager::ProcessReceivedMessages() {
    std::lock_guard<std::mutex> lock(m_receiveMutex);
    
    while (!m_receiveQueue.empty()) {
        NetworkMessage message = m_receiveQueue.front();
        m_receiveQueue.pop();
        
        m_profiler.RecordMessageReceived(message);
        HandleMessage(message);
        
        if (m_onMessageReceived) {
            m_onMessageReceived(message);
        }
    }
}

void NetworkManager::ProcessSendQueue() {
    std::lock_guard<std::mutex> lock(m_sendMutex);
    
    while (!m_sendQueue.empty()) {
        NetworkMessage message = m_sendQueue.front();
        m_sendQueue.pop();
        
        // В реальной реализации здесь бы отправлялись данные по сети
        std::cout << "NetworkManager: Sending message type " << static_cast<int>(message.type) << std::endl;
    }
}

void NetworkManager::HandleMessage(const NetworkMessage& message) {
    switch (message.type) {
        case NetworkMessageType::Connect:
            std::cout << "NetworkManager: Handle connect message" << std::endl;
            break;
        case NetworkMessageType::Disconnect:
            std::cout << "NetworkManager: Handle disconnect message" << std::endl;
            break;
        case NetworkMessageType::PlayerJoin:
            std::cout << "NetworkManager: Handle player join message" << std::endl;
            break;
        case NetworkMessageType::PlayerLeave:
            std::cout << "NetworkManager: Handle player leave message" << std::endl;
            break;
        case NetworkMessageType::PlayerUpdate:
            std::cout << "NetworkManager: Handle player update message" << std::endl;
            break;
        case NetworkMessageType::ObjectSpawn:
            std::cout << "NetworkManager: Handle object spawn message" << std::endl;
            break;
        case NetworkMessageType::ObjectDestroy:
            std::cout << "NetworkManager: Handle object destroy message" << std::endl;
            break;
        case NetworkMessageType::ObjectUpdate:
            std::cout << "NetworkManager: Handle object update message" << std::endl;
            break;
        case NetworkMessageType::Custom:
            std::cout << "NetworkManager: Handle custom message" << std::endl;
            break;
    }
}

void NetworkManager::UpdateObjects(float deltaTime) {
    for (auto& object : m_objects) {
        if (object) {
            object->Update(deltaTime);
        }
    }
}

void NetworkManager::SynchronizeObjects() {
    for (auto& object : m_objects) {
        if (object && object->IsDirty()) {
            // В реальной реализации здесь бы синхронизировались объекты
            object->SetDirty(false);
        }
    }
}

} // namespace FastEngine

