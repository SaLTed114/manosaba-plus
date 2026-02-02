// Game/Common/Logger.h
#ifndef GAME_COMMON_LOGGER_H
#define GAME_COMMON_LOGGER_H

#include <string>
#include <functional>
#include <cstdint>

namespace Salt2D::Game {
    
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

    Logger() = default;

    void SetMinLevel(LogLevel level) { minLevel_ = level; }
    LogLevel GetMinLevel() const { return minLevel_; }

    void SetSink(Sink sink) { sink_ = std::move(sink); }

    void Log(LogLevel level, const char* category, std::string message) const;

    void Debug(const char* cat, std::string msg) const { Log(LogLevel::Debug, cat, std::move(msg)); }
    void Info (const char* cat, std::string msg) const { Log(LogLevel::Info,  cat, std::move(msg)); }
    void Warn (const char* cat, std::string msg) const { Log(LogLevel::Warn,  cat, std::move(msg)); }
    void Error(const char* cat, std::string msg) const { Log(LogLevel::Error, cat, std::move(msg)); }

private:
    LogLevel minLevel_ = LogLevel::Info;
    Sink sink_;
};

Logger MakeConsoleLogger(LogLevel minLevel = LogLevel::Info);

} // namespace Salt2D::Game

#endif // GAME_COMMON_LOGGER_H
