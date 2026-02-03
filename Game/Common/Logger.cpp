// Game/Common/Logger.cpp
#include "Logger.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <filesystem>

namespace Salt2D::Game {

static const inline char* EnsureCategory(const char* category) {
    return category ? category : "";
}

static const char* LevelTag(LogLevel level) {
    switch (level) {
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Info:  return "INFO";
    case LogLevel::Warn:  return "WARN";
    case LogLevel::Error: return "ERROR";
    default:              return "UNKNOWN";
    }
}

static std::string NowTimestamp() {
    using namespace std::chrono;

    auto now = system_clock::now();
    auto in_time_t = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::tm tm{};
#if defined(WIN32) || defined(_WIN32)
    localtime_s(&tm, &in_time_t);
#else
    localtime_r(&in_time_t, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

static std::string FormatLine(const LogMessage& msg) {
    std::ostringstream oss;
    oss << "[" << NowTimestamp() << "]"
        << "[" << LevelTag(msg.level) << "]"
        << "[" << EnsureCategory(msg.category) << "] "
        << msg.message;
    return oss.str();
}

void Logger::AddSink(LogLevel minLevel, Sink sink) {
    sinks_.push_back(SinkEntry{minLevel, std::move(sink)});
}

void Logger::ClearSinks() {
    sinks_.clear();
}

void Logger::Log(LogLevel level, const char* category, std::string message) const {
    LogMessage logMsg{level, EnsureCategory(category), std::move(message)};
    for (const auto& entry : sinks_) {
        if (static_cast<uint8_t>(level) < static_cast<uint8_t>(entry.minLevel)) continue;
        if (entry.sink) entry.sink(logMsg);
    }
}

Logger MakeConsoleAndFileLogger(
    const std::string& logFilePath,
    LogLevel consoleMinLevel,
    LogLevel fileMinLevel
) {
    Logger logger;

    // Console sink
    logger.AddSink(consoleMinLevel, [](const LogMessage& msg) {
        std::cout << FormatLine(msg) << std::endl;
    });

    // File sink
    std::filesystem::path logPath(logFilePath);
    if (logPath.has_parent_path()) {
        std::error_code ec;
        std::filesystem::create_directories(logPath.parent_path(), ec);
        if (ec) {
            logger.Warn("Logger", "Failed to create log directory: " + ec.message());
            return logger;
        }
    }

    auto file = std::make_shared<std::ofstream>(logFilePath, std::ios::binary | std::ios::app);
    if (!file->is_open()) {
        logger.Warn("Logger", "Failed to open log file: " + logFilePath);
        return logger;
    }

    auto fileMutex = std::make_shared<std::mutex>();
    logger.AddSink(fileMinLevel, [file, fileMutex](const LogMessage& msg) {
        std::lock_guard<std::mutex> lock(*fileMutex);
        (*file) << FormatLine(msg) << std::endl;
        file->flush();
    });

    return logger;
}

} // namespace Salt2D::Game
