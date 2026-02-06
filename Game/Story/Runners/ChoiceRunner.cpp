// Game/Story/Runners/ChoiceRunner.cpp
#include "ChoiceRunner.h"
#include "Game/Story/Resources/ChoiceDefLoader.h"

#include <stdexcept>

namespace Salt2D::Game::Story {

ChoiceRunner::ChoiceRunner(Utils::IFileSystem& fs) : fs_(fs) {}

void ChoiceRunner::Enter(const Node& node) {
    if (node.type != NodeType::Choice) {
        throw std::runtime_error("ChoiceRunner::Enter: Node is not of type Choice");
    }

    def_ = LoadChoiceDef(fs_, node.resourceFullPath);
    
    if (logger_) {
        logger_->Debug("ChoiceRunner",
            "Entered Choice node: options=" + std::to_string(def_.options.size()));
    }
}

std::optional<GraphEvent> ChoiceRunner::Choose(const std::string& optionId) {
    for (const auto& option : def_.options) {
        if (option.optionId == optionId) {
            if (logger_) {
                logger_->Debug("ChoiceRunner",
                    "Chose valid option: " + optionId + " (" + option.label + ")");
            }
            return GraphEvent{Trigger::Option, optionId};
        }
    }
    
    if (logger_) {
        logger_->Debug("ChoiceRunner",
            "Choose failed: optionId \"" + optionId + "\" not found in available options");
    }
    return std::nullopt;
}

} // namespace Salt2D::Game::Story
