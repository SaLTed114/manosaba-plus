// Game/Story/StoryPlayer.cpp
#include "StoryPlayer.h"

namespace Salt2D::Game::Story {

StoryPlayer::StoryPlayer(const StoryGraph& graph, Utils::IFileSystem& fs)
    : rt_(graph), fs_(fs), vn_(fs), present_(fs), debate_(fs), choice_(fs) {}

void StoryPlayer::Start(const NodeId& startNode) {
    rt_.Start(startNode);
    OnEnteredNode();
}

void StoryPlayer::Tick(double dtSec) {
    float dtGame = static_cast<float>(dtSec) * timeScale_;

    if (!timer_.active) return;
    if (view_.nodeType != NodeType::Debate) return;

    bool pause = false;
    if (view_.debate.has_value()) pause = view_.debate->menuOpen;
    if (pause) return;

    timer_.remainSec -= dtGame;

    if (timer_.remainSec <= 0.0f) {
        timer_.remainSec = 0.0f;
        timer_.active = false;
        if (logger_) {
            logger_->Info("StoryPlayer", "Timer expired");
        }
        if (timer_.beNode.has_value()) {
            GraphEvent ev{Trigger::TimeDepleted, ""};
            rt_.PushEvent(ev);
            OnEnteredNode();
            PumpAuto();
        }
    }

    UpdateView();
}

void StoryPlayer::Advance() {
    const Node& node = rt_.CurrentNode();

    switch (node.type) {
    case NodeType::VN:
    case NodeType::BE:
    case NodeType::Error:
        if (auto ev = vn_.SkipLine(); ev.has_value()) {
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

        view.spanIds      = debate_.CurrentSpanIds();
        view.menuOpen     = debate_.IsMenuOpen();
        view.openedSpanId = debate_.OpenedSpanId();
        view.options      = debate_.CurrentOptions();

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
