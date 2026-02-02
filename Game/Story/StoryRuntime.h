// Game/Story/StoryRuntime.h
#ifndef GAME_STORY_STORYRUNTIME_H
#define GAME_STORY_STORYRUNTIME_H

#include "StoryGraph.h"
#include "StoryTypes.h"

#include <functional>
#include <string>

namespace Salt2D::Game::Story {

class StoryRuntime {
public:
    using EffectCallback = std::function<void(const Effect&)>;

public:
    explicit StoryRuntime(const StoryGraph& graph);

    void Start(const NodeId& startNodeId);

    void PushEvent(const GraphEvent& event);

    const NodeId& CurrentNodeId() const { return current_; }
    const Node&   CurrentNode()   const { return graph_.GetNode(current_); }

    void SetEffectCallback(EffectCallback callback) { onEffect_ = std::move(callback); }

private:
    void EnterNode(const NodeId& nodeId);
    void ApplyEffects(const Edge& edge);

private:
    const StoryGraph& graph_;
    NodeId current_;

    EffectCallback onEffect_;
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_STORYRUNTIME_H
