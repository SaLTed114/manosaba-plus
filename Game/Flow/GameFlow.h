// Game/Flow/GameFlow.h
#ifndef GAME_FLOW_GAMEFLOW_H
#define GAME_FLOW_GAMEFLOW_H

#include <memory>
#include <optional>
#include <vector>

#include "Core/Time/FrameClock.h"
#include "GameFlowTypes.h"
#include "Game/Session/StorySession.h"

namespace Salt2D::Game::Flow {

class GameFlow {
public:
    explicit GameFlow(Utils::IFileSystem& fs);

    // void LoadChapters(const std::filesystem::path& chaptersFile);
    void SetChapters(std::vector<ChapterDef> chapters);

    void Start();
    void ConfirmGate();
    void Tick(const Core::FrameTime& ft);

    FlowState State() const { return state_; }
    int ChapterIndex() const { return chapterIndex_; }
    const ChapterDef* CurrentChapter() const;

    bool HasSession() const { return session_ && session_->Ready(); }
    Session::StorySession* Session() { return session_.get(); }
    Story::StoryPlayer* Player();

    std::optional<FlowEvent> ConsumeEvent();

private:
    void EnterChapter(int index);
    void BeginChapter();
    void EndChapter();

    bool NeedGate(const ChapterDef& chapter) const;

    bool InvalidChapterIndex(int index) const {
        return index < 0 || index >= static_cast<int>(chapters_.size());
    }

private:
    Utils::IFileSystem& fs_;
    std::vector<ChapterDef> chapters_;

    int chapterIndex_ = -1;
    FlowState state_ = FlowState::Idle;

    std::unique_ptr<Session::StorySession> session_;

    std::optional<FlowEvent> event_;
};

} // namespace Salt2D::Game::Flow

#endif // GAME_FLOW_GAMEFLOW_H
