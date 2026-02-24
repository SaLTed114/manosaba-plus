// Game/Story/Runners/DebateRunner.h
#ifndef GAME_STORY_RUNNERS_DEBATERUNNER_H
#define GAME_STORY_RUNNERS_DEBATERUNNER_H

#include <optional>
#include <unordered_map>

#include "Game/Story/StoryTypes.h"
#include "Game/Story/Resources/DebateDef.h"
#include "Utils/IFileSystem.h"
#include "Utils/Logger.h"

namespace Salt2D::Game::Story {

class DebateRunner {
public:
    explicit DebateRunner(Utils::IFileSystem& fs);

    void Enter(const Node& node);

    std::optional<GraphEvent> AdvanceStatement();
    bool OpenSuspicion(const std::string& spanId);
    std::optional<GraphEvent> CommitOption(const std::string& optionId);
    void CloseMenu();

    int StatementIndex() const { return idx_; }
    int StatementCount() const { return static_cast<int>(def_.statements.size()); }
    const DebateStatement& CurrentStatement() const;
    std::vector<std::string> CurrentSpanIds() const;
    bool IsMenuOpen() const { return menuOpen_; }
    const std::string& OpenedSpanId() const { return openedSpanId_; }
    std::vector<std::pair<std::string, std::string>> CurrentOptions() const;

    bool IsCommitted() const { return commited_; }

    void SetLogger(const Utils::Logger* logger) { logger_ = logger; }

private:
    void BuildIndex();
    const DebateMenu* FindMenu(const std::string& spanId) const;

private:
    Utils::IFileSystem& fs_;
    DebateDef def_;

    int idx_ = 0;
    bool menuOpen_ = false;
    std::string openedSpanId_;
    bool commited_ = false;

    // index: statementIndex -> list of menu indices
    std::vector<std::vector<int>> menusByStmt_;
    // index: (statementIndex, spanId) -> menu index
    std::unordered_map<std::string, int> menuByStmtSpan_;
    
    const Utils::Logger* logger_ = nullptr;
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RUNNERS_DEBATERUNNER_H
