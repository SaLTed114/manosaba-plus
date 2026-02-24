// Game/Story/Runners/VnRunner.h
#ifndef GAME_STORY_RUNNERS_VNRUNNER_H
#define GAME_STORY_RUNNERS_VNRUNNER_H

#include "Game/Story/StoryTypes.h"
#include "Game/Story/Resources/VnScript.h"
#include "Utils/IFileSystem.h"
#include "Game/Common/Logger.h"

#include <optional>
#include <functional>

namespace Salt2D::Game::Story {

struct VnState {
    std::string speaker;
    std::string fullText;
    std::string perfId;
    size_t revealed = 0;
    size_t totalCp  = 0;
    float revealCpF = 0.0f;
    bool lineDone = false;
    bool finished = false;
    int lineSerial = 0;
};

class VnRunner {
public:
    using CueCallback = std::function<void(const std::string& cue)>;

public:
    explicit VnRunner(Utils::IFileSystem& fs);

    void Enter(const Node& node);

    std::optional<GraphEvent> Advance();
    std::optional<GraphEvent> SkipLine();
    std::optional<GraphEvent> FastForwardAll();

    void Tick(float dtSec, float cps);

    const VnState& State() const { return state_; }

    void SetCueCallback(CueCallback callback) { onCue_ = std::move(callback); }
    void SetLogger(const Game::Logger* logger) { logger_ = logger; }

private:
    void LoadNextLineOrFinish();
    void ApplyCurrentCmdAsLine();
    size_t CountCodepoints(const std::string& utf8) const;
    std::string Utf8PrefixByCodepoints(const std::string& utf8, size_t cpCount) const;

private:
    Utils::IFileSystem& fs_;
    VnScript script_;
    size_t cmdIndex_ = 0;

    size_t lineTotalCp_ = 0;
    float  revealAcc_ = 0.0f;

    VnState state_;
    CueCallback onCue_;
    const Game::Logger* logger_ = nullptr;
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RUNNERS_VNRUNNER_H
