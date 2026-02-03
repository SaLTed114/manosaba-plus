// Game/Story/Runners/DebateRunner.cpp
#include "DebateRunner.h"
#include "Game/Story/Resources/DebateDefLoader.h"

#include <stdexcept>

namespace Salt2D::Game::Story {

static inline std::string MakeStmtSpanKey(int stmtIdx, const std::string& spanId) {
    return std::to_string(stmtIdx) + "::" + spanId;
}

DebateRunner::DebateRunner(Utils::IFileSystem& fs) : fs_(fs) {}

void DebateRunner::Enter(const Node& node) {
    if (node.resourceFullPath.empty()) {
        throw std::runtime_error("DebateRunner::Enter: Node resource path is empty");
    }
    def_ = LoadDebateDef(fs_, node.resourceFullPath);

    idx_ = 0;
    menuOpen_ = false;
    openedSpanId_.clear();
    commited_ = false;

    BuildIndex();
    
    if (logger_) {
        logger_->Debug("DebateRunner",
            "Entered Debate node: " + std::to_string(def_.statements.size()) + 
            " statements, " + std::to_string(def_.menus.size()) + " menus");
    }
}

void DebateRunner::BuildIndex() {
    menusByStmt_.assign(def_.statements.size(), std::vector<int>{});
    menuByStmtSpan_.clear();

    for (size_t i = 0; i < def_.menus.size(); ++i) {
        const auto& menu = def_.menus[i];
        menusByStmt_[menu.statementIndex].push_back(static_cast<int>(i));

        std::string key = MakeStmtSpanKey(menu.statementIndex, menu.spanId);
        menuByStmtSpan_[key] = static_cast<int>(i);
    }
}

const DebateStatement& DebateRunner::CurrentStatement() const {
    if (def_.statements.empty()) {
        throw std::runtime_error("DebateRunner::CurrentStatement: No statements in debate definition");
    }
    if (idx_ < 0 || idx_ >= static_cast<int>(def_.statements.size())) {
        throw std::runtime_error("DebateRunner::CurrentStatement: Statement index out of range");
    }
    return def_.statements[idx_];
}

std::vector<std::string> DebateRunner::CurrentSpanIds() const {
    std::vector<std::string> spanIds;
    if (idx_ < 0 || idx_ >= static_cast<int>(menusByStmt_.size())) {
        return spanIds;
    }

    for (int menuIdx : menusByStmt_[idx_]) {
        spanIds.push_back(def_.menus[menuIdx].spanId);
    }
    return spanIds;
}

const DebateMenu* DebateRunner::FindMenu(const std::string& spanId) const {
    auto key = MakeStmtSpanKey(idx_, spanId);
    auto it = menuByStmtSpan_.find(key);
    if (it == menuByStmtSpan_.end()) return nullptr;
    int menuIdx = it->second;
    if (menuIdx < 0 || menuIdx >= static_cast<int>(def_.menus.size())) return nullptr;
    return &def_.menus[menuIdx];
}

bool DebateRunner::OpenSuspicion(const std::string& spanId) {
    if (menuOpen_) {
        if (logger_) {
            logger_->Debug("DebateRunner",
                "OpenSuspicion failed: menu already open (spanId=\"" + openedSpanId_ + "\")");
        }
        return false;
    }
    
    const DebateMenu* menu = FindMenu(spanId);
    if (!menu) {
        if (logger_) {
            logger_->Debug("DebateRunner",
                "OpenSuspicion failed: menu not found for spanId=\"" + spanId + 
                "\" at statement " + std::to_string(idx_));
        }
        return false;
    }

    menuOpen_ = true;
    openedSpanId_ = spanId;
    
    if (logger_) {
        logger_->Debug("DebateRunner",
            "Opened suspicion menu: spanId=\"" + spanId + 
            "\", options=" + std::to_string(menu->options.size()));
    }
    return true;
}

void DebateRunner::CloseMenu() {
    if (logger_ && menuOpen_) {
        logger_->Debug("DebateRunner",
            "Closed menu: spanId=\"" + openedSpanId_ + "\" without committing");
    }
    menuOpen_ = false;
    openedSpanId_.clear();
}

std::vector<std::pair<std::string, std::string>> DebateRunner::CurrentOptions() const {
    std::vector<std::pair<std::string, std::string>> options;
    if (!menuOpen_) return options;

    const DebateMenu* menu = FindMenu(openedSpanId_);
    if (!menu) return options;

    options.reserve(menu->options.size());
    for (const auto& option : menu->options) {
        options.emplace_back(option.optionId, option.label);
    }
    return options;
}

std::optional<GraphEvent> DebateRunner::CommitOption(const std::string& optionId) {
    if (!menuOpen_) {
        if (logger_) {
            logger_->Debug("DebateRunner",
                "CommitOption failed: no menu is open");
        }
        return std::nullopt;
    }

    const DebateMenu* menu = FindMenu(openedSpanId_);
    if (!menu) {
        if (logger_) {
            logger_->Debug("DebateRunner",
                "CommitOption failed: menu not found for spanId=\"" + openedSpanId_ + "\"");
        }
        return std::nullopt;
    }

    for (const auto& option : menu->options) {
        if (option.optionId == optionId) {
            if (logger_) {
                logger_->Debug("DebateRunner",
                    "Committed option: " + optionId + " (" + option.label + 
                    ") for spanId=\"" + openedSpanId_ + "\"");
            }
            commited_ = true;
            menuOpen_ = false;
            openedSpanId_.clear();
            return GraphEvent{Trigger::Option, optionId};
        }
    }
    
    if (logger_) {
        logger_->Debug("DebateRunner",
            "CommitOption failed: optionId \"" + optionId + "\" not found in menu");
    }
    return std::nullopt;
}

std::optional<GraphEvent> DebateRunner::AdvanceStatement() {
    // TODO: Can be advanced even if menu is open
    if (menuOpen_) {
        if (logger_) {
            logger_->Debug("DebateRunner",
                "AdvanceStatement: blocked by open menu");
        }
        return std::nullopt;
    }

    const int statementCount = static_cast<int>(def_.statements.size());
    if (statementCount == 0) {
        if (logger_) {
            logger_->Debug("DebateRunner",
                "AdvanceStatement: no statements, triggering NoCommit");
        }
        return GraphEvent{Trigger::NoCommit, ""};
    }

    if (idx_ < statementCount - 1) {
        ++idx_;
        if (logger_) {
            logger_->Debug("DebateRunner",
                "Advanced to statement " + std::to_string(idx_) + "/" + std::to_string(statementCount-1));
        }
        return std::nullopt;
    }
    
    if (!commited_) {
        if (logger_) {
            logger_->Debug("DebateRunner",
                "AdvanceStatement: reached end without commit, triggering NoCommit");
        }
        return GraphEvent{Trigger::NoCommit, ""};
    }

    if (logger_) {
        logger_->Debug("DebateRunner",
            "AdvanceStatement: reached end with commit, debate completed");
    }
    return std::nullopt;
}

} // namespace Salt2D::Game::Story
