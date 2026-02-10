// Game/Session/StorySession.cpp
#include "StorySession.h"
#include "Game/Story/StoryGraphLoader.h"
#include "Game/Story/Resources/PerformanceDefLoader.h"
#include "Utils/FileUtils.h"

#include <Windows.h>

namespace Salt2D::Game::Session {

StorySession::StorySession(Utils::DiskFileSystem& fs) : fs_(fs) {}

void StorySession::Initialize(const std::filesystem::path& storyRoot, const std::filesystem::path& graphPath, const Story::NodeId& startNode) {
    logger_ = MakeConsoleAndFileLogger(
        Utils::GenerateTimestampedFilename("Logs/game_scene.log"),
        LogLevel::Debug, LogLevel::Debug);

    history_.SetLogger(&logger_);
    history_.Clear();

    storyRoot_ = storyRoot;

    const auto perfTablePath = storyRoot_ / "Performance/performance_table.json";
    tables_.perf = Story::LoadPerformanceTable(fs_, perfTablePath);

    const auto fullGraphPath = storyRoot_ / graphPath;
    graph_ = Story::LoadStoryGraph(fs_, fullGraphPath);

    player_ = std::make_unique<Story::StoryPlayer>(graph_, fs_);
    player_->SetLogger(&logger_);

    player_->SetEffectCallback([](const Salt2D::Game::Story::Effect& e) {
        // Placeholder: later route to Director/CameraRig/etc.
        OutputDebugStringA(("[Effect] type=" + e.type + " name=" + e.name + "\n").c_str());
    });

    player_->Start(startNode);
}

} // namespace Salt2D::Game::Session
