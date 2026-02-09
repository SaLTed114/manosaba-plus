// Game/Session/StoryHistory.cpp
#include "StoryHistory.h"

namespace Salt2D::Game::Session {

void StoryHistory::Push(HistoryEntry entry) {
    entries_.push_back(std::move(entry));
    if (logger_) logger_->Info("History", Format(entries_.back()));
}

void StoryHistory::Push(Story::NodeType type, std::string speakerUtf8, std::string textUtf8) {
    Push(HistoryEntry{type, HistoryKind::Line, std::move(speakerUtf8), std::move(textUtf8)});
}

void StoryHistory::Push(Story::NodeType type, HistoryKind kind, std::string speakerUtf8, std::string textUtf8, std::string idUtf8) {
    Push(HistoryEntry{type, kind, std::move(speakerUtf8), std::move(textUtf8), std::move(idUtf8)});
}

void StoryHistory::DumpToLogger() const {
    if (!logger_) return;
    logger_->Info("History", "================ HISTORY DUMP ================");
    for (const auto& entry : entries_) logger_->Info("History", Format(entry));
    logger_->Info("History", "==============================================");
}

std::string StoryHistory::Format(const HistoryEntry& entry) {
    auto EscapeNewlines = [](const std::string& str) -> std::string {
        std::string result;
        result.reserve(str.size());
        for (char c : str) {
            if (c == '\n') result += "\\n";
            else if (c == '\r') result += "\\r";
            else result += c;
        }
        return result;
    };

    std::string formatStr = std::string(Story::ToString(entry.type));

    switch (entry.kind) {
    case HistoryKind::Line: break;
    case HistoryKind::MenuOpen:      formatStr += ": [Suspicious]"; break;
    case HistoryKind::MenuBack:      formatStr += ": [Back]";       break;
    case HistoryKind::OptionList:    formatStr += ": [Options]";    break;
    case HistoryKind::OptionPick:    formatStr += ": [Option]";     break;
    case HistoryKind::PresentPrompt: formatStr += ": [Prompt]";     break;
    case HistoryKind::PresentPick:   formatStr += ": [Present]";    break;
    case HistoryKind::AccelDebate:   formatStr += ": [Accel]";      break;
    default: formatStr += ": [Unknown]"; break;
    }

    if (!entry.speakerUtf8.empty()) formatStr += " - " + entry.speakerUtf8;
    if (!entry.idUtf8.empty()) formatStr += " (" + entry.idUtf8 + ")";
    
    if (!entry.textUtf8.empty()) formatStr += ": " + EscapeNewlines(entry.textUtf8);
    return formatStr;
}

} // namespace Salt2D::Game::Session
