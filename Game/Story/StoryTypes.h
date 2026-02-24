// Game/Story/StoryTypes.h
#ifndef GAME_STORY_STORYTYPES_H
#define GAME_STORY_STORYTYPES_H

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <cstdint>
#include <optional>

namespace Salt2D::Game::Story {

using NodeId  = std::string;
using EdgeKey = std::string;

// ========== Node Types ==========

enum class NodeType : uint8_t {
    VN,
    Debate,
    Present,
    Error,
    BE,
    Choice,
    ChapterEnd,
    Unknown,
};

inline std::string_view ToString(NodeType type) {
    switch (type) {
        case NodeType::VN:         return "vn";
        case NodeType::Debate:     return "debate";
        case NodeType::Present:    return "present";
        case NodeType::Error:      return "error";
        case NodeType::BE:         return "be";
        case NodeType::Choice:     return "choice";
        case NodeType::ChapterEnd: return "chapter_end";
        default:                   return "unknown";
    }
}

inline NodeType ParseNodeType(std::string_view str) {
    if (str == "vn")          return NodeType::VN;
    if (str == "debate")      return NodeType::Debate;
    if (str == "present")     return NodeType::Present;
    if (str == "error")       return NodeType::Error;
    if (str == "be")          return NodeType::BE;
    if (str == "choice")      return NodeType::Choice;
    if (str == "chapter_end") return NodeType::ChapterEnd;
    return NodeType::Unknown;
}

// ========== Trigger ==========

enum class Trigger : uint8_t {
    Auto,
    Option,
    Pick,
    NoCommit,
    TimeDepleted,
    HpDepleted,
    Unknown,
};

inline std::string_view ToString(Trigger trigger) {
    switch (trigger) {
        case Trigger::Auto:         return "auto";
        case Trigger::Option:       return "option";
        case Trigger::Pick:         return "pick";
        case Trigger::NoCommit:     return "no_commit";
        case Trigger::TimeDepleted: return "time_depleted";
        case Trigger::HpDepleted:   return "hp_depleted";
        default:                    return "unknown";
    }
}

inline Trigger ParseTrigger(std::string_view str) {
    if (str == "auto")          return Trigger::Auto;
    if (str == "option")        return Trigger::Option;
    if (str == "pick")          return Trigger::Pick;
    if (str == "no_commit")     return Trigger::NoCommit;
    if (str == "time_depleted") return Trigger::TimeDepleted;
    if (str == "hp_depleted")   return Trigger::HpDepleted;
    return Trigger::Unknown;
}

// ========== Effects on Edges ==========

// currently a placeholder

struct Effect {
    std::string type;
    std::string name;
};

// ========= Node Parameters ==========

// currently a placeholder

struct NodeParams {
    std::optional<int>            timeLimitSec;
    std::optional<int>            hpLimit;
    std::optional<NodeId>         beNode;
};

// ========= Node ==========

struct Node {
    NodeId                          id;
    NodeType                        type = NodeType::Unknown;
    std::filesystem::path           resourcePath;
    std::filesystem::path           resourceFullPath;
    NodeParams                      params;
};

// ========= Edge ==========

struct Edge {
    NodeId                          from;
    NodeId                          to;
    Trigger                         trigger = Trigger::Unknown;
    EdgeKey                         key;
    std::vector<Effect>             effects;
};

// ========= Runtime Event Produced by Runners ==========

struct GraphEvent {
    Trigger     trigger = Trigger::Unknown;
    EdgeKey     key;
};

// ========= TriggerKey for Hash ==========

struct TriggerKey {
    Trigger trigger;
    std::string key;

    bool operator==(const TriggerKey& other) const {
        return trigger == other.trigger && key == other.key;
    }
};

struct TriggerKeyHash {
    std::size_t operator()(const TriggerKey& tk) const noexcept {
        std::size_t h1 = std::hash<uint8_t>{}(static_cast<uint8_t>(tk.trigger));
        std::size_t h2 = std::hash<std::string>{}(tk.key);
        return h1 ^ (h2 + 0x9e3779b97f4a7c15ull + (h1 << 6) + (h1 >> 2));
    }
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_STORYTYPES_H
