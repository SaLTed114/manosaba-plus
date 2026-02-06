// Game/Story/Resources/ChoiceDefLoader.cpp
#include "ChoiceDefLoader.h"

#include <nlohmann/json.hpp>
#include <stdexcept>

namespace Salt2D::Game::Story {

using json = nlohmann::json;

static std::string RequireString(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_string()) {
        throw std::runtime_error("ChoiceDefLoader: missing/invalid string field '" + std::string(key) + "' in " + context);
    }
    return j[key].get<std::string>();
}

ChoiceDef LoadChoiceDef(Utils::IFileSystem& fs, const std::filesystem::path& fullPath) {
    if (!fs.Exists(fullPath)) {
        throw std::runtime_error("ChoiceDefLoader: file does not exist: " + fullPath.string());
    }
    const std::string text = fs.ReadTextFileUtf8(fullPath, true);

    json root = json::parse(text);
    if (!root.is_object()) {
        throw std::runtime_error("ChoiceDefLoader: root JSON is not an object in " + fullPath.string());
    }

    ChoiceDef def;
    
    if (!root.contains("options") || !root["options"].is_array()) {
        throw std::runtime_error("ChoiceDefLoader: missing/invalid 'options' array in " + fullPath.string());
    }

    for (size_t i = 0; i < root["options"].size(); i++) {
        const auto& jOption = root["options"][i];
        const auto ctx = "options[" + std::to_string(i) + "]";
        if (!jOption.is_object()) {
            throw std::runtime_error("ChoiceDefLoader: " + ctx + " must be object");
        }

        ChoiceOption option;
        option.optionId = RequireString(jOption, "option_id", ctx);
        option.label    = RequireString(jOption, "label", ctx);
        def.options.push_back(std::move(option));
    }

    return def;
}

} // namespace Salt2D::Game::Story
