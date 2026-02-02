// Game/Story/StoryPlayer.cpp
#include "StoryPlayer.h"

namespace Salt2D::Game::Story {

StoryPlayer::StoryPlayer(const StoryGraph& graph, Utils::IFileSystem& fs)
    : rt_(graph), fs_(fs), vn_(fs) {}

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
        return;
    }
}

void StoryPlayer::PumpAuto() {
    // placeholder for auto-pumping logic if needed in the future
}

} // namespace Salt2D::Game::Story
