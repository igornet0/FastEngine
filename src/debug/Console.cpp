#include "FastEngine/Debug/Console.h"
#include "FastEngine/Render/Renderer.h"
#include "FastEngine/Components/Text.h"
#include <chrono>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <iostream>

namespace FastEngine {
    Console& Console::GetInstance() {
        static Console instance;
        return instance;
    }
    
    bool Console::Initialize() {
        if (m_initialized) {
            return true;
        }
        
        m_visible = false;
        m_initialized = true;
        m_currentInput = "";
        m_historyIndex = -1;
        
        // Настройки логов
        m_maxLogEntries = 1000;
        m_logLifetime = 10.0f;
        
        // Настройки истории
        m_maxHistorySize = 100;
        
        // Настройки UI
        m_consoleHeight = 300.0f;
        m_backgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.8f);
        m_textColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        m_inputColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        
        // Инициализируем фильтры логов
        m_logFilters[LogLevel::Info] = true;
        m_logFilters[LogLevel::Warning] = true;
        m_logFilters[LogLevel::Error] = true;
        m_logFilters[LogLevel::Debug] = false; // Debug по умолчанию выключен
        
        // Регистрируем стандартные команды
        RegisterDefaultCommands();
        
        return true;
    }
    
    void Console::Shutdown() {
        m_logs.clear();
        m_history.clear();
        m_commands.clear();
        m_initialized = false;
    }
    
    void Console::Toggle() {
        m_visible = !m_visible;
    }
    
    void Console::Show() {
        m_visible = true;
    }
    
    void Console::Hide() {
        m_visible = false;
    }
    
    void Console::Log(LogLevel level, const std::string& message) {
        if (!m_initialized) {
            return;
        }
        
        std::string timestamp = GetCurrentTimestamp();
        LogEntry entry(level, message, timestamp, m_logLifetime);
        m_logs.push_back(entry);
        
        // Ограничиваем количество логов
        if (static_cast<int>(m_logs.size()) > m_maxLogEntries) {
            m_logs.erase(m_logs.begin());
        }
    }
    
    void Console::LogInfo(const std::string& message) {
        Log(LogLevel::Info, message);
    }
    
    void Console::LogWarning(const std::string& message) {
        Log(LogLevel::Warning, message);
    }
    
    void Console::LogError(const std::string& message) {
        Log(LogLevel::Error, message);
    }
    
    void Console::LogDebug(const std::string& message) {
        Log(LogLevel::Debug, message);
    }
    
    void Console::RegisterCommand(const std::string& name, std::function<void(const std::vector<std::string>&)> callback) {
        m_commands[name] = callback;
    }
    
    void Console::UnregisterCommand(const std::string& name) {
        auto it = m_commands.find(name);
        if (it != m_commands.end()) {
            m_commands.erase(it);
        }
    }
    
    void Console::ExecuteCommand(const std::string& command) {
        if (command.empty()) {
            return;
        }
        
        // Добавляем в историю
        AddToHistory(command);
        
        // Парсим команду
        std::vector<std::string> args = ParseCommand(command);
        if (args.empty()) {
            return;
        }
        
        std::string commandName = args[0];
        args.erase(args.begin()); // Удаляем имя команды из аргументов
        
        // Ищем команду
        auto it = m_commands.find(commandName);
        if (it != m_commands.end()) {
            try {
                it->second(args);
            } catch (const std::exception& e) {
                LogError("Command execution failed: " + std::string(e.what()));
            }
        } else {
            LogError("Unknown command: " + commandName);
        }
    }
    
    void Console::ProcessInput(const std::string& input) {
        m_currentInput = input;
    }
    
    void Console::AddToHistory(const std::string& command) {
        if (command.empty()) {
            return;
        }
        
        // Проверяем, не дублируется ли команда
        if (!m_history.empty() && m_history.back() == command) {
            return;
        }
        
        m_history.push_back(command);
        
        // Ограничиваем размер истории
        if (static_cast<int>(m_history.size()) > m_maxHistorySize) {
            m_history.erase(m_history.begin());
        }
        
        m_historyIndex = static_cast<int>(m_history.size());
    }
    
    std::string Console::GetHistoryEntry(int index) const {
        if (index < 0 || index >= static_cast<int>(m_history.size())) {
            return "";
        }
        return m_history[index];
    }
    
    int Console::GetHistorySize() const {
        return static_cast<int>(m_history.size());
    }
    
    void Console::Update(float deltaTime) {
        if (!m_visible || !m_initialized) {
            return;
        }
        
        UpdateLogs(deltaTime);
    }
    
    void Console::Render(Renderer* renderer) {
        if (!m_visible || !m_initialized || !renderer) {
            return;
        }
        
        RenderBackground(renderer);
        RenderLogs(renderer);
        RenderInput(renderer);
    }
    
    void Console::SetLogFilter(LogLevel level, bool enabled) {
        m_logFilters[level] = enabled;
    }
    
    bool Console::IsLogLevelEnabled(LogLevel level) const {
        auto it = m_logFilters.find(level);
        return it != m_logFilters.end() ? it->second : false;
    }
    
    void Console::ClearLogs() {
        m_logs.clear();
    }
    
    void Console::ClearHistory() {
        m_history.clear();
        m_historyIndex = -1;
    }
    
    std::vector<std::string> Console::GetCommandSuggestions(const std::string& partial) const {
        std::vector<std::string> suggestions;
        
        for (const auto& pair : m_commands) {
            if (pair.first.find(partial) == 0) {
                suggestions.push_back(pair.first);
            }
        }
        
        return suggestions;
    }
    
    std::string Console::CompleteCommand(const std::string& partial) const {
        auto suggestions = GetCommandSuggestions(partial);
        if (suggestions.size() == 1) {
            return suggestions[0];
        }
        return partial;
    }
    
    std::string Console::GetCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        
        return ss.str();
    }
    
    void Console::UpdateLogs(float deltaTime) {
        for (auto it = m_logs.begin(); it != m_logs.end();) {
            it->lifetime -= deltaTime;
            if (it->lifetime <= 0.0f) {
                it = m_logs.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void Console::RenderBackground(Renderer* renderer) {
        // В реальной реализации здесь бы рендерился фон консоли
        // Для простоты пропускаем
    }
    
    void Console::RenderLogs(Renderer* renderer) {
        // В реальной реализации здесь бы рендерились логи
        // Для простоты выводим в консоль
        for (const auto& log : m_logs) {
            if (IsLogLevelEnabled(log.level)) {
                std::cout << "[" << log.timestamp << "] " << log.message << std::endl;
            }
        }
    }
    
    void Console::RenderInput(Renderer* renderer) {
        // В реальной реализации здесь бы рендерился ввод
        // Для простоты выводим в консоль
        if (!m_currentInput.empty()) {
            std::cout << "> " << m_currentInput << std::endl;
        }
    }
    
    std::vector<std::string> Console::ParseCommand(const std::string& command) const {
        std::vector<std::string> args;
        std::stringstream ss(command);
        std::string arg;
        
        while (ss >> arg) {
            args.push_back(arg);
        }
        
        return args;
    }
    
    void Console::RegisterDefaultCommands() {
        // Команда help
        RegisterCommand("help", [this](const std::vector<std::string>& args) {
            LogInfo("Available commands:");
            for (const auto& pair : m_commands) {
                LogInfo("  " + pair.first);
            }
        });
        
        // Команда clear
        RegisterCommand("clear", [this](const std::vector<std::string>& args) {
            ClearLogs();
            LogInfo("Console cleared");
        });
        
        // Команда echo
        RegisterCommand("echo", [this](const std::vector<std::string>& args) {
            std::string message;
            for (const auto& arg : args) {
                message += arg + " ";
            }
            LogInfo(message);
        });
        
        // Команда quit
        RegisterCommand("quit", [this](const std::vector<std::string>& args) {
            LogInfo("Exiting...");
            // В реальной реализации здесь бы закрывалось приложение
        });
    }
}
