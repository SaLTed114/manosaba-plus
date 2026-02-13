// Game/Story/StoryPlayer.cpp
#include "StoryPlayer.h"
#include "Game/Story/TextMarkup/SusMarkup.h"
#include "Utils/StringUtils.h"
#include <iostream>

namespace Salt2D::Game::Story {

StoryPlayer::StoryPlayer(const StoryGraph& graph, Utils::IFileSystem& fs)
    : rt_(graph), fs_(fs), vn_(fs), present_(fs), debate_(fs), choice_(fs) {}

void StoryPlayer::Start(const NodeId& startNode) {
    rt_.Start(startNode);
    OnEnteredNode();
}

void StoryPlayer::Tick(double dtSec) {
    float dtGame = static_cast<float>(dtSec) * timeScale_;

    const auto& type = rt_.CurrentNode().type;
    // if (type != NodeType::Debate) return;


    switch (type) {
    case NodeType::VN:
    case NodeType::BE:
    case NodeType::Error: {
        vn_.Tick(dtGame, vnTimer_.cfg.cps_);

        const auto& state = vn_.State();
        if (!vnAutoMode_ || state.finished) break;
        if (!state.lineDone) break;

        vnTimer_.remainSec -= dtGame;
        if (vnTimer_.remainSec <= 0.0f) {
            vnTimer_.remainSec = 0.0f;
            vnTimer_.active = false;
            if (logger_) {
                logger_->Info("StoryPlayer", "VN auto timer expired: lineSerial=" +
                    std::to_string(state.lineSerial) +
                    ", totalSec=" + std::to_string(vnTimer_.totalSec));
            }

            Advance();
        }

        break;
    }
    case NodeType::Debate: {
        bool pause = debate_.IsMenuOpen();
        if (pause) return;

        if (timer_.active) {
            timer_.remainSec -= dtGame;

            if (timer_.remainSec <= 0.0f) {
                timer_.remainSec = 0.0f;
                timer_.active = false;
                if (logger_) {
                    logger_->Info("StoryPlayer", "Node timer expired: nodeId=" +
                        rt_.CurrentNodeId() +
                        ", totalSec=" + std::to_string(timer_.totalSec));
                }

                if (timer_.beNode.has_value()) {
                    GraphEvent ev{Trigger::TimeDepleted, ""};
                    rt_.PushEvent(ev);
                    OnEnteredNode();
                    PumpAuto();
                    UpdateView();
                    return;
                }
            }
        }

        if (stmtTimer_.active) {
            stmtTimer_.remainSec -= dtGame;

            if (stmtTimer_.remainSec <= 0.0f) {
                stmtTimer_.remainSec = 0.0f;
                stmtTimer_.active = false;
                if (logger_) {
                    logger_->Info("StoryPlayer", "Auto-advancing statement: stmtIndex=" +
                        std::to_string(stmtTimer_.statementIndex));
                }

                Advance();
                return;
            }
        }
        break;
    }
    default:
        break;
    }

    UpdateView();
}

void StoryPlayer::Advance() {
    const Node& node = rt_.CurrentNode();

    switch (node.type) {
    case NodeType::VN:
    case NodeType::BE:
    case NodeType::Error:
        if (auto ev = vn_.Advance(); ev.has_value()) {
            rt_.PushEvent(*ev);
            OnEnteredNode();
            PumpAuto();
        }
        UpdateView();
        return;
    case NodeType::Debate:
        if (auto ev = debate_.AdvanceStatement(); ev.has_value()) {
            rt_.PushEvent(*ev);
            OnEnteredNode();
            PumpAuto();
        }
        UpdateView();
        return;
    case NodeType::Present:
    case NodeType::Choice:
    default:
        if (logger_) {
            logger_->Debug("StoryPlayer",
                std::string("Advance ignored for node type=") +
                std::string(ToString(node.type)));
        }
        return;
    }
}

void StoryPlayer::FastForward() {
    const Node& node = rt_.CurrentNode();

    switch (node.type) {
    case NodeType::VN:
    case NodeType::BE:
    case NodeType::Error:
        if (auto ev = vn_.FastForwardAll(); ev.has_value()) {
            rt_.PushEvent(*ev);
            OnEnteredNode();
            PumpAuto();
        }
        UpdateView();
        return;
    case NodeType::Debate:
    case NodeType::Present:
    case NodeType::Choice:
    default:
        if (logger_) {
            logger_->Debug("StoryPlayer",
                std::string("FastForward ignored for node type=") +
                std::string(ToString(node.type)));
        }
        return;
    }
}

void StoryPlayer::CommitOption(const std::string& optionId) {
    GraphEvent ev{Trigger::Option, optionId};
    rt_.PushEvent(ev);
    OnEnteredNode();
    PumpAuto();
}

void StoryPlayer::PickEvidence(const std::string& evidenceId) {
    GraphEvent ev{Trigger::Pick, evidenceId};
    rt_.PushEvent(ev);
    OnEnteredNode();
    PumpAuto();
}

void StoryPlayer::OpenSuspicion(const std::string& spanId) {
    const Node& node = rt_.CurrentNode();
    if (node.type != NodeType::Debate) {
        if (logger_) {
            logger_->Debug("StoryPlayer", "OpenSuspicion ignored: not in Debate node");
        }
        return;
    }

    debate_.OpenSuspicion(spanId);
    UpdateView();
}

void StoryPlayer::CloseDebateMenu() {
    const Node& node = rt_.CurrentNode();
    if (node.type != NodeType::Debate) {
        if (logger_) {
            logger_->Debug("StoryPlayer", "CloseDebateMenu ignored: not in Debate node");
        }
        return;
    }

    debate_.CloseMenu();
    UpdateView();
}

void StoryPlayer::SetTimeScale(TimeScaleMode mode) {
    switch (mode) {
    case TimeScaleMode::Normal: SetTimeScale(1.0f); break;
    case TimeScaleMode::Fast:   SetTimeScale(3.0f); break;
    default:
        if (logger_) {
            logger_->Warn("StoryPlayer", "SetTimeScale: unknown mode");
        }
        break;
    }
}

void StoryPlayer::OnEnteredNode() {
    const Node& node = rt_.CurrentNode();
    if (logger_) {
        logger_->Info("StoryPlayer",
            "OnEnteredNode: " + node.id +
            " type=" + std::string(ToString(node.type)));
    }

    ResetTimer();

    switch (node.type) {
    case NodeType::VN:
    case NodeType::BE:
    case NodeType::Error:
        vn_.Enter(node);
        UpdateView();
        return;
    case NodeType::Present:
        present_.Enter(node);
        UpdateView();
        return;
    case NodeType::Debate:
        debate_.Enter(node);
        UpdateView();
        return;
    case NodeType::Choice:
        choice_.Enter(node);
        UpdateView();
        return;
    default:
        if (logger_) {
            logger_->Warn("StoryPlayer",
                "OnEnteredNode: Unsupported node type=" +
                std::string(ToString(node.type)));
        }
    }
}

void StoryPlayer::ResetTimer() {
    const Node& node = rt_.CurrentNode();
    if (node.type == NodeType::Present) { timer_.active = false; return; }
    if (node.type == NodeType::VN) { timer_.Reset(); return; }

    if (node.type != NodeType::Debate) return;
    if (node.id == timer_.lastActiveNodeId) return;

    timer_.Reset();

    const auto& params = node.params;
    if (!params.timeLimitSec.has_value() || !params.beNode.has_value()) return;

    timer_.active = true;
    timer_.lastActiveNodeId = node.id;

    timer_.totalSec = static_cast<float>(*params.timeLimitSec);
    timer_.remainSec = timer_.totalSec;
    timer_.beNode = *params.beNode;

    if (logger_) {
        logger_->Info("StoryPlayer",
            "Timer started: " + std::to_string(timer_.totalSec) + " seconds, BE node=" + *params.beNode);
    }
}

void StoryPlayer::ResetStatementTimer(std::string plainText, int stmtIndex) {
    if (view_.nodeType != NodeType::Debate) { stmtTimer_.Reset(); return; }

    auto parsed = Story::ParseSusMarkup(plainText);
    std::string_view plain = parsed.ok ? 
        std::string_view(parsed.plainTextUtf8) :
        std::string_view(plainText);

    float sec = Utils::EstimateReadingTimeSec(plain,
        stmtTimer_.cfg.stmtCps_, stmtTimer_.cfg.stmtBaseSec_,
        stmtTimer_.cfg.stmtMinSec_, stmtTimer_.cfg.stmtMaxSec_);
    stmtTimer_.active = true;
    stmtTimer_.statementIndex = stmtIndex;
    stmtTimer_.totalSec  = sec;
    stmtTimer_.remainSec = sec;

    if (logger_) {
        logger_->Info("StoryPlayer",
            "Statement timer started: stmtIndex=" + std::to_string(stmtIndex) +
            ", estimatedSec=" + std::to_string(sec));
    }
}

void StoryPlayer::ResetVnAutoTimer(std::string fullText, int lineSerial) {
    if (view_.nodeType != NodeType::VN && 
        view_.nodeType != NodeType::BE &&
        view_.nodeType != NodeType::Error) { vnTimer_.Reset(); return; }

    float sec = Utils::EstimateReadingTimeSec(fullText,
        vnTimer_.cfg.cps_, vnTimer_.cfg.baseSec_,
        vnTimer_.cfg.minSec_, vnTimer_.cfg.maxSec_);
    vnTimer_.active = true;
    vnTimer_.lineSerial = lineSerial;
    vnTimer_.totalSec  = sec;
    vnTimer_.remainSec = sec;

    if (logger_) {
        logger_->Info("StoryPlayer",
            "VN auto timer started: lineSerial=" + std::to_string(lineSerial) +
            ", estimatedSec=" + std::to_string(sec));
    }
}

void StoryPlayer::PumpAuto() {
    // placeholder for auto-pumping logic if needed in the future
}

void StoryPlayer::UpdateView() {
    view_ = {};
    const Node& node = rt_.CurrentNode();
    view_.nodeType = node.type;

    view_.timer.active    = timer_.active;
    view_.timer.totalSec  = timer_.totalSec;
    view_.timer.remainSec = timer_.remainSec;

    switch (node.type) {
    case NodeType::VN:
    case NodeType::BE:
    case NodeType::Error: {
        const VnState& state = vn_.State();
        StoryView::VnView view;
        view.speaker  = state.speaker;
        view.fullText = state.fullText;
        view.revealed = state.revealed;
        view.lineDone = state.lineDone;
        view.finished = state.finished;

        if (!vnTimer_.active || vnTimer_.lineSerial != state.lineSerial) {
            ResetVnAutoTimer(state.fullText, state.lineSerial);
        }

        view_.vn = std::move(view);
        break;
    }
    case NodeType::Present: {
        const PresentDef& def = present_.Def();
        StoryView::PresentView view;
        view.prompt = def.prompt;
        for (const auto& item : def.items) {
            view.items.emplace_back(item.itemId, item.label);
        }
        view_.present = std::move(view);
        break;
    }
    case NodeType::Debate: {
        StoryView::DebateView view;
        view.statementIndex = debate_.StatementIndex();
        view.statementCount = debate_.StatementCount();

        const DebateStatement& stmt = debate_.CurrentStatement();
        view.speaker  = stmt.speaker;
        view.fullText = stmt.text;

        view.prefId = stmt.prefId.empty() ? "debate_default" : stmt.prefId;

        view.spanIds      = debate_.CurrentSpanIds();
        view.menuOpen     = debate_.IsMenuOpen();
        view.openedSpanId = debate_.OpenedSpanId();
        view.options      = debate_.CurrentOptions();

        if (!stmtTimer_.active || stmtTimer_.statementIndex != view.statementIndex) {
            ResetStatementTimer(view.fullText, view.statementIndex);
        }

        view.stmtTotalSec  = stmtTimer_.totalSec;
        view.stmtRemainSec = stmtTimer_.remainSec;
        view.timeScale = timeScale_;

        view_.debate = std::move(view);
        break;
    }
    case NodeType::Choice: {
        const ChoiceDef& def = choice_.Def();
        StoryView::ChoiceView view;
        for (const auto& option : def.options) {
            view.options.emplace_back(option.optionId, option.label);
        }
        view_.choice = std::move(view);
        break;
    }
    default:
        break;
    }
}

} // namespace Salt2D::Game::Story
