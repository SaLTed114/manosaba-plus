// Game/Common/Logger.cpp
#include "Logger.h"
#include <iostream>
#include <sstream>

namespace Salt2D::Game {

static const char* LevelTag(LogLevel level) {
    switch (level) {
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Info:  return "INFO";
    case LogLevel::Warn:  return "WARN";
    case LogLevel::Error: return "ERROR";
    default:              return "UNKNOWN";
    }
}

void Logger::Log(LogLevel level, const char* category, std::string message) const {
    if (static_cast<uint8_t>(level) < static_cast<uint8_t>(minLevel_)) return;

    if (sink_) {
        auto cat = category ? category : "";
        sink_(LogMessage{level, cat, std::move(message)});
        return;
    }

    std::cout
        << "[" << LevelTag(level) << "]"
        << "[" << (category ? category : "") << "] "
        << message << "\n";
}

Logger MakeConsoleLogger(LogLevel minLevel) {
    Logger logger;
    logger.SetMinLevel(minLevel);
    logger.SetSink([](const LogMessage& msg) {
        std::cout
            << "[" << LevelTag(msg.level) << "]"
            << "[" << msg.category << "] "
            << msg.message << "\n";
    });
    return logger;
}

} // namespace Salt2D::Game
