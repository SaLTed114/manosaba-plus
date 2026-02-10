// Game/Story/Resources/CastDef.h
#ifndef GAME_STORY_RESOURCES_CASTDEF_H
#define GAME_STORY_RESOURCES_CASTDEF_H

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

namespace Salt2D::Game::Story {

struct Color4F {
    float r=1, g=1, b=1, a=1;
};

struct CastDef {
    std::string id;         // unique identifier, e.g. "emma"
    std::string name;       // display name, e.g. "樱羽 艾玛"
    std::vector<std::string> aliases;

    // for UI (nameplate, etc.)
    Color4F textColor{1,1,1,1};

    // for camera look-at
    float headY = 1.35f;

    // optional resource paths
    std::string cardImage;
    // placeholder for more resources
};

struct CastTable {
    std::unordered_map<std::string, CastDef> byId;
    std::unordered_map<std::string, std::string> nameToId; // for lookup by name/alias

    const CastDef* FindById(std::string_view id) const {
        auto it = byId.find(std::string(id));
        return it != byId.end() ? &it->second : nullptr;
    }

    const CastDef* FindByName(std::string_view name) const {
        auto it = nameToId.find(std::string(name));
        if (it == nameToId.end()) return nullptr;
        return FindById(it->second);
    }
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_CASTDEF_H
