// Game/Session/StorySession.h
#ifndef GAME_SESSION_STORYSESSION_H
#define GAME_SESSION_STORYSESSION_H

#include <memory>
#include <filesystem>

#include "Utils/IFileSystem.h"
#include "StoryHistory.h"
#include "Utils/Logger.h"
#include "Game/Story/StoryGraph.h"
#include "Game/Story/StoryPlayer.h"
#include "Game/Story/StoryTables.h"

namespace Salt2D::Game::Session {

enum class TableLoadMask : uint32_t {
    None    = 0,
    Pref    = 1u << 0,
    Cast    = 1u << 1,
    Stage   = 1u << 2,
    All     = Pref | Cast | Stage,
};
inline TableLoadMask operator|(TableLoadMask a, TableLoadMask b) {
    return static_cast<TableLoadMask>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline bool HasTable(TableLoadMask mask, TableLoadMask table) {
    return (static_cast<uint32_t>(mask) & static_cast<uint32_t>(table)) != 0;
}

struct StorySessionConfig {
    std::filesystem::path storyRoot;
    std::filesystem::path graphPath;
    Story::NodeId startNode;

    TableLoadMask loadTables = TableLoadMask::All;

    bool enableLogger = true;
    std::filesystem::path logPath; // if empty, will generate a timestamped log file
    Utils::LogLevel consoleLevel = Utils::LogLevel::Debug;
    Utils::LogLevel fileLevel    = Utils::LogLevel::Debug;
};

class StorySession {
public:
    explicit StorySession(Utils::IFileSystem& fs);

    void Initialize(const std::filesystem::path& storyRoot, const std::filesystem::path& graphPath, const Story::NodeId& startNode);
    void Initialize(const StorySessionConfig& cfg);

    bool Ready() const { return player_ != nullptr; }

    const Story::StoryPlayer& Player() const { return *player_; }
    Story::StoryPlayer& Player() { return *player_; }

    const StoryHistory& History() const { return history_; }
    StoryHistory& History() { return history_; }

    const Story::StoryTables& Tables() const { return tables_; }
    Story::StoryTables& Tables() { return tables_; }

private:
    Utils::IFileSystem& fs_;
    Story::StoryGraph graph_;
    Story::StoryTables tables_;
    std::unique_ptr<Story::StoryPlayer> player_;
    Utils::Logger logger_;
    StoryHistory history_;

    std::filesystem::path storyRoot_;
};

} // namespace Salt2D::Game::Session

#endif // GAME_SESSION_STORYSESSION_H
