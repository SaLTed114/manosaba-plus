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
}

std::optional<GraphEvent> PresentRunner::Pick(const std::string& itemId) {
    for (const auto& item : def_.items) {
        if (item.itemId == itemId) {
            return GraphEvent{Trigger::Pick, itemId};
        }
    }
    return std::nullopt;
}

} // namespace Salt2D::Game::Story