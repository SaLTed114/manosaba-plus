// Utils/Logger.h
#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H

#include <string>
#include <functional>
#include <cstdint>

namespace Salt2D::Utils {
    
enum class LogLevel : uint8_t {
    Debug = 0,
    Info  = 1,
    Warn  = 2,
    Error = 3,
};

struct LogMessage {
    LogLevel level = LogLevel::Info;
    const char* category = "";
    std::string message;
};

class Logger {
public:
    using Sink = std::function<void(const LogMessage&)>;

    struct SinkEntry {
        LogLevel minLevel;
        Sink sink;
    };

    void AddSink(LogLevel minLevel, Sink sink);
    void ClearSinks();

    Logger() = default;

    void Log(LogLevel level, const char* category, std::string message) const;

    void Debug(const char* cat, std::string msg) const { Log(LogLevel::Debug, cat, std::move(msg)); }
    void Info (const char* cat, std::string msg) const { Log(LogLevel::Info,  cat, std::move(msg)); }
    void Warn (const char* cat, std::string msg) const { Log(LogLevel::Warn,  cat, std::move(msg)); }
    void Error(const char* cat, std::string msg) const { Log(LogLevel::Error, cat, std::move(msg)); }

private:
    std::vector<SinkEntry> sinks_;
};

Logger MakeConsoleAndFileLogger(
    const std::string& logFilePath,
    LogLevel consoleMinLevel = LogLevel::Info,
    LogLevel fileMinLevel = LogLevel::Debug
);

} // namespace Salt2D::Utils

#endif // UTILS_LOGGER_H
