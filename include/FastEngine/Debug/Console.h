#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <memory>
#include <glm/glm.hpp>

namespace FastEngine {
    class Renderer;
    class Text;
    
    enum class LogLevel {
        Info,
        Warning,
        Error,
        Debug
    };
    
    struct LogEntry {
        LogLevel level;
        std::string message;
        std::string timestamp;
        float lifetime;
        
        LogEntry(LogLevel lvl, const std::string& msg, const std::string& time, float life = 5.0f)
            : level(lvl), message(msg), timestamp(time), lifetime(life) {}
    };
    
    class Console {
    public:
        static Console& GetInstance();
        
        // Инициализация и завершение
        bool Initialize();
        void Shutdown();
        
        // Управление консолью
        void Toggle();
        void Show();
        void Hide();
        bool IsVisible() const { return m_visible; }
        
        // Логирование
        void Log(LogLevel level, const std::string& message);
        void LogInfo(const std::string& message);
        void LogWarning(const std::string& message);
        void LogError(const std::string& message);
        void LogDebug(const std::string& message);
        
        // Команды
        void RegisterCommand(const std::string& name, std::function<void(const std::vector<std::string>&)> callback);
        void UnregisterCommand(const std::string& name);
        void ExecuteCommand(const std::string& command);
        
        // Ввод
        void ProcessInput(const std::string& input);
        void AddToHistory(const std::string& command);
        std::string GetHistoryEntry(int index) const;
        int GetHistorySize() const;
        
        // Обновление и рендеринг
        void Update(float deltaTime);
        void Render(Renderer* renderer);
        
        // Настройки
        void SetMaxLogEntries(int max) { m_maxLogEntries = max; }
        int GetMaxLogEntries() const { return m_maxLogEntries; }
        
        void SetLogLifetime(float lifetime) { m_logLifetime = lifetime; }
        float GetLogLifetime() const { return m_logLifetime; }
        
        void SetConsoleHeight(float height) { m_consoleHeight = height; }
        float GetConsoleHeight() const { return m_consoleHeight; }
        
        // Фильтрация логов
        void SetLogFilter(LogLevel level, bool enabled);
        bool IsLogLevelEnabled(LogLevel level) const;
        
        // Очистка
        void ClearLogs();
        void ClearHistory();
        
        // Получение информации
        const std::vector<LogEntry>& GetLogs() const { return m_logs; }
        const std::vector<std::string>& GetHistory() const { return m_history; }
        
        // Автодополнение
        std::vector<std::string> GetCommandSuggestions(const std::string& partial) const;
        std::string CompleteCommand(const std::string& partial) const;
        
    private:
        Console() = default;
        ~Console() = default;
        
        // Запрещаем копирование
        Console(const Console&) = delete;
        Console& operator=(const Console&) = delete;
        
        // Состояние
        bool m_visible;
        bool m_initialized;
        std::string m_currentInput;
        int m_historyIndex;
        
        // Логи
        std::vector<LogEntry> m_logs;
        int m_maxLogEntries;
        float m_logLifetime;
        
        // История команд
        std::vector<std::string> m_history;
        int m_maxHistorySize;
        
        // Команды
        std::map<std::string, std::function<void(const std::vector<std::string>&)>> m_commands;
        
        // Фильтры логов
        std::map<LogLevel, bool> m_logFilters;
        
        // UI
        float m_consoleHeight;
        glm::vec4 m_backgroundColor;
        glm::vec4 m_textColor;
        glm::vec4 m_inputColor;
        
        // Вспомогательные методы
        std::string GetCurrentTimestamp() const;
        void UpdateLogs(float deltaTime);
        void RenderBackground(Renderer* renderer);
        void RenderLogs(Renderer* renderer);
        void RenderInput(Renderer* renderer);
        std::vector<std::string> ParseCommand(const std::string& command) const;
        void RegisterDefaultCommands();
    };
}

