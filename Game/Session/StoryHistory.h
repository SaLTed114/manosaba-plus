// Game/Session/StoryHistory.h
#ifndef GAME_SESSION_STORYHISTORY_H
#define GAME_SESSION_STORYHISTORY_H

#include <string>
#include <vector>
#include "Game/Story/StoryTypes.h"
#include "Game/Common/Logger.h"

namespace Salt2D::Game::Session {

enum class HistoryKind : uint8_t {
    Line,
    MenuOpen,
    MenuBack,
    OptionList,
    OptionPick,
    PresentPrompt,
    PresentPick,
};

struct HistoryEntry {
    Story::NodeType type = Story::NodeType::Unknown;
    HistoryKind kind = HistoryKind::Line;
    std::string speakerUtf8;
    std::string textUtf8;
    std::string idUtf8;
};

class StoryHistory {
public:
    void SetLogger(const Logger* logger) { logger_ = logger; }

    void Push(HistoryEntry entry);
    void Push(Story::NodeType type, std::string speakerUtf8, std::string textUtf8);
    void Push(Story::NodeType type, HistoryKind kind,
        std::string speakerUtf8, std::string textUtf8, std::string idUtf8 = {});
    
    void DumpToLogger() const;

    void Clear() { entries_.clear(); }

    const std::vector<HistoryEntry>& GetEntries() const { return entries_; }

private:
    static std::string Format(const HistoryEntry& entry);

private:
    const Logger* logger_ = nullptr;
    std::vector<HistoryEntry> entries_;
};

} // namespace Salt2D::Game::Session

#endif // GAME_SESSION_STORYHISTORY_H
