// Game/Story/Resources/PerformanceDef.h
#ifndef GAME_STORY_RESOURCES_PERFORMANCEDEF_H
#define GAME_STORY_RESOURCES_PERFORMANCEDEF_H

#include <cstdint>
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

enum class Rotate2DMode : uint8_t {
    None  = 0,
    Slope = 1,
    Fixed = 2,
};

enum class Rotate3DMode : uint8_t {
    None   = 0,
    LookAt = 1,
    Fixed  = 2,
};

enum class MotionType : uint8_t {
    Linear = 0,
};

enum class CameraSpace : uint8_t {
    Anchor = 0,
    // World = 1, // future expansion
};

struct Rotation2DPolicy {
    Rotate2DMode mode  = Rotate2DMode::Slope;
    bool keepUpright = true;
    float fixedRad   = 0.0f;
};

struct Rotation3DPolicy {
    Rotate3DMode mode = Rotate3DMode::LookAt;
    float fixedYawRad   = 0.0f;
    float fixedPitchRad = 0.0f;
    float fixedRollRad  = 0.0f;
};

struct MotionPolicy {
    MotionType type = MotionType::Linear;
};

struct DebateTextTrack2D {
    TrackSpace space = TrackSpace::Normalized;

    Vec2F start{};
    Vec2F end{};

    PivotKind pivot = PivotKind::TopLeft;

    Rotation2DPolicy rotation{};
    MotionPolicy motion{};
};

struct StageCameraPoseAnchor {
    // eye = head + face * dist + up * liftY + right * sideX
    float dist = 2.0f;
    float liftY = 0.12f;
    float sideX = 0.0f;
    float fovYDeg = 55.0f;
};

struct StageCameraTrack {
    CameraSpace space = CameraSpace::Anchor;

    StageCameraPoseAnchor start{};
    StageCameraPoseAnchor end{};

    float targetListY = 0.05f; // look = head + up * targetListY

    Rotation3DPolicy rotation{};
    MotionPolicy motion{};
};

struct HudPerformance {
    std::optional<DebateTextTrack2D> debateTextTrack;
};

struct StagePerformance {
    std::optional<StageCameraTrack> cameraTrack;
};

struct PerformanceDef {
    std::string id;
    HudPerformance hud;
    StagePerformance stage;
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
