// Game/Story/Resources/DebateDefLoader.cpp
#include "DebateDefLoader.h"

#include <nlohmann/json.hpp>
#include <stdexcept>

namespace Salt2D::Game::Story {

using json = nlohmann::json;

static std::string RequireString(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_string()) {
        throw std::runtime_error("DebateDefLoader: missing/invalid string field '" + std::string(key) + "' in " + context);
    }
    return j[key].get<std::string>();
}

static int RequireInt(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_number_integer()) {
        throw std::runtime_error("DebateDefLoader: missing/invalid integer field '" + std::string(key) + "' in " + context);
    }
    return j[key].get<int>();
}

DebateDef LoadDebateDef(Utils::IFileSystem& fs, const std::filesystem::path& fullPath) {
    if (!fs.Exists(fullPath)) {
        throw std::runtime_error("DebateDefLoader: file does not exist: " + fullPath.string());
    }
    const std::string text = fs.ReadTextFileUtf8(fullPath, true);
    
    json root = json::parse(text);
    if (!root.is_object()) {
        throw std::runtime_error("DebateDefLoader: root JSON is not an object in " + fullPath.string());
    }

    DebateDef def;

    if (!root.contains("statements") || !root["statements"].is_array()) {
        throw std::runtime_error("DebateDefLoader: missing/invalid 'statements' array in " + fullPath.string());
    }
    for (size_t i = 0; i < root["statements"].size(); i++) {
        const auto& jStatement = root["statements"][i];
        const auto ctx = "statements[" + std::to_string(i) + "]";
        if (!jStatement.is_object()) {
            throw std::runtime_error("DebateDefLoader: " + ctx + " must be object");
        }

        DebateStatement statement;
        statement.speaker = RequireString(jStatement, "speaker", ctx);
        statement.text    = RequireString(jStatement, "text",    ctx);
        def.statements.push_back(std::move(statement));
    }

    if (root.contains("menus")) {
        if (!root["menus"].is_array()) {
            throw std::runtime_error("DebateDefLoader: 'menus' is not an array in " + fullPath.string());
        }
        for (size_t i = 0; i < root["menus"].size(); i++) {
            const auto& jMenu = root["menus"][i];
            const auto ctx = "menus[" + std::to_string(i) + "]";
            if (!jMenu.is_object()) {
                throw std::runtime_error("DebateDefLoader: " + ctx + " must be object");
            }

            DebateMenu menu;
            menu.menuId         = RequireString(jMenu, "menu_id",         ctx);
            menu.statementIndex = RequireInt   (jMenu, "statement_index", ctx);
            menu.spanId         = RequireString(jMenu, "span_id",         ctx);

            if (!jMenu.contains("options") || !jMenu["options"].is_array()) {
                throw std::runtime_error("DebateDefLoader: missing/invalid 'options' array in " + ctx);
            }
            for (size_t j = 0; j < jMenu["options"].size(); j++) {
                const auto& jOption = jMenu["options"][j];
                const auto optCtx = ctx + ".options[" + std::to_string(j) + "]";
                if (!jOption.is_object()) {
                    throw std::runtime_error("DebateDefLoader: " + optCtx + " must be object");
                }

                DebateOption option;
                option.optionId = RequireString(jOption, "option_id", optCtx);
                option.label    = RequireString(jOption, "label",     optCtx);
                menu.options.push_back(std::move(option));
            }

            if (menu.statementIndex < 0 || static_cast<size_t>(menu.statementIndex) >= def.statements.size()) {
                throw std::runtime_error("DebateDefLoader: invalid statement_index in " + ctx);
            }
            if (menu.spanId.empty()) {
                throw std::runtime_error("DebateDefLoader: empty span_id in " + ctx);
            }

            def.menus.push_back(std::move(menu));
        }
    }

    return def;
}

} // namespace Salt2D::Game::Story
