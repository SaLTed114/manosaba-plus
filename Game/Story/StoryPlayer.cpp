// Game/Story/StoryPlayer.cpp
#include "StoryPlayer.h"

namespace Salt2D::Game::Story {

StoryPlayer::StoryPlayer(const StoryGraph& graph, Utils::IFileSystem& fs)
    : rt_(graph), fs_(fs), vn_(fs), present_(fs), debate_(fs), choice_(fs) {}

void StoryPlayer::Start(const NodeId& startNode) {
    rt_.Start(startNode);
    OnEnteredNode();
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

void StoryPlayer::PumpAuto() {
    // placeholder for auto-pumping logic if needed in the future
}

void StoryPlayer::UpdateView() {
    view_ = {};
    const Node& node = rt_.CurrentNode();
    view_.nodeType = node.type;

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
