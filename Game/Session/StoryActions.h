// Game/Session/StoryActions.h
#ifndef GAME_SESSION_STORYACTIONS_H
#define GAME_SESSION_STORYACTIONS_H

#include <cstdint>

namespace Salt2D::Game::Session {

struct StoryActions {
    bool confirmPressed = false;
    bool cancelPressed  = false;
    int  navY           = 0; // -1 up, 1 down
    bool accelHolded    = false;

    bool vnHistoryUp    = false;

    bool debateOpenMenuPressed = false;

    bool ConsumeConfirm() { bool v = confirmPressed; confirmPressed = false; return v; }
    bool ConsumeCancel()  { bool v = cancelPressed;  cancelPressed  = false; return v; }
    int  ConsumeNavY()    { int  v = navY;           navY = 0;               return v; }
};

struct PointerFrame {
    int x = 0, y = 0;
    bool lPressed   = false;
    bool lDown      = false;
    bool lReleased  = false;
    int  wheel      = 0;
};

struct ActionFrame {
    StoryActions actions;
    PointerFrame pointer;
};

} // namespace Salt2D::Game::Session

#endif // GAME_SESSION_STORYACTIONS_H
