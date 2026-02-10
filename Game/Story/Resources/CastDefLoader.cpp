// Game/Story/Resources/CastDefLoader.cpp
#include "CastDefLoader.h"

#include <nlohmann/json.hpp>
#include <stdexcept>

namespace Salt2D::Game::Story {

using json = nlohmann::json;

static std::string RequireString(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_string()) {
        throw std::runtime_error("CastDefLoader: missing/invalid string field '" + std::string(key) + "' in " + context);
    }
    return j[key].get<std::string>();
}

[[maybe_unused]] static float RequireFloat(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_number()) {
        throw std::runtime_error("CastDefLoader: missing/invalid number field '" + std::string(key) + "' in " + context);
    }
    return j[key].get<float>();
}

static bool TryString(const json& j, const char* key, std::string& out) {
    if (!j.contains(key)) return false;
    if (!j[key].is_string()) return false;
    out = j[key].get<std::string>();
    return true;
}

static bool TryFloat(const json& j, const char* key, float& out) {
    if (!j.contains(key)) return false;
    if (!j[key].is_number()) return false;
    out = j[key].get<float>();
    return true;
}

static bool TryColor4F(const json& j, const char* key, Color4F& out) {
    if (!j.contains(key)) return false;
    const auto& val = j[key];
    if (!val.is_array() || val.size() != 4) return false;
    for (size_t i = 0; i < 4; i++) if (!val[i].is_number()) return false;
    out.r = val[0].get<float>();
    out.g = val[1].get<float>();
    out.b = val[2].get<float>();
    out.a = val[3].get<float>();
    return true;
}

static bool TryColor4FColorCode(const json& j, const char* key, Color4F& out) {
    if (!j.contains(key)) return false;
    if (!j[key].is_string()) return false;
    const std::string code = j[key].get<std::string>();
    if (code.size() != 7 || code[0] != '#') return false;
    try {
        out.r = std::stoi(code.substr(1, 2), nullptr, 16) / 255.0f;
        out.g = std::stoi(code.substr(3, 2), nullptr, 16) / 255.0f;
        out.b = std::stoi(code.substr(5, 2), nullptr, 16) / 255.0f;
        out.a = 1.0f;
        return true;
    } catch (...) {
        return false;
    }
}

CastTable LoadCastTable(Utils::IFileSystem& fs, const std::filesystem::path& fullPath) {
    if (!fs.Exists(fullPath)) {
        throw std::runtime_error("CastDefLoader: file does not exist: " + fullPath.string());
    }
    const std::string text = fs.ReadTextFileUtf8(fullPath, true);
    
    json root = json::parse(text);
    if (!root.is_object()) {
        throw std::runtime_error("CastDefLoader: root JSON is not an object in " + fullPath.string());
    }
    if (!root.contains("casts") || !root["casts"].is_array()) {
        throw std::runtime_error("CastDefLoader: missing/invalid 'casts' array in " + fullPath.string());
    }

    CastTable table;

    for (size_t i = 0; i < root["casts"].size(); i++) {
        const auto& jCast = root["casts"][i];
        const auto ctx = "casts[" + std::to_string(i) + "]";
        if (!jCast.is_object()) {
            throw std::runtime_error("CastDefLoader: " + ctx + " must be object");
        }

        CastDef def;
        def.id   = RequireString(jCast, "id", ctx);
        def.name = RequireString(jCast, "name", ctx);

        if (jCast.contains("aliases")) {
            const auto& jAliases = jCast["aliases"];
            if (!jAliases.is_array()) {
                throw std::runtime_error("CastDefLoader: " + ctx + ".aliases must be an array");
            }
            for (const auto& jAlias : jAliases) {
                if (!jAlias.is_string()) {
                    throw std::runtime_error("CastDefLoader: " + ctx + ".aliases must be an array of strings");
                }
                def.aliases.push_back(jAlias.get<std::string>());
            }
        }

        TryColor4F(jCast, "name_color", def.textColor);
        TryColor4FColorCode(jCast, "color_code", def.textColor);

        TryFloat(jCast, "head_y", def.headY);
        TryString(jCast, "card_image", def.cardImage);

        if (def.id.empty()) {
            throw std::runtime_error("CastDefLoader: " + ctx + ".id cannot be empty");
        }
        if (table.byId.contains(def.id)) {
            throw std::runtime_error("CastDefLoader: duplicate cast id '" + def.id + "' in " + ctx);
        }

        table.byId.emplace(def.id, std::move(def));
    }

    // build nameToId map
    for (const auto& [id, def] : table.byId) {
        table.nameToId.emplace(def.name, id);
        for (const auto& alias : def.aliases) {
            table.nameToId.emplace(alias, id);
        }
    }

    return table;
}

} // namespace Salt2D::Game::Story
