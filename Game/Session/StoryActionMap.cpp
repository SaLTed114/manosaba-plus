// Game/Session/StoryActionMap.cpp
#include "StoryActionMap.h"

#include <vector>
#include <Windows.h>

namespace Salt2D::Game::Session {

static inline bool PressedAny(const Core::InputState& in, const std::vector<uint16_t>& keys) {
    for (uint16_t key : keys) if (in.Pressed(key)) return true;
    return false;
}

ActionFrame StoryActionMap::Map(Story::NodeType type, const Core::InputState& in) const {
    ActionFrame frame;

    frame.pointer.x = in.mouseX;
    frame.pointer.y = in.mouseY;
    frame.pointer.wheel = in.wheel;
    frame.pointer.lPressed  = in.Pressed(VK_LBUTTON);
    frame.pointer.lDown     = in.Down(VK_LBUTTON);
    frame.pointer.lReleased = in.Released(VK_LBUTTON);

    frame.actions.accelHolded = in.Down(Core::Key::Ctrl);

    frame.actions.cancelPressed = PressedAny(in, {
        Core::KeyToVK(Core::Key::Escape),
        Core::KeyToVK(Core::Key::RButton),
    });

    if (in.Pressed(Core::Key::Up)   || in.wheel > 0) frame.actions.navY -= 1;
    if (in.Pressed(Core::Key::Down) || in.wheel < 0) frame.actions.navY += 1;

    const bool keyConfirm = PressedAny(in, {
        Core::KeyToVK(Core::Key::Enter),
        Core::KeyToVK(Core::Key::Space),
    });

    switch (type) {
    case Story::NodeType::VN:
    case Story::NodeType::BE:
    case Story::NodeType::Error:
        frame.actions.vnHistoryUp = (in.Pressed(Core::Key::Up) || in.wheel > 0);
        frame.actions.confirmPressed =
            keyConfirm || in.Pressed(Core::Key::Down) ||
            (in.wheel < 0);
        frame.actions.navY = 0; // Disable nav in VN
        break;
    case Story::NodeType::Debate:
        // current keyboard placeholders for Debate
        frame.actions.confirmPressed = keyConfirm;
        frame.actions.debateOpenMenuPressed = in.Pressed(Core::Key::M);
        break;
    case Story::NodeType::Present:
        frame.actions.confirmPressed = keyConfirm;
        break;
    case Story::NodeType::Choice:
        frame.actions.confirmPressed = keyConfirm;
    default:
        break;
    }

    return frame;
}

} // namespace Salt2D::Game::Session
