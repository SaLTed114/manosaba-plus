// Game/Story/Resources/StageDef.h
#ifndef GAME_STORY_RESOURCES_STAGEDEF_H
#define GAME_STORY_RESOURCES_STAGEDEF_H

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

namespace Salt2D::Game::Story {

enum class StageBgMode : uint32_t {
    PlaceholderBG, // placeholder background (e.g. solid color or default image)
};

struct StageRingLayout {
    int podiumCount = 13;
    float radius = 5.0f;
    float centerX = 0.0f;
    float centerY = 0.0f;
    float centerZ = 0.0f;
    float yawOffsetRad = 0.0f;
};

struct StageSlot {
    std::string castId; // cast id assigned to this slot, e.g. "emma"
    int podiumIndex = 0; // which podium this slot is on, 0-based
};

struct StageDef {
    std::string id; // unique identifier, e.g. "inquisition"

    StageBgMode bgMode = StageBgMode::PlaceholderBG;
    std::string bgImage; // for PlaceholderBG, the image to use

    StageRingLayout ringLayout;

    std::string podiumImage;
    std::vector<StageSlot> slots;
};

struct StageTable {
    std::unordered_map<std::string, StageDef> byId;

    const StageDef* Find(std::string_view id) const {
        auto it = byId.find(std::string(id));
        return it != byId.end() ? &it->second : nullptr;
    }
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_STAGEDEF_H
