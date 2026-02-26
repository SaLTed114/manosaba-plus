// Game/Session/StorySession.cpp
#include "StorySession.h"
#include "Game/Story/StoryGraphLoader.h"
#include "Game/Story/Resources/PerformanceDefLoader.h"
#include "Game/Story/Resources/CastDefLoader.h"
#include "Game/Story/Resources/StageDefLoader.h"
#include "Utils/FileUtils.h"

#include <Windows.h>
#include <iostream>

namespace Salt2D::Game::Session {

StorySession::StorySession(Utils::IFileSystem& fs) : fs_(fs) {}

void StorySession::Initialize(const std::filesystem::path& storyRoot, const std::filesystem::path& graphPath, const Story::NodeId& startNode) {
    logger_ = Utils::MakeConsoleAndFileLogger(
        Utils::GenerateTimestampedFilename("Logs/game_scene.log"),
        Utils::LogLevel::Debug, Utils::LogLevel::Debug);

    history_.SetLogger(&logger_);
    history_.Clear();

    storyRoot_ = storyRoot;

    const auto perfTablePath = storyRoot_ / "Performance/performance_table.json";
    tables_.perf = Story::LoadPerformanceTable(fs_, perfTablePath);

    const auto castTablePath = storyRoot_ / "Cast/cast_table.json";
    tables_.cast = Story::LoadCastTable(fs_, castTablePath);

    const auto stageTablePath = storyRoot_ / "Stage/stage_table.json";
    tables_.stage = Story::LoadStageTable(fs_, stageTablePath);

    const auto fullGraphPath = storyRoot_ / graphPath;
    graph_ = Story::LoadStoryGraph(fs_, fullGraphPath);

    player_ = std::make_unique<Story::StoryPlayer>(graph_, fs_);
    player_->SetLogger(&logger_);
    player_->SetHistory(&history_);

    player_->SetEffectCallback([](const Salt2D::Game::Story::Effect& e) {
        // Placeholder: later route to Director/CameraRig/etc.
        OutputDebugStringA(("[Effect] type=" + e.type + " name=" + e.name + "\n").c_str());
    });

    player_->Start(startNode);
}

void StorySession::Initialize(const StorySessionConfig& cfg) {
    storyRoot_ = cfg.storyRoot;

    if (cfg.enableLogger) {
        const auto path = cfg.logPath.empty()
            ? Utils::GenerateTimestampedFilename("Logs/game_scene.log")
            : Utils::GenerateTimestampedFilename(cfg.logPath.string());
        std::cout << "Initializing logger at: " << path << "\n";
        logger_ = Utils::MakeConsoleAndFileLogger(path, cfg.consoleLevel, cfg.fileLevel);
        history_.SetLogger(&logger_);
    } else {
        history_.SetLogger(nullptr);
    }
    history_.Clear();

    tables_ = {};
    if (HasTable(cfg.loadTables, TableLoadMask::Pref)) {
        const auto perfTablePath = storyRoot_ / "Performance/performance_table.json";
        tables_.perf = Story::LoadPerformanceTable(fs_, perfTablePath);
    }
    if (HasTable(cfg.loadTables, TableLoadMask::Cast)) {
        const auto castTablePath = storyRoot_ / "Cast/cast_table.json";
        tables_.cast = Story::LoadCastTable(fs_, castTablePath);
    }
    if (HasTable(cfg.loadTables, TableLoadMask::Stage)) {
        const auto stageTablePath = storyRoot_ / "Stage/stage_table.json";
        tables_.stage = Story::LoadStageTable(fs_, stageTablePath);
    }

    const auto fullGraphPath = storyRoot_ / cfg.graphPath;
    graph_ = Story::LoadStoryGraph(fs_, fullGraphPath);

    player_ = std::make_unique<Story::StoryPlayer>(graph_, fs_);
    if (cfg.enableLogger) player_->SetLogger(&logger_);
    player_->SetHistory(&history_);

    player_->SetEffectCallback([](const Salt2D::Game::Story::Effect& e) {
        // Placeholder: later route to Director/CameraRig/etc.
        OutputDebugStringA(("[Effect] type=" + e.type + " name=" + e.name + "\n").c_str());
    });

    player_->Start(cfg.startNode);
}

} // namespace Salt2D::Game::Session
