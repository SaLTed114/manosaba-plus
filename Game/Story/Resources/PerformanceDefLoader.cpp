// Game/Story/Resources/PerformanceDefLoader.cpp
#include "PerformanceDefLoader.h"

#include <nlohmann/json.hpp>
#include <stdexcept>

namespace Salt2D::Game::Story {

using json = nlohmann::json;

static std::string RequireString(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_string()) {
        throw std::runtime_error("PerformanceDefLoader: missing/invalid string field '" + std::string(key) + "' in " + context);
    }
    return j[key].get<std::string>();
}

static float RequireFloat(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_number()) {
        throw std::runtime_error("PerformanceDefLoader: missing/invalid number field '" + std::string(key) + "' in " + context);
    }
    return j[key].get<float>();
}

[[maybe_unused]] static bool TryString(const json& j, const char* key, std::string& out) {
    if (!j.contains(key)) return false;
    if (!j[key].is_string()) return false;
    out = j[key].get<std::string>();
    return true;
}

[[maybe_unused]] static bool TryBool(const json& j, const char* key, bool& out) {
    if (!j.contains(key)) return false;
    if (!j[key].is_boolean()) return false;
    out = j[key].get<bool>();
    return true;
}

static Vec2F RequireVec2F(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_array() || j[key].size() != 2) {
        throw std::runtime_error("PerformanceDefLoader: missing/invalid array field '" + std::string(key) + "' in " + context);
    }
    if (!j[key][0].is_number() || !j[key][1].is_number()) {
        throw std::runtime_error("PerformanceDefLoader: array field '" + std::string(key) + "' must contain numbers in " + context);
    }
    Vec2F vec;
    vec.x = j[key][0].get<float>();
    vec.y = j[key][1].get<float>();
    return vec;
}

static TrackSpace ParseTrackSpace(const std::string str, const std::string& context) {
    if (str == "normalized") return TrackSpace::Normalized;
    if (str == "pixels")     return TrackSpace::Pixels;
    throw std::runtime_error("PerformanceDefLoader: invalid TrackSpace value '" + str + "' in " + context);
}

static PivotKind ParsePivotKind(const std::string str, const std::string& context) {
    if (str == "top_left") return PivotKind::TopLeft;
    if (str == "center")  return PivotKind::Center;
    throw std::runtime_error("PerformanceDefLoader: invalid PivotKind value '" + str + "' in " + context);
}

static RotateMode ParseRotateMode(const std::string str, const std::string& context) {
    if (str == "none")  return RotateMode::None;
    if (str == "slope") return RotateMode::Slope;
    if (str == "fixed") return RotateMode::Fixed;
    throw std::runtime_error("PerformanceDefLoader: invalid RotateMode value '" + str + "' in " + context);
}

static MotionType ParseMotionType(const std::string str, const std::string& context) {
    if (str == "linear") return MotionType::Linear;
    throw std::runtime_error("PerformanceDefLoader: invalid MotionType value '" + str + "' in " + context);
}

static DebateTextTrack2D ParseDebateTextTrack2D(const json& j, const std::string& context) {
    if (!j.is_object()) {
        throw std::runtime_error("PerformanceDefLoader: " + context + " must be an object");
    }

    DebateTextTrack2D track;

    if (j.contains("space")) {
        if (!j["space"].is_string()) {
            throw std::runtime_error("PerformanceDefLoader: 'space' must be a string in " + context);
        }
        track.space = ParseTrackSpace(j["space"].get<std::string>(), context + ".space");
    }

    track.start = RequireVec2F(j, "start", context);
    track.end   = RequireVec2F(j, "end",   context);

    if (j.contains("pivot")) {
        if (!j["pivot"].is_string()) {
            throw std::runtime_error("PerformanceDefLoader: 'pivot' must be a string in " + context);
        }
        track.pivot = ParsePivotKind(j["pivot"].get<std::string>(), context + ".pivot");
    }

    if (j.contains("rotation")) {
        const auto& jRot = j["rotation"];
        const auto rotCtx = context + ".rotation";
        if (!jRot.is_object()) {
            throw std::runtime_error("PerformanceDefLoader: " + rotCtx + " must be an object");
        }

        if (jRot.contains("mode")) {
            if (!jRot["mode"].is_string()) {
                throw std::runtime_error("PerformanceDefLoader: 'mode' must be a string in " + rotCtx);
            }
            track.rotation.mode = ParseRotateMode(jRot["mode"].get<std::string>(), rotCtx + ".mode");
        }

        if (jRot.contains("keep_upright")) {
            if (!jRot["keep_upright"].is_boolean()) {
                throw std::runtime_error("PerformanceDefLoader: 'keep_upright' must be a boolean in " + rotCtx);
            }
            track.rotation.keepUpright = jRot["keep_upright"].get<bool>();
        }

        if (track.rotation.mode == RotateMode::Fixed) {
            track.rotation.fixedRad = RequireFloat(jRot, "fixed_rad", rotCtx);
        }
    }

    if (j.contains("motion")) {
        const auto& jMotion = j["motion"];
        const auto motionCtx = context + ".motion";
        if (!jMotion.is_object()) {
            throw std::runtime_error("PerformanceDefLoader: " + motionCtx + " must be an object");
        }

        if (jMotion.contains("type")) {
            if (!jMotion["type"].is_string()) {
                throw std::runtime_error("PerformanceDefLoader: 'type' must be a string in " + motionCtx);
            }
            track.motion.type = ParseMotionType(jMotion["type"].get<std::string>(), motionCtx + ".type");
        }
    }

    return track;
}

PerformanceTable LoadPerformanceTable(Utils::IFileSystem& fs, const std::filesystem::path& fullPath) {
    if (!fs.Exists(fullPath)) {
        throw std::runtime_error("PerformanceDefLoader: file does not exist: " + fullPath.string());
    }
    const std::string text = fs.ReadTextFileUtf8(fullPath, true);
    
    json root = json::parse(text);
    if (!root.is_object()) {
        throw std::runtime_error("PerformanceDefLoader: root JSON is not an object in " + fullPath.string());
    }
    if (!root.contains("performances") || !root["performances"].is_array()) {
        throw std::runtime_error("PerformanceDefLoader: missing/invalid 'performances' array in " + fullPath.string());
    }

    PerformanceTable table;

    for (size_t i = 0; i < root["performances"].size(); i++) {
        const auto& jPerf = root["performances"][i];
        const auto ctx = "performances[" + std::to_string(i) + "]";
        if (!jPerf.is_object()) {
            throw std::runtime_error("PerformanceDefLoader: " + ctx + " must be object");
        }

        PerformanceDef def;
        def.id = RequireString(jPerf, "id", ctx);

        if (jPerf.contains("hud")) {
            const auto& jHud = jPerf["hud"];
            const auto hudCtx = ctx + ".hud";
            if (!jHud.is_object()) {
                throw std::runtime_error("PerformanceDefLoader: " + hudCtx + " must be an object");
            }

            HudPerformance hud;

            if (jHud.contains("debate_text_track")) {
                hud.debateTextTrack = ParseDebateTextTrack2D(jHud["debate_text_track"], hudCtx + ".debate_text_track");
            }

            def.hud = std::move(hud);
        }

        table.byId.emplace(def.id, std::move(def));
    }

    return table;
}

} // namespace Salt2D::Game::Story
