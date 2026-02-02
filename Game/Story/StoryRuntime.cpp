// Game/Story/StoryRuntime.cpp
#include "StoryRuntime.h"

#include <iostream>
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
        std::cout
            << "[StoryRuntime] WARN: No edge from '" << current_
            << "' for event trigger=" << ToString(event.trigger)
            << " key='" << event.key << "'\n";
        return;
    }

    ApplyEffects(*edge);
    EnterNode(edge->to);
}

void StoryRuntime::ApplyEffects(const Edge& edge) {
    for (const auto& effect : edge.effects) {
        std::cout
            << "[StoryRuntime] Applying effect: type='" << effect.type
            << "' name='" << effect.name << "'\n";
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
    std::cout
        << "\n[StoryRuntime] Entered node '" << node.id
        << "' (type=" << ToString(node.type)
        << ", resource=" << node.resourcePath.string()
        << ")\n";
}

} // namespace Salt2D::Game::Story
