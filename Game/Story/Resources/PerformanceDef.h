// Game/Story/Resources/PerformanceDef.h
#ifndef GAME_STORY_RESOURCES_PERFORMANCEDEF_H
#define GAME_STORY_RESOURCES_PERFORMANCEDEF_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>

namespace Salt2D::Game::Story {

struct Vec2F {
    float x = 0.0f;
    float y = 0.0f;
};

enum class TrackSpace : uint8_t {
    Normalized = 0,
    Pixels     = 1,
};

enum class PivotKind : uint8_t {
    TopLeft = 0,
    Center  = 1,
};

enum class RotateMode : uint8_t {
    None  = 0,
    Slope = 1,
    Fixed = 2,
};

enum class MotionType : uint8_t {
    Linear = 0,
};

struct RotationPolicy {
    RotateMode mode  = RotateMode::Slope;
    bool keepUpright = true;
    float fixedRad   = 0.0f;
};

struct MotionPolicy {
    MotionType type = MotionType::Linear;
};

struct DebateTextTrack2D {
    TrackSpace space = TrackSpace::Normalized;

    Vec2F start{};
    Vec2F end{};

    PivotKind pivot = PivotKind::TopLeft;

    RotationPolicy rotation{};
    MotionPolicy motion{};
};

struct HudPerformance {
    std::optional<DebateTextTrack2D> debateTextTrack;
};

struct PerformanceDef {
    std::string id;
    HudPerformance hud;

    // placeholder for future camera/stage track definitions
};

struct PerformanceTable {
    std::unordered_map<std::string, PerformanceDef> byId;

    const PerformanceDef* Find(std::string_view id) const {
        auto it = byId.find(std::string(id));
        return it != byId.end() ? &it->second : nullptr;
    }
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_PERFORMANCEDEF_H
