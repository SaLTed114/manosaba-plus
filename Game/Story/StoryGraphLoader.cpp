// Game/Story/StoryGraphLoader.cpp
#include "StoryGraphLoader.h"
#include "Utils/FileUtils.h"

#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

namespace Salt2D::Game::Story {

using json = nlohmann::json;
namespace fs = std::filesystem;

static std::string RequireString(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_string()) {
        throw std::runtime_error("StoryGraphLoader: missing/invalid string field '" + std::string(key) + "' in " + context);
    }
    return j[key].get<std::string>();
}

static std::optional<std::string> OptionalString(const json& j, const char* key) {
    if (!j.contains(key)) return std::nullopt;
    if (j[key].is_null()) return std::nullopt;
    if (!j[key].is_string()) return std::nullopt;
    return j[key].get<std::string>();
}

[[maybe_unused]] static int OptionalInt(const json& j, const char* key, int defaultValue) {
    if (!j.contains(key) || j[key].is_null()) return defaultValue;
    if (!j[key].is_number_integer()) return defaultValue;
    return j[key].get<int>();
}

static void ParseNodeParams(const json& jNode, Node& node) {
    if (!jNode.contains("params") || !jNode["params"].is_object()) return;
    const auto& jParams = jNode["params"];
    if (!jParams.is_object()) return;

    if (jParams.contains("time_limit_sec") && jParams["time_limit_sec"].is_number_integer()) {
        node.params.timeLimitSec = jParams["time_limit_sec"].get<int>();
    }
    if (jParams.contains("be_node") && jParams["be_node"].is_string()) {
        node.params.beNode = jParams["be_node"].get<NodeId>();
    }
}

static std::vector<Effect> ParseEffects(const json& jEdge, const std::string& context) {
    std::vector<Effect> effects;
    if (!jEdge.contains("effects") || jEdge["effects"].is_null()) return effects;
    const auto& jEffects = jEdge["effects"];
    if (!jEffects.is_array()) throw std::runtime_error("StoryGraphLoader: 'effects' field is not an array in " + context);

    effects.reserve(jEffects.size());
    for (size_t i = 0; i < jEffects.size(); i++) {
        const auto& effect = jEffects[i];
        if (!effect.is_object()) {
            throw std::runtime_error("StoryGraphLoader: effect at index " + std::to_string(i) + " is not an object in " + context);
        }

        Effect ef;
        ef.type = RequireString(effect, "type", context + " effects[" + std::to_string(i) + "]");
        ef.name = RequireString(effect, "name", context + " effects[" + std::to_string(i) + "]");
        effects.push_back(std::move(ef));
    }
    return effects;
}

static bool HasOutgoingTrigger(const StoryGraph& graph, const NodeId& nodeId, Trigger trigger) {
    for (const auto& edge : graph.Edges()) {
        if (edge.from == nodeId && edge.trigger == trigger) return true;
    }
    return false;
}

static void ValidateDebateEdges(const StoryGraph& graph, const StoryGraphLoadOptions& options) {
    for (const auto& [id, node] : graph.Nodes()) {
        if (node.type != NodeType::Debate) continue;

        if (options.requireDebateNoCommit && !HasOutgoingTrigger(graph, id, Trigger::NoCommit)) {
            throw std::runtime_error("StoryGraphLoader: Debate node '" + id + "' missing outgoing 'no_commit' trigger");
        }
        if (options.requireDebateTimeDepleted && !HasOutgoingTrigger(graph, id, Trigger::TimeDepleted)) {
            throw std::runtime_error("StoryGraphLoader: Debate node '" + id + "' missing outgoing 'time_depleted' trigger");
        }
    }
}

StoryGraph LoadStoryGraph(
    Utils::IFileSystem& fs,
    const fs::path& graphFullPath,
    const StoryGraphLoadOptions& opt
) {
    if (graphFullPath.empty()) {
        throw std::runtime_error("StoryGraphLoader: graphFullPath is empty");
    }

    const fs::path baseDir = graphFullPath.parent_path();

    if (!fs.Exists(graphFullPath)) {
        throw std::runtime_error("StoryGraphLoader: graph file does not exist: " + graphFullPath.string());
    }
    
    const std::string text = fs.ReadTextFileUtf8(graphFullPath);

    json root;
    try {
        root = json::parse(text);
    } catch (const std::exception& e) {
        throw std::runtime_error("StoryGraphLoader: failed to parse JSON from " + graphFullPath.string() + ": " + e.what());
    }

    if (!root.is_object()) {
        throw std::runtime_error("StoryGraphLoader: root JSON is not an object in " + graphFullPath.string());
    }
    if (!root.contains("nodes") || !root["nodes"].is_array()) {
        throw std::runtime_error("StoryGraphLoader: missing/invalid 'nodes' array in " + graphFullPath.string());
    }
    if (!root.contains("edges") || !root["edges"].is_array()) {
        throw std::runtime_error("StoryGraphLoader: missing/invalid 'edges' array in " + graphFullPath.string());
    }

    StoryGraph graph;
    graph.SetBaseDir(baseDir);

    // ==== Parse Nodes ====
    const auto& jNodes = root["nodes"];
    for (size_t i = 0; i < jNodes.size(); i++) {
        const auto& jNode = jNodes[i];
        const auto ctx = "nodes[" + std::to_string(i) + "]";
        if (!jNode.is_object()) {
            throw std::runtime_error("StoryGraphLoader: " + ctx + " must be object");
        }

        Node node;
        node.id = RequireString(jNode, "id", ctx);

        const std::string typeStr = RequireString(jNode, "type", ctx);
        node.type = ParseNodeType(typeStr);
        if (node.type == NodeType::Unknown) {
            throw std::runtime_error("StoryGraphLoader: unknown node type '" + typeStr + "' in " + ctx);
        }

        const std::string resourceStr = RequireString(jNode, "resource", ctx);
        node.resourcePath = fs::path(resourceStr);

        node.resourceFullPath = Utils::ResolveRelative(baseDir, node.resourcePath);

        ParseNodeParams(jNode, node);

        if (opt.checkResourcesExists && !fs.Exists(node.resourceFullPath)) {
            throw std::runtime_error("StoryGraphLoader: resource file does not exist for node '" + node.id + "': " + node.resourceFullPath.string());
        }

        graph.AddNode(std::move(node));
    }

    // ==== Parse Edges ====
    const auto& jEdges = root["edges"];
    for (size_t i = 0; i < jEdges.size(); i++) {
        const auto& jEdge = jEdges[i];
        const auto ctx = "edges[" + std::to_string(i) + "]";
        if (!jEdge.is_object()) {
            throw std::runtime_error("StoryGraphLoader: " + ctx + " must be object");
        }

        Edge edge;
        edge.from = RequireString(jEdge, "from", ctx);
        edge.to   = RequireString(jEdge, "to",   ctx);

        const std::string triggerStr = RequireString(jEdge, "trigger", ctx);
        edge.trigger = ParseTrigger(triggerStr);
        if (edge.trigger == Trigger::Unknown) {
            throw std::runtime_error("StoryGraphLoader: unknown trigger '" + triggerStr + "' in " + ctx);
        }

        if (auto key = OptionalString(jEdge, "key"); key.has_value()) {
            edge.key = *key;
        }

        edge.effects = ParseEffects(jEdge, ctx);

        graph.AddEdge(std::move(edge));
    }

    // ==== Finalize ====
    graph.ValidateBasic();
    graph.BuildIndex();

    ValidateDebateEdges(graph, opt);

    return graph;
}

} // namespace Salt2D::Game::Story
