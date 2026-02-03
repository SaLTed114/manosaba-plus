// Game/Story/Resources/PresentDefLoader.cpp
#include "PresentDefLoader.h"

#include <nlohmann/json.hpp>
#include <stdexcept>

namespace Salt2D::Game::Story {

using json = nlohmann::json;

static std::string RequireString(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_string()) {
        throw std::runtime_error("PresentDefLoader: missing/invalid string field '" + std::string(key) + "' in " + context);
    }
    return j[key].get<std::string>();
}

PresentDef LoadPresentDef(Utils::IFileSystem& fs, const std::filesystem::path& fullPath) {
    if (!fs.Exists(fullPath)) {
        throw std::runtime_error("PresentDefLoader: file does not exist: " + fullPath.string());
    }
    const std::string text = fs.ReadTextFileUtf8(fullPath, true);

    json root = json::parse(text);
    if (!root.is_object()) {
        throw std::runtime_error("PresentDefLoader: root JSON is not an object in " + fullPath.string());
    }

    PresentDef def;
    def.prompt = (root.contains("prompt") && root["prompt"].is_string())
        ? root["prompt"].get<std::string>()
        : "";

    if (!root.contains("items") || !root["items"].is_array()) {
        throw std::runtime_error("PresentDefLoader: missing/invalid 'items' array in " + fullPath.string());
    }

    for (size_t i = 0; i < root["items"].size(); i++) {
        const auto& jItem = root["items"][i];
        const auto ctx = "items[" + std::to_string(i) + "]";
        if (!jItem.is_object()) {
            throw std::runtime_error("PresentDefLoader: " + ctx + " must be object");
        }

        PresentItem item;
        item.itemId = RequireString(jItem, "item_id", ctx);
        item.label  = RequireString(jItem, "label", ctx);
        def.items.push_back(std::move(item));
    }

    return def;
}

} // namespace Salt2D::Game::Story
