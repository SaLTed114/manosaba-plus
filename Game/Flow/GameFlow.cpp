// Game/Flow/GameFlow.cpp
#include "GameFlow.h"

#include <stdexcept>

namespace Salt2D::Game::Flow {

GameFlow::GameFlow(Utils::IFileSystem& fs) : fs_(fs) {}

void GameFlow::SetChapters(std::vector<ChapterDef> chapters) {
    chapters_ = std::move(chapters);
}

const ChapterDef* GameFlow::CurrentChapter() const {
    if (InvalidChapterIndex(chapterIndex_)) return nullptr;
    return &chapters_[chapterIndex_];
}

Story::StoryPlayer* GameFlow::Player() {
    if (!HasSession()) return nullptr;
    return &session_->Player();
}

std::optional<FlowEvent> GameFlow::ConsumeEvent() {
    auto event = event_;
    event_.reset();
    return event;
}

bool GameFlow::NeedGate(const ChapterDef& chapter) const {
    return chapter.kind == ChapterKind::Trial;
}

void GameFlow::Start() {
    if (chapters_.empty()) {
        state_ = FlowState::Finished;
        event_ = FlowEvent::Finished;
        return;
    }
    EnterChapter(0);
}

void GameFlow::EnterChapter(int index) {
    session_.reset();
    event_ = FlowEvent::SessionCleared;

    chapterIndex_   = index;

    if (InvalidChapterIndex(chapterIndex_)) { 
        state_ = FlowState::Finished; 
        event_ = FlowEvent::Finished;
        return; 
    }

    const auto& chapter = chapters_[chapterIndex_];
    if (NeedGate(chapter)) {
        state_ = FlowState::GateReady;
        return;
    }

    BeginChapter();
}

void GameFlow::BeginChapter() {
    const ChapterDef* chapter = CurrentChapter();
    if (!chapter) { state_ = FlowState::Finished; return; }

    session_ = std::make_unique<Session::StorySession>(fs_);

    Session::TableLoadMask loadTables{};
    switch (chapter->kind) {
    case ChapterKind::Novel:
        loadTables = Session::TableLoadMask::Cast;
        break;
    case ChapterKind::Trial:
        loadTables = Session::TableLoadMask::All;
        break;
    default:
        throw std::runtime_error("Unhandled chapter kind: " + std::to_string(static_cast<int>(chapter->kind)));
    }

    const auto loggerStr = "ch" + std::to_string(chapterIndex_) + "_" + chapter->chapterId;

    const auto config = Session::StorySessionConfig{
        .storyRoot = chapter->storyRoot,
        .graphPath = chapter->graphPath,
        .startNode = chapter->startNode,
        .loadTables = loadTables,
        .enableLogger = true,
        .logPath = std::filesystem::path("Logs/") / (loggerStr + ".log"),
        .consoleLevel = Utils::LogLevel::Info,
        .fileLevel = Utils::LogLevel::Debug,
    };
    session_->Initialize(config);

    state_ = FlowState::InChapter;
    event_ = FlowEvent::SessionCreated;
}

void GameFlow::EndChapter() {
    const int nextIndex = chapterIndex_ + 1;
    EnterChapter(nextIndex);
}

void GameFlow::ConfirmGate() {
    if (state_ != FlowState::GateReady) return;
    BeginChapter();
}

void GameFlow::Tick(const Core::FrameTime& /*ft*/) {
    if (state_ != FlowState::InChapter) return;
    if (!HasSession()) return;

    if (auto sig = session_->Player().ConsumeSignal(); sig.has_value()) {
        switch (sig->kind) {
        case Story::StorySignal::Kind::ChapterEnd:
            EndChapter();
            return;
        default:
            throw std::runtime_error("Unhandled story signal kind: " + std::to_string(static_cast<int>(sig->kind)));
        }
    }
}

} // namespace Salt2D::Game::Flow
