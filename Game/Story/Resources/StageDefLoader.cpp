// Game/Story/Resources/StageDefLoader.cpp
#include "StageDefLoader.h"

#include <nlohmann/json.hpp>
#include <stdexcept>

namespace Salt2D::Game::Story {

using json = nlohmann::json;

static std::string RequireString(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_string()) {
        throw std::runtime_error("StageDefLoader: missing/invalid string field '" + std::string(key) + "' in " + context);
    }
    return j[key].get<std::string>();
}

static int RequireInt(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_number_integer()) {
        throw std::runtime_error("StageDefLoader: missing/invalid integer field '" + std::string(key) + "' in " + context);
    }
    return j[key].get<int>();
}

static float RequireFloat(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_number()) {
        throw std::runtime_error("StageDefLoader: missing/invalid number field '" + std::string(key) + "' in " + context);
    }
    return j[key].get<float>();
}

static bool TryString(const json& j, const char* key, std::string& out) {
    if (!j.contains(key)) return false;
    if (!j[key].is_string()) return false;
    out = j[key].get<std::string>();
    return true;
}

static StageBgMode ParseStageBgMode(const std::string& str, const std::string& context) {
    if (str == "PlaceholderBG") return StageBgMode::PlaceholderBG;
    throw std::runtime_error("StageDefLoader: unknown bg_mode '" + str + "' in " + context);
}

StageTable LoadStageTable(Utils::IFileSystem& fs, const std::filesystem::path& fullPath) {
    if (!fs.Exists(fullPath)) {
        throw std::runtime_error("StageDefLoader: file does not exist: " + fullPath.string());
    }
    const std::string text = fs.ReadTextFileUtf8(fullPath, true);

    json root = json::parse(text);
    if (!root.is_object()) {
        throw std::runtime_error("StageDefLoader: root must be an object in " + fullPath.string());
    }
    if (!root.contains("stages") || !root["stages"].is_array()) {
        throw std::runtime_error("StageDefLoader: missing/invalid 'stages' array in " + fullPath.string());
    }

    StageTable table;

    for (size_t i = 0; i < root["stages"].size(); i++) {
        const auto& jStage = root["stages"][i];
        const auto ctx = "stages[" + std::to_string(i) + "]";
        if (!jStage.is_object()) {
            throw std::runtime_error("StageDefLoader: " + ctx + " must be object");
        }

        StageDef def;
        def.id = RequireString(jStage, "id", ctx);
        if (def.id.empty()) {
            throw std::runtime_error("StageDefLoader: " + ctx + ".id cannot be empty");
        }
        if (table.FindById(def.id) != nullptr) {
            throw std::runtime_error("StageDefLoader: duplicate stage id '" + def.id + "' in " + ctx);
        }

        if (jStage.contains("ring")) {
            const auto& jRing = jStage["ring"];
            if (!jRing.is_object()) {
                throw std::runtime_error("StageDefLoader: " + ctx + ".ring must be object");
            }

            def.ringLayout.podiumCount  = RequireInt  (jRing, "podium_count",   ctx + ".ring");
            def.ringLayout.radius       = RequireFloat(jRing, "radius",         ctx + ".ring");
            def.ringLayout.centerX      = RequireFloat(jRing, "center_x",       ctx + ".ring");
            def.ringLayout.centerY      = RequireFloat(jRing, "center_y",       ctx + ".ring");
            def.ringLayout.centerZ      = RequireFloat(jRing, "center_z",       ctx + ".ring");
            def.ringLayout.yawOffsetRad = RequireFloat(jRing, "yaw_offset_rad", ctx + ".ring");

            if (def.ringLayout.podiumCount <= 0) {
                throw std::runtime_error("StageDefLoader: " + ctx + ".ring.podium_count must be positive");
            }
        }

        TryString(jStage, "bg_image", def.bgImage);
        TryString(jStage, "podium_image", def.podiumImage);

        if (jStage.contains("bg_mode")) {
            if (!jStage["bg_mode"].is_string()) {
                throw std::runtime_error("StageDefLoader: " + ctx + ".bg_mode must be a string");
            }
            def.bgMode = ParseStageBgMode(jStage["bg_mode"].get<std::string>(), ctx + ".bg_mode");
        }

        if (jStage.contains("slots")) {
            const auto& jSlots = jStage["slots"];
            if (!jSlots.is_array()) {
                throw std::runtime_error("StageDefLoader: " + ctx + ".slots must be an array");
            }

            for (size_t j = 0; j < jSlots.size(); j++) {
                const auto& jSlot = jSlots[j];
                const auto slotCtx = ctx + ".slots[" + std::to_string(j) + "]";
                if (!jSlot.is_object()) {
                    throw std::runtime_error("StageDefLoader: " + slotCtx + " must be object");
                }

                StageSlot slot;
                slot.castId = RequireString(jSlot, "cast_id", slotCtx);
                slot.podiumIndex = RequireInt(jSlot, "podium", slotCtx);

                if (slot.castId.empty()) {
                    throw std::runtime_error("StageDefLoader: " + slotCtx + ".cast_id cannot be empty");
                }
                if (slot.podiumIndex < 0 || slot.podiumIndex >= def.ringLayout.podiumCount) {
                    throw std::runtime_error("StageDefLoader: " + slotCtx + ".podium_index out of range");
                }
                def.slots.push_back(std::move(slot));
            }
        }

        table.byId.emplace(def.id, std::move(def));
    }

    return table;
}

} // namespace Salt2D::Game::Story
