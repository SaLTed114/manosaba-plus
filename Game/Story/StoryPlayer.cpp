// Game/Story/StoryPlayer.cpp
#include "StoryPlayer.h"

namespace Salt2D::Game::Story {

StoryPlayer::StoryPlayer(const StoryGraph& graph, Utils::IFileSystem& fs)
    : rt_(graph), fs_(fs), vn_(fs), present_(fs), debate_(fs) {}

void StoryPlayer::Start(const NodeId& startNode) {
    rt_.Start(startNode);
    OnEnteredNode();
}

void StoryPlayer::Advance() {
    const Node& node = rt_.CurrentNode();

    if (node.type == NodeType::VN ||
        node.type == NodeType::BE ||
        node.type == NodeType::Error
    ) {
        if (auto ev = vn_.Advance(); ev.has_value()) {
            rt_.PushEvent(*ev);
            OnEnteredNode();
            PumpAuto();
        }
        UpdateView();
        return;
    }

    if (node.type == NodeType::Debate) {
        if (auto ev = debate_.AdvanceStatement(); ev.has_value()) {
            rt_.PushEvent(*ev);
            OnEnteredNode();
            PumpAuto();
        }
        UpdateView();
        return;
    }

    if (logger_) {
        logger_->Debug("StoryPlayer",
            std::string("Advance ignored for node type=") +
            std::string(ToString(node.type)));
    }
}

void StoryPlayer::FastForward() {
    const Node& node = rt_.CurrentNode();

    if (node.type == NodeType::VN ||
        node.type == NodeType::BE ||
        node.type == NodeType::Error
    ) {
        if (auto ev = vn_.FastForwardAll(); ev.has_value()) {
            rt_.PushEvent(*ev);
            OnEnteredNode();
            PumpAuto();
        }
        UpdateView();
        return;
    }

    if (logger_) {
        logger_->Debug("StoryPlayer",
            std::string("FastForward ignored for node type=") +
            std::string(ToString(node.type)));
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

    if (node.type == NodeType::VN ||
        node.type == NodeType::BE ||
        node.type == NodeType::Error
    ) {
        vn_.Enter(node);
        UpdateView();
        return;
    }

    if (node.type == NodeType::Present) {
        present_.Enter(node);
        UpdateView();
        return;
    }

    if (node.type == NodeType::Debate) {
        debate_.Enter(node);
        UpdateView();
        return;
    }
}

void StoryPlayer::PumpAuto() {
    // placeholder for auto-pumping logic if needed in the future
}

void StoryPlayer::UpdateView() {
    view_ = {};
    const Node& node = rt_.CurrentNode();
    view_.nodeType = node.type;

    if (node.type == NodeType::VN ||
        node.type == NodeType::BE ||
        node.type == NodeType::Error
    ) {
        const VnState& state = vn_.State();
        StoryView::VnView view;
        view.speaker  = state.speaker;
        view.fullText = state.fullText;
        view.revealed = state.revealed;
        view.lineDone = state.lineDone;
        view.finished = state.finished;
        view_.vn = std::move(view);
    }

    if (node.type == NodeType::Present) {
        const PresentDef& def = present_.Def();
        StoryView::PresentView view;
        view.prompt = def.prompt;
        for (const auto& item : def.items) {
            view.items.emplace_back(item.itemId, item.label);
        }
        view_.present = std::move(view);
    }

    if (node.type == NodeType::Debate) {
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
    }
}

} // namespace Salt2D::Game::Story
