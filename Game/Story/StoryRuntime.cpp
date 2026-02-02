// Game/Story/StoryRuntime.cpp
#include "StoryRuntime.h"

#include <stdexcept>

namespace Salt2D::Game::Story {

StoryRuntime::StoryRuntime(const StoryGraph& graph)
    : graph_(graph) {}

void StoryRuntime::Start(const NodeId& startNodeId) {
    EnterNode(startNodeId);
}

void StoryRuntime::PushEvent(const GraphEvent& event) {
    if (current_.empty()) {
        throw std::runtime_error("StoryRuntime: cannot push event, no current node");
    }

    const Edge* edge = graph_.FindEdge(current_, event);
    if (!edge) {
        if (logger_) {
            logger_->Warn("StoryRuntime", 
                "No edge from '" + current_ + 
                "' for event trigger=" + std::string(ToString(event.trigger)) +
                " key='" + event.key + "'");
        }
        return;
    }

    ApplyEffects(*edge);
    EnterNode(edge->to);
}

void StoryRuntime::ApplyEffects(const Edge& edge) {
    for (const auto& effect : edge.effects) {
        if (logger_) {
            logger_->Debug("StoryRuntime", 
                "Applying effect: type='" + effect.type + 
                "' name='" + effect.name + "'");
        }
        if (onEffect_) {
            onEffect_(effect);
        }
    }
}

void StoryRuntime::EnterNode(const NodeId& nodeId) {
    if (!graph_.HasNode(nodeId)) {
        throw std::runtime_error("StoryRuntime: cannot enter unknown node: " + nodeId);
    }
    current_ = nodeId;

    const Node& node = graph_.GetNode(current_);
    if (logger_) {
        logger_->Info("StoryRuntime", 
            "Entered node '" + node.id + 
            "' (type=" + std::string(ToString(node.type)) +
            ", resource=" + node.resourcePath.string() + ")");
    }
}

} // namespace Salt2D::Game::Story
