// Game/Session/StorySession.h
#ifndef GAME_SESSION_STORYSESSION_H
#define GAME_SESSION_STORYSESSION_H

#include <memory>
#include <filesystem>

#include "Utils/DiskFileSystem.h"
#include "StoryHistory.h"
#include "Game/Common/Logger.h"
#include "Game/Story/StoryGraph.h"
#include "Game/Story/StoryPlayer.h"
#include "Game/Story/StoryTables.h"

namespace Salt2D::Game::Session {

class StorySession {
public:
    explicit StorySession(Utils::DiskFileSystem& fs);

    void Initialize(const std::filesystem::path& storyRoot, const std::filesystem::path& graphPath, const Story::NodeId& startNode);

    bool Ready() const { return player_ != nullptr; }

    const Story::StoryPlayer& Player() const { return *player_; }
    Story::StoryPlayer& Player() { return *player_; }

    const StoryHistory& History() const { return history_; }
    StoryHistory& History() { return history_; }

    const Story::StoryTables& Tables() const { return tables_; }
    Story::StoryTables& Tables() { return tables_; }

private:
    Utils::DiskFileSystem& fs_;
    Story::StoryGraph graph_;
    Story::StoryTables tables_;
    std::unique_ptr<Story::StoryPlayer> player_;
    Logger logger_;
    StoryHistory history_;

    std::filesystem::path storyRoot_;
};

} // namespace Salt2D::Game::Session

#endif // GAME_SESSION_STORYSESSION_H
