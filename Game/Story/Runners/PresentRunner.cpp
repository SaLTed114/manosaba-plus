// Game/Story/Runners/PresentRunner.cpp
#include "PresentRunner.h"
#include "Game/Story/Resources/PresentDefLoader.h"

#include <stdexcept>

namespace Salt2D::Game::Story {

PresentRunner::PresentRunner(Utils::IFileSystem& fs) : fs_(fs) {}

void PresentRunner::Enter(const Node& node) {
    if (node.type != NodeType::Present) {
        throw std::runtime_error("PresentRunner::Enter: Node is not of type Present");
    }

    def_ = LoadPresentDef(fs_, node.resourceFullPath);
    
    if (logger_) {
        logger_->Debug("PresentRunner",
            "Entered Present node: prompt=\"" + def_.prompt + 
            "\", items=" + std::to_string(def_.items.size()));
    }
}

std::optional<GraphEvent> PresentRunner::Pick(const std::string& itemId) {
    for (const auto& item : def_.items) {
        if (item.itemId == itemId) {
            if (logger_) {
                logger_->Debug("PresentRunner",
                    "Picked valid item: " + itemId + " (" + item.label + ")");
            }
            return GraphEvent{Trigger::Pick, itemId};
        }
    }
    
    if (logger_) {
        logger_->Debug("PresentRunner",
            "Pick failed: itemId \"" + itemId + "\" not found in available items");
    }
    return std::nullopt;
}

} // namespace Salt2D::Game::Story