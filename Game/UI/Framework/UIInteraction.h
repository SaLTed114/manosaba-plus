// Game/UI/Framework/UIInteraction.h
#ifndef GAME_UI_FRAMEWORK_UIINTERACTION_H
#define GAME_UI_FRAMEWORK_UIINTERACTION_H

#include "UIFrame.h"
#include "Game/Session/StoryActions.h"

namespace Salt2D::Game::UI {

struct UIPointerState {
    HitKey hovered = 0;
    HitKey pressed = 0;
};

struct UIInteractionResult {
    HitKey hovered = 0;
    HitKey clicked = 0;
};

class UIInteraction {
public:
    static UIInteractionResult Update(
        const UIFrame& frame,
        const Session::ActionFrame& af,
        UIPointerState& state);
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_FRAMEWORK_UIINTERACTION_H
