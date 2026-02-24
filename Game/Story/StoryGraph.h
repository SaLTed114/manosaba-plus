// Game/Story/StoryGraph.h
#ifndef GAME_STORY_STORYGRAPH_H
#define GAME_STORY_STORYGRAPH_H

#include "StoryTypes.h"
#include <unordered_map>
#include <vector>
#include <stdexcept>

namespace Salt2D::Game::Story {

class StoryGraph {
public:
    const std::filesystem::path& GetBaseDir() const { return baseDir_; }
    void SetBaseDir(const std::filesystem::path& path) { baseDir_ = path; }

    bool HasNode(const NodeId& id) const { return nodesById_.find(id) != nodesById_.end(); }
    const Node& GetNode(const NodeId& id) const {
        auto it = nodesById_.find(id);
        if (it == nodesById_.end()) throw std::runtime_error("Node not found: " + id);
        return it->second;
    }
    Node& GetNodeMut(const NodeId& id) {
        auto it = nodesById_.find(id);
        if (it == nodesById_.end()) throw std::runtime_error("Node not found: " + id);
        return it->second;
    }

    const std::unordered_map<NodeId, Node>& Nodes() const { return nodesById_; }

    const std::vector<Edge>& Edges() const { return edges_; }

    const Edge* FindEdge(const NodeId& from, const GraphEvent& ev) const {
        auto triggerIt = triggerIndex_.find(from);
        if (triggerIt == triggerIndex_.end()) return nullptr;

        TriggerKey tk{ev.trigger, ev.key};
        auto edgeIt = triggerIt->second.find(tk);
        if (edgeIt == triggerIt->second.end()) return nullptr;

        const size_t edgeIdx = edgeIt->second;
        if (edgeIdx >= edges_.size()) return nullptr;
        return &edges_[edgeIdx];
    }

    void AddNode(Node node) {
        if (node.id.empty()) throw std::runtime_error("Cannot add node with empty ID");
        if (nodesById_.find(node.id) != nodesById_.end()) {
            throw std::runtime_error("Node with ID already exists: " + node.id);
        }
        nodesById_.emplace(node.id, std::move(node));
    }

    void AddEdge(Edge edge) {
        if (edge.from.empty() || edge.to.empty()) throw std::runtime_error("Cannot add edge with empty from/to");
        edges_.push_back(std::move(edge));
    }

    void BuildIndex() {
        outEdges_.clear();
        triggerIndex_.clear();

        outEdges_.reserve(nodesById_.size());
        triggerIndex_.reserve(nodesById_.size());

        for (size_t i = 0; i < edges_.size(); i++) {
            const auto& edge = edges_[i];

            outEdges_[edge.from].push_back(i);

            TriggerKey tk{edge.trigger, edge.key};
            auto& map = triggerIndex_[edge.from];
            if (map.find(tk) != map.end()) {
                throw std::runtime_error("Duplicate edge for trigger/key on node: " + edge.from);
            }
            map.emplace(std::move(tk), i);
        }
    }

    void ValidateBasic() const {
        for (const auto& [id, node] : nodesById_) {
            // ChapterEnd node should not have edges outbound, resource can be empty
            if (node.type == NodeType::ChapterEnd) {
                if (outEdges_.find(id) != outEdges_.end()) {
                    throw std::runtime_error("ChapterEnd node should not have outgoing edges: " + id);
                }
                continue;
            }
            if (node.resourcePath.empty()) throw std::runtime_error("Node has empty resource path: " + id);
        }

        for (const auto& edge : edges_) {
            if (!HasNode(edge.from)) throw std::runtime_error("Edge 'from' node does not exist: " + edge.from);
            if (!HasNode(edge.to))   throw std::runtime_error("Edge 'to' node does not exist: " + edge.to);

            if (edge.trigger == Trigger::Unknown) {
                throw std::runtime_error("Edge has unknown trigger from node: " + edge.from + " to node: " + edge.to);
            }

            if ((edge.trigger == Trigger::Option || edge.trigger == Trigger::Pick) && edge.key.empty()) {
                throw std::runtime_error("Edge with Option/Pick trigger must have non-empty key from node: " + edge.from + " to node: " + edge.to);
            }
        }
    }
    
private:
    std::filesystem::path baseDir_;

    std::unordered_map<NodeId, Node> nodesById_;
    std::vector<Edge> edges_;

    std::unordered_map<NodeId, std::vector<size_t>> outEdges_;
    std::unordered_map<NodeId, std::unordered_map<TriggerKey, size_t, TriggerKeyHash>> triggerIndex_;
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_STORYGRAPH_H
