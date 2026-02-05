// Game/Session/StoryActionMap.h
#ifndef GAME_SESSION_STORYACTIONMAP_H
#define GAME_SESSION_STORYACTIONMAP_H

#include "StoryActions.h"
#include "Game/Story/StoryTypes.h"
#include "Core/Input/InputState.h"

namespace Salt2D::Game::Session {

class StoryActionMap {
public:
    ActionFrame Map(Story::NodeType type, const Core::InputState& in) const;
};

} // namespace Salt2D::Game::Session

#endif // GAME_SESSION_STORYACTIONMAP_H
